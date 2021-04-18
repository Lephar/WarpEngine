#include "platformFramework.h"
#include <string.h>

GLFWwindow *window;

struct dimensions {
	int32_t width;
	int32_t height;
} dimensions;

struct controls {
	uint8_t keyW;
	uint8_t keyS;
	uint8_t keyA;
	uint8_t keyD;
	uint8_t keyQ;
	uint8_t keyE;
	uint8_t keyR;
	uint8_t keyF;
	double mouseX;
	double mouseY;
} controls;

void mouseCallback(GLFWwindow *handle, double x, double y) {
	(void) handle;

	controls.mouseX = x;
	controls.mouseY = y;
}

void keyboardCallback(GLFWwindow *handle, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
	(void) scancode;
	(void) mods;

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W)
			controls.keyW = 0;
		else if (key == GLFW_KEY_S)
			controls.keyS = 0;
		else if (key == GLFW_KEY_A)
			controls.keyA = 0;
		else if (key == GLFW_KEY_D)
			controls.keyD = 0;
		else if (key == GLFW_KEY_Q)
			controls.keyQ = 0;
		else if (key == GLFW_KEY_E)
			controls.keyE = 0;
		else if (key == GLFW_KEY_R)
			controls.keyR = 0;
		else if (key == GLFW_KEY_F)
			controls.keyF = 0;
	} else if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W)
			controls.keyW = 1;
		else if (key == GLFW_KEY_S)
			controls.keyS = 1;
		else if (key == GLFW_KEY_A)
			controls.keyA = 1;
		else if (key == GLFW_KEY_D)
			controls.keyD = 1;
		else if (key == GLFW_KEY_Q)
			controls.keyQ = 1;
		else if (key == GLFW_KEY_E)
			controls.keyE = 1;
		else if (key == GLFW_KEY_R)
			controls.keyR = 1;
		else if (key == GLFW_KEY_F)
			controls.keyF = 1;
		else if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(handle, 1);
	}
}

void resizeCallback(GLFWwindow *handle, int32_t width, int32_t height) {
	(void) handle;

	dimensions.width = width;
	dimensions.height = height;
}

void createWindow(void) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1280, 720, "Zero", NULL, NULL);

	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwGetCursorPos(window, &controls.mouseX, &controls.mouseY);
}

void getExtensions(uint32_t *count, const char ***names) {
	*names = glfwGetRequiredInstanceExtensions(count);
}

void createSurface(VkInstance instance, VkSurfaceKHR *surface) {
	glfwCreateWindowSurface(instance, window, NULL, surface);
}

void destroyWindow(void) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
