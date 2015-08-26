#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

//Include our math library
#include <algebra3.h>

#include <GLUT/glut.h>

using namespace std;

namespace {
	static char FILENAME[20] = "polygon.obj";
}

//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
	void renderGuides()
	{
		if (guide) {
			// Vertical line
			glBegin(GL_LINES);
			glVertex2f(xPos, -2);
			glVertex2f(xPos, 2);
			glEnd();
			// Horizontal line
			glBegin(GL_LINES);
			glVertex2f(-2, yPos);
			glVertex2f(2, yPos);
			glEnd();
		}
	}
	
	void mousePos(float x, float y) {
		xPos = x;
		yPos = y;
	}
	
	int w, h; // width and height
	bool guide;
	
	float xPos;
	float yPos;
};

class Vertex {
public:
	Vertex(): pos(0.0,0.0) {
		//Constuctor
	}
	Vertex(double x, double y) : pos(x,y) {
	}
	void setPos(double x, double y) {
		pos[0] = x;
		pos[1] = y;
	}
	double x() const { return pos[0]; }
	double y() const { return pos[1]; }
	
	static vec2 mapCoordinates(Viewport & vp, float x, float y) {
		float xPos = ((float)x - vp.w/2)/((float)(vp.w/2));
		float yPos = ((float)y - vp.h/2)/((float)(vp.h/2));
		return vec2(xPos,yPos);
	}
	
	static vec2 mapToScreen(Viewport & vp, float x, float y) {
		float xPos = (float)(vp.w/2)*((float)x + vp.w/2);
		float yPos = (float)(vp.h/2)*((float)y + vp.h/2);
		return vec2(xPos,yPos);
	}

private:
	vec2 pos;
};

class Polygon {
public:
	Polygon() {
		//Constructor
		// lineDrawMode = 
	}
	~Polygon() {
		vertexList.erase(vertexList.begin(), vertexList.end());
	}
	void addVertex(Vertex aVertex) {
		vertexList.push_back(aVertex);
	}
	
	void drawTemp(Vertex * aVertexPtr) {
		tempVertex = aVertexPtr;
	}
	
	
	void writeOBJFile()
	{
		ofstream output(FILENAME, ios::out);
		for (size_t i = 0; i < vertexList.size(); i++) {
			output << "v " << vertexList[i].x() << " " << vertexList[i].y() << endl;
		}
		output << "f ";
		size_t i = 1;
		while (i <= vertexList.size()) {
			output << i << " ";
			i++;
		}
		output << endl;
	}
	void finishPolygon() {
		if (count() == 0) {
			cout << "Cannot save obj file out. Polygon not complete" << endl;
			return;
		}
		Vertex first = vertexList.front();
		vertexList.push_back(first);
		writeOBJFile();
	}
	size_t count() { return vertexList.size(); }
	
	void render() {

		if (vertexList.size() > 1) {
		
			glBegin(GL_LINE_STRIP);
		} else {
			glBegin(GL_POINTS);
		}
		for (size_t i = 0; i < vertexList.size(); i++) {
			glVertex2f(vertexList[i].x(), vertexList[i].y());
		}		
		glEnd();
		
		if (tempVertex) {
			float tempX, tempY, backX, backY, diffX, diffY;
			tempX = tempVertex->x();
			tempY = tempVertex->y();
			
			if (vertexList.size() >= 1 ) {
				backX = vertexList.back().x();
				backY = vertexList.back().y();
				
				diffX = tempX - backX;
				diffY = tempY - backY;
			
				// Absolute vaue for difference
				diffX = diffX < 0 ? -diffX : diffX;
				diffY = diffY < 0 ? -diffY : diffY;
				glBegin(GL_LINES);
				glVertex2f(backX, backY);
				if (snapTo) {
					if (diffX > diffY) {
						tempY = backY;
					} else {
						tempX = backX;
					}
					tempVertex->setPos(tempX, tempY);
				}
				glVertex2f(tempX, tempY);
				glEnd();
			}
		}
	}
	bool snapTo;
	bool lineStyle;
private:
	int lineDrawMode;
	vector<Vertex> vertexList;
	Vertex * tempVertex;
};

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
Polygon polygon;
Vertex mouseVertex;
Vertex * tempVertex;

void renderBitmapText(float x, float y, void *font, const char *string)
{
	vec2 mappedPos = Vertex::mapCoordinates(viewport, x, y);
	// www.programming-techniques.com
	const char *c;
	glRasterPos2f(mappedPos[0], -mappedPos[1]);
	for (c=string; *c!= '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
void display()
{
	//Clear Buffers     
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); // indicate we are specifying camera transformations
	glLoadIdentity(); // make sure transformation is "zero'd"
	// draw some lines
	polygon.render();
	
	renderBitmapText(10,20, GLUT_BITMAP_8_BY_13, "Polygon Builder");
	
	renderBitmapText(10,550, GLUT_BITMAP_8_BY_13, "Guides - 'g'");
	renderBitmapText(10,563, GLUT_BITMAP_8_BY_13, "Snap-To lines - 's'");
	renderBitmapText(10,576, GLUT_BITMAP_8_BY_13, "Write obj file - 'f'");
	
	viewport.renderGuides();
	
	//Now that we've drawn on the buffer, swap the drawing buffer and the displaying buffer.
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	//Set up the viewport to ignore any size change.
	glViewport(0,0,viewport.w,viewport.h);
	
	//Set up the PROJECTION transformationstack to be a simple orthographic [-1, +1] projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch
	
	//Set the MODELVIEW transformation stack to the identity.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void myKeyboardFunc (unsigned char key, int x, int y) {
	switch (key) {
		case 27: // Escape key
			exit(0);
			break;
		case 'g':
			viewport.guide = !viewport.guide;
			break;
		case 's':
			polygon.snapTo = !polygon.snapTo;
			break;
		case 'l':
			polygon.lineStyle = !polygon.lineStyle;
			break;
		case 'f':
			// In case mouse is not available
			polygon.finishPolygon();
			break;
	}
	glutPostRedisplay();
}

void myMouseMotionFunc(int x, int y)
{
	if (!viewport.guide) return;
	vec2 mappedPos = Vertex::mapCoordinates(viewport,x,y);
	viewport.mousePos(mappedPos[0], -mappedPos[1]);
	glutPostRedisplay();
}

void myMouseMotionHoldFunc(int x, int y)
{
	// For drag holds
	vec2 mappedPos = Vertex::mapCoordinates(viewport,x,y);
	mouseVertex.setPos(mappedPos[0], -mappedPos[1]);

	tempVertex = &mouseVertex;
	polygon.drawTemp(tempVertex);
	if (viewport.guide) {
		vec2 mappedPos = Vertex::mapCoordinates(viewport,x,y);
		viewport.mousePos(mappedPos[0], -mappedPos[1]);
		
	}	
	glutPostRedisplay();
}
void myMouseFunc( int button, int state, int x, int y ) {
	if ( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) {
		// For quick releases
		vec2 mappedPos = Vertex::mapCoordinates(viewport,x,y);
		mouseVertex.setPos(mappedPos[0], -mappedPos[1]);
		tempVertex = &mouseVertex;
		polygon.drawTemp(tempVertex);
		
	} else if (button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN) {
		polygon.finishPolygon();

	} else if (state==GLUT_UP) {
		if (!tempVertex) return;
		polygon.addVertex(*tempVertex);
		tempVertex = NULL;		
	}
	glutPostRedisplay();
}

int main(int argc,char** argv)
{
	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	
	//Set up global variables
	viewport.w = 600;
	viewport.h = 600;
	
	//Create OpenGL Window
	glutInitWindowSize(viewport.w,viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Polygon Builder");
	
	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboardFunc);
	glutMotionFunc(myMouseMotionHoldFunc);
	glutPassiveMotionFunc(myMouseMotionFunc);
	glutMouseFunc(myMouseFunc);
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	
	//And Go!
	glutMainLoop();
}