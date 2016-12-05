// Neil Osbaldeston

#include <math.h>
#include "wtypes.h"
#include "Extras.h"

// get an angle between 2 vectors sitting on the X Z plane (in radians)
float getAngleXZ(float firstX, float firstZ, float secondX, float secondZ)
{
	// first //

	float distance = sqrtf(firstX * firstX + firstZ * firstZ);

	// normalise vector
	firstX /= distance;
	firstZ /= distance;

	// second //

	distance = sqrtf(secondX * secondX + secondZ * secondZ);

	// normalise vector
	secondX /= distance;
	secondZ /= distance;

	float dotProduct = firstX * secondX + firstZ * secondZ;

	float alpha = acosf(dotProduct);	// absolute value

	float crossProduct = firstZ * secondX - secondZ * firstX;	// get the angles sign

	if (crossProduct > 0.0f)
		return -alpha;

	return alpha;
}

// make sure acos doesn't return NAN
float safeAcosf(float x)
{
	if (x < -1.0f)
		x = -1.0f;
	else if (x > 1.0f)
		x = 1.0f;

	return acosf(x);
}

// Return a random number in the range between rangeMin and rangeMax inclusive
// range_min <= random number <= range_max
float random(int rangeMin, int rangeMax)
{
	float result = (float)rand() / (float)(RAND_MAX + 1);
	result *= (float)(rangeMax - rangeMin);
	result += rangeMin;

	return result;
}

// converts an angle in degrees to radians
float deg2Rads(float deg)
{
	return deg * 3.14159f / 180.0f;
}

// converts an angle in radians to degrees
float rads2Deg(float rads)
{
	return rads * 180.0f / 3.14159f;
}

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;

	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();

	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);

	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void rotateYandOrbit(float matrix[4][4], float rotation)
{
	// convert angle from degrees to radians
	float alpha = rotation * 3.142f / 180.0f;

	float sinAlpha = sinf(alpha);
	float cosAlpha = cosf(alpha);

	float tempX, tempZ;

	// Rotate the local x-axis (first row)
	tempX = matrix[0][0] * cosAlpha + matrix[0][2] * sinAlpha;
	tempZ = matrix[0][0] * -sinAlpha + matrix[0][2] * cosAlpha;
	matrix[0][0] = tempX;
	matrix[0][2] = tempZ;

	// Rotate the local y-axis (second row)
	tempX = matrix[1][0] * cosAlpha + matrix[1][2] * sinAlpha;
	tempZ = matrix[1][0] * -sinAlpha + matrix[1][2] * cosAlpha;
	matrix[1][0] = tempX;
	matrix[1][2] = tempZ;

	// Rotate the local z-axis (third row)
	tempX = matrix[2][0] * cosAlpha + matrix[2][2] * sinAlpha;
	tempZ = matrix[2][0] * -sinAlpha + matrix[2][2] * cosAlpha;
	matrix[2][0] = tempX;
	matrix[2][2] = tempZ;

	// Orbit position
	tempX = matrix[3][0] * cosAlpha + matrix[3][2] * sinAlpha;
	tempZ = matrix[3][0] * -sinAlpha + matrix[3][2] * cosAlpha;
	matrix[3][0] = tempX;
	matrix[3][2] = tempZ;
}


void rotateXandOrbit(float matrix[4][4], float rotation)
{
	// convert angle from degrees to radians
	float alpha = rotation * 3.142f / 180.0f;

	float sinAlpha = sinf(alpha);
	float cosAlpha = cosf(alpha);

	float tempY, tempZ;

	// Rotate the local x-axis (first row)
	tempY = matrix[0][1] * cosAlpha + matrix[0][2] * -sinAlpha;
	tempZ = matrix[0][1] * sinAlpha + matrix[0][2] * cosAlpha;
	matrix[0][1] = tempY;
	matrix[0][2] = tempZ;

	// Rotate the local y-axis (second row)
	tempY = matrix[1][1] * cosAlpha + matrix[1][2] * -sinAlpha;
	tempZ = matrix[1][1] * sinAlpha + matrix[1][2] * cosAlpha;
	matrix[1][1] = tempY;
	matrix[1][2] = tempZ;

	// Rotate the local z-axis (third row)
	tempY = matrix[2][1] * cosAlpha + matrix[2][2] * -sinAlpha;
	tempZ = matrix[2][1] * sinAlpha + matrix[2][2] * cosAlpha;
	matrix[2][1] = tempY;
	matrix[2][2] = tempZ;

	// Orbit position
	tempY = matrix[3][1] * cosAlpha + matrix[3][2] * -sinAlpha;
	tempZ = matrix[3][1] * sinAlpha + matrix[3][2] * cosAlpha;
	matrix[3][1] = tempY;
	matrix[3][2] = tempZ;
}