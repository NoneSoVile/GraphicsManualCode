#pragma once
#include "Common.h"
#include "ShaderTester.h"
#include "RenderState.h"
#include <string>
using std::string;
class Shader;

class GaussianFilterShaderTester : public ShaderTester

{
protected:

    enum BLUR_DIR {
        VERTICAL = 0,
        HORIZONTAL ,
        BOTH
    };

    int shaderType = 0;
    std::shared_ptr<Shader> blurShader;
    std::shared_ptr<Shader> renderShader;
    GLuint VBO;
	GLuint VAO;
	GLuint EBO;

    GLuint Filter_Kernel_Buffer;
    int kernel_W;
    int kernel_H;
    float* kernelBuffer = nullptr;


    GLuint writeTexture1 = 0;
    GLuint ourTexture = 0;
    GLuint writeTexture2 = 0;
    int srcImgWidth = 0;
    int srcImgHeight = 0;
    //render states



    void loadAssets0();
    void loadAssets1();
    void loadRenderState();
    void loadTexture();

    void renderFrame0(int w, int h);
    void renderFrame1(int w, int h);

    void doCompute(GLuint& inputTexture, GLuint& outputTexture, BLUR_DIR blurDir);
    void recalculateKernel(int w, int h, BLUR_DIR blurDir);
    void updateUI(int w, int h);
public:
    void init(float w, float h);
    void run(float w, float h);
};

