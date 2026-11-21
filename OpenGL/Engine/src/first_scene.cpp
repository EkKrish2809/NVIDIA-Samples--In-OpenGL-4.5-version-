#include "first_scene.h"


first_scene::first_scene()
{
}

first_scene::~first_scene()
{
}

void first_scene::update()
{
}

void first_scene::draw()
{
    m_shaderHelper.useShaderProgram();

    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f))));
    glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(debugCamera.getViewMatrix()));
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(debugCamera.getProjectionMatrix()));
    // draw the triangle
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    m_shaderHelper.unuseShaderProgram();

}


bool first_scene::init()
{
    g_logger.log(logger::LogType::_INFO_, "first_scene::init() called\n");

    GLuint vertexShaderObject = m_shaderHelper.loadShader("./src/shaders/helloWorldTriangle/triangle_vs.glsl", VERTEX_SHADER);
    if (vertexShaderObject == 0)
    {
        g_logger.log_shader(logger::LogType::_ERROR_, "Failed to load vertex shader\n");
        return false;
    }
    else
    {
        g_logger.log_shader(logger::LogType::_INFO_, "Vertex shader loaded successfully\n");
    }

    GLuint fragmentShaderObject = m_shaderHelper.loadShader("./src/shaders/helloWorldTriangle/triangle_fs.glsl", FRAGMENT_SHADER);
    if (fragmentShaderObject == 0)
    {
        g_logger.log_shader(logger::LogType::_ERROR_, "Failed to load fragment shader\n");
        return false;
    }
    else
    {
        g_logger.log_shader(logger::LogType::_INFO_, "Fragment shader loaded successfully\n");
    }

    GLuint shaderProgram = m_shaderHelper.createShaderProgram({ vertexShaderObject, fragmentShaderObject });
    if (shaderProgram == 0)
    {
        g_logger.log_shader(logger::LogType::_ERROR_, "Failed to create shader program\n");
        return false;
    }

    glBindAttribLocation(shaderProgram, 0, "a_position");

    if (!m_shaderHelper.linkShaderProgram())
    {
        g_logger.log_shader(logger::LogType::_ERROR_, "Failed to link shader program\n");
        return false;
    }

    // set uniform locations
    m_modelMatrix = m_shaderHelper.getUniformLocation("u_modelMatrix");
    m_viewMatrix = m_shaderHelper.getUniformLocation("u_viewMatrix");
    m_projectionMatrix = m_shaderHelper.getUniformLocation("u_projectionMatrix");

    GLfloat vertices[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // initialize camera
    debugCamera = Camera();
    
    return true;
}

void first_scene::uninit()
{
    g_logger.log(logger::LogType::_INFO_, "first_scene::uninit() called\n");

    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void first_scene::render_ui()
{
    ImGui::Begin("Scene");                          // Create a window called "Hello, world!" and append into it.
    ImGui::Text("First Test Scene");
    ImGui::End();
}

void first_scene::process_events(XEvent event)
{
    if (event.type == KeyPress)
    {
        char keys[26];
        XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
        debugCamera.processKeyboard(keys[0], 0.1f);
    }
    if (event.type == MotionNotify)
    {
        debugCamera.processMouseMovement((float)event.xmotion.x, (float)event.xmotion.y, true);
    }
    printf("events\n");
}

void first_scene::resize(int width, int height)
{
}

