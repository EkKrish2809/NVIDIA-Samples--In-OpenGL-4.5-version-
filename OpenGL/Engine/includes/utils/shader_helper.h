#ifndef SHADER_HELPER_H_
#define SHADER_HELPER_H_

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __GNUC__
#include <string.h>
#else
#include <string>
#endif
#include <vector>
#include <stdarg.h>
#include <GL/glew.h>

#include "../../Logger.h"

enum ShaderType
{
    VERTEX_SHADER = 0,
    FRAGMENT_SHADER ,
    GEOMETRY_SHADER ,
    TESS_CONTROL_SHADER ,
    TESS_EVALUATION_SHADER ,
    COMPUTE_SHADER 
};

class ShaderHelper
{
public:
    ShaderHelper();
    ~ShaderHelper();

    GLuint loadShader(const char *, GLenum);
    GLuint createShaderProgram(std::vector<GLuint>);
    bool linkShaderProgram();
    void useShaderProgram();
    void unuseShaderProgram();
    GLuint getUniformLocation(const char *);

private:
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_shaderProgram;
    GLuint m_shaderObject;
};

#endif // SHADER_HELPER_H_