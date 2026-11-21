#include "camera.h"

Camera debugCamera;

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix()
{
    return glm::perspective(glm::radians(45.0f), (float)1920/(float)1080, 0.1f, 1000.0f);
}

void Camera::updateResolution(float _width, float _height)
{
    m_width = _width;
    m_height = _height;
}

void Camera::processKeyboard(char key, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;
    if (key == 'w')
        m_position += m_front * velocity;
    if (key == 's')
        m_position -= m_front * velocity;
    if (key == 'a')
        m_position -= m_right * velocity;
    if (key == 'd')
        m_position += m_right * velocity;
    if (key == 'v')
        m_position += m_up * velocity;
    if (key == ' ')
        m_position -= m_up *velocity;
}

void Camera::processMouseMovement(float x, float y, GLboolean constrainPitch = true)
{
    if (firstMouse)
    {
        printf("clicked\n");
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;


    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw = fmod((m_yaw + xoffset), 360.0f);
    m_pitch += yoffset;

    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    glm::vec3 front_ = glm::vec3(
        cos(degToRad(m_yaw)) * cos(degToRad(m_pitch)),
        sin(degToRad(m_pitch)),
        sin(degToRad(m_yaw)) * cos(degToRad(m_pitch)));

    m_front = glm::normalize(front_);

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    if (m_zoom >= 1.0f && m_zoom <= 45.0f)
        m_zoom -= yoffset;
    if (m_zoom <= 1.0f)
        m_zoom = 1.0f;
    if (m_zoom >= 45.0f)
        m_zoom = 45.0f;
}

float Camera::getZoom()
{
    return m_zoom;
}

glm::vec3 Camera::getPosition()
{
    return m_position;
}

glm::vec3 Camera::getFront()
{
    return m_front;
}

glm::vec3 Camera::getUp()
{
    return m_up;
}

glm::vec3 Camera::getRight()
{
    return m_right;
}

float Camera::getYaw()
{
    return m_yaw;
}

float Camera::getPitch()
{
    return m_pitch;
}