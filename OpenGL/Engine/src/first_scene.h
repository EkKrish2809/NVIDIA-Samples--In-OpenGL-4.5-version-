#ifndef FIRST_SCENE_H
#define FIRST_SCENE_H

#include "scene.h"
#include "../common.h"

#include "../includes/utils/shader_helper.h"
#include "../includes/utils/camera.h"

#ifdef __GNUC__
// X11 header (11 th version of 'X')
#include <X11/Xlib.h>
#include <X11/Xutil.h>  // XVisualInfo struct
#include <X11/XKBlib.h> // for KeyBoard input
#endif

class first_scene : public scene
{
public:
    first_scene();
    ~first_scene();

    void update();
    void draw();
    bool init();
    void uninit();

    void render_ui();
    void process_events(XEvent event);
    void resize(int width, int height);

private:
    ShaderHelper m_shaderHelper;

    GLuint m_vbo;
    GLuint m_vao;

    GLuint m_modelMatrix;
    GLuint m_viewMatrix;
    GLuint m_projectionMatrix;
};
#endif // FIRST_SCENE_H