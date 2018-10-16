#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector> 
#include <string>
#include "DDA.h"
#include "Bresenham.h"

using namespace std;

////////////////////
//Global Variables//
////////////////////
float *PixelBuffer;
string inputFile;
int windowSizeX, windowSizeY, style, mode, polygonCount;
float Xmin, Xmax, Ymin, Ymax;
ifstream inFile;


//////////////////////////
//Function Defininitions//
//////////////////////////
void display();
void getSettings(int, char*[]);
void getSettings2();
void setPixelBuffer(float* PixelBuffer);
void makePixel(int x, int y, float* PixelBuffer);

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
	int filled = 0;
	float *PolygonBuffer;

	//Set count of vertices
	void setMatrix(int x) {
		vertexCount = x;
	}
	//Set polygon buffer size
	void setBuffer() {
		PolygonBuffer = new float[windowSizeX * windowSizeY * 3];
		setPixelBuffer(PolygonBuffer);

	}
	//Add a new vertex to the vertice vertex
	void addVertex(float x, float y) {
		Vertex newVertex = { x, y };
		vertices.push_back(newVertex);
	}
	//Print polygon to screen, This is for debugging
	void printPolygon() {
		for (int i = 0; i < vertexCount; i++) {
			cout << vertices[i].x << " " << vertices[i].y<< endl;
		}
	}

	//Set polygon status to filled
	void setFill() {
		filled = 1;
	}

	//Implement the Bresenham algorithm for the polygon
	void drawBresenham() {
		for (int i = 0; i < vertexCount; i++) {
			if (i == (vertexCount - 1)) {
				Bresenham(vertices[i].x, vertices[0].x, vertices[i].y, vertices[0].y, PolygonBuffer, windowSizeX);
			}
			else {
				Bresenham(vertices[i].x, vertices[i+1].x, vertices[i].y, vertices[i+1].y, PolygonBuffer, windowSizeX);
			}
		}
	}
	//Implement the DDA algorithm for the polygon
	void drawDDA() {
		for (int i = 0;  i < vertexCount; i++) {
			if (i == (vertexCount - 1)) {
				DDA(vertices[i].x, vertices[0].x, vertices[i].y, vertices[0].y, PolygonBuffer, windowSizeX);
			}
			else {
				DDA(vertices[i].x, vertices[i+1].x, vertices[i].y, vertices[i+1].y, PolygonBuffer, windowSizeX);
			}
		}
	}

	//Write polygon to the Pixel Buffer
	void drawPolygon() {
		for (int i = 0; i < windowSizeX; i++) {
			for (int j = 0; j < windowSizeY; j++) {
				PixelBuffer[((windowSizeX * j) + i) * 3] += PolygonBuffer[((windowSizeX * j) + i) * 3];
			}			
		}
	}

	int isVertex(int x, int y) {
		for (int i = 0; i < vertexCount; i++) {
			if ((int(vertices[i].x) == x) && (int(vertices[i].y) == y)){
				return i;
			}
		}
		return -1;
	}

	bool isRelativeMinMaxY(int index) {
		int oneLess = index - 1;
		int oneMore = index + 1;
		if (index == 0) {
			oneLess = vertexCount - 1;
		}
		if (index == vertexCount - 1) {
			oneMore = 0;
		}
		if (int(vertices[index].y) >= int(vertices[oneLess].y))
			if (int(vertices[index].y) >= int(vertices[oneMore].y))
				return true;

		if (int(vertices[index].y) <= int(vertices[oneLess].y))
			if (int(vertices[index].y) <= int(vertices[oneMore].y))
				return true;

		return false;
	}
	bool isRelativeMinMaxX(int index) {
		int oneLess = index - 1;
		int oneMore = index + 1;
		if (index == 0) {
			oneLess = vertexCount - 1;
		}
		if (index == vertexCount - 1) {
			oneMore = 0;
		}
		if (int(vertices[index].x) >= int(vertices[oneLess].x))
			if (int(vertices[index].x) >= int(vertices[oneMore].x))
				return true;

		if (int(vertices[index].x) <= int(vertices[oneLess].x))
			if (int(vertices[index].x) <= int(vertices[oneMore].x))
				return true;

		return false;
	}

	void translate(float tx, float ty) {
		for (int i = 0; i < vertexCount; i++) {
			vertices[i].x += tx;
			vertices[i].y += ty;
		}
	}

	void scale(float sx, float sy) {
		for (int i = 0; i < vertexCount; i++) {
			vertices[i].x *= sx;
			vertices[i].y *= sy;
		}
	}

	void rotate(double alpha) {
		//Convert alpha(Degrees) to Radians
		double pi = acos(-1);
		double rotation_radians = (double)alpha / 180.0 * pi;
		//Find Centroid
		double centerX = 0, centerY = 0;
		for (int i = 0; i < vertexCount; i++) {
			centerX += vertices[i].x;
			centerY += vertices[i].y;
		}
		centerX /= vertexCount;
		centerY /= vertexCount;
		//Translate by -C
		translate(-centerX, -centerY);
		//Translate in center by alpha
		for (int i = 0; i < vertexCount; i++) {
			int tempX = vertices[i].x;
			vertices[i].x = (float)round((tempX*cos(rotation_radians)) - (vertices[i].y * sin(rotation_radians)));
			vertices[i].y = (float)round((tempX*sin(rotation_radians)) + (vertices[i].y * cos(rotation_radians)));
		}
		//Translate back by C
		translate(centerX, centerY);
	}

	void writeBack(std::ofstream& file) {
		file << vertexCount << endl;;
		for (int i = 0; i < vertexCount; i++) {
			file << vertices[i].x << " " << vertices[i].y << endl;
		}
	}

	//If choosen to be filled, then rasterize, else return
	void Rasterize() {
		if (filled == 1) {
			for (int y = 0; y < windowSizeY; y++) {
				bool flag = false;
				int left = 0;
				for (int x = 0; x < windowSizeX; x++) {
					if (PolygonBuffer[((windowSizeX * y) + x) * 3] != 0) {
						int index = isVertex(x, y);
						//If vertex, check if Max or Min of X or Y
						if (index != -1) {
							if (isRelativeMinMaxY(index)) { //Don't change flag if min or max of Y
								continue;
							}
							else if (isRelativeMinMaxX(index)) { //Change flag if min or max of X
								if (flag) {
									flag = false;
								}
								else {
									flag = true;
								}
							}
						}
						else {
							//Check if it has a rightmost or a leftmost neighbor, if it does then don't change flag,
							//if it doesn't then change
							int right = PolygonBuffer[((windowSizeX * y) + x + 1) * 3];
							if (x != windowSizeX && left == 0 && right == 0) {
								if (flag) {
									flag = false;
								}
								else {
									flag = true;
								}
							}
						}
					}
					left = PolygonBuffer[((windowSizeX * y) + x) * 3];
					if (flag) {
						makePixel(x,y,PolygonBuffer);
					}
				}
			}
			//Done, don't do this again, but remember that it was filled, needed for transformation!
			return;
		}//Need's no Rasterization needed!
		else {
			return;
		}
	}
};

/////////////////////////////////
//Polygon Vector Initialization//
/////////////////////////////////
vector<polygonObject> polygons;


///////////////////////////
//Reset Pixel Buffer to 0//
///////////////////////////
void setPixelBuffer(float* PixelBuffer) {
	for (int i = 0; i < windowSizeX; i++) {
		for (int j = 0; j < windowSizeY; j++) {
			PixelBuffer[((windowSizeX * j) + i) * 3] = 0;
		}
	}
}

/////////////////
//Main Function//
/////////////////
int main(int argc, char *argv[])
{
	//allocate new pixel buffer, need initialization!!
	getSettings(argc, argv);
	PixelBuffer = new float[windowSizeX * windowSizeY * 3];
	setPixelBuffer(PixelBuffer);
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
	while (1) {
		display();
		getSettings2();
	}
	
	//glutDisplayFunc(display);

	glutMainLoop();//main display loop, will display until terminate
	return 0;
}
///////////////////////
//Make Pixel Function//
///////////////////////

void makePixel(int x, int y, float* PixelBuffer)
{
	PixelBuffer[((windowSizeX * y) + x) * 3] = 5;
}

/////////////////////////////////////////////////////////////////////////////
//main display loop, this function will be called again and again by OpenGL//
/////////////////////////////////////////////////////////////////////////////
void display()
{
	//Misc.
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	//Choose which draw method to use, then draw it to the pixel buffer
	//IF set for Rasterization, then it will rasterize the polygon
	setPixelBuffer(PixelBuffer);
	if (style == 1) {
		for (int i = 0; i < polygonCount; i++) {
			polygons[i].setBuffer();
			polygons[i].drawDDA();
			polygons[i].Rasterize();
			polygons[i].drawPolygon();
		}
	}
	else{
		for (int i = 0; i < polygonCount; i++) {
			polygons[i].setBuffer();
			polygons[i].drawBresenham();
			polygons[i].Rasterize();
			polygons[i].drawPolygon();
		}
	}
	
	//draws pixel on screen, width and height must match pixel buffer dimension
	glDrawPixels(windowSizeX, windowSizeY, GL_RGB, GL_FLOAT, PixelBuffer);

	//window refresh
	glFlush();
}

//////////////////////////////
//Handles File Input//////////
//Handles Initial User Input//
//////////////////////////////
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

		cout << "Specify Window Size for x: ";
		cin >> windowSizeX;
		cout << "Specify Window Size for y: ";
		cin >> windowSizeY;

		string space;
		int vertexCount;
		float x, y;

		inFile >> polygonCount;
		polygons.resize(polygonCount);
		for (int i = 0; i < polygonCount; i++) {
			getline(inFile, space);
			inFile >> vertexCount;
			polygons[i].setMatrix(vertexCount);
			polygons[i].setBuffer();
			for (int j = 0; j < vertexCount; j++) {
				inFile >> x;
				inFile >> y;
				polygons[i].addVertex(x, y);
			}
		}


	cout << "Choose either 1 or 2 on which algorithm you want to use to draw polygons: (1) DDA or (2) Bresenham algorithm\nChoose:";
	cin >> style;
	if (style != 1 && style != 2) {
		cout << "Please choose either 1 or 2\n1 for DDA\n2for Bresenham\nDefault will be Bresenham if none chosen\nChoose:";
		cin >> style;
	}
	getSettings2();

}
void getSettings2() {
	int choice = 4;
	int id = 0;
	cout << "Which operation would you like to do next?\n1) Rasterization\n2) Clipping\n3) Transformations\n4) Display\n5) Write to a File\n6) exit\nChoose:";
	cin >> choice;
	while (choice < 1 || choice > 6) {
		cout << "Wrong Choice, Please choose a possible Action!\nChoose:";
		cin >> choice;
	}

	if (choice == 1) {//Rasterization
		cout << "Which polygon would you like to fill (id's 1 to " << polygonCount << ")\nID:";
		while (id <= 0 || id > polygonCount) {
			cin >> id;
			if (id > polygonCount || id <= 0) {
				cout << "Wrong ID, Please choose a possible ID!\nID:";
			}
		}
		polygons[id-1].setFill();
	}
	else if (choice == 2) {//Clipping
		//cout << "Specify Viewport for Xmin: ";
		//cin >> Xmin;
		//cout << "Specify Viewport for Xmax: ";
		//cin >> Xmax;
		//cout << "Specify Viewport for Ymin: ";
		//cin >> Ymin;
		//cout << "Specify Viewport for Ymax: ";
		//cin >> Ymax;
	}
	else if (choice == 3) {//Transformations
		int function;
		cout << "Which Transformations would you like to do?" << endl;
		cout << "1) Translation\n2) Scale\n3) Rotation\n4) Display(none of the above)\nChoice:";
		cin >> function;
		while (function < 1 || function > 4) {
			cout << "Wrong Action, Please choose a possible Action!\nChoose:";
			cin >> function;
		}
		if (function == 1) {//Translate
			cout << "Which polygon would you like to translate (id's 1 to " << polygonCount << ")\nID:";
			while (id <= 0 || id > polygonCount) {
				cin >> id;
				if (id > polygonCount || id <= 0) {
					cout << "Wrong ID, Please choose a possible ID!\nID:";
				}
			}
			float tx, ty;
			cout << "tx: ";
			cin >> tx;
			cout << "ty: ";
			cin >> ty;
			polygons[id - 1].translate(tx, ty);
		}
		if (function == 2) {//Scaling
			cout << "Which polygon would you like to scale (id's 1 to " << polygonCount << ")\nID:";
			while (id <= 0 || id > polygonCount) {
				cin >> id;
				if (id > polygonCount || id <= 0) {
					cout << "Wrong ID, Please choose a possible ID!\nID:";
				}
			}
			float sx, sy;
			cout << "sx: ";
			cin >> sx;
			cout << "sy: ";
			cin >> sy;
			polygons[id - 1].scale(sx, sy);
		}
		if (function == 3) {//Rotation
			cout << "Which polygon would you like to Rotate (id's 1 to " << polygonCount << ")\nID:";
			while (id <= 0 || id > polygonCount) {
				cin >> id;
				if (id > polygonCount || id <= 0) {
					cout << "Wrong ID, Please choose a possible ID!\nID:";
				}
			}
			float alpha;
			cout << "Degrees Rotate: ";
			cin >> alpha;
			polygons[id - 1].rotate(alpha);
		}
	}
	else if (choice == 5) {//File Write-back
		string fileName;
		cout << "File Name you want to write to: ";
		cin >> fileName;
		ofstream myfile(fileName);
		if (myfile.is_open()) {
			myfile << polygonCount << endl;
			for (int i = 0; i < polygonCount; i++) {
				myfile << endl;
				polygons[i].writeBack(myfile);
			}
		}
		else {
			cout << "Error! Unable to open file!";
			exit(-1);
		}
		myfile.close();
	}
	else if (choice == 6) {//Exit
		exit(0);
	}
	return;
}