#include <ctime> 
#include <iostream> 
#include <algorithm>
#include <cmath>

#include "EShape.h"
#include "FrameBuffer.h"
#include "Raytracer.h"
#include "IScene.h"
#include "Light.h"
#include "Camera.h"
#include "Utilities.h"
#include "VertexOps.h"

PositionalLightPtr theLight = new PositionalLight(glm::vec3(2, 1, 3), pureWhiteLight);
std::vector<LightSourcePtr> lights = { theLight };

glm::vec3 position(0, 1, 5);
float azimuth = glm::radians(180.0f);
float elevation = 0.0f;
float moveAngle = azimuth;
bool isMoving = false;
bool twoViewOn = false;
const float SPEED = 0.1;

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

std::vector<glm::vec4> get_points() {
	std::vector<glm::vec4> points;
	points.push_back(glm::vec4(-3.0f, 1.0f, 1.0f, 1.0f));
	points.push_back(glm::vec4(3.0f, 1.0f, 1.0f, 1.0f));
	points.push_back(glm::vec4(3.0f, 4.0f, 1.0f, 1.0f));
	points.push_back(glm::vec4(-3.0f, 4.0f, 1.0f, 1.0f));
	return points;
}

EShapeData checkerBoard = EShape::createECheckerBoard(copper, tin, 10, 10, 10);
EShapeData plane = EShape::createEPlanes(silver, get_points());
EShapeData cone = EShape::createECone(greenPlastic, 1.5f, 2.0f, 16, 3);
EShapeData YCylinder = EShape::createECylinder(redRubber, 1.5f, 2.0f, 12, 4);
EShapeData ZCylinder = EShape::createECylinder(chrome, 0.5f, 3.0f, 16, 4);

void renderObjects() {
	VertexOps::render(frameBuffer, checkerBoard, lights, glm::mat3());
	VertexOps::render(frameBuffer, plane, lights, glm::mat3());
	VertexOps::render(frameBuffer, cone, lights, T(0.0f, 0.0f, 1.0f));
	VertexOps::render(frameBuffer, YCylinder, lights, T(-4.0f, 1.0f, -1.0f));
	VertexOps::render(frameBuffer, ZCylinder, lights, T(4.0f, 0.5f, -1.0f) * Rx(M_PI_2));
}

static void render() {
	frameBuffer.clearColorAndDepthBuffers();
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();
	//VertexOps::viewingTransformation = glm::lookAt(position, ORIGIN3D, Y_AXIS);

	float x, y, z;
	computeXYZFromAzimuthAndElevation(1.0f, azimuth, elevation, x, y, z);
	glm::vec3 lookAtPos = position + glm::vec3(x, y, z);
	VertexOps::viewingTransformation = glm::lookAt(position, lookAtPos, Y_AXIS);

	float AR = (float)width / height;
	VertexOps::projectionTransformation = glm::perspective(float(glm::radians(60.0)), AR, 0.1f, 100.0f);
	VertexOps::setViewport(0, width - 1, 0, height - 1);
	renderObjects();
	frameBuffer.showColorBuffer();
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	float AR = (float)width / height;

	VertexOps::setViewport(0, width - 1, 0, height - 1);
	VertexOps::projectionTransformation = glm::perspective(M_PI_3, AR, 0.5f, 80.0f);

	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	const float INC = 0.5;
	switch (key) {
	case 'X':
	case 'x': theLight->lightPosition.x += (isupper(key) ? INC : -INC);
				std::cout << theLight->lightPosition << std::endl;
				break;
	case 'Y':
	case 'y': theLight->lightPosition.y += (isupper(key) ? INC : -INC);
				std::cout << theLight->lightPosition << std::endl;
				break;
	case 'Z':
	case 'z':	theLight->lightPosition.z += (isupper(key) ? INC : -INC);
				std::cout << theLight->lightPosition << std::endl;
				break;
	case 'P':
	case 'p':	isMoving = !isMoving;
				break;
	case 'C':	// Do something here
	case 'c':	moveAngle = azimuth;
				break;
	case '?':	twoViewOn = !twoViewOn;
				break;
	case ESCAPE:
		glutLeaveMainLoop();
		break;
	default:
		std::cout << (int)key << "unmapped key pressed." << std::endl;
	}

	glutPostRedisplay();
}

static void special(int key, int x, int y) {
	static const float rotateInc = glm::radians(10.0);
	static const float minEL = -glm::radians(80.0);
	static const float maxEL = glm::radians(80.0);
	switch (key) {
	case(GLUT_KEY_LEFT):	azimuth = fmod(azimuth + rotateInc, M_2PI);
							break;
	case(GLUT_KEY_RIGHT):	azimuth = fmod(azimuth - rotateInc, M_2PI);
							break;
	case(GLUT_KEY_DOWN):	elevation = std::max(elevation - rotateInc, minEL);
							break;
	case(GLUT_KEY_UP):		elevation = std::min(elevation + rotateInc, maxEL);
							break;
	}
	glutPostRedisplay();
}

static void timer(int id) {
	// You should change this.
	if (isMoving) {
		position += glm::vec3(glm::sin(moveAngle), 0.0f, glm::cos(moveAngle)) * SPEED;
		// position.z -= SPEED;
		// std::cout << position << std::endl;
		// angle += glm::radians(5.0);
	}
	glutTimerFunc(100, timer, 0);
	glutPostRedisplay();
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	GLuint world_Window = glutCreateWindow(extractBaseFilename(__FILE__).c_str());
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(100, timer, 0);
	glutMouseFunc(mouseUtility);

	frameBuffer.setClearColor(lightGray);

	glutMainLoop();

	return 0;
}
