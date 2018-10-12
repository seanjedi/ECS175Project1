#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector> 
#include <string>
#include "DDA.h"
#include "Bresenham.h"

using namespace std;

///////////////////////////////////
//Polygon Object Class Definition//
///////////////////////////////////
class polygonObject {
public:
	int vertexCount;
	struct Vertex
	{
		float x;
		float y;
	};
	vector<Vertex> vertices;

	void setMatrix(int x) {
		vertexCount = x;
	}
	void addVertex(float x, float y) {
		Vertex newVertex = { x, y };
		vertices.push_back(newVertex);
	}
	void printPolygon() {
		for (int i = 0; i < vertexCount; i++) {
			cout << vertices[i].x << " " << vertices[i].y<< endl;
		}
	}
};

////////////////////
//Global Variables//
////////////////////
float *PixelBuffer;
string inputFile;
int windowSizeX, windowSizeY, style;
float Xmin, Xmax, Ymin, Ymax;
ifstream inFile;
vector<polygonObject> polygons;

//////////////////////////
//Function Defininitions//
//////////////////////////
void display();
void getSettings(int, char*[]);


/////////////////
//Main Function//
/////////////////
int main(int argc, char *argv[])
{
	//allocate new pixel buffer, need initialization!!
	getSettings(argc, argv);
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
///////////////////////
//Make Pixel Function//
///////////////////////
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
	//DDA(150, 30, 50, 170, PixelBuffer, windowSizeX);
	//Bresenham(150, 30, 30, 150, PixelBuffer, windowSizeX);

	
	//Bresenham(30.3, 50.8, 30.3, 50.8, PixelBuffer, windowSizeX);
	//Bresenham(70.5, 50.8, 30.3, 50.8, PixelBuffer, windowSizeX);
	//Bresenham(70.5, 50.8, 30, 10, PixelBuffer, windowSizeX);
	//Bresenham(50.8, 30.3, 10, 30.3, PixelBuffer, windowSizeX);
	//Bresenham(50.8, 50.8, 10, 50.8, PixelBuffer, windowSizeX);
	//Bresenham(70.5, 30.3, 30.3, 30.3, PixelBuffer, windowSizeX);
	
	//draws pixel on screen, width and height must match pixel buffer dimension
	glDrawPixels(windowSizeX, windowSizeY, GL_RGB, GL_FLOAT, PixelBuffer);

	//window refresh
	glFlush();
}
//////////////////////
//Handles File Input//
//////////////////////
void getSettings(int argc, char* argv[]){
		if (argc > 2) {
		cout << "Too Many Arguments!\nStopping Execution";
		exit(1);
	}
	else if (argc == 1) {
		//cout << "Specify Input File: ";
		//getline(cin, inputFile);
		//inFile.open(inputFile);
		inFile.open("input.txt");
		if (!inFile) {
			cerr << "Unable to open input file \nStopping Execution";
			exit(1);
		}
	}
	else {
		inFile.open(argv[1]);
		if (!inFile) {
			cerr << "Unable to open input file \nStopping Execution";
			exit(1);
		}
	}
		string space;
		int polygonCount, vertexCount, flag = 0;
		float x, y;

		inFile >> polygonCount;
		polygons.resize(polygonCount);
		for (int i = 0; i < polygonCount; i++) {
			getline(inFile, space);
			inFile >> vertexCount;
			polygons[i].setMatrix(vertexCount);
			for (int j = 0; j < vertexCount; j++) {
				inFile >> x;
				inFile >> y;
				polygons[i].addVertex(x, y);
			}
		}


	cout << "Specify Window Size for x: ";
	cin >> windowSizeX;
	cout << "Specify Window Size for y: ";
	cin >> windowSizeY;
	//cout << "Specify Viewport for Xmin: ";
	//cin >> Xmin;
	//cout << "Specify Viewport for Xmax: ";
	//cin >> Xmax;
	//cout << "Specify Viewport for Ymin: ";
	//cin >> Ymin;
	//cout << "Specify Viewport for Ymax: ";
	//cin >> Ymax;
	cout << "Choose either 1 or 2 on which algorithm you want to use to draw polygons: (1) DDA or (2) Bresenham algorithm\n";
	cin >> style;
}