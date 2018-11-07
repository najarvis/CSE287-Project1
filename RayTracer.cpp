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
	const IScene &theScene, int antiAliasing) const {
	const RaytracingCamera &camera = *theScene.camera;
	const std::vector<VisibleIShapePtr> &objs = theScene.visibleObjects;
	const std::vector<PositionalLightPtr> &lights = theScene.lights;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {
		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {
			DEBUG_PIXEL = false;
			if (xDebug == x && yDebug == y) {
				DEBUG_PIXEL = true;
			}

			// Handle Anti-aliasing
			color avgColor = black;

			int offset = antiAliasing / 2; // <! the number of sample rays on each side of the default ray
			for (int yAnti = 0; yAnti < antiAliasing; yAnti++) {
				for (int xAnti = 0; xAnti < antiAliasing; xAnti++) {
					/*  ___________
					 * | o | o | o | This box represents a single pixel, and the dashes represent the
					 * |-----------| origin of each antiAliasing ray.
					 * | o | o | o |
					 * |-----------| The center point is the original (antiAliasing=1) ray, and you
					 * | o | o | o | can see there are [antiAliasing / 2] rays above, below, left, and right
					 *  -----------  of the original ray.
					 */
					 // These coordinates are the origin of each new ray.
					float xPos = x + (-offset + xAnti) / (float)antiAliasing;
					float yPos = y + (-offset + yAnti) / (float)antiAliasing;

					Ray ray = camera.getRay((float)xPos, (float)yPos);
					avgColor += traceIndividualRay(ray, theScene, depth);
				}
			}
			color colorForPixel = avgColor * (1.0f / (antiAliasing * antiAliasing));
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

	color texCol;
	if (theHit.t < FLT_MAX) {

		// Handle lighting + shadows
		for (const PositionalLight *l : theScene.lights) {
			// Send ray from the intercept point to each light source, if it collides with anything we know we are in shadow.
			glm::vec3 shadowCheckerOrigin = theHit.interceptPoint + EPSILON * theHit.surfaceNormal;
			Ray shadowChecker = Ray(shadowCheckerOrigin, glm::normalize(l->lightPosition - shadowCheckerOrigin));
			HitRecord shadowHit = VisibleIShape::findIntersection(shadowChecker, theScene.visibleObjects);

			bool shadow = (shadowHit.t < glm::distance(l->lightPosition, theHit.interceptPoint));

			color matContrib;
			matContrib = l->illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, theScene.camera->cameraFrame, shadow);

			// Handle textures
			if (theHit.texture != nullptr) {
				float u = glm::clamp(theHit.u, 0.0f, 1.0f);
				float v = glm::clamp(theHit.v, 0.0f, 1.0f);
				texCol = theHit.texture->getPixel(u, v);
				color texContrib = l->illuminate(theHit.interceptPoint, theHit.surfaceNormal, texCol, theScene.camera->cameraFrame, shadow);
				result += glm::clamp((matContrib + texContrib) / 2.0f, 0.0f, 1.0f);
				RayTracer::adjustForTransparency(ray, theScene, theHit, result);
			}
			else {
				result += matContrib;
				RayTracer::adjustForTransparency(ray, theScene, theHit, result);
			}
		}
	}
	else {
		// If we don't collide with any visible objects, return the 'sky' color.
		result = defaultColor;
		RayTracer::adjustForTransparency(ray, theScene, theHit, result);
	}

	/*
	// Handle transparent objects
	HitRecord transHit = VisibleIShape::findIntersection(ray, theScene.transparentObjects);
	if (transHit.t < FLT_MAX) {
		if (transHit.t < theHit.t) {
			float a = transHit.material.alpha;
			result = glm::clamp(result * (1.0f - a) + transHit.material.ambient * a, 0.0f, 1.0f);
		}
	}
	
	*/

	if (recursionLevel > 0) {
		// Handle reflections
		glm::vec3 I = glm::normalize(ray.direction);
		glm::vec3 N = glm::normalize(theHit.surfaceNormal);

		glm::vec3 reflectionDirection = I - 2.0f * glm::dot(I, N) * N;
		
		glm::vec3 reflectionOrigin = theHit.interceptPoint + EPSILON * theHit.surfaceNormal;

		Ray reflectionRay = Ray(reflectionOrigin, reflectionDirection);

		// At the moment, each reflection is 50% weaker than the previous
		color reflectionColor = traceIndividualRay(reflectionRay, theScene, recursionLevel - 1);
		return glm::clamp(result + reflectionColor * 0.5f, 0.0f, 1.0f);

	} return glm::clamp(result, 0.0f, 1.0f);
}

void RayTracer::adjustForTransparency(const Ray &ray, const IScene &theScene, const HitRecord &theHit, color &result) const {
	// Handle transparent objects
	HitRecord transHit = VisibleIShape::findIntersection(ray, theScene.transparentObjects);
	if (transHit.t < FLT_MAX) {
		if (transHit.t < theHit.t) {
			float a = transHit.material.alpha;
			result = glm::clamp(result * (1.0f - a) + transHit.material.ambient * a, 0.0f, 1.0f);
		}
	}
}