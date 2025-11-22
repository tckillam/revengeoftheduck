/*******************************************************************
		   Hierarchical Multi-Part Model Example
********************************************************************/
#include <Duck.h>

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <math.h>
//#include <gl/glut.h>
//#include <utility>
//#include <vector>
#include "Vectors.h"
#include "CubeMesh.h"
#include "QuadMesh.h"


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

// Control duck body rotation on base
float duckAngle = 0;
float duckAngle2 = 0;

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
	float angle2 = 0; // rotating for flipping when shot
} Duck;

// array of ducks
Duck ducks[5];

float cameraX = 0;
float cameraY = 6.0;
float cameraZ = 22.0;

// Spin Cube Mesh
float cubeAngle = 0.0;

// Lighting/shading and material properties for duck - upcoming lecture - just copy for now
// duck RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat duckBody_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat duckBody_mat_specular[] = { 0.9f,0.9f,0.9f,1.0f };
GLfloat duckBody_mat_diffuse[] = { 0.1f,0.35f,0.1f,1.0f };
GLfloat duckBody_mat_shininess[] = { 150.0F };


GLfloat duckArm_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat duckArm_mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat duckArm_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat duckArm_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gun_mat_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat gun_mat_shininess[] = { 100.0F };

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
void drawDuck();
void drawBody();
void drawBooth();

void drawWaterWave();

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
	for (int i = 0; i < 5; i++) {
		ducks[i].x = -7.0f + i * 3.0f; // spread ducks out along x-axis
		ducks[i].y = 0.0f;
		if (i % 2 == 0) {
			ducks[i].angle = 180.0f;
		}
		else {
			ducks[i].angle = 0.0f;
		}
		//ducks[i].angle = 0.0f;
		ducks[i].angle2 = 0.0f;
	}

	// Initialize GL
	initOpenGL(vWidth, vHeight);

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

	// Draw duck
	// Apply modelling transformations M to move duck
	// ModelView matrix is set to IV, where I is identity matrix
	// M = IV
	drawDuck();

	// draw the ducks
	for (int i = 0; i < 5; i++) {
		glPushMatrix();
		glTranslatef(ducks[i].x, ducks[i].y, 0);
		glTranslatef(0, -2, 0);
		glRotatef(ducks[i].angle, 0.0, 0.0, -1.0);
		glTranslatef(0, 2, 0);
		drawBody();
		glPopMatrix();
	}

	//glPushMatrix();
	//glTranslatef(duckPosX, duckPosY, 0);
	//glTranslatef(0, -2, 0);
	//glRotatef(duckAngle, 0.0, 0.0, -1.0);
	//glTranslatef(0, 2, 0);


	/*
	not needed until requirement 5 - GUNS ABLAZINGGGGGGGGGGGG

	//for flipping duck
	glPushMatrix();
	glRotatef(duckAngle2, -1.0, 0.0, 0);
	drawBody();
	glPopMatrix();
    */
	//drawBody(); // this drawBody() got moved from above because it usually sits in the flipper
	//glPopMatrix();


	// second duck
	/*glPushMatrix();
	glTranslatef(-7, duckPosY, 0);
	glTranslatef(0, -2, 0);
	glRotatef(180, 0.0, 0.0, -1.0);
	glTranslatef(0, 2, 0);
	drawBody(); 
	glPopMatrix();*/

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

	//drawBooth();

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

void drawHead()
{
	// Set duck material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, duckBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, duckBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, duckBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, duckBody_mat_shininess);

	glPushMatrix();
	// Position head with respect to its parent part (the body)
	glTranslatef(2, 0.5 * bodyRadius + 0.5 * headRadius, 0); // this will be done last

	// "Build" Head (i.e. scale it and draw it)
	glPushMatrix();
	glScalef(headRadius, headRadius, headRadius);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

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
	}
	glutPostRedisplay();  // Trigger a window redisplay
}


void animationHandler(int param)
{

	// animate each duck in array
	for (int i = 0; i < 5; i++) {
		if ((ducks[i].angle == 0.0f || ducks[i].angle >= 360.0f) && ducks[i].x < 7.0f) {
			ducks[i].angle = 0.0f;
			ducks[i].y = amplitude * sin(frequency * ducks[i].x);
			ducks[i].x += 0.00004;
		}
		// rotate duck down
		else if (ducks[i].x >= 7.0f && ducks[i].angle < 180.0f) {
			ducks[i].angle += 0.0009;
			ducks[i].y = 0;
		}
		// animate the duck right to left
		else if (ducks[i].angle >= 180.0f && ducks[i].x > -7.0f) {
			ducks[i].angle = 180.0f;
			ducks[i].angle2 = 0.0f;
			ducks[i].x -= 0.00004;
			ducks[i].y = 0;
		}
		// rotate the duck back up
		else if (ducks[i].x <= -7.0f && ducks[i].angle < 360.0f) {
			ducks[i].angle += 0.0009;
			ducks[i].y = 0;
		}
	}

	//// animate the duck left to right in a sine wave
	//if ((duckAngle == 0.0f || duckAngle >= 360.0f) && duckPosX < 7.0f) {
	//	duckAngle = 0.0f;
	//	duckPosY = amplitude * sin(frequency * duckPosX);
	//	duckPosX += 0.00004;
	//}
	//// rotate duck down
	//else if (duckPosX >= 7.0f && duckAngle < 180.0f) {
	//	duckAngle += 0.0005;
	//	duckPosY = 0;
	//}
	//// animate the duck right to left
	//else if (duckAngle >= 180.0f && duckPosX > -7.0f) {
	//	duckAngle = 180.0f;
	//	duckAngle2 = 0.0f;
	//	duckPosX -= 0.0009;
	//	duckPosY = 0;
	//}
	//// rotate the duck back up
	//else if (duckPosX <= -7.0f && duckAngle < 360.0f) {
	//	duckAngle += 0.0005;
	//	duckPosY = 0;
	//}
	glutPostRedisplay();
	glutTimerFunc(10, animationHandler, 0);
}

// when f/F is pressed, flip the duck
void animationDuckFlip(int param)
{
	if ((duckAngle == 0.0f || duckAngle >= 360.0f) && duckPosX < 7.0f && duckAngle2 <= 90) {
		duckAngle2 += 9;
		glutTimerFunc(35, animationDuckFlip, 0);
	}
	else {
		duckAngle2 = 90.0f;
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
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		//
		if (xMouse > mX && cameraX < 10.0) {
			cameraX += 0.5;
			mX = xMouse;
		}
		if (xMouse < mX && cameraX > -10.0) {
			cameraX -= 0.5;
			mX = xMouse;
		}
	}
	if (currentButton == GLUT_RIGHT_BUTTON)
	{
		if (yMouse > mY && cameraZ < 40.0) {
			cameraZ += 0.5;
			mY = yMouse;
		}
		if (yMouse < mY && cameraZ > 10.0) {
			cameraZ -= 0.5;
			mY = yMouse;
		}
	}

	glutPostRedisplay();   // Trigger a window redisplay
}