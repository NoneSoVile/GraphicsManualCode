#pragma once
#include "Common.h"
#include "Sprite2d.h"
#include <string>
#include "NvMath.h"
using MB::vec2i;
using MB::vec2f;
using MB::vec4f;
class Shader;
class FFTLine: public Sprite2d
{
public:
//locate the sprite2d in screen view coordinate
    int pixelWidth, pixHeight;
    vec2i pixelPosition;

    vec2f u_point1;
    vec2f u_point2;
    vec2f u_resolution;
    float u_lineWidth;
    vec4f u_lineColor = vec4f(1.0, 1.0, 0.0, 1.0);  // The color of the line
    vec4f u_bgColor = vec4f(0.0, 0.0, 0.0, 1.0);   // The background color
    float fadeRangeFactor = 0.2;
    float edgeSoftness = 0.02; // Adjust this value for the desired softness at the ends
protected:
    std::shared_ptr<Shader> renderShader;
    GLuint VBO;
	GLuint VAO;
	GLuint EBO;

    GLuint writeTexture = 0;
    GLuint ourTexture = 0;

protected:
    void loadShader();
    void loadMesh();
    void loadTexture();

    void updateUI(int w, int h);

public:
    void init(int w, int h);
    void run(float w, float h);


};