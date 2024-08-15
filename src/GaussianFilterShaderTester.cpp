#include "GaussianFilterShaderTester.h"
#include "Shader.h"
#include <numeric>
#include "Matrix.h"
#include "Vector.h"
#include "FileConfig.h"
#include "Common.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"

using MB::matrix4f;
using MB::vec3f;
using MB::vec2f;

void GaussianFilterShaderTester::init(float w, float h) {
    /*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/basic.vert");
    string fragShaderFile = resourceFolder + std::string("shaders/basic.frag");

    renderShader = std::make_shared<Shader>();
    renderShader->setShader(vertexShaderFile.c_str(), fragShaderFile.c_str());//geometryShaderFile.c_str()
    if (!renderShader->isValid()) {
        printf("failed to create shader: %s\n", vertexShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        vertexShaderFile.c_str(), renderShader->program);
    }

    //======compute shader type================
    string computeShaderFile = resourceFolder + std::string("shaders/gaussianBlur.comp");
    blurShader = std::make_shared<Shader>();
    blurShader->setComputeShader(computeShaderFile.c_str());
    if (!blurShader->isValid()) {
        printf("failed to create shader: %s\n", computeShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        computeShaderFile.c_str(), blurShader->program);
    }

    /*Init data that one of the shaders needs.  The form of data could be a texture,
    a ssbo buffer, etc. Usually we can classify the data into two types. One is the input, 
    the other is the output.
    */
    Size_X = 256;
    Size_Y = 1;
    Size_Z = 1;
    if (shaderType == 0)
        loadAssets0();
    else if(shaderType == 1)
        loadAssets1();

    //kernel data
    int kernelSize = 15;

    recalculateKernel(kernelSize, kernelSize, BOTH);
    
    //load textures
    loadTexture();
}
#define MAX_KERNEL_BUF 400*400
void GaussianFilterShaderTester::recalculateKernel(int w, int h, BLUR_DIR dir){
    kernel_W = w;
    kernel_H = h;
    int bufferSize = MAX_KERNEL_BUF;// 
    int dataSize = kernel_W * kernel_H;
    if (!kernelBuffer) {
        kernelBuffer = new float[MAX_KERNEL_BUF];
    }
     

    if(dir == VERTICAL){
        w = 2;
    }else if(dir == HORIZONTAL){
        h = 2;
    }
    int beginX = -w / 2;
    int beginY = -h / 2;
    float sum = 0.0;
    float sigma = 13.5;
    int size = w;  //or h
    for (int x = beginX; x < w/2; x++) {
        for (int y = beginY; y < h/2; y++) {
            kernelBuffer[(x + w/2)*h + y + h/2] = exp(-(x*x + y*y) / (2*sigma*sigma)) / (2*PI*sigma*sigma);
            sum += kernelBuffer[(x + w/2)*h + y + h/2];
        }
    }


  // Normalize the kernel
   for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            kernelBuffer[x*h + y] /= sum;
        }
    }

    //init kernel
    SetupSSBO(Filter_Kernel_Buffer, kernelBuffer, sizeof(float), bufferSize);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * dataSize, kernelBuffer, GL_STATIC_COPY);
    blurShader->Use(0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, Filter_Kernel_Buffer);
    blurShader->setUniform1i("kernel_W", kernel_W);
    blurShader->setUniform1i("kernel_H", kernel_H);

}


void GaussianFilterShaderTester::loadTexture()
{
	string name = "resource/images/car.jpg";
	cv::Mat img = cv::imread(name, cv::IMREAD_UNCHANGED);
	if (img.empty())
	{
		printf("failed on opening  texture image %s \n", name.c_str());
		return;
	}
	cv::cvtColor(img, img, cv::COLOR_RGB2BGRA);
	cv::Size s = img.size();  
	int h = s.height;
	int w = s.width;
    srcImgWidth = w;
    srcImgHeight = h;
	SetupTextureData(ourTexture, w, h, img.data);
	printf("Created a new texture with id = %d h = %d, w = %d\n", ourTexture, h, w);
	//SetupTexture(writeTexture, w, h, nullptr);
}

void GaussianFilterShaderTester::loadAssets0(){
    GLfloat vertices[] = {
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,    //RB       from the view direction of -z
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //RT 
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,    //LT
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f     //LB
	};
	GLuint indices[] = {
	0,1,3,            
	1,2,3             
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1 , &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); CHECK_GL;

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER , EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER , sizeof(indices) , indices , GL_STATIC_DRAW ); CHECK_GL;

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); CHECK_GL;

	// TexCoord attribute
	glVertexAttribPointer(1 , 2 , GL_FLOAT , GL_FALSE , 5*sizeof(GLfloat) , (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1); CHECK_GL;

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
	CHECK_GL;
}

void GaussianFilterShaderTester::loadAssets1() {
    
}

void GaussianFilterShaderTester::loadRenderState(){
	Config fileConfig;
    fileConfig.LoadConfig("resourceglshow.txt");

    
}

void GaussianFilterShaderTester::renderFrame0(int w, int h){
    recalculateKernel(kernel_W, kernel_H, VERTICAL);
    doCompute(ourTexture, writeTexture1, VERTICAL);
    recalculateKernel(kernel_W, kernel_H, HORIZONTAL);
    doCompute(writeTexture1, writeTexture2, HORIZONTAL);

	CHECK_GL;
    renderShader->Use();
    renderShader->UseAndBindTexture("ourTexture", writeTexture2);
	glBindVertexArray(VAO); CHECK_GL;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); CHECK_GL;
	GLuint positionHandle = 0;
	GLuint texcoordHandle = 1;
    glEnableVertexAttribArray(positionHandle);
    glEnableVertexAttribArray(texcoordHandle);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); CHECK_GL;

    glDisableVertexAttribArray(positionHandle);
	glDisableVertexAttribArray(texcoordHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GaussianFilterShaderTester::renderFrame1(int w, int h){
    matrix4f model;

   
}

void GaussianFilterShaderTester::doCompute(GLuint& inputTexture, GLuint& outputTexture, BLUR_DIR blurDir){
    
    int dim_x = 16;
	int dim_y = 16;
	static float time = 0.0;
	glBindTexture(GL_TEXTURE_2D, 0);
	blurShader->Use(0);CHECK_GL;
    blurShader->setUniform1i("kernel_W", kernel_W);CHECK_GL;
    blurShader->setUniform1i("kernel_H", kernel_H);CHECK_GL;
    blurShader->setUniform1i("dir", blurDir); CHECK_GL;
	//blurShader->setUniform1f("time", time);
	time += 0.01;
	//computeShader[threadid]->setUniform1i("threadid", threadid);
	SetupTexture(outputTexture, srcImgWidth, srcImgHeight);
    glBindImageTexture(1, inputTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI); CHECK_GL;
	glBindImageTexture(2, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI); CHECK_GL;
    glDispatchCompute((srcImgWidth + dim_x - 1) / dim_x, (srcImgHeight + dim_y - 1) / dim_y, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}

void GaussianFilterShaderTester::updateUI(int w, int h){
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::SetNextWindowBgAlpha(0.5f);
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static int counter = 0;

        ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.
        ImGui::SliderInt("kernel size W", &kernel_W, 1, 400);
        ImGui::SliderInt("kernel size H", &kernel_H, 1, 400); 

        ImGui::SameLine();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
}

void GaussianFilterShaderTester::run(float w, float h) {
    LoadConfig("");
    loadRenderState();
    glViewport(0, 0, w, h);
    if(shaderType == 0)
	    renderFrame0(w, h);
    else if(shaderType == 1){
        renderFrame1(w, h);
    }
    updateUI(w, h);
}
