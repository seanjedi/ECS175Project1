#include "DDA.h"
#include <math.h>

void makePixel(int x, int y, int rows, float* PixelBuffer);
inline int round(const float a) { return int(a + 0.5); }
inline void max(int& a, int& b){
	if (a > b) {
		int c = a;
		a = b;
		b = c;
	}
}

void DDA(int x1, int x2, int y1, int y2, float* PixelBuffer, int windowSizeX) {
	if (x1 == x2) {
		max(y1, y2);
		for (int i = y1; i < y2; i++) {
			makePixel(x1, i, windowSizeX, PixelBuffer);
		}
	}else if (y1 == y2) {
		max(x1, x2);
		for (int i = x1; i < x2; i++) {
			makePixel(i, y1, windowSizeX, PixelBuffer);
		}
	} else {
		int dx = x2 - x1, dy = y2 - y1, steps, k;
		float xInc, yInc, x = x1, y = y1;

		if (fabs(dx) > fabs(dy))
			steps = fabs(dx);
		else
			steps = fabs(dy);
		xInc = float(dx) / float(steps);
		yInc = float(dy) / float(steps);

		makePixel(x1, y1, windowSizeX, PixelBuffer);
		for (k = 0; k < steps; k++) {
			x += xInc;
			y += yInc;
			makePixel(round(x), round(y), windowSizeX, PixelBuffer);
		}
	}
}