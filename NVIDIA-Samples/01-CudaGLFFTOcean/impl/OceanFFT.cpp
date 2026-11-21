// Standard Header files
#include <stdio.h> // for standard I/O
#include <stdlib.h> // for exit()
#include <memory.h> // for memset()

// X11 header (11 th version of 'X') 
#include <X11/Xlib.h> 
#include <X11/Xutil.h> // XVisualInfo
#include <X11/XKBlib.h> // for KeyBoard

// OpenGL header files
#include <GL/glew.h> // 
#include <GL/gl.h> // for OpenGL functionality
#include <GL/glx.h> // for bridging API's

// Texture header
#include <SOIL/SOIL.h>

// CUDA headers
#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>
#include <cufft.h>

FILE *gpFile = NULL;
#include "../../core/shader.h"
#include "../../core/helper.h"
#include "../../core/buffer.h"

#include "../../core/helper_timer.h"

#include "../../core/vmath.h"
using namespace vmath;

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define CUDART_PI_F 3.141592654F
#define CUDART_SQRT_HALF_F 0.707106781F


// Global variables
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, 
	Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;

struct x_window {
	Display *display = NULL;
	XVisualInfo *visualInfo = NULL;
	Colormap colorMap;
	Window window;
	Bool fullScreen = False;
	GLXFBConfig glxFBConfig;

	GLXContext glxContext; // OpenGL related
	Bool bActiceWindow = False;
};
x_window g_window;

GLuint shaderProgramObject;

enum
{
	KPE_ATTRIBUTE_POSITION = 0,
	KPE_ATTRIBUTE_COLOR,
	KPE_ATTRIBUTE_NORMAL,
	KPE_ATTRIBUTE_TEXTURE0
};


GLuint vao;
GLuint heightScaleUniform, chopinessUniform, sizeUniform;
GLuint modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
GLuint deepColorUniform, shallowColorUniform, skyColorUniform, lightDirUniform;
GLuint textureSamplerUniform;

mat4 perspectiveProjectionMatrix;

// OpenGL vertex buffers
GLuint posVertexBuffer;
GLuint heightVertexBuffer, slopeVertexBuffer;
struct cudaGraphicsResource *cuda_posVB_resource, *cuda_heightVB_resource,
    *cuda_slopeVB_resource; // handles OpenGL-CUDA exchange

GLuint indexBuffer;

// simulation parameters
const float g = 9.81f; // gravitational constant
const float A = 1e-7f; // wave scale factor
const float patchSize = 100;   // patch size
float windSpeed = 100.0f;
float windDir   = CUDART_PI_F / 3.0f;
float dirDepend = 0.07f;

// timer variables
StopWatchInterface* timer = NULL;
float prevTime = 0.0f;
float animTime = 0.0f;
float animationRate = -0.001f;

bool animate     = true;
bool drawPoints  = false;
bool wireFrame   = false;
bool g_hasDouble = false;

// CUDA variables
cufftHandle fftPlan;
unsigned int meshSize = 256 * 2;
unsigned int spectrumW = meshSize + 4;
unsigned int spectrumH = meshSize + 1;
float2 *d_h0    = 0; // heightfield at time 0
float2 *h_h0    = 0;
float2 *d_ht    = 0; // heightfield at time t
float2 *d_slope = 0;

// pointers to device object
float  *g_hptr = NULL;
float2 *g_sptr = NULL;

// 
char titleText[255];
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int numFrames = 0;

////////////////////////////////////////////////////////////////////////////////
// kernels
extern "C" void cudaGenerateSpectrumKernel(float2      *d_h0,
                                           float2      *d_ht,
                                           unsigned int in_width,
                                           unsigned int out_width,
                                           unsigned int out_height,
                                           float        animTime,
                                           float        patchSize);

extern "C" void
cudaUpdateHeightmapKernel(float *d_heightMap, float2 *d_ht, unsigned int width, 
	unsigned int height, bool autoTest);

extern "C" void cudaCalculateSlopeKernel(float *h, float2 *slopeOut, unsigned int width, 
	unsigned int height);

// Entry point function
int main(void)
{
	// function declarations
	void chooseBestVisual(int);
	void toggleFullscreen(void);
	int initialize(void);
	void handleEvents();
	
	void uninitialize(void);

	// local variables
	int defaultScreen;
	int defaultDepth;
	
	// PP variables
	XSetWindowAttributes windowAttribute;
	int styleMask;
	Atom wm_delete_window_atom;
	
	int screenWidth;
	int screenHeight;
	
	
	// code
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		printf("Creation of Log file failed. Exitting...\n");
	}

	// display struct is availed by XServer to us
	g_window.display = XOpenDisplay(NULL);
	if (g_window.display == NULL)	
	{
		fprintf(gpFile,"ERROR : XOpenDisplay() Failed !!!\n");
		uninitialize();
		exit(1);
	}

	
	defaultScreen = XDefaultScreen(g_window.display);
	// defaultDepth = XDefaultDepth(g_window.display, defaultScreen);

	// choose best visual for opengl compatible window
	chooseBestVisual(defaultScreen);

	// set window properties and event masks allowed to xlib 
	memset(&windowAttribute, 0, sizeof(XSetWindowAttributes));
	windowAttribute.border_pixel = 0;
	windowAttribute.background_pixel = XBlackPixel(g_window.display, defaultScreen);
	windowAttribute.background_pixmap = 0; // bkg picture vapraychay ka asa meaning hoto
	windowAttribute.colormap = XCreateColormap(g_window.display, 
						RootWindow(g_window.display, g_window.visualInfo->screen),
						g_window.visualInfo->visual,
						AllocNone);
	windowAttribute.event_mask = ExposureMask | KeyPressMask| StructureNotifyMask 
			| FocusChangeMask;

	// set colormap from window attributes for current application to be used below
	g_window.colorMap = windowAttribute.colormap;

	// 
	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

	// create window in a RAM
	g_window.window = XCreateWindow(g_window.display,
			RootWindow(g_window.display, g_window.visualInfo->screen),
			100, 															// x
			100, 															// y
			WIN_WIDTH, 														// width
			WIN_HEIGHT, 													// height
			0, 																// border width
			g_window.visualInfo->depth,  									// depth of window
			InputOutput,
			g_window.visualInfo->visual,
			styleMask,
			&windowAttribute); 
	if (!g_window.window)
	{
		fprintf(gpFile,"ERROR : XCreateWindow() Failed !!!\n");
		uninitialize();
		exit(1);
	}
	
	// set the window title
	XStoreName(g_window.display, g_window.window, "KPE : OpenGL-XWindows");
	
	// configuring for closing window ... Xwindow does not have close button by default, 
	// we ask for it to Window Manager(GNOME, KDE, etc)
	// WM -> Window Manager
	wm_delete_window_atom = XInternAtom(g_window.display, "WM_DELETE_WINDOW", True);

	XSetWMProtocols(g_window.display, g_window.window, &wm_delete_window_atom, 1);

	// showing window
	XMapWindow(g_window.display, g_window.window);

	// centering of Window
	screenWidth = XWidthOfScreen(XScreenOfDisplay(g_window.display, defaultScreen)); 
	screenHeight = XHeightOfScreen(XScreenOfDisplay(g_window.display, defaultScreen));
	XMoveWindow(g_window.display, g_window.window, screenWidth/2 - WIN_WIDTH/2, 
		screenHeight/2 - WIN_HEIGHT/2);

	// call to initialize
	initialize();

	// handle events
	handleEvents();
	
	uninitialize();

	return(0);
}

void chooseBestVisual(int defaultScreen)
{
	// forward declaration
	void uninitialize();

	// local variables
	GLXFBConfig *glxFBConfigs = NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *tmpXVisualInfo = NULL;
	int numFBConfig;

	// code
	static int frameBufferAttributes[] = 
	{
		GLX_DOUBLEBUFFER, True,
		GLX_X_RENDERABLE, True, // context becomes hardware renderable
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, // where we have to draw 
											// -> on window (GLX_WINDOW_BIT)
		GLX_RENDER_TYPE, GLX_RGBA_BIT, // RGBA type image (frame)
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_STENCIL_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		None
	}; // like pfd in Windows OS

	// choosing best visualInfo
	glxFBConfigs = glXChooseFBConfig(g_window.display, defaultScreen, frameBufferAttributes, 
		&numFBConfig); // returns array of FB configs and its count
	if (glxFBConfigs == NULL)
	{
		fprintf(gpFile, "ERROR : glXChooseFBConfig() Failed\n");
		uninitialize();
		exit(1);
	}
	fprintf(gpFile, "INFO : Found number of FBConfigs are %d\n", numFBConfig);

	// find best FBConfig from the aray
	int bestFrameBufferConfig = -1;
	int worstFrameBufferConfig = -1;
	int bestNumberOfSamples = -1;
	int worstNumberOfSamples = 999;

	for (int i = 0; i < numFBConfig; i++)
	{
		tmpXVisualInfo = glXGetVisualFromFBConfig(g_window.display, glxFBConfigs[i]);
		if (tmpXVisualInfo != NULL)
		{
			int samples, sampleBuffers;
			glXGetFBConfigAttrib(g_window.display, glxFBConfigs[i], GLX_SAMPLE_BUFFERS, 
				&sampleBuffers);
			glXGetFBConfigAttrib(g_window.display, glxFBConfigs[i], GLX_SAMPLES, &samples);
	
			fprintf(gpFile, "VisualInfo = 0x%lu found SampleBuffers = %d and samples = %d\n", 
				tmpXVisualInfo->visualid, sampleBuffers, samples);

			if (bestFrameBufferConfig < 0 || sampleBuffers && 
				samples > bestNumberOfSamples)
			{
				bestFrameBufferConfig = i;
				bestNumberOfSamples = samples;
			}

			if (worstFrameBufferConfig < 0 || !sampleBuffers || 
				samples < worstNumberOfSamples)
			{
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(tmpXVisualInfo);
		tmpXVisualInfo = NULL;
	}
	
	bestGLXFBConfig = glxFBConfigs[bestFrameBufferConfig];
	g_window.glxFBConfig = bestGLXFBConfig;
	XFree(glxFBConfigs);
	glxFBConfigs = NULL;

	g_window.visualInfo = glXGetVisualFromFBConfig(g_window.display, bestGLXFBConfig);
	fprintf(gpFile, "visualId of bestVisualInfo is 0x%lu\n", g_window.visualInfo->visualid);

}

void toggleFullscreen(void)
{
	// local variables declarations
	Atom wm_current_state_atom;
	Atom wm_fullscreen_state_atom;
	XEvent event;

	// code
	wm_current_state_atom = XInternAtom(g_window.display, "_NET_WM_STATE", False);
	wm_fullscreen_state_atom = XInternAtom(g_window.display, "_NET_WM_STATE_FULLSCREEN", False);

	memset(&event, 0, sizeof(XEvent));
	event.type = ClientMessage;
	event.xclient.window = g_window.window;
	event.xclient.message_type = wm_current_state_atom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = g_window.fullScreen ? 0 : 1;
	event.xclient.data.l[1] = wm_fullscreen_state_atom;

	// like sent and post messages in Windows OS
	XSendEvent(g_window.display,
			RootWindow(g_window.display, g_window.visualInfo->screen),
			False, // if this msg is for this window or for its child window
			SubstructureNotifyMask,
			&event);
}

void handleEvents()
{
	// forward declaration
	void resize(int, int);
	void draw(void);

	// local variables
	char keys[26];
	Bool bDone = False;
	static int win_width, win_height;
	XEvent event;
	KeySym keysym;

	// message loop
	while(bDone == False)
	{
			while (XPending(g_window.display)) // analogus to PeekMessage() in Windows OS
                	{
                		XNextEvent(g_window.display, &event); // GetMessage()
                		switch (event.type)
                		{
                			case MapNotify: // WM_CREATE in windows
                				break;
					case FocusIn:
						g_window.bActiceWindow = True;
						break;
					case FocusOut:
						g_window.bActiceWindow = False;
						break;
                			case KeyPress:
                				keysym = XkbKeycodeToKeysym(g_window.display, 
									event.xkey.keycode, 0, 0);
                				switch (keysym)
                				{
                					case XK_Escape:
								bDone = True;
                						break;
                				}
                
                				XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
                				switch (keys[0])
                				{
                					case 'F':
                					case 'f':
                						if (g_window.fullScreen == False)
                						{
                							toggleFullscreen();
                							g_window.fullScreen = True;
                						}
                						else
                						{
                							toggleFullscreen();
                							g_window.fullScreen = False;
                						}
                						break;
										case 'w':
											wireFrame = !wireFrame;
											break;

										case 'p':
											drawPoints = !drawPoints;
											break;

										case ' ':
											animate = !animate;
											break;
										}
                				break;
					case ConfigureNotify:
						win_width = event.xconfigure.width;
						win_height = event.xconfigure.height;
						resize(win_width, win_height);
						break;
                			case 33: // retuened by WM_DELETE_WINDOW_ATOM i.e. close button
						bDone = True;
                				break;
                
                		}
                	}
			if (g_window.bActiceWindow == True)
			{
				draw();
			}
                
	}

}

void checkCUFFTErrors(cufftResult result){
	switch (result){
		// case CUFFT_SUCCESS: {printf("CUFFT SUCCESS\n"); break;}
		case CUFFT_INVALID_PLAN: {printf("CUFFT INVALID PLAN\n"); break;}
		case CUFFT_ALLOC_FAILED: {printf("CUFFT ALLOC FAILED\n"); break;}
		case CUFFT_INVALID_TYPE: {printf("CUFFT INVALID TYPE\n"); break;}
		case CUFFT_INVALID_VALUE: {printf("CUFFT INVALID VALUE\n"); break;}
		case CUFFT_INTERNAL_ERROR: {printf("CUFFT INTERNAL ERROR"); break;}
		case CUFFT_EXEC_FAILED: {printf("CUFFT EXEC FAILED"); break;}
		case CUFFT_SETUP_FAILED: {printf("CUFFT SETUP FAILED"); break;}
		case CUFFT_INVALID_SIZE: {printf("CUFFT INVALID SIZE"); break;}
		case CUFFT_UNALIGNED_DATA: {printf("CUFFT UNALIGNED DATA"); break;}
		case CUFFT_INCOMPLETE_PARAMETER_LIST: {printf("CUFFT INCOMPLETE PARAMETER LIST"); break;}
		case CUFFT_INVALID_DEVICE: {printf("CUFFT INVALID DEVICE"); break;}
		case CUFFT_PARSE_ERROR: {printf("CUFFT PARSE ERROR"); break;}
		case CUFFT_NO_WORKSPACE: {printf("CUFFT NO WORKSPACE"); break;}
		case CUFFT_NOT_IMPLEMENTED: {printf("CUFFT NOT IMPLEMENTED"); break;}
		case CUFFT_LICENSE_ERROR: {printf("CUFFT LICENSE ERROR"); break;}
		case CUFFT_NOT_SUPPORTED: {printf("CUFFT NOT SUPPORTED"); break;}
		// default: {printf("UNKNOWN CUDA RESULT\nFIND YOURSELF\n"); break;}
	}
}

void checkCudaErrors(cudaError error, int line){
	void uninitialize(void);

	if (error != cudaSuccess)
	{
		printf(" Failed at %d \n", line);
		uninitialize();
		exit(EXIT_FAILURE);
	}
	// else if (dev_count == 0)
	// {
	// 	fprintf(gpFile, "No CUDA Supported Device \n");
	// 	uninitialize();
	// 	exit(EXIT_FAILURE);
	// }
}

int initialize(void)
{
	// function declarations
	void resize(int, int);
	void printGLInfo(void);
	Bool loadGlTexture(GLuint *texture, const char *path);
	void generate_h0(float2 *h0);
	void runCuda(void);
	void uninitialize(void);

	// code
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
		(GLubyte *)"glXCreateContextAttribsARB");
	if (glXCreateContextAttribsARB == NULL)
	{
		fprintf(gpFile, "glXGetProcAddressARB() Failed\n");
		uninitialize();
		exit(1);
	}

	// version = 4.5, then 4 is major version and 6 is minor version
	GLint contextAttributes[] = {
				GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
				GLX_CONTEXT_MINOR_VERSION_ARB, 6,
				GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
				None
	};

	// fallback
	g_window.glxContext = glXCreateContextAttribsARB(g_window.display, g_window.glxFBConfig, 
		NULL, True, contextAttributes);
	if (!g_window.glxContext)
	{
	
		GLint contextAttributes[] = {
                			GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
                			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                			None
                };
		g_window.glxContext = glXCreateContextAttribsARB(g_window.display, g_window.glxFBConfig, 
			NULL, True, contextAttributes);

		fprintf(gpFile, "Cannot support 4.6, and hence falling back to default varsion\n");

	}
	else
	{
		fprintf(gpFile, "Found the Support to 4.6 version\n");
	}

	// checking if Hardware rendering
	if (!glXIsDirect(g_window.display, g_window.glxContext))
	{
		fprintf(gpFile, "Direct Rendering i.e. H.W. rendering not supported\n");
	}
	else
	{
		fprintf(gpFile, "Direct Rendering i.e. H.W. rendering supported\n");
	}

	// make it as current context
	glXMakeCurrent(g_window.display, g_window.window, g_window.glxContext);

	// GLEW initialization
	if (glewInit() != GLEW_OK)
	{
		return (-5);
	}

	// Print OpenGL info
	// printGLInfo();

	// vertex shader
	const GLchar *vertexShaderSourcePath = "./doc/ocean.vert";

	// Fragment Shader
	const GLchar *fragmentShaderSourcePath = "./doc/ocean.frag";

	shaderProgramObject = glCreateProgram();
	// create and compile shaders
	if(!attachShader(shaderProgramObject, GL_VERTEX_SHADER, vertexShaderSourcePath, 
		"Vertex Ocean")){
		glDeleteProgram(shaderProgramObject);
		fprintf(stderr, "Couldn't attach vertex shader from file %s\n", vertexShaderSourcePath);
		return 0;
	}

	if(!attachShader(shaderProgramObject, GL_FRAGMENT_SHADER, fragmentShaderSourcePath, 
		"Fragment Ocean")){
		glDeleteProgram(shaderProgramObject);
		fprintf(stderr, "Couldn't attach fragment shader form file %s\n", fragmentShaderSourcePath);
		return 0;
	}
	
	// prelinked binding
	glBindAttribLocation(shaderProgramObject, 0, "a_vertex");
	glBindAttribLocation(shaderProgramObject, 1, "a_height");
	glBindAttribLocation(shaderProgramObject, 2, "a_slope");

	GLint linked;
	glLinkProgram(shaderProgramObject);
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        glDeleteProgram(shaderProgramObject);
        char temp[256];
        glGetProgramInfoLog(shaderProgramObject, 256, 0, temp);
        fprintf(stderr, "Failed to link program: %s\n", temp);
        return 0;
    }

	heightScaleUniform = glGetUniformLocation(shaderProgramObject, "heightScale");
	chopinessUniform = glGetUniformLocation(shaderProgramObject, "chopiness");
	sizeUniform = glGetUniformLocation(shaderProgramObject, "size");
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "modelMatrixUniform");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "viewMatrixUniform");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "projectionMatrixUniform");
	deepColorUniform = glGetUniformLocation(shaderProgramObject, "deepColor");
	shallowColorUniform = glGetUniformLocation(shaderProgramObject, "shallowColor");
	skyColorUniform = glGetUniformLocation(shaderProgramObject, "skyColor");
	lightDirUniform = glGetUniformLocation(shaderProgramObject, "lightDir");


	// CUDA related initializations
	// findCudaDevice(argc, (const char **)argv);
	// CUDA initialization
	int dev_count = 0;
	checkCudaErrors(cudaGetDeviceCount(&dev_count), __LINE__);
	// select CUDA suported device
	cudaSetDevice(0); // default 0'th cuda supported device

    // create FFT plan
    checkCUFFTErrors(cufftPlan2d(&fftPlan, meshSize, meshSize, CUFFT_C2C));

    // allocate memory
    int spectrumSize = spectrumW * spectrumH * sizeof(float2);
    checkCudaErrors(cudaMalloc((void **)&d_h0, spectrumSize), __LINE__);
    h_h0 = (float2 *)malloc(spectrumSize);
    generate_h0(h_h0);
    checkCudaErrors(cudaMemcpy(d_h0, h_h0, spectrumSize, cudaMemcpyHostToDevice), __LINE__);

    int outputSize = meshSize * meshSize * sizeof(float2);
    checkCudaErrors(cudaMalloc((void **)&d_ht, outputSize), __LINE__);
    checkCudaErrors(cudaMalloc((void **)&d_slope, outputSize), __LINE__);

    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);
    prevTime = sdkGetTimerValue(&timer);

	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
    // create vertex buffers and register with CUDA
    createVBO(&heightVertexBuffer, meshSize * meshSize * sizeof(float));
    checkCudaErrors(
        cudaGraphicsGLRegisterBuffer(&cuda_heightVB_resource, heightVertexBuffer, 
			cudaGraphicsMapFlagsWriteDiscard), __LINE__);

    createVBO(&slopeVertexBuffer, outputSize);
    checkCudaErrors(
        cudaGraphicsGLRegisterBuffer(&cuda_slopeVB_resource, slopeVertexBuffer, 
			cudaGraphicsMapFlagsWriteDiscard), __LINE__
    );

    // create vertex and index buffer for mesh
    createMeshPositionVBO(&posVertexBuffer, meshSize, meshSize);
    createMeshIndexBuffer(&indexBuffer, meshSize, meshSize);

	glBindBuffer(GL_ARRAY_BUFFER, posVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, heightVertexBuffer);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, slopeVertexBuffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

/*	// declaring vertex data array
	const GLfloat squareVertices[] = 
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	const GLfloat texcoord[] =
        {
        	1.0f, 0.0f,
        	0.0f, 0.0f, 
        	0.0f, 1.0f,
        	1.0f, 1.0f
        };

	// vao and vbo_position related changes
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Below 5 'vbo_position' steps are reorded in 'vao'

	// recording started
	glGenBuffers(1, &vbo_position); // 1st para -> no of buffers, 2nd para -> address from GPU 
	// 1st para -> type of buffer to which vbo_position is to be bind, 2nd para -> vbo_position
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position); 

	// 1st para -> data bharnyachi jaga, 2nd para ->size of data, 3rd para -> actual data, 
	// 4th para -> how to fill the data (statically)
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW); 

	glVertexAttribPointer(KPE_ATTRIBUTE_POSITION, // fill the data at 0th position in shader
			3, // read the data as set of 3
			GL_FLOAT, // Data type of the data in array in GPU context
			GL_FALSE, // if TRUE, we have to give the data in range '1 to -1' i.e. normalized data
			0, // dhanga takaychi garaj nahi , karan fakt position cha array ahe (stride)
			NULL //dhanga astil tr data kuthay he sangaych ast i.e. data interleaved asel tr
			);

	// enabling the 0th position in shader, i.e. enabling the array mapped to that position
	glEnableVertexAttribArray(KPE_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo_position

	// vbo for texture
	glGenBuffers(1, &vbo_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(KPE_ATTRIBUTE_TEXTURE0,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			NULL);
	glEnableVertexAttribArray(KPE_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // unbind vao

*/

       	// here starts OpenGL code
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	perspectiveProjectionMatrix = mat4::identity();

	runCuda();	
        
	// warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);
	return(0);
}

float urand() { return rand() / (float)RAND_MAX; }

// Generates Gaussian random number with mean 0 and standard deviation 1.
float gauss()
{
    float u1 = urand();
    float u2 = urand();

    if (u1 < 1e-6f) {
        u1 = 1e-6f;
    }

    return sqrtf(-2 * logf(u1)) * cosf(2 * CUDART_PI_F * u2);
}

// Phillips spectrum
// (Kx, Ky) - normalized wave vector
// Vdir - wind angle in radians
// V - wind speed
// A - constant
float phillips(float Kx, float Ky, float Vdir, float V, float A, float dir_depend)
{
    float k_squared = Kx * Kx + Ky * Ky;

    if (k_squared == 0.0f) {
        return 0.0f;
    }

    // largest possible wave from constant wind of velocity v
    float L = V * V / g;

    float k_x     = Kx / sqrtf(k_squared);
    float k_y     = Ky / sqrtf(k_squared);
    float w_dot_k = k_x * cosf(Vdir) + k_y * sinf(Vdir);

    float phillips = A * expf(-1.0f / (k_squared * L * L)) / (k_squared * k_squared) 
		* w_dot_k * w_dot_k;

    // filter out waves moving opposite to wind
    if (w_dot_k < 0.0f) {
        phillips *= dir_depend;
    }

    // damp out waves with very small length w << l
    // float w = L / 10000;
    // phillips *= expf(-k_squared * w * w);

    return phillips;
}

// Generate base heightfield in frequency space
void generate_h0(float2 *h0)
{
    for (unsigned int y = 0; y <= meshSize; y++) {
        for (unsigned int x = 0; x <= meshSize; x++) {
            float kx = (-(int)meshSize / 2.0f + x) * (2.0f * CUDART_PI_F / patchSize);
            float ky = (-(int)meshSize / 2.0f + y) * (2.0f * CUDART_PI_F / patchSize);

            float P = sqrtf(phillips(kx, ky, windDir, windSpeed, A, dirDepend));

            if (kx == 0.0f && ky == 0.0f) {
                P = 0.0f;
            }

            // float Er = urand()*2.0f-1.0f;
            // float Ei = urand()*2.0f-1.0f;
            float Er = gauss();
            float Ei = gauss();

            float h0_re = Er * P * CUDART_SQRT_HALF_F;
            float h0_im = Ei * P * CUDART_SQRT_HALF_F;

            int i   = y * spectrumW + x;
            h0[i].x = h0_re;
            h0[i].y = h0_im;
        }
    }
}

Bool loadGlTexture(GLuint *texture, const char *path)
{
	// variable declarations
	int width, height;
	unsigned char *imageData = NULL;

	// code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);
	if (imageData == NULL)
	{
		return False;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// gluBuild2DMipmaps
	glTexImage2D(GL_TEXTURE_2D, 
			0, 
			GL_RGB, 
			width, 
			height, 
			0,
			GL_RGB, 
			GL_UNSIGNED_BYTE, 
			imageData);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);
	return True;

}

void printGLInfo(void)
{
	// local variable declarations
	GLint numExtensions = 0;

	// code
	fprintf(stdout, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(stdout, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, "OpenGL version : %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "GLSL version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(stdout, "Number Of Supported Extension : %d\n", numExtensions);

	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(stdout, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;	// to avoid divided 0(Illeegal Instruction) in future cause

	// screen cha konta bhag dakhvu... sadhya sagli screen dakhvli jat ahe pn he game need nusar 
	// customize karta yet.. eth width ai height he GL_SIZEI ya type che ahet 
	// ...resize la yetana te int ya type madhe yetat
	glViewport(0, 0, width, height); 
	
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 
		0.1f, 100.0f);
}



void calculateFPS(const char *device)
{
	// code
	prevTime = sdkGetTimerValue(&timer);
	float ELAPSED_TIME = prevTime / 1000.0f;
	float currentFrame = ELAPSED_TIME;
	deltaTime = currentFrame - lastFrame;
	numFrames++;

	float FPS = (1.0f / deltaTime) * numFrames;
	// char titleText[255];
	sprintf(titleText, "FPS : %f  ::\tMesh Size : %u  ::\tDEVICE : %s", 
		FPS, meshSize*meshSize, device);
	// SetWindowTextA(ghwnd, (LPCSTR)titleText);
	lastFrame = currentFrame;
	numFrames = 0;
}

////////////////////////////////////////////////////////////////////////////////
//! Run the Cuda kernels
////////////////////////////////////////////////////////////////////////////////
void runCuda()
{
    size_t num_bytes;

    // generate wave spectrum in frequency domain
    cudaGenerateSpectrumKernel(d_h0, d_ht, spectrumW, meshSize, meshSize, animTime, patchSize);

    // execute inverse FFT to convert to spatial domain
    checkCUFFTErrors(cufftExecC2C(fftPlan, d_ht, d_ht, CUFFT_INVERSE));

    // update heightmap values in vertex buffer
    checkCudaErrors(cudaGraphicsMapResources(1, &cuda_heightVB_resource, 0), __LINE__);
    checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&g_hptr, &num_bytes, 
		cuda_heightVB_resource), __LINE__);

    cudaUpdateHeightmapKernel(g_hptr, d_ht, meshSize, meshSize, false);

    // calculate slope for shading
    checkCudaErrors(cudaGraphicsMapResources(1, &cuda_slopeVB_resource, 0), __LINE__);
    checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&g_sptr, &num_bytes, 
		cuda_slopeVB_resource), __LINE__);

    cudaCalculateSlopeKernel(g_hptr, g_sptr, meshSize, meshSize);

    checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_heightVB_resource, 0), __LINE__);
    checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_slopeVB_resource, 0), __LINE__);
}

void draw(void)
{
	// run cuda
	if (animate){
		runCuda();
	}

	// code
	// initialize madhe sangitlele glClearColor che painting actually eth hot.. 
	// Frame buffer == COLOR_BUFFER -> true
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	calculateFPS("On GPU");
	XStoreName(g_window.display, g_window.window, titleText);

	// use the shaderProgramObject
	glUseProgram(shaderProgramObject);
        
	// transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	                                                                              
	translationMatrix = vmath::translate(0.0f, 0.0f, -1.0f);
	modelMatrix = translationMatrix;
	viewMatrix = vmath::lookat(vec3(0.0f, .3f, 1.0f), vec3(0.0f, 0.0f, 0.0f), 
							vec3(0.0f, 1.0f, 0.0f));
	
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	
    glUniform1f(heightScaleUniform, 0.5f);

    glUniform1f(chopinessUniform, 1.0f);

    glUniform2f(sizeUniform, float(meshSize), float(meshSize));
    // glUniform2fv(sizeUniform, 1, vec2(256.0f, 256.0f));

    // Set default uniform variables parameters for the pixel shader
    glUniform4f(deepColorUniform, 0.0f, 0.1f, 0.4f, 1.0f);

    glUniform4f(shallowColorUniform, 0.1f, 0.3f, 0.3f, 1.0f);

    glUniform4f(skyColorUniform, 1.0f, 1.0f, 1.0f, 1.0f);

    glUniform3f(lightDirUniform, 0.0f, 1.0f, 0.0f);
    // end of uniform settings

	glBindVertexArray(vao);
	//glColor3f(1.0, 1.0, 1.0);                                         
	// Game coding here
	if (drawPoints) {
        glDrawArrays(GL_POINTS, 0, meshSize * meshSize);
    }
    else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
        glDrawElements(GL_TRIANGLE_STRIP, ((meshSize * 2) + 2) * (meshSize - 1), 
				GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
	                  
	glBindVertexArray(0);
	// unuse the shaderProgramObject
	glUseProgram(0);

	glXSwapBuffers(g_window.display, g_window.window);

	float time = sdkGetTimerValue(&timer);

    if (animate) {
        animTime += (time - prevTime) * animationRate;
    }

	prevTime = time;
}


void uninitialize(void)
{
	// code
	// shader uninitialization
	sdkDeleteTimer(&timer);
    checkCudaErrors(cudaGraphicsUnregisterResource(cuda_heightVB_resource), __LINE__);
    checkCudaErrors(cudaGraphicsUnregisterResource(cuda_slopeVB_resource), __LINE__);

    deleteVBO(&posVertexBuffer);
    deleteVBO(&heightVertexBuffer);
    deleteVBO(&slopeVertexBuffer);

    checkCudaErrors(cudaFree(d_h0), __LINE__);
    checkCudaErrors(cudaFree(d_slope), __LINE__);
    checkCudaErrors(cudaFree(d_ht), __LINE__);
    free(h_h0);
    cufftDestroy(fftPlan);

	// deletion and uninitialization of vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	
	
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
	                                                                                                           
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint *shaderObjects = NULL;
		shaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, 
			shaderObjects);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}
	
	GLXContext currentContext;
	currentContext = glXGetCurrentContext();
	if (currentContext && currentContext == g_window.glxContext)
	{
		glXMakeCurrent(g_window.display, 0, 0);
	}

	if (g_window.glxContext)
	{
		glXDestroyContext(g_window.display, g_window.glxContext);
		g_window.glxContext = NULL;
	}

	if (g_window.visualInfo)
	{
		free(g_window.visualInfo);
	}

	if (g_window.window)
	{
		XDestroyWindow(g_window.display, g_window.window);
	}

	if (g_window.colorMap)
	{
		XFreeColormap(g_window.display, g_window.colorMap);
	}

	if (gpFile)
	{
		fprintf(gpFile, "\n========================================\n");
		fprintf(gpFile, "Closing Log File\n");
		free(gpFile);
		gpFile = NULL;
	}

	if (g_window.display)	
	{
		XCloseDisplay(g_window.display);
		g_window.display = NULL;
	}

}


