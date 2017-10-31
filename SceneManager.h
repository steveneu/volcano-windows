#pragma once

#include <glad/glad.h>
#include "Point.h"
#include "Vec3.h"
#include "Matrix3x3.h"
#include "particle.h"
#include "ResizingArray.h"

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

static const unsigned short TRIANGLE = 0x1; // for sanity check
static const unsigned short OBELISK = 0x2;
static const unsigned short PARTICLES = 0x4;
static const unsigned short GROUND = 0x8;
static const unsigned short TEXTURE = 0x10;
static const unsigned short WIREFRAME = 0x20;        // draw wireframes, if available

static const char* TAG = "Particle";
static const int vCount = 48;              // number of values in vertex array for obelisk
										   // java equivalent is vertexdata.length
static const int particleCount = 200;
static const bool drawParticles = false;

const double M_PI = 3.14159265358979323;

class sceneManager {
	static const int mode_count = 3;
	int current_mode;
	unsigned short drawModes[mode_count];
	unsigned short drawFlags;
	GLuint mProgram_particles;
	GLuint mProgram_texmesh;

	GLint vertex_attrib_idx;
	GLint color_attrib_idx;
	GLint texture_coords_idx;
	GLint texture_uniform;
	GLint textureBool;
	GLint muMVPMatrixHandle;

	Matrix3x3 mMVPMatrix;
	Matrix3x3 mVMatrix;
	Matrix3x3 mProjMatrix;
	float vCameraPos[4]; // = new float[4];
	float vUpVector[4]; // = new float[4];
	float vCurrentUpVector[4]; // = new float[4];

	Matrix3x3 mOrientation;

	std::vector<AParticle> particles;
	int particles_per_sec;  // the mean number of particles generated per interval (second)

	int framesdrawn;        // tracks the frames drawn for fps calculation
	ULONGLONG intervalbegin;
	ULONGLONG updatetime;
	ULONGLONG lastParticleTime;    // the time the last particle was created

	int updates_second;
	int particle_life;
	int arraysize;
	GLuint textureUnit0;              // main texture unit for application

	Vec3 gravity;

	bool windowInitialized;
	bool debugLog;

	bool first_egl_query;

	float obeliskVertices[48];
	float colordata[4];

	ResizingArray<GLfloat>* volcanoComponents;
	ResizingArray<GLushort>* volcanoIndices;
	ResizingArray<GLfloat>* volcanoColors;
	ResizingArray<GLfloat>* volcanoTexComponents;

	ResizingArray<GLfloat>* groundComponents;
	ResizingArray<GLfloat>* groundTexComponents;
	ResizingArray<GLushort>* groundIndicesLines;
	ResizingArray<GLushort>* groundIndicesTriangles;
	ResizingArray<GLfloat>* groundColors;

	ResizingArray<GLfloat>* particleComponents;
	ResizingArray<GLushort>* particleIndicesLines;
	ResizingArray<GLushort>* particleIndicesPoints;
	ResizingArray<GLfloat>* particleColors;

	// convert indices of 3d array to absolute position of row major array
	inline void assign(GLubyte* array, int row, int col, int pos, GLubyte value, int column_count) {
		int idx = row * column_count + col + pos;
		array[idx] = value;
	}

	void setupVolcanoData();
	void setupGroundData();
	void setupParticleData();

	void drawVolcano();
	void drawVolcanoWithTexture();
	void drawGroundNoTexture();
	void drawGroundWithTexture();
	void drawParticles();

	void setTextureProgram();
	void setBasicProgram();
	inline bool flagSet(unsigned short flags, unsigned short bits) {
		if ((flags & bits) == bits)
			return true;
		else
			return false;
	}

	// helper functions
	ULONGLONG sceneManager::now_ms(void);
	float sceneManager::rnd_float(void);
	float sceneManager::toRadians(double degrees);

public:
	sceneManager();
	~sceneManager();
	void setIdentity(float* m);
	void changeLookAt();
	void toggleTouchdown();
	void storeOrientation(float* in);
	void printGLString(const char *name, GLenum s);
	void checkGlError(const char* op);
	void setLookAt(Matrix3x3& matrix,			// allocate this before calling with: matrix = new float[16];
		const Vec3& eye,
		const Vec3& target,
		const Vec3& upDir) const;
	void initTextures();
	void storeTexture(GLubyte* tex, int width, int height);
	GLuint loadShader(GLenum shaderType, const char* pSource);
	void getStringFromFile(const char* filename, char** source);
	void makePrograms(const char* vertexFn, const char* fragmentFn, const char* vertexTexFn, const char* fragmentTexFn);
	void createProgram(GLuint* program, const char* pVertexSource, const char* pFragmentSource);
	void createPrograms(const char* pVertexSource, const char* pFragmentSource,
		const char* pVertexTextureSrc, const char* pFragmentTextureSrc);
	void surfaceChanged(int w, int h);
	void drawFrame();
};
