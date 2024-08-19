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
    enum FFT_DIR {
        VERTICAL = 0,
        HORIZONTAL ,
        BOTH
    };
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

    int srcImgWidth = 0;
    int srcImgHeight = 0;
protected:
    std::shared_ptr<Shader> renderShader;
    std::shared_ptr<Shader> imageShader;
    std::shared_ptr<Shader> fftShader_horizontal;
    std::shared_ptr<Shader> fftShader_vertical;
    std::shared_ptr<Shader> ifftShader_horizontal;
    std::shared_ptr<Shader> ifftShader_vertical;
    GLuint VBO;
	GLuint VAO;
	GLuint EBO;

    GLuint writeTexture1 = 0;
    GLuint ourTexture = 0;
    GLuint writeTexture2 = 0;
    GLuint writeTexture3 = 0;
    GLuint writeTexturefinal = 0;

    // Create a band-pass filter mask
    float lowFreq1 = 0.f;  // Low-frequency cutoff (e.g., 10% of the max frequency)
    float highFreq1= 1.f; // High-frequency cutoff (e.g., 50% of the max frequency)


    float lowFreq2 = 0.f;  // Low-frequency cutoff (e.g., 10% of the max frequency)
    float highFreq2 = 1.f; // High-frequency cutoff (e.g., 50% of the max frequency)

    float lowFreq3 = 0.f;  // Low-frequency cutoff (e.g., 10% of the max frequency)
    float highFreq3 = 1.f; // High-frequency cutoff (e.g., 50% of the max frequency)

    float lowFreq4 = 0.f;  // Low-frequency cutoff (e.g., 10% of the max frequency)
    float highFreq4 = 1.f; // High-frequency cutoff (e.g., 50% of the max frequency)

    float sigma = 1;
    int kernelSize = 79;  // Example large kernel size

protected:
    void loadShader();
    void loadMesh();
    void loadTexture();

    void updateUI(int w, int h);
    void updateLinesUI(int w, int h);
    void updateTextureUI(int w, int h);
    void drawLines(int w, int h);
    void drawTexture(int w, int h);

    void doCompute(std::shared_ptr<Shader> shader, GLuint& inputTexture, GLuint& outputTexture, FFT_DIR dir);
    int  grayfft();
    int  colorFFT();
    int gaussfilterGrayscale();
    int gaussfilterRGB();

    void resetBandStopFilter();
    void resetBandPassFilter();
public:
    void init(int w, int h);
    void run(float w, float h);


};