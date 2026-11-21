#include "shader_helper.h"

ShaderHelper::ShaderHelper()
{
    m_vertexShader = 0;
    m_fragmentShader = 0;
    m_shaderProgram = 0;
}

ShaderHelper::~ShaderHelper()
{
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    glDeleteProgram(m_shaderProgram);
}

GLuint ShaderHelper::loadShader(const char *filePath, GLenum shaderType)
{
    char shaderNameTag[50];
    GLuint glShaderType;
    // read the shader source from the file
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open())
    {
        std::cerr << "Error opening shader file: " << filePath << std::endl;
        g_logger.log(logger::LogType::_ERROR_, "Error opening shader file: %s\n", filePath);
        return 0;
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    std::string shaderSource = shaderStream.str();
    const char *shaderSourceCStr = shaderSource.c_str();

    switch (shaderType)
	{
	case VERTEX_SHADER:
    #ifdef __GNUC__
        strcpy(shaderNameTag, "Vertex");
    #else
        strcpy_s(shaderNameTag, 50, "Vertex");
    #endif
		glShaderType = GL_VERTEX_SHADER;
		break;

	case TESS_CONTROL_SHADER:
    #ifdef __GNUC__
        strcpy(shaderNameTag, "Tessellation Control");
    #else
		strcpy_s(shaderNameTag, 50, "Tessellation Control");
    #endif
		glShaderType = GL_TESS_CONTROL_SHADER;
		break;

	case TESS_EVALUATION_SHADER:
    #ifdef __GNUC__
        strcpy(shaderNameTag, "Tessellation Evaluation");
    #else
		strcpy_s(shaderNameTag, 50, "Tessellation Evaluation");
    #endif
		glShaderType = GL_TESS_EVALUATION_SHADER;
		break;

	case GEOMETRY_SHADER:
    #ifdef __GNUC__
        strcpy(shaderNameTag, "Geometry");
    #else
		strcpy_s(shaderNameTag, 50, "Geometry");
    #endif
		glShaderType = GL_GEOMETRY_SHADER;
		break;

	case FRAGMENT_SHADER:
    #ifdef __GNUC__
        strcpy(shaderNameTag, "Fragment");
    #else
		strcpy_s(shaderNameTag, 50, "Fragment");
    #endif
		glShaderType = GL_FRAGMENT_SHADER;
		break;

	case COMPUTE_SHADER:
    #ifdef __GNUC__
        strcpy(shaderNameTag, "Compute");
    #else
		strcpy_s(shaderNameTag, 50, "Compute");
    #endif
		glShaderType = GL_COMPUTE_SHADER;
		break;

	default:
		// PrintLog("Invalid Shader Type! Exitting Now ...\n");
        g_logger.log(logger::LogType::_ERROR_, "Invalid Shader Type! Exitting Now ...\n");
		return(0);
	}

    // create the vertex shader
    m_shaderObject = glCreateShader(glShaderType);
    glShaderSource(m_shaderObject, 1, &shaderSourceCStr, NULL);
    glCompileShader(m_shaderObject);

    // check for errors
    GLint result;
    glGetShaderiv(m_shaderObject, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(m_shaderObject, GL_INFO_LOG_LENGTH, &length);
        std::string errorLog(length, ' ');
        glGetShaderInfoLog(m_shaderObject, length, &length, &errorLog[0]);
        std::cerr << "Error compiling the "<< shaderNameTag <<" shader: " << errorLog << std::endl;
        g_logger.log(logger::LogType::_ERROR_, "Error compiling the %s shader: %s\n", shaderNameTag, errorLog);
        return 0;
    }

    return m_shaderObject;
}

GLuint ShaderHelper::createShaderProgram(std::vector<GLuint> shaderObjects)
{
    m_shaderProgram = glCreateProgram();
    for (auto shaderObject : shaderObjects)
    {
        glAttachShader(m_shaderProgram, shaderObject);
    }

    return m_shaderProgram;
}

bool ShaderHelper::linkShaderProgram()
{
    glLinkProgram(m_shaderProgram);

    // check for errors
    GLint result;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::string errorLog(length, ' ');
        glGetProgramInfoLog(m_shaderProgram, length, &length, &errorLog[0]);
        std::cerr << "Error linking the shader program: %s" << errorLog << std::endl;
        g_logger.log(logger::LogType::_ERROR_, "Error linking the shader program: %s\n", errorLog);
        return false;
    }

    return true;
}

void ShaderHelper::useShaderProgram()
{
    glUseProgram(m_shaderProgram);
}

void ShaderHelper::unuseShaderProgram()
{
    glUseProgram(0);
}

GLuint ShaderHelper::getUniformLocation(const char *name)
{
    return glGetUniformLocation(m_shaderProgram, name);
}

