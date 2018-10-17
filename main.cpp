#include <ctime>
#include <vector>
#include "defs.h"
#include "Utilities.h"
#include "FrameBuffer.h"
#include "ColorAndMaterials.h"

int main(int argc, char *argv[]) {
	// swap
	std::cout << "Testing swap between a = 2.0 and b = 92.3" << std::endl;
	float a = 2.0f;
	float b = 92.3f;
	swap(a, b);
	std::cout << a << std::endl;
	std::cout << b << std::endl;
	std::cout << std::endl;

	// approximatelyEqual
	std::cout << "Testing approximatelyEqual between 1.0 and 1.000001" << std::endl;
	std::cout << approximatelyEqual(1.0f, 1.000001f) << std::endl;
	std::cout << std::endl;
	
	// approximatelyZero
	std::cout << "Testing approximatelyZero for 0.000001" << std::endl;
	std::cout << approximatelyZero(0.000001f) << std::endl;
	std::cout << std::endl;

	// normalizeDegrees
	std::cout << "Testing normalizeDegrees for: 0, 1, -1, -721" << std::endl;
	std::cout << normalizeDegrees(0) << std::endl;
	std::cout << normalizeDegrees(1) << std::endl;
	std::cout << normalizeDegrees(-1) << std::endl;
	std::cout << normalizeDegrees(-721) << std::endl;
	std::cout << std::endl;

	// normalizeRadians
	std::cout << "Testing normalizeRadians for: 0, 1, 3PI, 31PI" << std::endl;
	std::cout << normalizeRadians(0) << std::endl;
	std::cout << normalizeRadians(1) << std::endl;
	std::cout << normalizeRadians(M_PI * 3.0f) << std::endl;
	std::cout << normalizeRadians(M_PI * 31.0f) << std::endl;
	std::cout << std::endl;

	// rad2deg
	std::cout << "Testing rad2deg for: 0, M_PI_2, 2/3 M_PI, -3/4 M_PI" << std::endl;
	std::cout << rad2deg(0) << std::endl;
	std::cout << rad2deg(M_PI_2) << std::endl;
	std::cout << rad2deg((2.0f / 3.0f) * M_PI) << std::endl;
	std::cout << rad2deg((-3.0f / 2.0f) * M_PI) << std::endl;
	std::cout << std::endl;

	// deg2rad
	std::cout << "Testing deg2rad for: 0, 90, 120, -270" << std::endl;
	std::cout << deg2rad(0) << std::endl;
	std::cout << deg2rad(90.0f) << std::endl;
	std::cout << deg2rad(120.0f) << std::endl;
	std::cout << deg2rad(-270.0f) << std::endl;
	std::cout << std::endl;

	// min
	std::cout << "Testing min for: 0, -10, 10" << std::endl;
	std::cout << min(0.0f, -10.0f, 10.0f) << std::endl;
	std::cout << std::endl;

	// max
	std::cout << "Testing max for: 0, -10, 10" << std::endl;
	std::cout << max(0.0f, -10.0f, 10.0f) << std::endl;
	std::cout << std::endl;

	// pointOnUnitCircle
	std::cout << "Testing pointOnUnitCircle for M_PI_3: " << std::endl;
	float x, y;
	pointOnUnitCircle(M_PI_3, x, y);
	std::cout << "(" << x << ", " << y << std::endl;
	std::cout << std::endl;

	// areaOfTriangle
	std::cout << "Testing areaOfTriangle for 2, 3, 4: " << std::endl;
	std::cout << areaOfTriangle(2.0f, 3.0f, 4.0f) << std::endl;
	std::cout << std::endl;

	return 0;
}