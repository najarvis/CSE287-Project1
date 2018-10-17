#include <ctime>
#include <vector>
#include "defs.h"
#include "Utilities.h"
#include "FrameBuffer.h"
#include "ColorAndMaterials.h"
#include "Rasterization.h"

#include <iostream>

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

void closed5x5Square(int x, int y, color C) {
	drawLine(frameBuffer, x - 2, y - 2, x + 2, y - 2, C);
	drawLine(frameBuffer, x - 2, y - 1, x + 2, y - 1, C);
	drawLine(frameBuffer, x - 2, y, x + 2, y, C);
	drawLine(frameBuffer, x - 2, y + 1, x + 2, y + 1, C);
	drawLine(frameBuffer, x - 2, y + 2, x + 2, y + 2, C);
}

void closed5x5Square(const glm::vec2 &centerPt, color C) {
	closed5x5Square(centerPt.x, centerPt.y, C);
}

void open5x5Square(const glm::vec2 &centerPt, color C) {
	int x = centerPt.x;
	int y = centerPt.y;
	drawLine(frameBuffer, x - 2, y - 2, x + 2, y - 2, C);
	drawLine(frameBuffer, x - 2, y + 2, x + 2, y + 2, C);

	drawLine(frameBuffer, x - 2, y - 2, x - 2, y + 2, C);
	drawLine(frameBuffer, x + 2, y - 2, x + 2, y + 2, C);
}

void pieChart(const glm::vec2 &centerPt, float rad, float perc, const color &C1, const color &C2) {
	// Initial Pie
	float length = M_2PI * perc;
	float initialAngle = M_PI - (0.5f * length);

	drawArc(frameBuffer, centerPt, rad, initialAngle, length, C1);
	glm::vec2 startPoint, endPoint;
	startPoint = pointOnCircle(centerPt, rad, initialAngle);
	endPoint = pointOnCircle(centerPt, rad, initialAngle + length);

	drawLine(frameBuffer, startPoint, centerPt, C1);
	drawLine(frameBuffer, endPoint, centerPt, C1);

	// Remaining Pie
	glm::vec2 newCenter;
	newCenter.x = centerPt.x + 10.0f;
	newCenter.y = centerPt.y;

	drawArc(frameBuffer, newCenter, rad, initialAngle + length, M_2PI - length, C2);

	startPoint = pointOnCircle(newCenter, rad, initialAngle);
	endPoint = pointOnCircle(newCenter, rad, initialAngle + length);

	drawLine(frameBuffer, startPoint, newCenter, C2);
	drawLine(frameBuffer, endPoint, newCenter, C2);
}

void render() {
	frameBuffer.clearColorAndDepthBuffers();
	closed5x5Square(50, 50, red);
	closed5x5Square(glm::vec2(100, 50), green);
	open5x5Square(glm::vec2(150, 50), blue);
	pieChart(glm::vec2(250, 100), 50, 0.25, red, green);
	frameBuffer.showColorBuffer();
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case ESCAPE:	glutLeaveMainLoop();
					break;
	}
}

int main(int argc, char *argv[]) {
	std::cout << "map(-0.8,-1,1,0,10) = " << map(-0.8, -1, 1, 0, 10) << std::endl;
	/*
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	GLuint world_Window = glutCreateWindow(__FILE__);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseUtility);

	frameBuffer.setClearColor(black);

	glutMainLoop();
	*/

	return 0;
}