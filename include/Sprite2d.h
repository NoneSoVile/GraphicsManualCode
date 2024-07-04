#pragma once
#include "Common.h"
#include "ShaderTester.h"
#include <string>
#include "NvMath.h"
using MB::vec2i;

class Shader;
class Sprite2d: public ShaderTester
{
public:
//locate the sprite2d in screen view coordinate
    int pixelWidth, pixHeight;
    vec2i pixelPosition;

protected:
    std::shared_ptr<Shader> renderShader;
    GLuint VBO;
	GLuint VAO;
	GLuint EBO;

    GLuint writeTexture = 0;
    GLuint ourTexture = 0;

protected:
    virtual void loadShader();
    virtual void loadMesh();
    virtual void loadTexture();

    virtual void updateUI(int w, int h);

public:
    virtual void init(int w, int h);
    virtual void run(float w, float h);


};