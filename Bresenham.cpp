#include "Bresenham.h"
#include <math.h>

void makePixel(int x, int y, float* PixelBuffer);
inline void max(int& a, int& b) {
	if (a > b) {
		int c = a;
		a = b;
		b = c;
	}
}

void Bresenham(int x1, int x2, int y1, int y2, float* PixelBuffer, int windowSizeX) {
	if (x1 == x2) {
		max(y1, y2);
		for (int i = y1; i < y2; i++) {
			makePixel(x1, i, PixelBuffer);
		}
	}
	else if (y1 == y2) {
		max(x1, x2);
		for (int i = x1; i < x2; i++) {
			makePixel(i, y1,  PixelBuffer);
		}
	}
	else {
		int dx = fabs(x2 - x1), dy = fabs(y2 - y1);
		int x, y;

		float m = (float(y2 - y1) / float(x2 - x1));
		if (m >= 1) {
			if (y1 > y2) {
				x = x2;
				y = y2;
				y2 = y1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dx2 = 2 * dx, Dx2y = 2 * (dx - dy);
			int p = 2 * dx - dy;
			makePixel(x, y, PixelBuffer);
			while (y < y2) {
				y++;
				if (p < 0)
					p += Dx2;
				else {
					x++;
					p += Dx2y;
				}
				makePixel(x, y, PixelBuffer);
			}
		}
		else if (m > 0 && m < 1) {
			if (x1 > x2) {
				x = x2;
				y = y2;
				x2 = x1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dy2 = 2 * dy, Dy2x = 2 * (dy - dx);
			int p = 2 * dy - dx;
			makePixel(x, y, PixelBuffer);
			while (x < x2) {
				x++;
				if (p < 0)
					p += Dy2;
				else {
					y++;
					p += Dy2x;
				}
				makePixel(x, y, PixelBuffer);
			}
		}
		else if (m <= -1) {
			if (y1 > y2) {
				x = x2;
				y = y2;
				y2 = y1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dx2 = 2 * dx, Dx2y = 2 * (dx - dy);
			int p = 2 * dx - dy;
			makePixel(x, y, PixelBuffer);
			while (y < y2) {
				y++;
				if (p < 0)
					p += Dx2;
				else {
					x--;
					p += Dx2y;
				}
				makePixel(x, y, PixelBuffer);
			}
		}
		else if (m < 0 && m > -1) {
			if (x1 > x2) {
				x = x2;
				y = y2;
				x2 = x1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dy2 = 2 * dy, Dy2x = 2 * (dy - dx);
			int p = 2 * dy - dx;
			makePixel(x, y, PixelBuffer);
			while (x < x2) {
				x++;
				if (p < 0)
					p += Dy2;
				else {
					y--;
					p += Dy2x;
				}
				makePixel(x, y, PixelBuffer);
			}
		}
	}
}