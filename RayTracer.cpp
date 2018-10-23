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
			// Send ray from the intercept point to each light source, if it collides with anything we know we are in shadow.
			glm::vec3 shadowCheckerOrigin = theHit.interceptPoint + EPSILON * theHit.surfaceNormal;
			Ray shadowChecker = Ray(shadowCheckerOrigin, glm::normalize(l->lightPosition - shadowCheckerOrigin));
			HitRecord shadowHit = VisibleIShape::findIntersection(shadowChecker, theScene.visibleObjects);

			while (shadowHit.material.alpha < 1.0f && shadowHit.t < FLT_MAX) {
				// If our shadow checker collides with a transparent object, continue the ray in the direction
				// it was headed, checking for either an opaque object or missing everything else.
				shadowCheckerOrigin = shadowHit.interceptPoint + EPSILON * shadowChecker.direction;
				shadowChecker = Ray(shadowCheckerOrigin, shadowChecker.direction);

				shadowHit = VisibleIShape::findIntersection(shadowChecker, theScene.visibleObjects);
			}

			bool shadow = (shadowHit.t < FLT_MAX);

			result += l->illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, theScene.camera->cameraFrame, shadow);
		}
	}
	else {
		result = defaultColor;
	}

	if (theHit.material.alpha < 1.0f) {
		// Handle transparency
		float a = theHit.material.alpha;
		Ray transRay = Ray(theHit.interceptPoint + EPSILON * ray.direction, ray.direction);
		result = glm::clamp(result * a + traceIndividualRay(transRay, theScene, recursionLevel) * (1.0f - a), 0.0f, 1.0f);
	}

	if (has_tex) {
		result = glm::clamp((result + texCol) / 2.0f, 0.0f, 1.0f); // Combine the colors with 50% weight to each one.
	}
	else {
		result = glm::clamp(result, 0.0f, 1.0f);
	}

	if (recursionLevel > 0) {
		// Handle reflections
		glm::vec3 I = glm::clamp(ray.direction, 0.0f, 1.0f);
		glm::vec3 N = glm::clamp(theHit.surfaceNormal, 0.0f, 1.0f);

		glm::vec3 reflectionDirection = I - 2.0f * glm::dot(I, N) * N;
		glm::vec3 reflectionOrigin = theHit.interceptPoint + EPSILON * reflectionDirection;

		Ray reflectionRay = Ray(reflectionOrigin, reflectionDirection);

		// At the moment, each reflection is 50% weaker than the previous
		color reflectionColor = traceIndividualRay(reflectionRay, theScene, recursionLevel - 1);
		return glm::clamp(result + reflectionColor * 0.5f, 0.0f, 1.0f);

	} return result;
}