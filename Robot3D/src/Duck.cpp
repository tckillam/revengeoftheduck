/*******************************************************************
		   Hierarchical Multi-Part Model Example
********************************************************************/
#include <Duck.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h> // For wglSwapInterval
#endif

#define FREEGLUT_STATIC
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


#include "CubeMesh.h"
#include "QuadMesh.h"
#include "Vectors.h"


// so i can use remainder on duckAngle
#include <cmath>
#include <math.h>


const int vWidth = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

// Note how everything depends on duck body dimensions so that can scale entire duck proportionately
// just by changing duck body scale
float bodyRadius = 3.0;
float headRadius = 0.6f * bodyRadius;
float beakRadius = 0.6f * headRadius;
float beakLength = 0.7f * headRadius;
float tailRadius = 0.45f * bodyRadius;
float tailLength = 0.5f * bodyRadius;
float boothLength = 20;
float gunLength = 5;
float bulletRadius = gunLength * 0.05f;

// Control duck body rotation on base
float duckAngle = 0;
float duckAngle2 = 0;
float duckFlipAngle = 0;

int score = 0;

float amplitude = 0.2f;   // Height of the sine wave
float frequency = 3.0f;   // Controls number of waves
float length = boothLength;      // Length of the wall
float height = 2.0f;      // Height of the wall

///////////////// my attempt to animate the duck left to right
float duckPosX = -7.0f;
float duckPosY = 0;

typedef struct Duck {
	float x; // duck's pos x
	float y; // duck's pos y
	float angle = 0; // for rotating up and down
	float duckFlipAngle = 0; // rotating for flipping when shot
} Duck;

const int flockSize = 6;

// array of ducks
Duck ducks[flockSize];

float cameraX = 0.0;
float cameraY = 6.0;
float cameraZ = 22.0;

// Spin Cube Mesh
float cubeAngle = 0.0;

// Animation control for ze gun
float gunAngle = 0.0;
float gunPosX = 0.0;
float gunPosY = 0.0;

float bulletPosX = 0.0;
float bulletPosY = 0.0;
float bulletPosZ = 0.0;

bool bulletFlying = false;

// Lighting/shading and material properties for duck - upcoming lecture - just copy for now
// duck RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat duckBody_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat duckBody_mat_specular[] = { 0.9f,0.9f,0.9f,1.0f };
GLfloat duckBody_mat_diffuse[] = { 0.1f,0.35f,0.1f,1.0f };
GLfloat duckBody_mat_shininess[] = { 150.0F };

GLfloat bullet_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat bullet_mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat bullet_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat bullet_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat gun_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat gun_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat gun_mat_shininess[] = { 76.8F };

GLfloat booth_mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat booth_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat booth_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat booth_mat_shininess[] = { 76.8F };


// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.9F, 0.9F, 0.9F, 1.0F };


// Mouse button
int currentButton;

// A template cube mesh
CubeMesh* cubeMesh = NULL;

//// from profs code ************************************************
// Booth consists of top, sides and bottom
CubeMesh* boothTop = NULL;
CubeMesh* boothLeftSide = NULL;
CubeMesh* boothRightSide = NULL;
CubeMesh* boothFront = NULL;
bool drawBoothFront = true;
// Arcade Booth 
GLfloat booth_ambient[] = { 0.05f, 0.0f, 0.0f, 1.0f };
GLfloat booth_diffuse[] = { 0.05f, 0.0f, 0.0f, 1.0f };
GLfloat booth_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat booth_shininess[] = { 4.0F };



// A flat open mesh
QuadMesh* groundMesh = NULL;

// Default Ground Mesh Size
int meshSize = 16;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void animationDuckFlip(int param);
void animationBullet(int param);
void drawDuck();
void drawBody();
void drawBooth();
void drawGun();
void drawBullet();
void drawWaterWave();

bool initGlobalVariables();

/////////////////////////////////////////////////////////////////////////////////
//// Initialize global variables
/////////////////////////////////////////////////////////////////////////////////
bool initGlobalVariables()
{
	Vector3 ambient = Vector3(0.0f, 0.05f, 0.0f);
	Vector3 diffuse = Vector3(0.4f, 0.8f, 0.4f);
	Vector3 specular = Vector3(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	//groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

	boothTop = new CubeMesh();
	ambient = Vector3(0.05f, 0.00f, 0.0f);
	diffuse = Vector3(0.05f, 0.0f, 0.0f);
	specular = Vector3(0.5f, 0.5f, 0.5f);
	shininess = 4.0;
	boothTop->setMaterial(ambient, diffuse, specular, shininess);

	boothLeftSide = new CubeMesh();
	ambient = Vector3(0.00f, 0.00f, 0.05f);
	diffuse = Vector3(0.00f, 0.0f, 0.05f);
	specular = Vector3(0.5f, 0.5f, 0.5f);
	shininess = 4.0;
	boothLeftSide->setMaterial(ambient, diffuse, specular, shininess);

	boothRightSide = new CubeMesh();

	boothRightSide->setMaterial(ambient, diffuse, specular, shininess);

	boothFront = new CubeMesh();
	boothFront->setMaterial(ambient, diffuse, specular, shininess);

	return true;
}

// this was chatgpt as we were supposed to use for requirement 8
void drawWaterWave() {
	// Save current OpenGL color and lighting state
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);

	// Disable lighting or color material side effects
	glDisable(GL_LIGHTING);  // <- easiest way to keep color pure
	glColor3f(0.4f, 0.7f, 1.0f);  // light blue water

	float depth = 0.5f;     // how thick the water wall is
	int segments = 100;

	glBegin(GL_QUADS);
	for (int i = 0; i < segments; i++) {
		float x1 = (i / (float)segments) * length - length / 2;
		float x2 = ((i + 1) / (float)segments) * length - length / 2;

		float y1 = sin(frequency * x1) * amplitude + height;
		float y2 = sin(frequency * x2) * amplitude + height;

		// FRONT FACE (z = 0)
		glVertex3f(x1, 0.0f, 0.0f);
		glVertex3f(x2, 0.0f, 0.0f);
		glVertex3f(x2, y2, 0.0f);
		glVertex3f(x1, y1, 0.0f);

		// BACK FACE (z = -depth)
		glVertex3f(x1, 0.0f, -depth);
		glVertex3f(x2, 0.0f, -depth);
		glVertex3f(x2, y2, -depth);
		glVertex3f(x1, y1, -depth);

		// TOP FACE (connect front & back along sine)
		glColor3f(0.2f, 0.5f, 0.8f);  // darker blue for top
		glVertex3f(x1, y1, 0.0f);
		glVertex3f(x2, y2, 0.0f);
		glVertex3f(x2, y2, -depth);
		glVertex3f(x1, y1, -depth);

		// reset color for next face
		glColor3f(0.4f, 0.7f, 1.0f);
	}
	glEnd();

	// Restore the previous OpenGL lighting/material state
	glPopAttrib();
}


int main(int argc, char** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("3D Hierarchical Example");


	// Initalize array of ducks
	for (int i = 0; i < flockSize; i++) {
		ducks[i].x = -7.0f + i * 3.0f; // spread ducks out along x-axis
		ducks[i].y = 0.0f;
		if (i % 2 == 0) {
			ducks[i].angle = 0.0f;
		}
		else {
			ducks[i].angle = 180.0f;
		}
		//ducks[i].angle = 0.0f;
		ducks[i].duckFlipAngle = 0.0f;
	}


	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// init global vars
	initGlobalVariables();

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	glEnable(GL_TEXTURE_2D);


	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// Other initializatuion
	// Set up ground quad mesh
	Vector3 origin = Vector3(-16.0f, 0.0f, 16.0f);
	Vector3 dir1v = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 dir2v = Vector3(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 32.0);
	groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1v, dir2v);

	Vector3 ambient = Vector3(0.0f, 0.05f, 0.0f);
	Vector3 diffuse = Vector3(0.4f, 0.8f, 0.4f);
	Vector3 specular = Vector3(0.04f, 0.04f, 0.04f);
	float shininess = 0.2f;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

	cubeMesh = new CubeMesh();
	ambient = Vector3(0.05f, 0.00f, 0.0f);
	diffuse = Vector3(0.05f, 0.0f, 0.0f);
	specular = Vector3(0.5f, 0.5f, 0.5f);
	shininess = 4.0;
	cubeMesh->setMaterial(ambient, diffuse, specular, shininess);

}

// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity(); // M = I
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0, 0.0, 1.0, 0.0); // M = IV

	//**************************************************************************************** prof booth
	// Draw Booth 
	glPushMatrix();
	glTranslatef(0, 8.0f, -8.0);
	glScalef(16.0f, 2.0f, 2.0f);
	boothTop->drawCubeMesh();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-14.0, 0.0f, -8.0);
	glScalef(1.0f, 10.0f, 2.0f);
	boothLeftSide->drawCubeMesh();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(14.0, 0.0f, -8.0);
	glScalef(1.0f, 10.0f, 2.0f);
	boothRightSide->drawCubeMesh();
	glPopMatrix();

	if (drawBoothFront)
	{
		glPushMatrix();
		glTranslatef(0, -6.0, 1.0);
		glScalef(12.0f, 4.0f, 0.5f);
		boothFront->drawCubeMesh();
		glPopMatrix();
	}

	// draw the ducks
	for (int i = 0; i < flockSize; i++) {
		glPushMatrix();
		//glScalef(1.5, 1.5, 1.5);
		glTranslatef(ducks[i].x, ducks[i].y, 0);
		glTranslatef(0, -2, 0);
		glRotatef(ducks[i].angle, 0.0, 0.0, -1.0);
		glTranslatef(0, 2, 0);
		glPushMatrix();
		glRotatef(ducks[i].duckFlipAngle, -1.0, 0.0, 0);
		drawBody();
		glPopMatrix();
		glPopMatrix();
	}

	// draw gun
	glPushMatrix();
	glTranslatef(gunPosX, gunPosY, 15); // this will be done last
	glTranslatef(0.0, 0, 15);
	glRotatef(-gunAngle, 0.0, 1.0, 0.0);
	glTranslatef(0.0, 0, -15);
	drawGun();

	// draw bullet
	glPushMatrix();
	glTranslatef(bulletPosX, bulletPosY, bulletPosZ);
	drawBullet();
	glPopMatrix();

	glPopMatrix();

	glutTimerFunc(10, animationHandler, 0);

	// draw water wave
	glPushMatrix();
	glTranslatef(0.0, -3.0, 0.0);
	drawWaterWave();
	glPopMatrix();

	// Draw ground mesh
	glPushMatrix();
	glTranslatef(0.0, -20.0, 0.0);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}

void drawDuck()
{
	glPushMatrix(); // copy M = IV and push onto the stack

	glScalef(1.0, 1.0, 1.0);
	drawBooth();

	glPopMatrix();
}

void drawBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, duckBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, duckBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, duckBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, duckBody_mat_shininess);

	// Isolate scale of duck body
	glPushMatrix();
	//glRotatef(-90, 0, 10, 0);
	glTranslatef(0, 0, 0);
	glScalef(0.3, 0.3, 0.175);

	// duck body
	glPushMatrix(); // M = I V R_y(duckAngle) copy and push onto stack 
	glScalef(bodyRadius, bodyRadius * 0.75, bodyRadius * 0.8); // M = I V R_y(duckAngle) S
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix(); // M = I V R_y(duckAngle)

	// duck head
	glPushMatrix();
	// Position head with respect to its parent part (the body)
	glTranslatef(1.5, (0.5 * bodyRadius + 0.5 * headRadius) + 0.5, 0); // this will be done last
	// "Build" Head (i.e. scale it and draw it)
	glPushMatrix();
	glScalef(headRadius, headRadius, headRadius);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	// duck beak
	glPushMatrix();
	// Position beak with respect to its parent part (the head)
	glRotatef(90, 0, 10, 0);
	glTranslatef(0, (0.5 * headRadius + 0.5 * beakRadius) - 1.5, 1); // this will be done last
	// "Build" Beak (i.e. scale it and draw it)
	glPushMatrix();
	glScalef(beakRadius, beakRadius, beakRadius);
	glutSolidCone(beakRadius, beakLength, 20, 20);
	glPopMatrix();
	// end of duck beak
	glPopMatrix();


	// end of duck head
	glPopMatrix();


	// duck tail
	glPushMatrix();
	// Position tail with respect to its parent part (the body)
	glRotatef(-90, 0, 10, 0);
	glTranslatef(0, (0.5 * bodyRadius + 0.5 * tailRadius) - 1.5, 2); // this will be done last
	glRotatef(-45, 10, 0, 0);
	// "Build" Beak (i.e. scale it and draw it)
	glPushMatrix();
	glScalef(tailRadius, tailRadius * 0.5, tailRadius);
	glutSolidCone(tailRadius, tailLength, 20, 20);
	glPopMatrix();
	// end of duck tail
	glPopMatrix();

	// end of duck body
	glPopMatrix();
}

void drawBullet()
{
	// Set bullet material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, bullet_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bullet_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bullet_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, bullet_mat_shininess);

	glPushMatrix();
	glutSolidSphere(bulletRadius, 20, 20);
	glPopMatrix();
}


void drawBooth()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, booth_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, booth_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, booth_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, booth_mat_shininess);

	glPushMatrix();
	glTranslatef(0, 0, 0); // this will be done last

	// booth left
	glPushMatrix();
	glTranslatef(-10, 0, 0);
	glScalef(boothLength * 0.1, boothLength, boothLength * 0.2);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	// booth right
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glScalef(boothLength * 0.1, boothLength, boothLength * 0.2);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	// booth top
	glPushMatrix();
	glTranslatef(0, 8, 0);
	glScalef(boothLength + 2, boothLength * 0.2, boothLength * 0.1);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	// booth bottom
	glPushMatrix();
	glTranslatef(0, -7, 0);
	glScalef(boothLength, boothLength * 0.4, boothLength * 0.09);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
}

void drawGun()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	GLUquadricObj* myNozzle;
	myNozzle = gluNewQuadric();
	gluQuadricDrawStyle(myNozzle, GLU_FILL);

	glPushMatrix();

	// the main gun part thingy
	glPushMatrix();
	glScalef(gunLength * 0.3, gunLength * 0.3, gunLength);
	glutSolidCube(1.0);

	// the handle
	glPushMatrix();
	glTranslatef(0, -0.75, 0.35);
	glScalef(gunLength * 0.2, gunLength * 0.5, gunLength * 0.06);
	glRotatef(0.0, 0.0, 0.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	// the nozzle/end thingy
	glPushMatrix();
	glTranslatef(0, 0.0, -0.7);
	glScalef(gunLength * 0.2, gunLength * 0.2, gunLength * 0.2);
	glRotatef(0.0, 0.0, 0.0, 0.0);
	gluCylinder(myNozzle, 0.45, 0.45, 0.25, 50, 50);
	glPopMatrix();


	glPopMatrix();


	glPopMatrix();
}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 140.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		glutTimerFunc(10, animationDuckFlip, 0);
		break;
	case 'F':
		duckAngle2 += 2.0;
		break;
	case 'a':
		bulletFlying = true;
		break;
	}
	glutPostRedisplay();  // Trigger a window redisplay
}


void animationHandler(int param)
{

	if (bulletFlying == true) {
		bulletPosZ += -0.0004f;
	}
	if (bulletPosZ <= -20.0f || bulletFlying == false) {
		bulletFlying = false;
		bulletPosZ = 0.0f;
	}

	// animate each duck in array
	for (int i = 0; i < flockSize; i++) {
		if ((ducks[i].angle == 0.0f || ducks[i].angle >= 360.0f) && ducks[i].x < 7.0f) {
			ducks[i].angle = 0.0f;
			ducks[i].y = amplitude * sin(frequency * ducks[i].x);
			ducks[i].x += 0.00004;

			// check for bullet hit
			if (bulletPosZ < -13 && bulletPosZ > -17 &&
				gunAngle / 2 > ducks[i].x - 2.0f && gunAngle / 2 < ducks[i].x + 2.0f &&
				gunPosY > ducks[i].y - 2.0f && gunPosY < ducks[i].y + 2.0f) {
				// hit duck
				ducks[i].duckFlipAngle = 90.0f;
				score += 1;
				bulletPosZ = 0.0f; // reset bullet
				bulletFlying = false;
			}
		}
		// rotate duck down
		else if (ducks[i].x >= 7.0f && ducks[i].angle < 180.0f) {
			ducks[i].angle += 0.0009;
			ducks[i].y = 0;
		}
		// animate the duck right to left
		else if (ducks[i].angle >= 180.0f && ducks[i].x > -7.0f) {
			ducks[i].angle = 180.0f;
			ducks[i].duckFlipAngle = 0.0f;
			ducks[i].x -= 0.00004;
			ducks[i].y = 0;
		}
		// rotate the duck back up
		else if (ducks[i].x <= -7.0f && ducks[i].angle < 360.0f) {
			ducks[i].angle += 0.0009;
			ducks[i].y = 0;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(10, animationHandler, 0);
}

void animationDuckFlip(int param)
{
	if ((duckAngle == 0.0f || duckAngle >= 360.0f) && duckPosX < 7.0f && duckAngle2 <= 90) {
		duckFlipAngle += 9;
		glutTimerFunc(35, animationDuckFlip, 0);
	}
	else {
		duckFlipAngle = 90.0f;
	}
	glutPostRedisplay();
}

void animationBullet(int param)
{
	if (bulletPosZ >= -20) {
		bulletPosZ -= 1;
		bulletPosX += gunAngle;
		glutTimerFunc(10, animationBullet, 0);
	}
	else {
		// bullet went too far
		bulletPosZ = 0.0;
	}
	glutPostRedisplay();
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{

	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	glutPostRedisplay();   // Trigger a window redisplay
}

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			stop = false;


		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			stop = true;

		}
		break;
	default:
		break;
	}
	glutPostRedisplay();   // Trigger a window redisplay
}

float mX = vWidth / 2;
float mY = vHeight / 2;
// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (xMouse < mX && gunAngle > -14) {
		gunAngle -= 0.25;
		mX = xMouse;
	}
	if (xMouse > mX && gunAngle < 14) {
		gunAngle += 0.25;
		mX = xMouse;
	}
	if (yMouse < mY && gunPosY < 6.0) {
		gunPosY += 0.1;
		mY = yMouse;
	}
	if (yMouse > mY && gunPosY > -2.0) {
		gunPosY -= 0.1;
		mY = yMouse;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}