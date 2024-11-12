#pragma once
#include "Common.h"
#include "ShaderTester.h"
#include "RenderState.h"
#include "CubicSpline.h"
#include "NvMath.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include <string>
using std::string;
using MB::vec2f;
using MB::vec2i;
using MB::matrix4f;
using MB::matrix3f;
using MB::quaternionf;
class Shader;
#define MAX_RESULT_LEN 10


#define GRID_X_DIM 200
#define GRID_Y_DIM 200
#define GRID_SIZE_X 10    //PIXELS
#define GRID_SIZE_Y 10    //PIXELS

#define PARTICLES_PER_GRID 4
#define MARKER_PARTICLES (GRID_X_DIM*GRID_Y_DIM*PARTICLES_PER_GRID)

#define TIME_STEP 10


class Field2d: public ShaderTester{

public:
    enum GRID_CONTENT_TYPE{
        EMPTY = 0,
        SURFACE,
        FULL
    };
    vec2f velocity[GRID_X_DIM + 1][GRID_Y_DIM + 1];
    vec2f pressure[GRID_X_DIM][GRID_Y_DIM];
    GRID_CONTENT_TYPE gridTypes[GRID_X_DIM][GRID_Y_DIM];
    vec2f markerParticles[MARKER_PARTICLES];

    //the size of workgroup
    uint Size_X;
    uint Size_Y;
    uint Size_Z;

    uint work_groups_x;
    uint work_groups_y;
    uint work_groups_z;

public:
    virtual void init(int w, int h);
    virtual void run(float w, float h);

    GLuint getDensityTexture();
    GLuint getVelocityTexture();
    GLuint getPressureTexture();

protected:
    virtual void loadShader();
    virtual void loadMesh();
    virtual void loadTexture();

    void apply_forces(vec2f velocity[GRID_X_DIM + 1][GRID_Y_DIM + 1], vec2f gravity, float dt);
    void advect(float velocity[GRID_X_DIM + 1][GRID_Y_DIM + 1], float dt);
    void diffuse(float velocity[GRID_X_DIM + 1][GRID_Y_DIM + 1], float viscosity, float dt);
    void project(float velocity[GRID_X_DIM + 1][GRID_Y_DIM + 1]);
    void update_particles(float velocity[GRID_X_DIM + 1][GRID_Y_DIM + 1], vec2f gravity, float dt);

    std::shared_ptr<Shader> imageShader;
    // Define textures for the output color maps
    GLuint velocityTexture, pressureTexture, densityTexture;
    GLuint velocityBuffer, pressureBuffer, particleBuffer;
};

class FluidField : public ShaderTester
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

    std::shared_ptr<Field2d> fluid2d;

protected:
    virtual void loadShader();
    virtual void loadMesh();
    virtual void loadTexture();

    virtual void updateUI(int w, int h);

public:
    virtual void init(int w, int h);
    virtual void run(float w, float h);
};

