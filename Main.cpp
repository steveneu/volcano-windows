#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SceneManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

using namespace std;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	sceneManager scene;

	scene.printGLString("Vendor", GL_VENDOR);
	scene.printGLString("Renderer", GL_RENDERER);
	scene.printGLString("Version", GL_VERSION);
	scene.printGLString("Shader compiler version", GL_SHADING_LANGUAGE_VERSION);
	scene.printGLString("Extensions", GL_EXTENSIONS);
	GLint64 majorVersion = 0;
	GLint64 minorVersion = 0;
	glGetInteger64v(GL_MAJOR_VERSION, &majorVersion);
	glGetInteger64v(GL_MINOR_VERSION, &minorVersion);

	scene.makePrograms("particle.vp", "particle.fp", "texmesh.vp", "texmesh.fp");

	// TODO: need equivalent of Java_com_neusoft_particle_ObjectJNI_jni_1pushTexture here
	// when ready with texture file loading
	scene.initTextures();

	scene.surfaceChanged(SCR_WIDTH, SCR_HEIGHT);
	//float orientation[] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	float* orientation;
	
	Matrix3x3 orientationMat;
	orientationMat.makeIdentity();
	orientation = orientationMat.get();

	scene.storeOrientation(orientation);
	scene.setupBuffers();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		scene.drawSanityFrame();
		//scene.drawFrame();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	int key = glfwGetKey(window, GLFW_KEY_ESCAPE);
	if (key == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}