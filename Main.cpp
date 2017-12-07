#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SceneManager.h"
#include "lodepng.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

using namespace std;

// The callback functions receives the cursor position, measured in screen coordinates but relative to the top - left corner 
// of the window client area.On platforms that provide it, the full sub - pixel cursor position is passed on.

static float rotAboutx; // rotation about x-axis
static float rotAbouty; // rotation about y-axis

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	int halfwidth = width / 2;
	int halfheight = height / 2;

	// xpos-halfwidth gives displacement from the center of the window
	float rotation_pct_x = static_cast<float>(xpos - halfwidth) / static_cast<float>(halfwidth);
	rotAbouty = M_PI * rotation_pct_x; // compute rotation about y in radians

	float rotation_pct_y = static_cast<float>(ypos - halfheight) / static_cast<float>(halfheight);
	rotAboutx = M_PI * rotation_pct_y; // compute rotation about y in radians

	char message[200] = { 0 };
	char formatstr[] = "\ncursor position -- xpos: %f, ypos %f angle about y: %f angle about x: %f";
	sprintf_s(message, 200, formatstr, xpos, ypos, rotAbouty, rotAboutx);
	OutputDebugString(message);
}

// handle glfw errors
void error_callback(int error, const char* description)
{
	puts(description);
}

int main()
{
	rotAboutx = 0.0;
	rotAbouty = 0.0;
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Volcano", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// setup cursor control of orientation.  hide the cursor and lock it to the specified window. 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_position_callback);

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

	scene.printGLString("Extensions", GL_EXTENSIONS);
	scene.printGLString("vendor", GL_VENDOR);
	scene.printGLString("renderer", GL_RENDERER);
	scene.printGLString("version", GL_VERSION);
	scene.printGLString("shading language version", GL_SHADING_LANGUAGE_VERSION);

	cout << "\nMove pointer to change camera location, <ESC> to quit";

	GLint64 majorVersion = 0;
	GLint64 minorVersion = 0;
	glGetInteger64v(GL_MAJOR_VERSION, &majorVersion);
	glGetInteger64v(GL_MINOR_VERSION, &minorVersion);

	scene.makePrograms("particle.vp", "particle.fp", "texmesh.vp", "texmesh.fp");

	// TODO: need equivalent of Java_com_neusoft_particle_ObjectJNI_jni_1pushTexture here
	// when ready with texture file loading
	//scene.initTextures();
	vector<unsigned char> image;
	unsigned width = 0, height = 0;
	//const char* bitmap_file = "Z:\\Software-dev\\volcano-windows\\volcano_ground.png";
	const char* bitmap_file = "volcano_ground.png";
	unsigned error = lodepng::decode(image, width, height, bitmap_file);
	
	if (error) { 
		cout << endl << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
	}

	scene.storeTexture(image.data(), width, height);

	scene.surfaceChanged(SCR_WIDTH, SCR_HEIGHT);
	//float orientation[] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	float* orientation;
	
	Matrix3x3 orientationMat;
	orientationMat.makeIdentity();
	orientation = orientationMat.get();

	scene.storeOrientation(orientation);
	//scene.sanityCheckSetup();

	//glfwSetCursorPos(window, static_cast<double>(SCR_WIDTH), static_cast<double>(SCR_HEIGHT));
	glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		//scene.drawSanityFrame();
		scene.drawFrame();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

		// update orientation matrix
		orientationMat.makeRotation(rotAboutx, rotAbouty, 0);
		orientation = orientationMat.get();
		scene.storeOrientation(orientation);
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