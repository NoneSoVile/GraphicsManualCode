#include "Shader.h"
#include <numeric>
//#include "Matrix.h"
//#include "Vector.h"
#include "FileConfig.h"
#include "FluidField.h"
#include "BSpline.h"
#include "BezierCurve.h"

#include "imgui.h"


using MB::matrix4f;
using MB::vec3f;
using MB::vec2f;

void FluidField::init(int w, int h) {
	/*create shaders*/
	loadShader();

    /*create vertices*/
    loadMesh();


    /*create and load textures*/
    loadTexture();

    fluid2d = std::make_shared<Field2d>();
    fluid2d->init(w, h);
}

void FluidField::loadShader(){
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

void FluidField::loadMesh()
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

void FluidField::loadTexture()
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

void FluidField::updateUI(int w, int h) {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::SetNextWindowBgAlpha(0.5f);
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.
            // Display some text (you can use a format strings too)
		ImVec2 buttonSize(200, 60);
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        if (ImGui::Button("Exit", buttonSize))                            // Buttons return true when clicked (most widgets return true when edited/activated)
           exit(0);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
}

void FluidField::run(float w, float h) {
    fluid2d->run(w, h);
    GLuint velocityTexture = fluid2d->getVelocityTexture();
    GLuint pressureTexture = fluid2d->getPressureTexture();
    GLuint densityTexture = fluid2d->getDensityTexture();
    glViewport(0, 0, w, h);
    renderShader->Use();
    //renderShader->UseAndBindTexture("ourTexture", velocityTexture);
    //renderShader->UseAndBindTexture("ourTexture", pressureTexture);
    renderShader->UseAndBindTexture("ourTexture", densityTexture);
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

void Field2d::init(int w, int h)
{
    Size_X = 32;
    Size_Y = 32;
    Size_Z = 1;

    /*create shaders*/
	loadShader();

    /*create vertices*/
    loadMesh();


    /*create and load textures*/
    loadTexture();
}

void Field2d::run(float w, float h)
{
    static float dt = 0;
    dt += 0.1;
    for (size_t i = 0; i < GRID_X_DIM + 1; i++)
    {
        for (size_t j = 0; j < GRID_Y_DIM + 1; j++)
        {
            velocity[i][j] = vec2f(sin(dt*float(i)/ GRID_X_DIM), float(j) / GRID_Y_DIM);
        }

    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2f) * (GRID_X_DIM + 1) * (GRID_Y_DIM + 1), velocity, GL_DYNAMIC_DRAW);


    // Set up grid dimensions
    imageShader->Use();
    imageShader->setUniform1i("GRID_X_DIM", GRID_X_DIM);
    imageShader->setUniform1i("GRID_Y_DIM", GRID_Y_DIM);
    //imageShader->setUniform1i("PARTICLES_PER_GRID", PARTICLES_PER_GRID);

    // Run compute shader
    glDispatchCompute((GRID_X_DIM + 1 + Size_X) / Size_X, (GRID_Y_DIM + 1 + Size_Y) / Size_Y, 1); // Assuming 16x16 workgroups
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}

GLuint Field2d::getDensityTexture()
{
    return densityTexture;
}

GLuint Field2d::getVelocityTexture()
{
    return velocityTexture;
}

GLuint Field2d::getPressureTexture()
{
    return pressureTexture;
}

void Field2d::loadShader()
{
    string computeShaderFile = resourceFolder + std::string("shaders/fluidfield2d.comp");
    imageShader = std::make_shared<Shader>();
    imageShader->setComputeShader(computeShaderFile.c_str());
    if (!imageShader->isValid()) {
        printf("failed to create shader: %s\n", computeShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        computeShaderFile.c_str(), imageShader->program);
    }
}

void Field2d::loadMesh()
{
    for (size_t i = 0; i < GRID_X_DIM + 1; i++)
    {
        for (size_t j = 0; j < GRID_Y_DIM + 1; j++)
        {
            velocity[i][j] = vec2f(float(i)/GRID_X_DIM, float(j) / GRID_Y_DIM);
        }
        
    }

    for (size_t i = 0; i < GRID_X_DIM; i++)
    {
        for (size_t j = 0; j < GRID_Y_DIM; j++)
        {
            pressure[i][j] = vec2f(float(i) / GRID_X_DIM, float(j) / GRID_Y_DIM);
        }

    }

    for (size_t i = 0; i < MARKER_PARTICLES; i++)
    {
        markerParticles[i] = float(i) / MARKER_PARTICLES;
    }
    
    
}

void Field2d::loadTexture()
{
    work_groups_x = 32;

    glGenTextures(1, &velocityTexture);
    glBindTexture(GL_TEXTURE_2D, velocityTexture);
    SetupTexture_rgba32(velocityTexture, GRID_X_DIM + 1, GRID_Y_DIM + 1);
    glBindImageTexture(0, velocityTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glGenTextures(1, &pressureTexture);
    glBindTexture(GL_TEXTURE_2D, pressureTexture);
    SetupTexture(pressureTexture, GRID_X_DIM , GRID_Y_DIM );
    glBindImageTexture(1, pressureTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI);

    glGenTextures(1, &densityTexture);
    glBindTexture(GL_TEXTURE_2D, densityTexture);
    SetupTexture_rgba32(densityTexture, GRID_X_DIM, GRID_Y_DIM );
    glBindImageTexture(2, densityTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Create and populate buffer objects for velocity, pressure, and particles
    glGenBuffers(1, &velocityBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2f) * (GRID_X_DIM + 1) * (GRID_Y_DIM + 1), velocity, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, velocityBuffer);

    glGenBuffers(1, &pressureBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pressureBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2f) * GRID_X_DIM * GRID_Y_DIM, pressure, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pressureBuffer);

    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2f) * MARKER_PARTICLES, markerParticles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, particleBuffer); 

    imageShader->Use();
    imageShader->setUniform1i("GRID_X_DIM", GRID_X_DIM);
    imageShader->setUniform1i("GRID_Y_DIM", GRID_Y_DIM);
    //imageShader->setUniform1i("PARTICLES_PER_GRID", PARTICLES_PER_GRID);
}
