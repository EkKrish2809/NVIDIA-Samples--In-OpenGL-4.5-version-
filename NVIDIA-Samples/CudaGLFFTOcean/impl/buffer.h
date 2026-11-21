#ifndef BUFFER_H
#define BUFFER_H

void createVBO(GLuint *vbo, int size);

void deleteVBO(GLuint *vbo);

void createMeshIndexBuffer(GLuint *id, int w, int h);

void createMeshPositionVBO(GLuint *id, int w, int h);

#endif