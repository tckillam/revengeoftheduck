#pragma once
#include "Vectors.h"
#include <SOIL.h>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

class CubeMesh {
private:
    GLfloat(*vertices)[3];
    GLfloat(*quadNormals)[3];
    GLubyte(*quads)[4];

    float mat_ambient[4];
    float mat_specular[4];
    float mat_diffuse[4];
    float mat_shininess[1];

    GLuint boothTexture;
    GLuint LoadTexture(const std::string& file);

public:
    CubeMesh();
    void drawCubeMesh();
    void setMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular, double shininess);
};