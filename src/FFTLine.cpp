#include "FFTLine.h"
#include "Shader.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include <string>

class Shader;

void FFTLine::loadShader(){
     /*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/basic330.vert");
    string fragShaderFile = resourceFolder + std::string("shaders/ddaline_anti_aliasing4.frag");

    renderShader = std::make_shared<Shader>();
    renderShader->setShader(vertexShaderFile.c_str(), fragShaderFile.c_str());//geometryShaderFile.c_str()
    if (!renderShader->isValid()) {
        printf("failed to create shader: %s\n", vertexShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        vertexShaderFile.c_str(), renderShader->program);
    }

	vertexShaderFile = resourceFolder + std::string("shaders/basic.vert");
    fragShaderFile = resourceFolder + std::string("shaders/basic.frag");

    imageShader = std::make_shared<Shader>();
    imageShader->setShader(vertexShaderFile.c_str(), fragShaderFile.c_str());//geometryShaderFile.c_str()
    if (!imageShader->isValid()) {
        printf("failed to create shader: %s\n", vertexShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        vertexShaderFile.c_str(), imageShader->program);
    }

	//======compute shader type================
    string computeShaderFile = resourceFolder + std::string("shaders/FFT2D_Horizontal.comp");
    fftShader_horizontal = std::make_shared<Shader>();
    fftShader_horizontal->setComputeShader(computeShaderFile.c_str());
    if (!fftShader_horizontal->isValid()) {
        printf("failed to create shader: %s\n", computeShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        computeShaderFile.c_str(), fftShader_horizontal->program);
    }

	computeShaderFile = resourceFolder + std::string("shaders/FFT2D_Vertical.comp");
    fftShader_vertical = std::make_shared<Shader>();
    fftShader_vertical->setComputeShader(computeShaderFile.c_str());
    if (!fftShader_vertical->isValid()) {
        printf("failed to create shader: %s\n", computeShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        computeShaderFile.c_str(), fftShader_vertical->program);
    }

	computeShaderFile = resourceFolder + std::string("shaders/IFFT2D_Vertical.comp");
    ifftShader_vertical = std::make_shared<Shader>();
    ifftShader_vertical->setComputeShader(computeShaderFile.c_str());
    if (!ifftShader_vertical->isValid()) {
        printf("failed to create shader: %s\n", computeShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        computeShaderFile.c_str(), ifftShader_vertical->program);
    }

	computeShaderFile = resourceFolder + std::string("shaders/IFFT2D_Horizontal.comp");
    ifftShader_horizontal = std::make_shared<Shader>();
    ifftShader_horizontal->setComputeShader(computeShaderFile.c_str());
    if (!ifftShader_horizontal->isValid()) {
        printf("failed to create shader: %s\n", computeShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        computeShaderFile.c_str(), ifftShader_horizontal->program);
    }

}

void FFTLine::init(int w, int h) {
	u_point1 = vec2f(10, 100);
	u_point2 = vec2f(300, 100);
	u_resolution = vec2f(w, h);
	u_lineWidth = 10.0;
     /*create shaders
    */
    loadShader();

    /*create vertices*/
    loadMesh();


    /*create and load textures*/
    loadTexture();
}

void FFTLine::loadMesh()
{
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

void FFTLine::loadTexture()
{
    string name = "resource/images/steelball.png";
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
}

void FFTLine::updateUI(int w, int h) {
	//updateLinesUI(w, h);
	updateTextureUI(w, h);
}

void FFTLine::updateLinesUI(int w, int h){
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground); // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("u_lineWidth", &u_lineWidth, 1.0f, 1350.0f);																				  // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::SliderFloat2("u_point1", u_point1._array, 15.0f, 2850.0f);
	ImGui::SliderFloat2("u_point2", u_point2._array, 15, 2500);
	ImGui::SliderFloat4("u_lineColor", u_lineColor._array, 0, 1);
	ImGui::SliderFloat4("u_bgColor", u_bgColor._array, 0, 1);
	ImGui::SliderFloat("fadeRangeFactor", &fadeRangeFactor, 0.f, 1.0f);
	ImGui::SliderFloat("edgeSoftness", &edgeSoftness, .0f, 1.0f);
	ImGui::SameLine();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
}

#define SLIDER_FLOAT(id) \
    ImGui::SliderFloat("lowFreq" #id, &lowFreq##id, 0.f, 1.0f); \
    ImGui::SliderFloat("highFreq" #id, &highFreq##id, 0.f, 1.0f)

void FFTLine::updateTextureUI(int w, int h){
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground); // Create a window called "Hello, world!" and append into it.
	
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	if(ImGui::Button("gray fft opencv")){
		grayfft();
	}

    if (ImGui::Button("color fft opencv")) {
        colorFFT();
    }
    SLIDER_FLOAT(1);
    SLIDER_FLOAT(2);
    SLIDER_FLOAT(3);
    SLIDER_FLOAT(4);

    ImGui::SliderFloat("sigma", &sigma, 0.f, 100.0f);

    if (ImGui::Button("reset band pass filter")) {
        resetBandPassFilter();
    }
    if (ImGui::Button("reset band stop filter")) {
        resetBandStopFilter();
    }


	ImGui::End();
}
#define RESET_FILTER(id, lowval, highval) lowFreq##id = lowval; highFreq##id = highval;
void FFTLine::resetBandStopFilter() {
    RESET_FILTER(1, -1, -1);
    RESET_FILTER(2, -1, -1);
    RESET_FILTER(3, -1, -1);
    RESET_FILTER(4, -1, -1);
}

void FFTLine::resetBandPassFilter() {
    RESET_FILTER(1, 0, 1);
    RESET_FILTER(1, 0, 1);
    RESET_FILTER(3, 0, 1);
    RESET_FILTER(4, 0, 1);
}

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Function to create a band-pass filter mask
Mat createBandPassFilter(Size size, float lowFreq, float highFreq) {
    Mat filter(size, CV_32F, Scalar::all(0));

    Point center = Point(size.width / 2, size.height / 2);
    float maxDist = sqrt(center.x * center.x + center.y * center.y);

    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            float distance = sqrt(pow(j - center.x, 2) + pow(i - center.y, 2)) / maxDist;
            if (distance >= lowFreq && distance <= highFreq) {
                filter.at<float>(i, j) = 1.0f;
            }
        }
    }

    return filter;
}

Mat createBandStopFilter(Size size, float lowFreq, float highFreq) {
    Mat filter(size, CV_32F, Scalar::all(1)); // Start with all frequencies allowed

    Point center = Point(size.width / 2, size.height / 2);
    float maxDist = sqrt(center.x * center.x + center.y * center.y);

    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            float distance = sqrt(pow(j - center.x, 2) + pow(i - center.y, 2)) / maxDist;
            if (distance >= lowFreq && distance <= highFreq) {
                filter.at<float>(i, j) = 0.0f; // Block frequencies within the band
            }
        }
    }

    return filter;
}

// Function to create a Gaussian filter mask in the frequency domain
Mat createGaussianFilter(Size size, float radius, float sigma) {
    Mat filter(size, CV_32F, Scalar::all(1));

    Point center = Point(size.width / 2, size.height / 2);

    for (int i = 0; i < radius*size.height; ++i) {
        for (int j = 0; j < radius*size.width; ++j) {
            float distance = pow(j - center.x, 2) + pow(i - center.y, 2);
            filter.at<float>(i, j) = exp(-distance / (2.0 * sigma * sigma));
        }
    }

    // Normalize the filter to ensure the sum of all weights is 1
    normalize(filter, filter, 0, 1, NORM_MINMAX);

    return filter;
}

#define APPLY_FILTER(filter) multiply(planes[0], filter, planes[0]);\
                            multiply(planes[1], filter, planes[1]);

#define CREATE_BAND_PASS_FILTER(size, id)  createBandPassFilter(size, lowFreq##id, highFreq##id);
#define CREATE_BAND_STOP_FILTER(size, id)  createBandStopFilter(size, lowFreq##id, highFreq##id);


int FFTLine::grayfft() {
    // Load the input image
    string name = "resource/images/bathroom.jpg";
    Mat image = imread(name, IMREAD_GRAYSCALE);//IMREAD_GRAYSCALE
    if (image.empty()) {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }

    // Expand the image to an optimal size
    Mat padded;
    int m = getOptimalDFTSize(image.rows);
    int n = getOptimalDFTSize(image.cols); // on the border add zero values
    copyMakeBorder(image, padded, 0, m - image.rows, 0, n - image.cols, BORDER_CONSTANT, Scalar::all(0));

    // Create planes to hold the complex image
    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexImage;
    merge(planes, 2, complexImage);

    // Perform DFT
    dft(complexImage, complexImage);
    Size size = complexImage.size();
    Mat filter1 = CREATE_BAND_STOP_FILTER(size, 1);
    Mat filter2 = CREATE_BAND_STOP_FILTER(size, 2);
    Mat filter3 = CREATE_BAND_STOP_FILTER(size, 3);
    Mat filter4 = CREATE_BAND_STOP_FILTER(size, 4); //createGaussianFilter(size, 1, sigma);//

    // Split the complex image into real and imaginary parts
    split(complexImage, planes);

    // Apply the filter to both the real and imaginary parts
    APPLY_FILTER(filter1);
    APPLY_FILTER(filter2);
    APPLY_FILTER(filter3);
    APPLY_FILTER(filter4);

    // Merge the real and imaginary parts back together
    merge(planes, 2, complexImage);

    // Perform the inverse DFT
    idft(complexImage, complexImage);

    // Compute the magnitude (real part is the image)
    split(complexImage, planes);
    magnitude(planes[0], planes[1], planes[0]);
    Mat magImage = planes[0];

    // Crop the image to the original size
    magImage = magImage(Rect(0, 0, image.cols, image.rows));

    // Normalize the magnitude image for better visualization
    normalize(magImage, magImage, 0, 1, NORM_MINMAX);

    // Display the result
    imshow("Filtered Image", magImage);
    //waitKey(0);

    return 0;
}

// Function to perform FFT on a single channel
void performFFT(Mat& channel, Mat& complexImage) {
    Mat planes[] = { Mat_<float>(channel), Mat::zeros(channel.size(), CV_32F) };
    merge(planes, 2, complexImage);

    dft(complexImage, complexImage);
}

// Function to perform inverse FFT on a single channel
void performInverseFFT(Mat& complexImage, Mat& channel) {
    idft(complexImage, complexImage);

    Mat planes[2];
    split(complexImage, planes);
    magnitude(planes[0], planes[1], channel);
}

// Function to apply a band-pass filter in the frequency domain
void applyBandPassFilter(Mat& complexImage, const Mat& filter) {
    Mat planes[2];
    split(complexImage, planes);

    multiply(planes[0], filter, planes[0]);
    multiply(planes[1], filter, planes[1]);

    merge(planes, 2, complexImage);
}

#define APPLY_BAND_FILTER_COLOR(id)     applyBandPassFilter(complexImageR, filter##id);\
                                            applyBandPassFilter(complexImageG, filter##id);\
                                                applyBandPassFilter(complexImageB, filter##id);


int FFTLine::colorFFT() {
    // Load the input image
    Mat image = imread("resource/images/bathroom.jpg");
    if (image.empty()) {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }

    // Split the image into R, G, B channels
    vector<Mat> channels;
    split(image, channels);

    // Perform FFT on each channel
    Mat complexImageR, complexImageG, complexImageB;
    performFFT(channels[0], complexImageR); // Red channel
    performFFT(channels[1], complexImageG); // Green channel
    performFFT(channels[2], complexImageB); // Blue channel

    Size size = complexImageR.size();

    //Mat filter = createBandPassFilter(complexImageR.size(), lowFreq, highFreq);
    Mat filter1 = CREATE_BAND_STOP_FILTER(size, 1);
    Mat filter2 = CREATE_BAND_STOP_FILTER(size, 2);
    Mat filter3 = CREATE_BAND_STOP_FILTER(size, 3);
    Mat filter4 = CREATE_BAND_STOP_FILTER(size, 4); //createGaussianFilter(size, 1, sigma);//

    // Apply the band-pass filter to each channel in the frequency domain
    APPLY_BAND_FILTER_COLOR(1);
    APPLY_BAND_FILTER_COLOR(2);
    APPLY_BAND_FILTER_COLOR(3);
    APPLY_BAND_FILTER_COLOR(4);
    // Perform inverse FFT on each channel
    performInverseFFT(complexImageR, channels[0]);
    performInverseFFT(complexImageG, channels[1]);
    performInverseFFT(complexImageB, channels[2]);

    // Merge the channels back together
    Mat filteredImage;
    merge(channels, filteredImage);

    // Normalize the result for display
    normalize(filteredImage, filteredImage, 0, 1, NORM_MINMAX);

    // Display the result
    imshow("Filtered Image", filteredImage);
    return 0;
}


void FFTLine::drawTexture(int w, int h)
{
	doCompute(fftShader_horizontal, ourTexture, writeTexture1, HORIZONTAL);
	doCompute(fftShader_vertical, writeTexture1, writeTexture2, VERTICAL);
	doCompute(ifftShader_vertical, writeTexture2, writeTexture3, VERTICAL);
	doCompute(ifftShader_horizontal, writeTexture3, writeTexturefinal, HORIZONTAL);
	imageShader->Use();
	imageShader->UseAndBindTexture("ourTexture", writeTexturefinal);
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

void FFTLine::drawLines(int w, int h){
	renderShader->Use();
    //renderShader->UseAndBindTexture("ourTexture", ourTexture);
	renderShader->setUniform2fv("u_point1", u_point1, 1);
	renderShader->setUniform2fv("u_point2", u_point2, 1);
	renderShader->setUniform2fv("u_resolution", u_resolution, 1);
	renderShader->setUniform1f("u_lineWidth", u_lineWidth);
	renderShader->setUniform1f("fadeRangeFactor", fadeRangeFactor);
	renderShader->setUniform1f("edgeSoftness", edgeSoftness);
	renderShader->setUniform4fv("u_lineColor", u_lineColor, 1);
	renderShader->setUniform4fv("u_bgColor", u_bgColor, 1);
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

void FFTLine::run(float w, float h) {
	
    glViewport(0, 0, w, h);
	//drawTexture(w, h);
    //drawLines(w, h);

    updateUI(w, h);
}

void FFTLine::doCompute(std::shared_ptr<Shader> shader, GLuint& inputTexture, GLuint& outputTexture, FFT_DIR dir){
    int dim_x = 64;
	int dim_y = 1;
	if (dir == VERTICAL) {
		dim_x = 1;
		dim_y = 64;
	}
	static float time = 0.0;
	glBindTexture(GL_TEXTURE_2D, 0);
	shader->Use(0);

	SetupTexture(outputTexture, srcImgWidth, srcImgHeight);
    glBindImageTexture(1, inputTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI); CHECK_GL;
	glBindImageTexture(2, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI); CHECK_GL;
    glDispatchCompute((srcImgWidth + dim_x - 1) / dim_x, (srcImgHeight + dim_y - 1) / dim_y, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
