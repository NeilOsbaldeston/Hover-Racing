// Neil Osbaldeston

#pragma once

// get an angle between 2 vectors sitting on the X Z plane (in radians)
float getAngleXZ(float firstX, float firstZ, float secondX, float secondZ);

// make sure acos doesn't return NAN
float safeAcosf(float x);

// Return a random number in the range between rangeMin and rangeMax inclusive
float random(int rangeMin, int rangeMax);

// converts an angle in degrees to radians
float deg2Rads(float deg);

// converts an angle in radians to degrees
float rads2Deg(float rads);

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical);

void rotateYandOrbit(float matrix[4][4], float rotation);
void rotateXandOrbit(float matrix[4][4], float rotation);
void rotateLocalX(float matrix[4][4], float rotation);