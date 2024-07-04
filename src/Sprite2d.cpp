#include "Sprite2d.h"
#include "Shader.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include <string>

class Shader;

void Sprite2d::init(int w, int h) {
	/*create shaders*/
	loadShader();

    /*create vertices*/
    loadMesh();


    /*create and load textures*/
    loadTexture();
}

void Sprite2d::loadShader(){
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

}

void Sprite2d::loadMesh()
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

void Sprite2d::loadTexture()
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

	SetupTextureData(ourTexture, w, h, img.data);
	printf("Created a new texture with id = %d h = %d, w = %d\n", ourTexture, h, w);
}

void Sprite2d::updateUI(int w, int h) {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::SetNextWindowBgAlpha(0.5f);
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)


        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
}

void Sprite2d::run(float w, float h) {
    glViewport(0, 0, w, h);
    renderShader->Use();
    renderShader->UseAndBindTexture("ourTexture", ourTexture);
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

    updateUI(w, h);
}
