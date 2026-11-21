#ifndef COMMON_H
#define COMMON_H

// IMGUI header files
#include "includes/imgui/imgui.h"
#include "includes/imgui/imgui_impl_opengl3.h"
#ifdef __GNUC__
#include "includes/imgui/imgui_impl_x11.h"
#else
#include "includes/imgui/imgui_impl_win32.h"
#endif

// #include "Logger.h"

// logger g_logger;

#endif // COMMON_H