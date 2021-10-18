#include<iostream>
#include<array>
#include<chrono>
#include<thread>
#include<glad/glad.h>
#include<GLFW/glfw3.h>


#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

const int WIDTH = 40;
const int HEIGHT = 40;

using namespace std::this_thread; 
using namespace std::chrono; 

//make opengl draw a triangle out of the given vertices
GLuint indices[] =
{
	0, 1, 2,
	0, 1, 3
};

int main() {
	//initialize glfw
	glfwInit();

	//using version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//using core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//800 x 800 window
	GLFWwindow* window = glfwCreateWindow(800, 800, "Game of Life", NULL, NULL);
	if (window == NULL) {
		std::cout << "window didn't work" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, 800, 800);

	//shader and graphic setup
	Shader shaderProgram("default.vert", "default.frag");
	VAO VAO1;
	VAO1.Bind();
	EBO EBO1(indices, sizeof(indices));

	//board setup
	std::array<std::array<char, HEIGHT>, WIDTH> board = {{0}};
	std::array<std::array<char, HEIGHT>, WIDTH> oboard = {{0}};

	//random board generation
	srand((unsigned int)time(NULL));
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (rand() % 3 == 0) board[i][j] = 1;
			else board[i][j] = 0;
		}
	}

	//main graphics loop:
	while (!glfwWindowShouldClose(window)) {
		glClearColor(.004f, .141f, .298f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.Activate();

		//copy board to old board
		std::copy(std::begin(board), std::end(board), std::begin(oboard));
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				//define square size, x and y coordinates of the top left corner of the board in window, and the gap size between squares
				float sqSize = .045f;
				float lCorner = -.90f;
				float tCorner = .90f;
				float gap = .005f;
				//set position of square and color as white
				GLfloat vertices[] = {
					lCorner + i * sqSize, tCorner - j * sqSize, 0.0f,					0.0f, .29f, .506f,
					lCorner + sqSize + i * sqSize - gap, tCorner - sqSize - j * sqSize + gap, 0.0f,		0.0f, .29f, .506f,
					lCorner + sqSize + i * sqSize - gap, tCorner - j * sqSize, 0.0f,			0.0f, .29f, .506f,
					lCorner + i * sqSize, tCorner - sqSize - j * sqSize + gap, 0.0f,			0.0f, .29f, .506f
				};
				//if cell is alive, make it yellow (also slight gradient)
				if (board[i][j] == 1) {
					for (int k = 0; k < 4; k++) {
						vertices[3 + k * 6] = .988f - i * .001f - j * .003f;
						vertices[4 + k * 6] = .702f - i * .001f - j * .003f;
						vertices[5 + k * 6] = .157f - i * .001f - j * .003f;
					}
				}

				//draw square
				VBO VBO1(vertices, sizeof(vertices));

				VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
				VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				//check number of neighbors
				int left = (j - 1 + WIDTH) % WIDTH;
				int right = (j + 1) % WIDTH;
				int up = (i - 1 + HEIGHT) % HEIGHT;
				int down = (i + 1) % HEIGHT;
				int neighbors = 0;
				if (oboard[up][left] == 1) neighbors++;
				if (oboard[up][j] == 1) neighbors++;
				if (oboard[up][right] == 1) neighbors++;
				if (oboard[i][left] == 1) neighbors++;
				if (oboard[i][right] == 1) neighbors++;
				if (oboard[down][left] == 1) neighbors++;
				if (oboard[down][j] == 1) neighbors++;
				if (oboard[down][right] == 1) neighbors++;
				//determine if cell is alive or dead
				if (oboard[i][j] == 1) {
					if (neighbors > 3 || neighbors < 2) board[i][j] = 0;
					else board[i][j] = 1;
				}
				else {
					if (neighbors == 3) board[i][j] = 1;
					else board[i][j] = 0;
				}
				VBO1.Delete();
			}
		}

		glfwSwapBuffers(window);

		glfwPollEvents();

		//control speed of simulation
		sleep_for(milliseconds(0));
	}

	//clean up graphics
	VAO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
