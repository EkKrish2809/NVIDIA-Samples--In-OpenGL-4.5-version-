#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>

inline const char* glErrorToString(GLenum err) {
    switch (err) {
        case GL_NO_ERROR:                       return "GL_NO_ERROR";
        case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:                 return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:                return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST:                   return "GL_CONTEXT_LOST";
        case GL_TABLE_TOO_LARGE:                return "GL_TABLE_TOO_LARGE";
        default:                                return "UNKNOWN";
    }
}

inline bool sdkCheckErrorGL(const char *file, const int line)
    {
        bool ret_val = true;

        // check for error
        GLenum gl_error = glGetError();

        if (gl_error != GL_NO_ERROR)
        {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
            char tmpStr[512];
            // NOTE: "%s(%i) : " allows Visual Studio to directly jump to the file at the right line
            // when the user double clicks on the error line in the Output pane. Like any compile error.
            sprintf_s(tmpStr, 255, "\n%s(%i) : GL Error : %s\n\n", file, line, glErrorToString(gl_error));
            fprintf(stderr, "%s", tmpStr);
#endif
            fprintf(stderr, "GL Error in file '%s' in line %d :\n", file, line);
            fprintf(stderr, "%s\n", glErrorToString(gl_error));
            ret_val = false;
        }

        return ret_val;
    }

#define SDK_CHECK_ERROR_GL()                                              \
    if( false == sdkCheckErrorGL( __FILE__, __LINE__)) {                  \
        exit(EXIT_FAILURE);                                               \
    }       

#endif