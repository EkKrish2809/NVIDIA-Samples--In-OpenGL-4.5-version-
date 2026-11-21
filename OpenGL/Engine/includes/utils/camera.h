#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <GL/gl.h>

class Camera
{
public:
    enum CameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    float YAW = -90.0f;
    float PITCH = 0.0f;
    float SPEED = 2.5f;
    float SENSITIVITY = 0.1f;
    float ZOOM = 45.0f;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
    {
        m_position = position;
        m_worldUp = up;
        m_yaw = yaw;
        m_pitch = pitch;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
    {
        m_position = glm::vec3(posX, posY, posZ);
        m_worldUp = glm::vec3(upX, upY, upZ);
        m_yaw = yaw;
        m_pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix();

    glm::mat4 getProjectionMatrix();

    void processKeyboard(char key, float deltaTime);

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);

    void processMouseScroll(float yoffset);

    void updateResolution(float _width, float _height);

    float getZoom();

    glm::vec3 getPosition();

    glm::vec3 getFront();

    glm::vec3 getUp();

    glm::vec3 getRight();

    float getYaw();

    float getPitch();

private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

    float m_width;
    float m_height;

    float lastX = 0.0;
    float lastY = 0.0;
    bool firstMouse = true;

    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

    float degToRad(float degrees)
    {
        return (degrees * M_PI / 180.0);
    }
    
};

extern Camera debugCamera;;

#endif // CAMERA_H_