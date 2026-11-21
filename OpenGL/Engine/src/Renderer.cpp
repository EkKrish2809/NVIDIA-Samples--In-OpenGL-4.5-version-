#include "Renderer.h"

Renderer::Renderer()
{
    m_scene = nullptr;
}

Renderer::~Renderer()
{
    if (m_scene)
    {
        delete m_scene;
        m_scene = nullptr;
    }
}

void Renderer::render()
{
    m_scene->update();
    m_scene->draw();
}

void Renderer::process_events(XEvent event)
{
    m_scene->process_events(event);

}

void Renderer::render_ui()
{
    m_scene->render_ui();
}

bool Renderer::init()
{
    m_scene = new first_scene();
    m_scene->init();

    return true;
}

void Renderer::uninit()
{
}

