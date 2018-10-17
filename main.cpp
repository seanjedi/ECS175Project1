#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector> 
#include <cmath>
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
float viewXmin, viewXmax, viewYmin, viewYmax;
ifstream inFile;
bool clipping = false;


//////////////////////////
//Function Defininitions//
//////////////////////////
void display();
void getSettings(int, char*[]);
void getSettings2();
void setPixelBuffer(float* PixelBuffer);
void makePixel(int x, int y, float* PixelBuffer);

//////////////////////////////
//Cohen-Sutherland Functions//
//////////////////////////////
//Make a new class for world Coodinate points
class wcPt2D {
public:
	float x, y;
};

//Make a round function for class
inline GLint roundwcPt2D(const float a) { return GLint(a + 0.5); }

//Make byte encodings for left, right, bottom, and top
const GLint winLeftBitCode = 0x1;
const GLint winRightBitCode = 0x2;
const GLint winBottomBitCode = 0x4;
const GLint winTopBitCode = 0x8;
//Make functions to check if lines are inside of outside viewport
inline GLint inside(GLint code) { return GLint(!code); }
inline GLint reject(GLint code1, GLint code2)
{
	return GLint(code1 & code2);
}
inline GLint accept(GLint code1, GLint code2)
{
	return GLint(!(code1 | code2));
}
//Make encodings for each point on where it lays outside the viewport
GLubyte encode(wcPt2D pt, wcPt2D winMin, wcPt2D winMax)
{
	GLubyte code = 0x00;
	if (pt.x < winMin.x)
		code = code | winLeftBitCode;
	if (pt.x > winMax.x)
		code = code | winRightBitCode;
	if (pt.y < winMin.y)
		code = code | winBottomBitCode;
	if (pt.y > winMax.y)
		code = code | winTopBitCode;
	return (code);
}
//Swap points
void swapPts(wcPt2D * p1, wcPt2D * p2)
{
	wcPt2D tmp;
	tmp = *p1; *p1 = *p2; *p2 = tmp;
}
void swapCodes(GLubyte * c1, GLubyte * c2)
{
	GLubyte tmp;
	tmp = *c1; *c1 = *c2; *c2 = tmp;
}
//Cohen Sutherland Algorithm
void CohenSutherland(wcPt2D winMin, wcPt2D winMax, wcPt2D p1, wcPt2D p2, float* PolygonBuffer)
{
	GLubyte code1, code2;
	GLint done = false, plotLine = false;
	float m;
	while (!done) {
		code1 = encode(p1, winMin, winMax);
		code2 = encode(p2, winMin, winMax);
		if (accept(code1, code2)) {
			done = true;
			plotLine = true;
		}
		else
			if (reject(code1, code2))
				done = true;
			else {
				/* Label the endpoint outside the display window as p1. */
				if (inside(code1)) {
					swapPts(&p1, &p2);
					swapCodes(&code1, &code2);
				}
				/* Use slope m to find line-clipEdge intersection. */
				if (p2.x != p1.x)
					m = (p2.y - p1.y) / (p2.x - p1.x);
				if (code1 & winLeftBitCode) {
					p1.y += (winMin.x - p1.x) * m;
					p1.x = winMin.x;
				}
				else
					if (code1 & winRightBitCode) {
						p1.y += (winMax.x - p1.x) * m;
						p1.x = winMax.x;
					}
					else
						if (code1 & winBottomBitCode) {
							/* Need to update p1.x for nonvertical lines only. */
							if (p2.x != p1.x)
								p1.x += (winMin.y - p1.y) / m;
							p1.y = winMin.y;
						}
						else
							if (code1 & winTopBitCode) {
								if (p2.x != p1.x)
									p1.x += (winMax.y - p1.y) / m;
								p1.y = winMax.y;
							}
			}
	}
	if (plotLine)
		Bresenham(roundwcPt2D(p1.x), roundwcPt2D(p2.x), roundwcPt2D(p1.y), roundwcPt2D(p2.y), PixelBuffer, windowSizeX);
}


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
	vector<Vertex> cutout;
	int cutout_size;
	bool filled = false;
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
		filled = true;
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

	void translate(float tx, float ty) {
		for (int i = 0; i < vertexCount; i++) {
			vertices[i].x += tx;
			vertices[i].y += ty;
		}
	}

	void scale(float sx, float sy) {
		double centerX = 0, centerY = 0;
		//Find Centroid
		for (int i = 0; i < vertexCount; i++) {
			centerX += vertices[i].x;
			centerY += vertices[i].y;
		}
		centerX /= vertexCount;
		centerY /= vertexCount;
		//Translate by -C
		translate(-centerX, -centerY);
		for (int i = 0; i < vertexCount; i++) {
			vertices[i].x *= sx;
			vertices[i].y *= sy;
		}
		//Translate back by C
		translate(centerX, centerY);
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

	//Write Polygon Buffer to output file!
	void writeBack(std::ofstream& file) {
		file << vertexCount << endl;;
		for (int i = 0; i < vertexCount; i++) {
			file << vertices[i].x << " " << vertices[i].y << endl;
		}
	}

	//////////////////////////
	//Raterization Functions//
	//////////////////////////
	//Find if a point is a vertex
	int isVertex(int x, int y) {
		for (int i = 0; i < vertexCount; i++) {
			if ((int(vertices[i].x) == x) && (int(vertices[i].y) == y)) {
				return i;
			}
		}
		return -1;
	}
	//Find if a vertex is a relative min or max of Y
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
	//Find if a vertex is a relative min or max or X
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

	//If choosen to be filled, then rasterize, else return
	void Rasterize() {
		if (filled) {
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
							int right = 0;
							if (x != windowSizeX) {
								right = PolygonBuffer[((windowSizeX * y) + x + 1) * 3];
							}

							if (left == 0 && right == 0) {
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
	/////////////////////////////////
	//Sutherland-Hodgeman Algorithm//
	/////////////////////////////////
	// Returns x-value of point of intersectipn of two lines 
	int x_intersect(int x1, int y1, int x2, int y2,
		int x3, int y3, int x4, int y4)
	{
		int num = (x1*y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3 * x4);
		int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		return num / den;
	}

	// Returns y-value of point of intersectipn of two lines 
	int y_intersect(int x1, int y1, int x2, int y2,
		int x3, int y3, int x4, int y4)
	{
		int num = (x1*y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3 * x4);
		int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		return num / den;
	}

	// This functions clips all the edges, one clip at a time edge of clipping area 
	void clipEdge(int x1, int y1, int x2, int y2)
	{
		// (ix,iy),(kx,ky) are the co-ordinate values of 
		// the points 
		for (int i = 0; i < vertexCount; i++)
		{
			// i and k form a line in polygon 
			int k = (i + 1) % vertexCount;
			int ix = vertices[i].x, iy = vertices[i].y;
			int kx = vertices[k].x, ky = vertices[k].y;

			// Calculating position of first point 
			// w.r.t. clipper line 
			int i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

			// Calculating position of second point 
			// w.r.t. clipper line 
			int k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

			// Case 1 : When both points are inside 
			if (i_pos < 0 && k_pos < 0)
			{
				//Only second point is added
				Vertex newVertex = { float(kx), float(ky) };
				cutout.push_back(newVertex);
				cutout_size++;
			}

			// Case 2: When only first point is outside 
			else if (i_pos >= 0 && k_pos < 0)
			{
				// Point of intersection with edge 
				// and the second point is added 
				int xintercept = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
				int yintercept= y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
				Vertex newVertex = { float(xintercept), float(yintercept) };
				cutout.push_back(newVertex);
				cutout_size++;

				newVertex = { float(kx), float(ky) };
				cutout.push_back(newVertex);
			}

			// Case 3: When only second point is outside 
			else if (i_pos < 0 && k_pos >= 0)
			{
				//Only point of intersection with edge is added 
				int xintercept = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
				int yintercept = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
				Vertex newVertex = { float(xintercept), float(yintercept) };
				cutout.push_back(newVertex);
				cutout_size++;
			}

			// Case 4: When both points are outside 
			else
			{
				//No points are added 
			}
		}
	}

	// Implements Sutherland–Hodgman algorithm 
	void sutherlandHodgeman()
	{
		int clipper_points[][2] = { {int(viewXmin),int(viewYmin)}, {int(viewXmin),int(viewYmax)}, {int(viewXmax),int(viewYmax)}, {int(viewXmax),int(viewYmin)} };
		//i and k are two consecutive indexes 
		for (int i = 0; i < 4; i++)
		{
			int k = (i + 1) % 4;
			clipEdge(clipper_points[i][0], clipper_points[i][1], clipper_points[k][0], clipper_points[k][1]);
		}	
	}


	//Clipping Algorithms!
	void clip() {
		//If it is supposed to be filled (A polygon)
		//Implement Sutherland-Hodgeman Algorithm!
		if (filled) {
			cutout_size = 0;
			cutout.resize(0);
			sutherlandHodgeman();
			setPixelBuffer(PolygonBuffer);
			for (int i = 0; i < cutout_size; i++) {
				if (i == (cutout_size - 1)) {
					Bresenham(cutout[i].x, cutout[0].x, cutout[i].y, cutout[0].y, PolygonBuffer, windowSizeX);
				}
				else {
					Bresenham(cutout[i].x, cutout[i + 1].x, cutout[i].y, cutout[i + 1].y, PolygonBuffer, windowSizeX);
				}
			}
		}
		else {//Else implement Cohen-Sutherland Algorithm!
			setPixelBuffer(PolygonBuffer);
			for (int i = 0; i < vertexCount; i++) {
				if (i == (vertexCount - 1)) {
					wcPt2D winMin = { viewXmin,viewYmin };
					wcPt2D winMax = { viewXmax, viewYmax };
					wcPt2D p1 = { vertices[i].x, vertices[i].y };
					wcPt2D p2 = { vertices[0].x, vertices[0].y };
					CohenSutherland(winMin, winMax, p1, p2, PolygonBuffer);
				}
				else {
					wcPt2D winMin = { viewXmin,viewYmin };
					wcPt2D winMax = { viewXmax, viewYmax };
					wcPt2D p1 = { vertices[i].x, vertices[i].y };
					wcPt2D p2 = { vertices[i + 1].x, vertices[i + 1].y };
					CohenSutherland(winMin, winMax, p1, p2, PolygonBuffer);
				}
			}
			
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
			if (clipping)
				polygons[i].clip();
			else
				polygons[i].Rasterize();
			polygons[i].drawPolygon();
		}
	}
	else{
		for (int i = 0; i < polygonCount; i++) {
			polygons[i].setBuffer();
			polygons[i].drawBresenham();
			if (clipping)
				polygons[i].clip();
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
		cout << "Specify Input File: ";
		getline(cin, inputFile);
		inFile.open(inputFile);
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

		cout << "Specify Window Size for x (recommend 200): ";
		cin >> windowSizeX;
		cout << "Specify Window Size for y (recommend 200): ";
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
		cout << "Specify Viewport for Xmin: ";
		cin >> viewXmin;
		cout << "Specify Viewport for Xmax: ";
		cin >> viewXmax;
		cout << "Specify Viewport for Ymin: ";
		cin >> viewYmin;
		cout << "Specify Viewport for Ymax: ";
		cin >> viewYmax;
		clipping = true;

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
		cout << "File Name you want to write to (q to cancel): ";
		cin >> fileName;
		if (fileName != "q") {
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
	}
	else if (choice == 6) {//Exit
		exit(0);
	}
	return;
}