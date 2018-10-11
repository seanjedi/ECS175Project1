#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include "DDA.h"
#include "Bresenham.h"

using namespace std;

float *PixelBuffer;
char* inputFile;
int windowSizeX, windowSizeY, style;

ifstream inFile;

void display();
void getSettings();

int main(int argc, char *argv[])
{
	//allocate new pixel buffer, need initialization!!
	getSettings();
	PixelBuffer = new float[windowSizeX * windowSizeY * 3];
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	//set window size to windowSizeX by windowSizeX
	glutInitWindowSize(windowSizeX, windowSizeY);
	//set window position
	glutInitWindowPosition(100, 100);

	//create and set main window title
	int MainWindow = glutCreateWindow("Hello Graphics!!");
	glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
	//sets display function
	glutDisplayFunc(display);

	glutMainLoop();//main display loop, will display until terminate
	return 0;
}

void makePixel(int x, int y, int rows, float* PixelBuffer)
{
	PixelBuffer[((rows * y) + x) * 3] = 5;
}

//main display loop, this function will be called again and again by OpenGL
void display()
{
	//Misc.
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	//DDA(30, 50, 30, 50, PixelBuffer, windowSizeX);
	//DDA(70, 50, 30, 50, PixelBuffer, windowSizeX);
	//DDA(70, 50, 30, 10, PixelBuffer, windowSizeX);
	//DDA(50, 30, 10, 30, PixelBuffer, windowSizeX);
	//DDA(50, 50, 10, 50, PixelBuffer, windowSizeX);
	//DDA(70, 30, 30, 30, PixelBuffer, windowSizeX);

	//DDA(30, 150, 50, 70, PixelBuffer, windowSizeX);
	//Bresenham(30, 150, 50, 70, PixelBuffer, windowSizeX);
	DDA(150, 30, 50, 170, PixelBuffer, windowSizeX);
	//Bresenham(150, 30, 30, 150, PixelBuffer, windowSizeX);

	//Bresenham(30, 50, 30, 50, PixelBuffer, windowSizeX);
	//Bresenham(70, 50, 30, 50, PixelBuffer, windowSizeX);
	//Bresenham(70, 50, 30, 10, PixelBuffer, windowSizeX);
	//Bresenham(50, 30, 10, 30, PixelBuffer, windowSizeX);
	//Bresenham(50, 50, 10, 50, PixelBuffer, windowSizeX);
	//Bresenham(70, 30, 30, 30, PixelBuffer, windowSizeX);
	
	//draws pixel on screen, width and height must match pixel buffer dimension
	glDrawPixels(windowSizeX, windowSizeY, GL_RGB, GL_FLOAT, PixelBuffer);

	//window refresh
	glFlush();
}

void getSettings(){
	//	if (argc > 2) {
//		cout << "Too Many Arguments!\nStopping Execution";
//		exit(1);
//	}
//	else if (argc == 1) {
//		cout << "Specify Input File: ";
//		cin >> inputFile;
//		inFile.open(inputFile);
//		if (!inFile) {
//			cerr << "Unable to open file " + inputFile + "\nStopping Execution";
//			exit(1);
//		}
//	}
//	else {
//		inputFile = argv[1];
//		inFile.open(inputFile);
//		if (!inFile) {
//			cerr << "Unable to open file " + inputFile + "\nStopping Execution";
//			exit(1);
//		}
//	}
	cout << "Specify Window Size for x: ";
	cin >> windowSizeX;
	cout << "Specify Window Size for y: ";
	cin >> windowSizeY;
	cout << "Choose either 1 or 2 on which algorithm you want to use to draw polygons: (1) DDA or (2) Bresenham algorithm\n";
	cin >> style;
}