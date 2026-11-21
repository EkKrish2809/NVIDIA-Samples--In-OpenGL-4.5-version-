#ifndef SCENE_H
#define SCENE_H

#ifdef __GNUC__
// X11 header (11 th version of 'X')
#include <X11/Xlib.h>
#include <X11/Xutil.h>  // XVisualInfo struct
#include <X11/XKBlib.h> // for KeyBoard input
#endif

class scene
{
public:
    
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual bool init() = 0;
    virtual void uninit() = 0;

    virtual void render_ui() = 0;
    virtual void process_events(XEvent) = 0;
    virtual void resize(int width, int height) = 0;
};

#endif // SCENE_H