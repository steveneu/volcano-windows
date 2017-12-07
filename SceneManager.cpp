#include "SceneManager.h"


sceneManager::sceneManager() {
	//drawFlags = OBELISK | PARTICLES | GROUND | TEXTURE; // controls what objects are drawn (obelisk, particles, textures, etc.)
											  //    drawFlags = OBELISK | PARTICLES | GROUND;
	drawModes[0] = OBELISK | PARTICLES | GROUND;
	drawModes[1] = OBELISK | PARTICLES | GROUND | WIREFRAME;
	drawModes[2] = OBELISK | PARTICLES | GROUND | TEXTURE;

	debugPrintFlags = GLENVIRONMENT | CURSOR; // CURSOR, GLENVIRONMENT, GLERROR, PARTICLEINFO, MATRIX 

	current_mode = 2;
	drawFlags = drawModes[current_mode];

	mProgram_particles = mProgram_texmesh = 0;
	vertex_attrib_idx = color_attrib_idx = muMVPMatrixHandle = 0;

	memset(vCameraPos, 0, sizeof(float) * 4); // = new float[4];
	memset(vUpVector, 0, sizeof(float) * 4); // = new float[4];
	memset(vCurrentUpVector, 0, sizeof(float) * 4); // = new float[4];

	colordata[0] = 1.0f;
	colordata[1] = 1.0f;
	colordata[2] = 1.0f;
	colordata[3] = 1.0f;

	particles_per_sec = framesdrawn = 0;
	intervalbegin = updatetime = lastParticleTime = 0;  // the mean number of particles generated per interval (second)
	updates_second = particle_life = 0;
	arraysize = 200;

	windowInitialized = false;
	mOrientation.makeIdentity();

	framesdrawn = 0;

	// todo: get clock values from system for the following:
	intervalbegin = now_ms();
	updatetime = now_ms();
	lastParticleTime = now_ms();

	// todo: initialize pseudorandom number generator

	vCameraPos[0] = 0;
	vCameraPos[1] = 0;
	vCameraPos[2] = 3;
	vCameraPos[3] = 1;

	gravity.set(0.0f, 0.0f, -0.000098f);

	// this value can affect the trajectory height of the particles.  should be factored into particle position update
	updates_second = 48;

	particles_per_sec = 12; // 12; // 50; // 12; //5
	particle_life = 10; // 2000;

	// global vertex array object for now
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_particles);
	glGenBuffers(1, &vbo_particle_indices);
	
	glGenBuffers(1, &vbo_volcano);
	glGenBuffers(1, &vbo_volcano_indices);
	glGenBuffers(1, &vbo_volcano_indices_lines);

	glGenBuffers(1, &vbo_ground);
	glGenBuffers(1, &vbo_ground_indices);

	glEnable(GL_CULL_FACE);

	volcanoTextureData();
	setupVolcanoData();
	setupParticleData();
	setupGroundData();

	first_egl_query = true;
}

void sceneManager::setupVolcanoData() {
	float obelisk_verts[] = { // 15 elements
							  // e: 0f, 0f, 1.0f, - obelisk vertices
							  // a: -.5f, .5f, 0,
							  // b: .5f, .5f, 0f,
							  // c: .5f, -.5f, 0f,
							  // d: -.5f, -.5f, 0f,

							  // need to duplicate vertices for proper texture coordinate mapping.
							  // order for right hand rule winding (CCW): apex, bl, br

							  // 4,1,0    4,2,1   4,3,2   4,0,3
							  // e,b,a    e,c,b   e,d,c   e,a,d

		0.0, 0.0, 1.0,      // e, 4			// e, 0
		0.5, 0.5, 0.0,      // b, 1			// b, 1
		-0.5, 0.5, 0.0,     // a, 0			// a, 2

		0.0, 0.0, 1.0,      // e, 4
		0.5, -0.5, 0.0,     // c, 2			// c, 4
		0.5, 0.5, 0.0,      // b, 1

		0.0, 0.0, 1.0,      // e, 4
		-0.5, -0.5, 0.0,    // d, 3			// d, 7
		0.5, -0.5, 0.0,     // c, 2

		0.0, 0.0, 1.0,      // e, 4
		-0.5, 0.5, 0.0,     // a, 0
		-0.5, -0.5, 0.0,    // d, 3
	};

	GLfloat ground_verts[] = { // 12 elements
		-2.0,  2.0, 0.0,		// e, 0  // ground vertices
		2.0, 2.0, 0.0,			// f, 1
		2.0, -2.0, 0.0,			// g, 2
		-2.0,  -2.0, 0.0		// h, 3
	};

	//float obelisk_verts2[] = {
	//	-0.5, 0.5, 0.0,		// a		- tl			0
	//	0.5, 0.5, 0.0,		// b		- tr			1
	//	0.5, -0.5, 0.0,		// c		- br			2
	//	-0.5, -0.5, 0.0,	// d		- bl			3
	//	0.0, 0.0, 1.0		// e	- emitter location	4
	//};

	// start with somewhat arbitrary number of 200 particles. particleCount=200
	// 2 vertices per particle (not for obelisk), 3 components per vertex
	// vertices = new float[2 * 3 * (particles.size() + obeliskVerts)];
	int componentCount = 0;

	if (flagSet(drawFlags, OBELISK)) {
		componentCount += sizeof(obelisk_verts) / sizeof(*obelisk_verts); // gives # of elements (floats)
		volcanoComponents = new ResizingArray<GLfloat>(componentCount);

		for (int i = 0; i<componentCount; i++) {
			volcanoComponents->add(obelisk_verts[i]);
		}
	}

	//int groundComponentsCount = 0;
	//if (flagSet(drawFlags, GROUND)) {
	//	groundComponentsCount = sizeof(ground_verts) / sizeof(*ground_verts);
	//	groundComponents = new ResizingArray<GLfloat>(groundComponentsCount);

	//	for (int i = 0; i < groundComponentsCount; i++) {
	//		groundComponents->add(ground_verts[i]);
	//	}
	//}

	//GLushort indices_ground[] = { 0,2,1, 0,3,2 }; // GL_TRIANGLES
	//int num_ground_indices = sizeof(indices_ground) / sizeof(*indices_ground);
	//groundIndicesTriangles = new ResizingArray<GLushort>(num_ground_indices);
	//for (int k = 0; k < num_ground_indices; k++) {
	//	groundIndicesTriangles->add(indices_ground[k]);
	//}

	GLushort ind[] = { 0,1,2, 3,4,5, 6,7,8, 9,10,11 }; // GL_TRIANGLES

	int numIndices = sizeof(ind) / sizeof(*ind);
	volcanoIndices = new ResizingArray<GLushort>(numIndices);

	for (int i = 0; i<numIndices; i++) {
		volcanoIndices->add(ind[i]);
	}

	GLushort indLines[] = {2,1, 1,4, 4,7, 7,2, // a-b, b-c, c-d, d-a,
							2,0, 1,0, 4,0, 7,0 }; // a-e, b-e, c-e, d-e
	int numIndicesLines = sizeof(indLines) / sizeof(*indLines);
	volcanoIndicesLines = new ResizingArray<GLushort>(numIndicesLines);
	for (int i = 0; i < numIndicesLines; i++) {
		volcanoIndicesLines->add(indLines[i]);
	}

	volcanoColors = new ResizingArray<GLfloat>(componentCount);
	GLfloat red = 1.0, green = 1.0, blue = 1.0;

	volcanoColors->add(1.0); //0
	volcanoColors->add(0.0);
	volcanoColors->add(0.0);

	volcanoColors->add(1.0); //1
	volcanoColors->add(0.0);
	volcanoColors->add(0.0);

	volcanoColors->add(1.0); //2
	volcanoColors->add(0.0);
	volcanoColors->add(0.0);

	volcanoColors->add(0.0); //3
	volcanoColors->add(0.5);
	volcanoColors->add(0.0);

	volcanoColors->add(0.0); //4
	volcanoColors->add(0.5);
	volcanoColors->add(0.0);

	volcanoColors->add(0.0); //5
	volcanoColors->add(0.5);
	volcanoColors->add(0.0);

	volcanoColors->add(0.0); //6
	volcanoColors->add(0.0);
	volcanoColors->add(1.0);

	volcanoColors->add(0.0); //7
	volcanoColors->add(0.0);
	volcanoColors->add(1.0);

	volcanoColors->add(0.0); //8
	volcanoColors->add(0.0);
	volcanoColors->add(1.0);

	volcanoColors->add(1.0); //9
	volcanoColors->add(1.0);
	volcanoColors->add(0.0);

	volcanoColors->add(1.0); //10
	volcanoColors->add(1.0);
	volcanoColors->add(0.0);

	volcanoColors->add(1.0); //11
	volcanoColors->add(1.0);
	volcanoColors->add(0.0);

	//int numGroundComponents = sizeof(ground_verts) / sizeof(*ground_verts); // gives # of elements (floats)
	//groundColors = new ResizingArray<GLfloat>(numGroundComponents);
	//for (int i = 0; i < numGroundComponents * 3; i++) { // white for all ground verts
	//	groundColors->add(1.0);
	//	groundColors->add(1.0);
	//	groundColors->add(1.0);
	//}

	int volcanoVertsBytes = volcanoComponents->sizeBytes(); // volcanoComponents->size() * sizeof(float);
	int volcanoColorsBytes = volcanoColors->sizeBytes();// volcanoColors->size() * sizeof(float);
	int volcanoTexBytes = volcanoTexComponents->sizeBytes();

	// vbo_volcano map: volcano vertices, colors for vertices, texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, vbo_volcano);
	glBufferData(GL_ARRAY_BUFFER, volcanoVertsBytes + volcanoColorsBytes + volcanoTexBytes, NULL, GL_STATIC_DRAW);
	int offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, volcanoVertsBytes, volcanoComponents->data());
	offset += volcanoVertsBytes;
	glBufferSubData(GL_ARRAY_BUFFER, offset, volcanoColorsBytes, volcanoColors->data());
	offset += volcanoColorsBytes;
	glBufferSubData(GL_ARRAY_BUFFER, offset, volcanoTexBytes, volcanoTexComponents->data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_volcano_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * volcanoIndices->size(), volcanoIndices->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_volcano_indices_lines);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * volcanoIndicesLines->size(), volcanoIndicesLines->data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// return current time in milliseconds
ULONGLONG sceneManager::now_ms(void) {
	// Retrieves the number of milliseconds that have elapsed since the system was started.
	ULONGLONG ticks = GetTickCount64();

	return ticks;
}

float sceneManager::rnd_float(void) {
	int random_number = rand();// random();
	float result = static_cast<float>(random_number) / static_cast<float>(RAND_MAX);
	return result;
}

float sceneManager::toRadians(float degrees) {
	return M_PI / 180 * degrees;
}

void sceneManager::storeTexture(GLubyte* tex, int width, int height) {
	//GLubyte is unsigned char

	glGenTextures(1, &textureUnit0);
	glBindTexture(GL_TEXTURE_2D, textureUnit0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// copies client memory to GPU texture unit
	glTexImage2D(GL_TEXTURE_2D,
		0, // base image level for mipmapping
		GL_RGBA, // internal format
		width, // width
		height, // height
		0,  // border width, must be 0
		GL_RGBA, // format of texels, should match 3rd parameter
		GL_UNSIGNED_BYTE, //
		tex); // pointer to image data
	//glEnable(GL_CULL_FACE);
}

// use this for generated textures, call from initialize() 
void sceneManager::initTextures() {
	glGenTextures(1, &textureUnit0);
	glBindTexture(GL_TEXTURE_2D, textureUnit0);

	// generate square texture, checkerboard pattern
	const int dimension = 64;
	GLubyte* checkImage = new GLubyte[dimension * dimension * 4];
	int index = 0;

	int c = 0;
	for (int i = 0; i<dimension; i++) {
		for (int j = 0; j<dimension; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			// inline int sceneManager::assign(GLubyte* array, int row, int col, int pos, GLubyte value, int column_count) {
			checkImage[index++] = static_cast<GLubyte>(c); // red
			checkImage[index++] = static_cast<GLubyte>(c); // grn
			checkImage[index++] = static_cast<GLubyte>(c); // blu
			checkImage[index++] = 0xFF; // alpha
		}
	}

	// make a solid red texture- for testing
	//    for(int r=0; r<dimension; r++) {
	//        for(int c=0; c<dimension; c++) {
	//            checkImage[index++] = 0xFF; // red
	//            checkImage[index++] = 0x0; // grn
	//            checkImage[index++] = 0x0; // blu
	//            checkImage[index++] = 0xFF; // alpha
	//        }
	//    }

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// copies client memory to GPU texture unit?
	glTexImage2D(GL_TEXTURE_2D,
		0, // base image level for mipmapping
		GL_RGBA, // internal format
		dimension, // width
		dimension, // height
		0,  // border width, must be 0
		GL_RGBA, // format of texels, should match 3rd parameter
		GL_UNSIGNED_BYTE, //
		checkImage); // pointer to image data

					 // at this point can release memory?
	delete[] checkImage;
	//glEnable(GL_CULL_FACE);
}

void sceneManager::volcanoTextureData() {
	// store texture coordinates for volcano
	GLfloat volcanoTexCoords[] = {
		.8941f, .0352f, // apex            228, 10     .8906, .0391
		.7843f, .1137f, // bottom left   204, 27      .7969, .1055
		1.0f, .1137f, // bottom right    255, 27      1.0,    .1055

		.8941f, .0352f, // apex            228, 10     .8906, .0391
		.7843f, .1137f, // bottom left   204, 27      .7969, .1055
		1.0f, .1137f, // bottom right    255, 27      1.0,    .1055

		.8941f, .0352f, // apex            228, 10     .8906, .0391
		.7843f, .1137f, // bottom left   204, 27      .7969, .1055
		1.0f, .1137f, // bottom right    255, 27      1.0,    .1055

		.8941f, .0352f, // apex            228, 10     .8906, .0391
		.7843f, .1137f, // bottom left   204, 27      .7969, .1055
		1.0f, .1137f // bottom right    255, 27      1.0,    .1055
	};

	int stCountVolcano = sizeof(volcanoTexCoords) / sizeof(*volcanoTexCoords);
	volcanoTexComponents = new ResizingArray<GLfloat>(stCountVolcano);
	for (int i = 0; i<stCountVolcano; i++) {
		volcanoTexComponents->add(volcanoTexCoords[i]);
	}

	// store texture coordinates for ground
	//GLfloat groundTexCoords[] = {
	//	0.0, 1.0,
	//	0.777, 1.0,
	//	0.777, 0.0,
	//	0.0, 0.0
	//};
	//int stCountGround = sizeof(groundTexCoords) / sizeof(*groundTexCoords);
	//groundTexComponents = new ResizingArray<GLfloat>(stCountGround);
	//for (int i = 0; i < stCountGround; i++) {
	//	groundTexComponents->add(groundTexCoords[i]);
	//}
}

void sceneManager::setupGroundData() {
	GLfloat ground_verts[] = { // 12 elements
		-2.0,  2.0, 0.0,      // e, 0  // ground vertices
		2.0, 2.0, 0.0,      // f, 1
		2.0, -2.0, 0.0,      // g, 2
		-2.0,  -2.0, 0.0      // h, 3
	};

	int componentCount = sizeof(ground_verts) / sizeof(*ground_verts); // gives # of elements (floats)
	groundComponents = new ResizingArray<GLfloat>(componentCount);

	for (int i = 0; i<componentCount; i++) {
		groundComponents->add(ground_verts[i]);
	}

	// store texture coordinates for ground
	GLfloat groundTexCoords[] = {
		0.0f, 1.0f,
		0.777f, 1.0f,
		0.777f, 0.0f,
		0.0f, 0.0f
	};

	int stCount = sizeof(groundTexCoords) / sizeof(*groundTexCoords);
	groundTexComponents = new ResizingArray<GLfloat>(stCount);
	for (int i = 0; i<stCount; i++) {
		groundTexComponents->add(groundTexCoords[i]);
	}

	GLushort indices_lines[] = { 0,1, 1,2, 2,3, 3,0, 0,2 };   // GL_LINES
	GLushort indices_triangles[] = { 0,2,1, 0,3,2 }; // GL_TRIANGLES
	int numIndicesLines = 0;
	int numIndicesTriangles = 0;

	numIndicesLines = sizeof(indices_lines) / sizeof(*indices_lines);
	numIndicesTriangles = sizeof(indices_triangles) / sizeof(*indices_triangles);

	groundIndicesLines = new ResizingArray<GLushort>(numIndicesLines);
	groundIndicesTriangles = new ResizingArray<GLushort>(numIndicesTriangles);

	for (int i = 0; i<numIndicesLines; i++) {
		groundIndicesLines->add(indices_lines[i]);
	}
	for (int i = 0; i<numIndicesTriangles; i++) {
		groundIndicesTriangles->add(indices_triangles[i]);
	}

	groundColors = new ResizingArray<GLfloat>(componentCount);
	for (int i = 0; i<componentCount/3; i++) {  // ground colors all white
		groundColors->add(1.0); 
		groundColors->add(1.0);
		groundColors->add(1.0);
	}

	int groundComponentsBytes = groundComponents->sizeBytes();
	int groundColorsBytes = groundColors->sizeBytes();
	int groundTexBytes = groundTexComponents->sizeBytes();

	// vbo_ground map: verts, colors, texture coords
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ground);
	glBufferData(GL_ARRAY_BUFFER, groundComponentsBytes + groundColorsBytes + groundTexBytes, NULL, GL_STATIC_DRAW);
	int offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, groundComponentsBytes, groundComponents->data());
	offset += groundComponentsBytes;
	glBufferSubData(GL_ARRAY_BUFFER, offset, groundColorsBytes, groundColors->data());
	offset += groundColorsBytes;
	glBufferSubData(GL_ARRAY_BUFFER, offset, groundTexBytes, groundTexComponents->data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ground_indices);
	GLsizeiptr indicesBytes = groundIndicesTriangles->sizeBytes();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesBytes, groundIndicesTriangles->data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void sceneManager::setupParticleData() {
	if (drawFlags & PARTICLES) {
		// 3 components per vertex, 2 vertices per particle (particle is a little line segment)
		particleComponents = new ResizingArray<GLfloat>(3 * 2 * particleCount);

		particleIndicesLines = new ResizingArray<GLushort>(2 * particleCount);
		for (int i = 0; i < 2 * particleCount; i++) {
			particleIndicesLines->add(i);
		}

		particleIndicesPoints = new ResizingArray<GLushort>(particleCount);
		for (int i = 0; i<particleCount; i++) {
			particleIndicesPoints->add(i);
		}

		// 4 floats per color(rgba), 2 colors per particle (each end of line seg.)
		particleColors = new ResizingArray<GLfloat>(3 * 2 * particleCount);

		//        for(int j=0; j<particleCount; j++) {
		//            // line segment starts at red..
		//            particleColors->add(1.0); // r
		//            particleColors->add(0.0); // g
		//            particleColors->add(0.0); // b
		//
		//            // ..to white
		//            particleColors->add(1.0); // r
		//            particleColors->add(1.0); // g
		//            particleColors->add(1.0); // b
		//        }
		// component data & color data updated in draw loop
	}
}

sceneManager::~sceneManager() {
	delete volcanoComponents;
	delete volcanoColors;
	delete volcanoIndices;

	delete groundComponents;
	delete groundColors;
	delete groundIndicesLines;
	delete groundIndicesTriangles;

	if (drawFlags & TEXTURE) {
		glDeleteTextures(1, &textureUnit0);
	}

	glDeleteProgram(mProgram_particles);
	glDeleteProgram(mProgram_texmesh);

	//EGLSurface surf = eglGetCurrentSurface(	EGL_DRAW);

	//const NativeDisplayType native_display = EGL_DEFAULT_DISPLAY;
	//EGLDisplay egl_display = eglGetDisplay(	native_display);

	//EGLBoolean result = 0;

	//result = eglDestroySurface(egl_display, surf);
	//LOGI("\nresult: %d", result);
}

//// set param m to identity matrix.  assumes 4x4 matrix in column major order (OpenGL convention)
//// todo: delete, dead code
//void sceneManager::setIdentity(float* m) {
//    m[0] = m[5] = m[10] = m[15] = 1;
//    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11]= m[12]= m[13]= m[14]= 0;
//}

// replacement for Matrix.setLookAtM()
// todo: use this in place of setLookAtM() to see if program still works.  if true, then use same code
// on native side instead of hardcoded chunk in changeLookAt()
// ported to C++ from Songho's code: http://songho.ca/opengl/gl_camera.html#lookat
void sceneManager::setLookAt(Matrix3x3& matrix,		// in/out
	const Vec3& eye,                     // in
	const Vec3& target,                  // in
	const Vec3& upDir) const             // in
{
	Vec3 forward = Vec3::subtract(eye, target);
	forward.normalize();

	Vec3 left = Vec3::cross(upDir, forward);
	left.normalize();

	Vec3 up = Vec3::cross(forward, left);

	matrix.makeIdentity();

	// set rotation part, inverse rotation matrix: M^-1 = M^T for Euclidean transform
	matrix.set(0, left.getx());         //    matrix[0] = left.a;
	matrix.set(4, left.gety());         //    matrix[4] = left.b;
	matrix.set(8, left.getz());         //    matrix[8] = left.c;
	matrix.set(1, up.getx());           //    matrix[1] = up.a;
	matrix.set(5, up.gety());           //    matrix[5] = up.b;
	matrix.set(9, up.getz());           //    matrix[9] = up.c;
	matrix.set(2, forward.getx());      //    matrix[2] = forward.a;
	matrix.set(6, forward.gety());      //    matrix[6] = forward.b;
	matrix.set(10, forward.getz());     //    matrix[10]= forward.c;

										// set translation part
	matrix.set(12, -left.getx() * eye.getx() - left.gety() * eye.gety() - left.getz() * eye.getz());      //    matrix[12]= -left.a * eye.a - left.b * eye.getb - left.c * eye.c;
	matrix.set(13, -up.getx() * eye.getx() - up.gety() * eye.gety() - up.getz() * eye.getz());      //    matrix[13]= -up.a * eye.a - up.b * eye.getb - up.c * eye.getc;
	matrix.set(14, -forward.getx() * eye.getx() - forward.gety() * eye.gety() - forward.getz() * eye.getz());      //    matrix[14]= -forward.a * eye.a - forward.b * eye.b - forward.c * eye.c;
}

void sceneManager::changeLookAt() {
	cameraPos.putx(vCameraPos[0]);
	cameraPos.puty(vCameraPos[1]);
	cameraPos.putz(vCameraPos[2]);

	upVector.set(vUpVector[0], vUpVector[1], vUpVector[2]);

	// muMVPMatrixHandle = glGetUniformLocation(mProgram_particles, "uMVPMatrix");
	Vec3 target(0, 0, 0);

	// old look at matrix was hardcoded, now use handbuilt setLookAt()
	setLookAt(mLookAtMat, cameraPos, target, upVector);

	mLookAtMat.debugPrint(debugPrintFlags, "mLookAtMat");
	mOrientation.debugPrint(debugPrintFlags, "mOrientation");

	Matrix3x3::mul(mVMatrix, mLookAtMat, mOrientation);

	// mVMatrix is combined with projection matrix in drawframe()
	mVMatrix.debugPrint(debugPrintFlags, "mVMatrix");
}

void sceneManager::toggleTouchdown() {
	// cycle draw modes
	//drawFlags; WIREFRAME, TEXTURE, <neither>
	// INITIAL: drawFlags = OBELISK | PARTICLES | GROUND; // controls what objects are drawn (obelisk, particles, textures, etc.)
	current_mode == mode_count - 1 ? current_mode = 0 : current_mode++;
	drawFlags = drawModes[current_mode];

	changeLookAt();
}

void sceneManager::storeOrientation(float* in) {
	mOrientation.put(in);
}

void sceneManager::printGLString(const char *name, GLenum s) {
	if (debugPrintFlags & GLENVIRONMENT) {
		const char *v = (const char *)glGetString(s);
		if (v!=0) {
			char format_string[] = "\nGL %s: %s";

			size_t string_size = strlen(name) + strlen(v) + strlen(format_string);
			char* message = new char[string_size];
			memset(message, '\0', string_size);
			sprintf_s(message, string_size-1, format_string, name, v);
	
			cout << message;
			OutputDebugString(message);
			delete[] message;
		}
		//LOGI("GL %s = %s\n", name, v);
	}
}

// errors defined in glad.h when using glad
void sceneManager::checkGlError(const char* op) {
	if (debugPrintFlags & GLERROR) {
		for (GLint error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
			char str_value[10] = { 0 };
			_itoa_s(error, str_value, 16);
			char format_string[] = "after %s() glError (0x%x)\n";
			size_t string_size = strlen(op) + strlen(format_string) + strlen(str_value);
			char* message = new char[string_size];
		
			sprintf_s(message, string_size, format_string, op, error);
			OutputDebugString(message);
			delete[] message;

			//LOGI("after %s() glError (0x%x)\n", op, error);
		}
	}
}

//GLuint sceneManager::loadShader(GLenum shaderType, const char* pSource) {
//	GLuint shader = glCreateShader(shaderType);
//	if (shader) {
//		glShaderSource(shader, 1, &pSource, NULL);
//		glCompileShader(shader);
//		GLint compiled = 0;
//		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
//		if (!compiled) {
//			GLint infoLen = 0;
//			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
//			if (infoLen) {
//				char* buf = new char[infoLen];
//				if (buf) {
//					glGetShaderInfoLog(shader, infoLen, NULL, buf);
//					//LOGE("Could not compile shader %d:\n%s\n",
//					//     shaderType, buf);
//					delete[] buf;
//				}
//				glDeleteShader(shader);
//				shader = 0;
//			}
//		}
//	}
//	return shader;
//}

GLuint sceneManager::loadShader(GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	checkGlError("glCreateShader");

	if (shader) {
		glShaderSource(shader, 1, &pSource, NULL);
		checkGlError("glShaderSource");
		glCompileShader(shader);
		checkGlError("glCompileShader");
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		checkGlError("glGetShaderiv");
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			checkGlError("glGetShaderiv");
			if (infoLen) {
				char* buf = new char[infoLen];
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					checkGlError("glGetShaderInfoLog");
					//LOGE("Could not compile shader %d:\n%s\n",
					//     shaderType, buf);
					delete[] buf;
				}
				glDeleteShader(shader);
				checkGlError("glDeleteShader");
				shader = 0;
			}
		}
	}
	return shader;
}

// given filename to shader, return shader as string
// filename [in] - path to shader
// source[in/out] - string to shader source.  caller must free this resource
void sceneManager::getStringFromFile(const char* filename, char** source) {
	ifstream inFile(filename, ifstream::ate);
	int size = static_cast<int>(inFile.tellg());
	inFile.close();

	*source = new char[size] {0};

	inFile.open(filename);
	if (!inFile) {
		cout << "Unable to open file";
		exit(1); // terminate with error
	}
	else {
		int i = 0;
		while (i < size) {
			(*source)[i++] = inFile.get();
		}
	}

	inFile.close();
}

// given the shader filenames, open the files, read them into strings and pass to compile/link functions
void sceneManager::makePrograms(const char* vertexFn, const char* fragmentFn, const char* vertexTexFn, const char* fragmentTexFn) {
	char* strVertexSrc = 0;
	char* strFragmentSrc = 0;
	char* strTexVpSrc = 0;
	char* strTexFpSrc = 0;

	getStringFromFile(vertexFn, &strVertexSrc);
	getStringFromFile(fragmentFn, &strFragmentSrc);
	getStringFromFile(vertexTexFn, &strTexVpSrc);
	getStringFromFile(fragmentTexFn, &strTexFpSrc);

	createPrograms(strVertexSrc, strFragmentSrc, strTexVpSrc, strTexFpSrc);

	delete[] strVertexSrc;
	delete[] strFragmentSrc;
	delete[] strTexVpSrc;
	delete[] strTexFpSrc;
}

void sceneManager::createPrograms(const char* pVertexSource, const char* pFragmentSource,
	const char* pVertexTextureSrc, const char* pFragmentTextureSrc)
{
	createProgram(&mProgram_particles, pVertexSource, pFragmentSource);
	createProgram(&mProgram_texmesh, pVertexTextureSrc, pFragmentTextureSrc);
}

// create OpenGL program given source to vertex and fragment shaders
// pVertexSource[in]- pointer to vertex program src
// pFragmentSource[in]- pointer to fragment program src
// program [in/out]- pointer to program identifier, stored as member
void sceneManager::createProgram(GLuint* program, const char* pVertexSource, const char* pFragmentSource) {
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		// todo: handle error
		return;
	}

	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		// todo: handle error
		return;
	}

	/*gProgram*/ *program = glCreateProgram();
	checkGlError("glCreateProgram");
	if (*program) {
		glAttachShader(*program, vertexShader);
		checkGlError("glAttachShader");
		glAttachShader(*program, pixelShader);
		checkGlError("glAttachShader");
		glLinkProgram(*program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(*program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {

				char* buf = new char[bufLength];
				if (buf) {
					glGetProgramInfoLog(*program, bufLength, NULL, buf);
					//LOGE("Could not link program:\n%s\n", buf);
					delete[] buf;
				}
			}
			glDeleteProgram(*program);
			*program = 0;
		}
	}

	if (!*program) {
		//LOGE("Could not create program.");
		return;
	}
}

void sceneManager::surfaceChanged(int w, int h) {
	// only do this once, then it won't be called again when the user rotates their device with the rotation set to 'unlocked'
	if (!windowInitialized)
	{
		glViewport(0, 0, w, h);

		// for desktop, orientation does not change width is greater than height
		vUpVector[0] = 0.0f;
		vUpVector[1] = 1.0f;
		vUpVector[2] = 0.0f;

		// for mobile phones
		//if (h>w) {
		//	vUpVector[0] = 0.0f;
		//	vUpVector[1] = 1.0f;
		//	vUpVector[2] = 0.0f;
		//}
		//else {
		//	vUpVector[0] = 1.0f;
		//	vUpVector[1] = 0.0f;
		//	vUpVector[2] = 0.0f;
		//}

		Matrix3x3::frustum(mProjMatrix, -1, 1, -1, 1, 1, 8); // make projection matrix given clip planes

		changeLookAt();
		windowInitialized = true;
	}
}

// set program state for drawing with textures
void sceneManager::setTextureProgram() {
	glUseProgram(mProgram_texmesh);
	// send the composite modelview projection matrix to the vertex shader
	muMVPMatrixHandle = glGetUniformLocation(mProgram_texmesh, "uMVPMatrix");
	glUniformMatrix4fv(muMVPMatrixHandle, 1, false, mMVPMatrix.get());
}

// set state for drawing basic vertex/fragment shader pair (flat shading or wireframe)
void sceneManager::setBasicProgram() {
	glUseProgram(mProgram_particles);
	muMVPMatrixHandle = glGetUniformLocation(mProgram_particles, "uMVPMatrix");
	glUniformMatrix4fv(muMVPMatrixHandle, 1, false, mMVPMatrix.get());
}

void sceneManager::sanityCheckSetup() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_sanity);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sanity);

	GLfloat triangle[] = { // 12 elements
		0.0,  0.0, 0.0,      // a, 0  // ground vertices
		1.0, 1.0, 0.0,      // b, 1
		1.0, 0.0, 0.0,      // c, 2
	};

	GLfloat colorData[] = {
		1.0,  1.0, 1.0,
		1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle) + sizeof(colorData), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangle), triangle);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(triangle), sizeof(colorData), colorData);

	vertex_attrib_idx = glGetAttribLocation(mProgram_particles, "vPosition");
	glVertexAttribPointer(static_cast<GLuint>(vertex_attrib_idx),
		3, // # of components per vertex attribute. Must be 1, 2, 3, or 4.
		GL_FLOAT, // data type for component
		GL_FALSE, // Normalized?
		0, //3 * sizeof(GLfloat), // byte offset between vertex attributes. attribute is a set of elements
		0); // offset into GL_ARRAY_BUFFER

	color_attrib_idx = glGetAttribLocation(mProgram_particles, "vColor");
	glVertexAttribPointer(static_cast<GLuint>(color_attrib_idx),
		3, // # of components per generic vertex attribute
		GL_FLOAT,
		GL_FALSE, // normalized?
		0, //3 * sizeof(GLfloat), // byte offset between attributes. attribute is a set of elements
		(const GLvoid*)sizeof(triangle));

	GLushort indices_triangle[] = { 0,1,2 }; // GL_TRIANGLES

	glGenBuffers(1, &indexbo_sanity);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbo_sanity);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_triangle), indices_triangle, GL_STATIC_DRAW);
}

// draw a simple triangle in the x-y plane as a sanity test for correct GL and scene setup
void sceneManager::drawSanityFrame() {
	changeLookAt();

	mProjMatrix.debugPrint(debugPrintFlags, "mProjMatrix");
	mVMatrix.debugPrint(debugPrintFlags, "mVMatrix");
	Matrix3x3::mul(mMVPMatrix, mProjMatrix, mVMatrix); // result, lhs, rhs (result = lhs x rhs)

	mMVPMatrix.debugPrint(debugPrintFlags, "mMVPMatrix");

	// clear frame buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use basic shader for santiy drawing
	glUseProgram(mProgram_particles);
	muMVPMatrixHandle = glGetUniformLocation(mProgram_particles, "uMVPMatrix");
	glUniformMatrix4fv(muMVPMatrixHandle, 1, false, mMVPMatrix.get());

	vertex_attrib_idx = glGetAttribLocation(mProgram_particles, "vPosition");
	color_attrib_idx = glGetAttribLocation(mProgram_particles, "vColor");

	glEnableVertexAttribArray(vertex_attrib_idx);
	glEnableVertexAttribArray(color_attrib_idx);

	//GLushort indices_triangle[] = { 0,1,2 }; // GL_TRIANGLES

	glDrawElements(GL_TRIANGLES, 
		3,    // # of indicies in index array (# of short values, last param)
		GL_UNSIGNED_SHORT,             // data type of index array
		(const GLvoid*)0);   // indicies_array

	glDisableVertexAttribArray(vertex_attrib_idx);
	glDisableVertexAttribArray(color_attrib_idx);
}

void sceneManager::drawFrame() {
	ULONGLONG timeSincelastupdate = now_ms()-updatetime;
	// 24 times per second, update the particles
	if (drawFlags & PARTICLES && timeSincelastupdate > 1000 / updates_second) // 1000ms/24 = 42
	{
		{    // decrement lifetime for all live particles
			std::vector<AParticle>::iterator it = particles.begin();
			while (it != particles.end()) {
				if ((*it).get_lifeleft() > 0) {
					(*it).subtract_lifetime(1000 / updates_second);
				}
				it++;
			}
		}

		// remove particles that are past their lifetime.
		// currently only removes the first expired particle encountered in the list
		// this should be ok considering the frequency the list is checked
		{
			vector<AParticle>::iterator it = particles.begin();
				while (it != particles.end()) {
					if ((*it).expired()) {
						particles.erase(it);
						break;
					}
					it++;
				}
		}

		// Generate particles
		//        long millis_since_last_particle = now_ms() - lastParticleTime;
		//        float particles_per_milli = static_cast<float>(particles_per_sec)/1000.0f;
		//        bool make_particle = (millis_since_last_particle > particles_per_milli);
		//        LOGI("\ntime since last particle(ms): %ld particles/ms: %f make particle? %s", millis_since_last_particle, particles_per_milli, make_particle ? "TRUE" : "FALSE");
		//        if (make_particle) {

		if (now_ms() - lastParticleTime >
			1000 / particles_per_sec) // 1 particle/sec
		{
			// speed of particle upper and lower bounds
			float lower = 0.08f; //.7
			float upper = 0.092f; //.12
			float rnd01 = rnd_float();
			//LOGI("random #1: %f()\n", rnd01);
			float magnitude = rnd01 * (upper - lower) + lower; // generate random velocity within bounds

															   // given a random angle within range alpha_s, and magnitude (speed), calculate y, z coordinates
			float alpha_s = toRadians(30); // angle spread from xy plane (away from z axis?)
			float half_alpha_s = alpha_s / 2.0f;
			float rnd02 = rnd_float();
			//LOGI("random #2: %f()\n", rnd02);
			float angle_a = rnd02 * alpha_s;
			//            float right_angle = toRadians(90);

			//LOGI("\t%.3f\t%.3f\t%.3f\t%.3f", m[0], m[4], m[8], m[12]);
			//            LOGI("\tright_angle: %.3f", right_angle);
			//LOGI("\thalf_alpha_s: %.3f", half_alpha_s);
			//LOGI("\tangle_a: %.3f", angle_a);

			Vec3 particle_vector;
			particle_vector.set(0.0, 0.0, 0.0);
			particle_vector.setd(1.0);

			// magnitude & angle_a are random values
			particle_vector.setb(magnitude * (float)cos(toRadians(90) - half_alpha_s + angle_a));
			particle_vector.setc(magnitude * (float)sin(toRadians(90) - half_alpha_s + angle_a));

			float rotateYaxisArray[16];

			float beta_s = toRadians(30); // angle spread from yz plane
			float aay_radians = beta_s * rnd_float() - beta_s / 2.0f;

			//            LOGI("cos(aay_rad): %f", resultVec.geta());
			//            LOGI("sin(aay_rad): %f", resultVec.geta());

			// setup rotation matrix for rotating about y axis by angle_about_y
			rotateYaxisArray[0] = static_cast<float>(cos(aay_radians));            // column 0
			rotateYaxisArray[1] = 0;
			rotateYaxisArray[2] = static_cast<float>(-sin(aay_radians));
			rotateYaxisArray[3] = 0;

			rotateYaxisArray[4] = 0;                                        // column 1
			rotateYaxisArray[5] = 1;
			rotateYaxisArray[6] = 0;
			rotateYaxisArray[7] = 0;

			rotateYaxisArray[8] = static_cast<float>(sin(aay_radians));            // column 2
			rotateYaxisArray[9] = 0;
			rotateYaxisArray[10] = static_cast<float>(cos(aay_radians));
			rotateYaxisArray[11] = 0;

			rotateYaxisArray[12] = 0;                                        // column 4
			rotateYaxisArray[13] = 0;
			rotateYaxisArray[14] = 0;
			rotateYaxisArray[15] = 1;

			Vec3 resultVec;

			// result = lhs x rhs --- resultVec(vec) = rotateYaxisArray(mat) x particle_vector(vec)
			Matrix3x3 rotateYaxisMatrix(rotateYaxisArray);

			rotateYaxisMatrix.debugPrint(debugPrintFlags, "rotateYaxisMatrix");
			//particle_vector.debugPrint(debugPrintFlags, "particle_vector");
			Matrix3x3::mul(resultVec, rotateYaxisMatrix, particle_vector);
//			resultVec.debugPrint(debugPrintFlags, "resultVec");

			particle_vector.seta(resultVec.geta());
			particle_vector.setb(resultVec.getb());
			particle_vector.setc(resultVec.getc());
			particle_vector.setd(resultVec.getd());

			//LOGI("particle_vector[0] %.3f", particle_vector.geta());
			//LOGI("particle_vector[1] %.3f", particle_vector.getb());
			//LOGI("particle_vector[2] %.3f", particle_vector.getc());
			//LOGI("particle_vector[3] %.3f", particle_vector.getd());

			AParticle newparticle(0.0f, 0.0f, 1.0f, // location of emitter, a point source
				particle_vector.geta(), // velocity and direction of particle
				particle_vector.getb(),
				particle_vector.getc(),
				1.0f, 0.0f, 0.0f, 1.0f, // initial color of particle
				1.0f, 1.0f, 1.0f, 1.0f, // final color of particle
				gravity,
				2000); // static particle life of 2000 ms.  could randomize?
			particles.push_back(newparticle);
			lastParticleTime = now_ms();
		}

		// update position of all particles
		for (int i = 0; i < particles.size(); i++) {
			particles[i].UpdateAttributes();
			particles[i].UpdatePosition();
			particles[i].applyGravity();
		}

		// refresh time of last update
		updatetime = now_ms();

		// copy the particles into the floatbuffer containing the vertices
		//            vertices.position(vertexdata.length); // set position to the end of the floatbuffer- why is this done
		// particleColors.position(4); -- why is position set to 4 here?

		// 2 vertices per particle, 3 components per vertex

		// 3 floats per particle (x,y,z), 2 points(start, end) for each line segment
		// todo: check if vertices are initialized to zero on allocation
		//vertices = new float[2 * 3 * (particles.size() + obeliskVerts)];
		int vPos = 0;
		int pcPos = 0;

		// todo: check if allocation initializes to 0
		//indicies = new GLushort[2 * 3 * particles.size() + obeliskVerts];
		
		// copy particle data to float type resizing array
		{
			std::vector<AParticle>::iterator it = particles.begin();
			//int test = particles.size(); test;

			// shallow delete of old data
			particleComponents->blank();
			particleColors->blank();

			for (; it != particles.end(); it++) { // std::vector<AParticle> particles;
				Point current_point = (*it).getRenderPos();
				
				particleComponents->add(current_point.getx());
				particleComponents->add(current_point.gety());
				particleComponents->add(current_point.getz());
				//particleComponents->assign(vPos++, point.getx());
				//particleComponents->assign(vPos++, point.gety());
				//particleComponents->assign(vPos++, point.getz());

				
				float r = (*it).getr();
				float g = (*it).getg();
				float b = (*it).getb();
				float a = (*it).geta();

				//float r = 1.0;
				//float g = 1.0;
				//float b = 1.0;
				//float a = 1.0;

				// add colors for 1st point in line segment
				particleColors->add(r);
				particleColors->add(g);
				particleColors->add(b);
				//particleColors->add(a);

				//// Log.d(TAG, "red: " + particle.getr());
				//particleColors->assign(pcPos++, (*it).getr());
				//// Log.d(TAG, "grn: " + particle.getg());
				//particleColors->assign(pcPos++, (*it).getg());
				//// Log.d(TAG, "blu: " + particle.getb());
				//particleColors->assign(pcPos++, (*it).getb());
				//// Log.d(TAG, "alpha: " + particle.geta());
				//particleColors->assign(pcPos++, (*it).geta());

				Point previous_point = (*it).getPreviousPos(); // trailing part of line segment

				particleComponents->add(previous_point.getx());
				particleComponents->add(previous_point.gety());
				particleComponents->add(previous_point.getz());

				// add colors for 2nd point in line segment
				r = (*it).getr();
				g = (*it).getg();
				b = (*it).getb();
				a = (*it).geta();

				particleColors->add(r);
				particleColors->add(g);
				particleColors->add(b);
				//particleColors->add(a);
				//particleComponents->assign(vPos++, prev.getx());
				//particleComponents->assign(vPos++, prev.gety());
				//particleComponents->assign(vPos++, prev.getz());

				//// Log.d(TAG, "red: " + particle.getr());
				//particleColors->assign(pcPos++, (*it).getr());
				//// Log.d(TAG, "grn: " + particle.getg());
				//particleColors->assign(pcPos++, (*it).getg());
				//// Log.d(TAG, "blu: " + particle.getb());
				//particleColors->assign(pcPos++, (*it).getb());
				//// Log.d(TAG, "alpha: " + particle.geta());
				//particleColors->assign(pcPos++, (*it).geta());
			}
		}
	}

	// update the MVP composite matrix with camera orientation
	Matrix3x3::mul(mVMatrix, mLookAtMat, mOrientation);

	mProjMatrix.debugPrint(debugPrintFlags, "mProjMatrix");
	mVMatrix.debugPrint(debugPrintFlags, "mVMatrix");
	Matrix3x3::mul(mMVPMatrix, mProjMatrix, mVMatrix); // result, lhs, rhs (result = lhs x rhs)

	mMVPMatrix.debugPrint(debugPrintFlags, "mMVPMatrix");

	// Redraw background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (flagSet(drawFlags, TEXTURE)){
		//setBasicProgram();
		//drawGroundNoTexture();
		
		setTextureProgram();
		drawGroundWithTexture();
		drawVolcanoWithTexture();

		setBasicProgram();
		drawParticles();
		
	}
	else {
		setBasicProgram();
		drawParticles();
		//drawGroundNoTexture();
		drawVolcano();
	}

	framesdrawn++;
	// every few seconds, write average fps over the last 10 seconds to log
	if (now_ms() - intervalbegin > 4000)// velocity and direction of particle
	{
		ULONGLONG elapsed = now_ms() - intervalbegin;
		double fps = static_cast<double>(framesdrawn) / static_cast<double>(elapsed / 1000);
		//            Log.d(TAG, "fps: " + fps);
		// ALOG("This message comes from C at line %d.", __LINE__);
		//LOGI("fps: %f", fps);
		intervalbegin = now_ms();
		framesdrawn = 0;
	}

	// query egl surface
	//    EGLint readdraw = 0; // need value for read/draw, use draw?
	if (first_egl_query) {
		//EGLint value = 0;
		//EGLint attribute = EGL_CONFIG_ID;
		//EGLSurface surf = eglGetCurrentSurface(	EGL_DRAW);

		//NativeDisplayType native_display = EGL_DEFAULT_DISPLAY;
		//EGLDisplay display = eglGetDisplay(	native_display);

		//EGLBoolean eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl config id: %d", value);

		//attribute = EGL_HEIGHT;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl height: %d", value);

		//attribute = EGL_HORIZONTAL_RESOLUTION;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl horizontal res: %d", value);

		//attribute = EGL_LARGEST_PBUFFER;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl largest pbuffer: %d", value);

		//attribute = EGL_MIPMAP_LEVEL;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl mipmap level: %d", value);

		//attribute = EGL_MIPMAP_TEXTURE;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl mipmap texture: %d", value);

		//attribute = EGL_MULTISAMPLE_RESOLVE;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl multisample resolve: %d", value);

		//attribute = EGL_PIXEL_ASPECT_RATIO;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl pixel aspect: %d", value);

		//attribute = EGL_RENDER_BUFFER;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl render buffer: %d", value);

		//attribute = EGL_SWAP_BEHAVIOR;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl swap behaviour: %d", value);

		//attribute = EGL_TEXTURE_FORMAT;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl texture format: %d", value);

		//attribute = EGL_TEXTURE_TARGET;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl texture target: %d", value);

		//attribute = EGL_VERTICAL_RESOLUTION;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl vertical resolution: %d", value);

		//attribute = EGL_WIDTH;
		//eglResult = eglQuerySurface(	display, surf, attribute, &value);
		//LOGI("\negl width: %d", value);

		first_egl_query = false;
	}
}

void sceneManager::drawVolcano() {
	if (OBELISK & drawFlags) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_volcano);
		int volcanoComponentsBytes = volcanoComponents->size() * sizeof(GLfloat);
		glVertexAttribPointer(static_cast<GLuint>(vertex_attrib_idx),
			3, // # of components per generic vertex attribute
			GL_FLOAT, // component data type
			GL_FALSE, // normalized?
			3 * sizeof(GL_FLOAT), // byte offset between consecutive vertex attributes
			0); // offset of the first component of the first generic vertex attribute in the array in the 
				// data store of the buffer currently bound to the GL_ARRAY_BUFFER target.The initial value is 0.

		glVertexAttribPointer(static_cast<GLuint>(color_attrib_idx),
			3,
			GL_FLOAT,
			GL_FALSE,
			3 * sizeof(GL_FLOAT),
			(const GLvoid*)volcanoComponentsBytes);

		glEnableVertexAttribArray(vertex_attrib_idx);
		glEnableVertexAttribArray(color_attrib_idx);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_volcano_indices_lines);
		glDrawElements(GL_LINES,
			volcanoIndicesLines->size(),    // # of indicies in index array (# of short values, last param)
			GL_UNSIGNED_SHORT,             // data type of index array
			(const GLvoid*)0);   // indicies_array

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_volcano_indices);
		glDrawElements(GL_TRIANGLES,
			volcanoIndices->size(),    // # of indicies in index array (# of short values, last param)
			GL_UNSIGNED_SHORT,             // data type of index array
			(const GLvoid*)0);   // indicies_array

		// drawing finished, break bindings
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(vertex_attrib_idx);
		glDisableVertexAttribArray(color_attrib_idx);
	}
}

void sceneManager::drawVolcanoWithTexture() {
	if (flagSet(drawFlags, OBELISK)) {
		vertex_attrib_idx = glGetAttribLocation(mProgram_texmesh, "vPosition");
		texture_coords_idx = glGetAttribLocation(mProgram_texmesh, "vTexCoords");
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_volcano);
		glVertexAttribPointer(static_cast<GLuint>(vertex_attrib_idx),
			3, // # of components per vertex attribute. Must be 1, 2, 3, or 4.
			GL_FLOAT, // data type for component
			GL_FALSE, // Normalized?
			3 * sizeof(GLfloat), // byte offset between vertex attributes. attribute is a set of elements
			0); // offset to data in bytes

		int offset = volcanoComponents->sizeBytes() + volcanoColors->sizeBytes();
		glVertexAttribPointer(static_cast<GLuint>(texture_coords_idx),
			2, // # of components per generic vertex attribute
			GL_FLOAT,
			GL_FALSE, // normalized?
			2 * sizeof(GLfloat), // byte offset between attributes. attribute is a set of elements
			(const GLvoid*)offset);

		glEnableVertexAttribArray(vertex_attrib_idx);
		glEnableVertexAttribArray(texture_coords_idx);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_volcano_indices);

		glDrawElements(GL_TRIANGLES,
			volcanoIndices->size(),    // # of indicies in index array (# of short values, last param)
			GL_UNSIGNED_SHORT,             // data type of index array
			0);   // indicies_array
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(vertex_attrib_idx);
		glDisableVertexAttribArray(texture_coords_idx);
	}
}

void sceneManager::drawParticles() {
	if ( (drawFlags & PARTICLES) && particleComponents->size() > 0) {
		vertex_attrib_idx = glGetAttribLocation(mProgram_particles, "vPosition");
		color_attrib_idx = glGetAttribLocation(mProgram_particles, "vColor");

		glBindBuffer(GL_ARRAY_BUFFER, vbo_particles);
		int particleComponentsBytes = particleComponents->sizeBytes();
		int particleColorsBytes = particleColors->sizeBytes();

		if (debugPrintFlags & PARTICLEINFO) {
			char formatString[] = "\n # floats: %d line segments(particles): %d particle components(bytes): %d";
			int strsize = strlen(formatString) + 12;
			char message[200] = { 0 };
			sprintf_s(message, strsize, formatString, particleComponents->size(), particleComponents->size() /3/2, particleComponentsBytes);
			OutputDebugString(message);
		}
		
		int vertexCount = particleComponents->size() / 3;

		glBufferData(GL_ARRAY_BUFFER, particleComponentsBytes + particleColorsBytes, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particleComponentsBytes, particleComponents->data());
		glBufferSubData(GL_ARRAY_BUFFER, particleComponentsBytes, particleColorsBytes, particleColors->data());
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		glVertexAttribPointer(static_cast<GLuint>(vertex_attrib_idx),
			3, // # of components
			GL_FLOAT, // data type
			GL_FALSE, // not normalized
			3 * sizeof(GL_FLOAT), // byte offset between attribs or 0 for packed data
			0); // offset into GL_ARRAY_BUFFER
		
		glVertexAttribPointer(static_cast<GLuint>(color_attrib_idx),
			3,
			GL_FLOAT,
			GL_FALSE,
			3 * sizeof(GL_FLOAT),
			(const GLvoid*) sizeof(particleComponents->size() * sizeof(GL_FLOAT)));

		// indices are set up for the ALLOCATED size of the array, actual storage needed is only for the vertices to draw -- sizeof(GLushort) * vertexCount
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_particle_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * vertexCount, particleIndicesPoints->data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(vertex_attrib_idx);
		glEnableVertexAttribArray(color_attrib_idx);

		// output particle debug data
		char message[255] = { 0 };
		char line1format[] = "\nframe %d";
		sprintf_s(message, 255, line1format, framesdrawn);
		OutputDebugString(message);
		char linexformat[] = "\n\tid: %d\tpos: (%f, %f, %f)\t\tcolor: (%f, %f, %f)";
		for (int i = 0; i < particleComponents->size()/3; i++) {
			memset(message, '\0', 255);
			GLfloat x=0, y=0, z=0;
			GLfloat r = 0, g = 0, b = 0;
			int offsetvertex = i * 3;
			x = *(particleComponents->data() + (offsetvertex + 0));
			y = *(particleComponents->data() + (offsetvertex + 1));
			z = *(particleComponents->data() + (offsetvertex + 2));

			int offsetcolor = i * 3;
			r = *(particleColors->data() + (offsetcolor + 0));
			g = *(particleColors->data() + (offsetcolor + 1));
			b = *(particleColors->data() + (offsetcolor + 2));
			sprintf_s(message, 255, linexformat, i, x,y,z, r,g,b);
			OutputDebugString(message);
		}

		glDrawElements(GL_LINES,
			//particleIndicesLines->size(),    // # of indicies in index array (# of short values, last param)
			vertexCount,
			GL_UNSIGNED_SHORT,             // data type of index array
			(const GLvoid*)0);   // indicies_array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(vertex_attrib_idx);
		glDisableVertexAttribArray(color_attrib_idx);
	}
}

void sceneManager::drawGroundNoTexture() {
	if (true) { //flagSet(drawFlags, GROUND) && !flagSet(drawFlags, TEXTURE)) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_ground);
		vertex_attrib_idx = glGetAttribLocation(mProgram_particles, "vPosition");
		glVertexAttribPointer(static_cast<GLuint>(vertex_attrib_idx),
			3, // # of components per vertex attribute. Must be 1, 2, 3, or 4.
			GL_FLOAT, // data type for component
			GL_FALSE, // Normalized?
			3 * sizeof(GLfloat), // byte offset between vertex attributes. attribute is a set of elements
			0); // define vertex array

		color_attrib_idx = glGetAttribLocation(mProgram_particles, "vColor");
		int offset = groundComponents->sizeBytes();
		glVertexAttribPointer(static_cast<GLuint>(color_attrib_idx),
			3, // # of components per generic vertex attribute
			GL_FLOAT,
			GL_FALSE, // normalized?
			3 * sizeof(GLfloat), // byte offset between attributes. attribute is a set of elements
			(const GLvoid*)offset);

		glEnableVertexAttribArray(vertex_attrib_idx);
		glEnableVertexAttribArray(color_attrib_idx);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ground_indices);
		glDrawElements(GL_TRIANGLES,
			groundIndicesTriangles->size(),     // # of indicies in index array (# of short values, last param)
			GL_UNSIGNED_SHORT,					// data type of index array
			0);    // indicies_array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//if (drawFlags & WIREFRAME) {
		//	glDrawElements(GL_LINES,
		//		groundIndicesLines->size(),    // # of indicies in index array (# of short values, last param)
		//		GL_UNSIGNED_SHORT,             // data type of index array
		//		groundIndicesLines->data());   // indicies_array
		//}
		//else {
		//	glDrawElements(GL_TRIANGLES,
		//		groundIndicesTriangles->size(),    // # of indicies in index array (# of short values, last param)
		//		GL_UNSIGNED_SHORT,             // data type of index array
		//		groundIndicesTriangles->data());   // indicies_array
		//}

		glDisableVertexAttribArray(vertex_attrib_idx);
		glDisableVertexAttribArray(color_attrib_idx);
	}
}

void sceneManager::drawGroundWithTexture() {
	if (flagSet(drawFlags, GROUND | TEXTURE)) {
		vertex_attrib_idx = glGetAttribLocation(mProgram_texmesh, "vPosition");
		texture_coords_idx = glGetAttribLocation(mProgram_texmesh, "vTexCoords");

		glBindBuffer(GL_ARRAY_BUFFER, vbo_ground);
		glVertexAttribPointer(static_cast<GLuint>(vertex_attrib_idx),
			3, // # of components per vertex attribute. Must be 1, 2, 3, or 4.
			GL_FLOAT, // data type for component
			GL_FALSE, // Normalized?
			3 * sizeof(GLfloat), // byte offset between vertex attributes. attribute is a set of elements
			0); // define vertex array

		int offset = groundComponents->sizeBytes() + groundColors->sizeBytes();
		glVertexAttribPointer(static_cast<GLuint>(texture_coords_idx),
			2, // # of components per generic vertex attribute
			GL_FLOAT,
			GL_FALSE, // normalized?
			2 * sizeof(GLfloat), // byte offset between attributes. attribute is a set of elements
			(const GLvoid*) offset);

		glEnableVertexAttribArray(vertex_attrib_idx);
		glEnableVertexAttribArray(texture_coords_idx);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ground_indices);
		glDrawElements(GL_TRIANGLES,
			groundIndicesTriangles->size(),     // # of indicies in index array (# of short values, last param)
			GL_UNSIGNED_SHORT,					// data type of index array
			0);									// offset to indicies array in vbo_ground_indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(vertex_attrib_idx);
		glDisableVertexAttribArray(texture_coords_idx);
	}
}

