#ifndef SHADER_H
#define SHADER_H

int attachShader(GLuint prg, GLenum type, const char* name, const char* typeName);

GLuint loadGLSLProgram(const char* vertFileName, const char* fragFileName);

#endif
