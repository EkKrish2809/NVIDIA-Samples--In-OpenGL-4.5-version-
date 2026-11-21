
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "shader.h"

extern FILE* gpFile;

// Attach shader to a program
int attachShader(GLuint prg, GLenum type, const char *name, const char* typeName)
{
    GLuint shader;
    FILE  *fp;
    int    size, compiled;
    char  *src;

    fp = fopen(name, "rb");

    if (!fp) {
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    src  = (char *)malloc(size);

    fseek(fp, 0, SEEK_SET);
    fread(src, sizeof(char), size, fp);
    fclose(fp);

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char **)&src, (const GLint *)&size);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, (GLint *)&compiled);

    if (!compiled) {
        char log[2048];
        int  len;

        glGetShaderInfoLog(shader, 2048, (GLsizei *)&len, log);
        printf("%s Shader Info log: %s\n", typeName, log);
        glDeleteShader(shader);
        return 0;
    } else {
        fprintf(gpFile, "\n%s Shader Compilation Completed\n", typeName);
    }

    free(src);

    glAttachShader(prg, shader);
    glDeleteShader(shader);

    return 1;
}

// Create shader program from vertex shader and fragment shader files
GLuint loadGLSLProgram(const char *vertFileName, const char *fragFileName)
{
    GLint  linked;
    GLuint program;

    program = glCreateProgram();

    if (!attachShader(program, GL_VERTEX_SHADER, vertFileName, "Vertex")) {
        glDeleteProgram(program);
        fprintf(stderr, "Couldn't attach vertex shader from file %s\n", vertFileName);
        return 0;
    }

    if (!attachShader(program, GL_FRAGMENT_SHADER, fragFileName, "Fragment")) {
        glDeleteProgram(program);
        fprintf(stderr, "Couldn't attach fragment shader from file %s\n", fragFileName);
        return 0;
    }

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        glDeleteProgram(program);
        char temp[256];
        glGetProgramInfoLog(program, 256, 0, temp);
        fprintf(stderr, "Failed to link program: %s\n", temp);
        return 0;
    }

    return program;
}