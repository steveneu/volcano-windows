#pragma once

#include <glad/glad.h>
#include "Point.h"
#include "Vec3.h"
#include "Matrix3x3.h"
#include "particle.h"
#include "ResizingArray.h"
#include "types.h"

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;



static const char* TAG = "Particle";
static const int vCount = 48;              // number of values in vertex array for obelisk
										   // java equivalent is vertexdata.length
static const int particleCount = 200;
static const bool drawParticles = false;

const float M_PI = 3.14159265f;// 358979323;

class sceneManager {
	static const int mode_count = 3;
	int current_mode;
	unsigned short drawModes[mode_count];
	unsigned short drawFlags;
	unsigned short debugPrintFlags;
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
	Matrix3x3 mLookAtMat;
	Vec3 cameraPos;
	Vec3 upVector;
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

	bool first_egl_query;

	float obeliskVertices[48];
	float colordata[4];

	GLuint vao;
	GLuint vbo_sanity;
	GLuint vbo_particles;
	GLuint vbo_ground;
	GLuint vbo_volcano;

	GLuint indexbo_sanity;
	GLuint vbo_particle_indices;
	GLuint vbo_ground_indices;
	GLuint vbo_volcano_indices;
	GLuint vbo_volcano_indices_lines;

	ResizingArray<GLfloat>* volcanoComponents;
	ResizingArray<GLushort>* volcanoIndices;
	ResizingArray<GLushort>* volcanoIndicesLines;
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
	void volcanoTextureData();
	void setupGroundData();
	void setupParticleData();

	void drawVolcano();
	void drawVolcanoWithTexture();
	void drawGroundNoTexture();
	void drawGroundWithTexture();

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
	float sceneManager::toRadians(float degrees);

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
	void sanityCheckSetup();
	void drawFrame();
	void drawSanityFrame();

	void drawParticles();
};
