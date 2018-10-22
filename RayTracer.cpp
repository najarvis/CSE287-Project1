#include "RayTracer.h"
#include "IShape.h"

/**
 * @fn	RayTracer::RayTracer(const color &defa)
 * @brief	Constructs a raytracers.
 * @param	defa	The clear color.
 */

RayTracer::RayTracer(const color &defa)
	: defaultColor(defa) {
}

/**
 * @fn	void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth, const IScene &theScene) const
 * @brief	Raytrace scene
 * @param [in,out]	frameBuffer	Framebuffer.
 * @param 		  	depth	   	The current depth of recursion.
 * @param 		  	theScene   	The scene.
 */

void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth,
	const IScene &theScene) const {
	const RaytracingCamera &camera = *theScene.camera;
	const std::vector<VisibleIShapePtr> &objs = theScene.visibleObjects;
	const std::vector<PositionalLightPtr> &lights = theScene.lights;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {
		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {
			DEBUG_PIXEL = false;
			if (xDebug == x && yDebug == y) {
				DEBUG_PIXEL = true;
			}
			Ray ray = camera.getRay((float)x, (float)y);
			color colorForPixel = traceIndividualRay(ray, theScene, depth);
			frameBuffer.setColor(x, y, colorForPixel);
		}
	}

	frameBuffer.showColorBuffer();

}

/**
 * @fn	color RayTracer::traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const
 * @brief	Trace an individual ray.
 * @param	ray			  	The ray.
 * @param	theScene	  	The scene.
 * @param	recursionLevel	The recursion level.
 * @return	The color to be displayed as a result of this ray.
 */

color RayTracer::traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const {
	HitRecord theHit = VisibleIShape::findIntersection(ray, theScene.visibleObjects);
	color result = black;

	

	bool has_tex;
	color texCol;
	if (theHit.t < FLT_MAX) {
		if (theHit.texture != nullptr) {
			has_tex = true;
			float u = glm::clamp(theHit.u, 0.0f, 1.0f);
			float v = glm::clamp(theHit.v, 0.0f, 1.0f);
			texCol = theHit.texture->getPixel(u, v);
		}

		for (const PositionalLight *l : theScene.lights) {
			glm::vec3 shadowCheckerOrigin = theHit.interceptPoint + EPSILON * theHit.surfaceNormal;
			Ray shadowChecker = Ray(shadowCheckerOrigin, glm::normalize(l->lightPosition - shadowCheckerOrigin));
			bool shadow = VisibleIShape::findIntersection(shadowChecker, theScene.visibleObjects).t < FLT_MAX;

			result += l->illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, theScene.camera->cameraFrame, shadow);
		}
	}
	else {
		result = defaultColor;
	}
	if (has_tex)
		return glm::clamp((result + texCol) / 2.0f, 0.0f, 1.0f);
	return glm::clamp(result, 0.0f, 1.0f);
}