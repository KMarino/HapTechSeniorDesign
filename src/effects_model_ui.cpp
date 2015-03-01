#include "effectsmodel.h"
#include "eventinfo.h"
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

using namespace std;

#define SCREENWIDTH 480 
#define SCREENHEIGHT 272

GLUquadric* quad;
float transx, transy, transz = 0;
EffectsModel* model;

void keyboard(unsigned char key, int x, int y){
	//will map touchscreen buttons to keys
	//key is input as char, e.g. key=='a' = true
	EventInfo event(key);
	model->updateModel(event);
	//glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
	transx = ((float)x-(SCREENWIDTH/2))/(SCREENWIDTH/2);
	transy = ((SCREENHEIGHT/2)-(float)y)/(SCREENHEIGHT/2);
	EventInfo event(x+(SCREENWIDTH/2),y+(SCREENHEIGHT/2));
	model->updateModel(event);
}

void mouseMove(int x, int y){
	transx = ((float)x-(SCREENWIDTH/2))/(SCREENWIDTH/2);
	transy = ((SCREENHEIGHT/2)-(float)y)/(SCREENHEIGHT/2);
	EventInfo event(x+(SCREENWIDTH/2),y+(SCREENHEIGHT/2));
	model->updateModel(event);
}

void timer(int n){
	EventInfo event;
	model->updateModel(event);
}

void closeWin(){
	printf("Exiting.\n");
	delete model;
	gluDeleteQuadric(quad);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT); //clear all
	glColor3f(1.0,1.0,1.0); //draw white circle
	GLdouble innerRad = 0.0; //variables for drawing
	GLdouble outerRad = .2;
	GLint slices = 100;
	GLint loops = 100;
	GLdouble start = 0;
	GLdouble sweep = 360;
	gluPartialDisk(quad, innerRad, outerRad, slices, loops, start, sweep);
	//GLUquadric*, GLdouble, GLdouble, GLint (subdivisions around z axis), GLint (concentric rings about origin for subdivision), GLdouble (start angle in deg), GLdouble (sweep angle in deg)
	glLoadIdentity(); //need to reset translation
	glTranslatef(0,0,0);
	glTranslatef(transx, transy, transz); //translate
	glScalef(0.5666,1.0,1.0); //make a circle, not an oval
	//swap buffers and redraw
	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	if(argc!=3){ //taking inputs
		printf("Usage: %s hwConfigFile mappingConfigFile\n", argv[0]);
		exit(0);
	}
	model = new EffectsModel(argv[1], argv[2]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //double buffering, rgba
	glutInitWindowSize(480, 272); //size of BBB touchscreen
	glutInitWindowPosition(0,0); //aligned to corner
	glutCreateWindow("HapTech Guitar Effects"); //open a window
	glClearColor(0.0,0.0,0.0,0.0); //clear screen in black
	//glutFullScreen(); //fullscreen (no window border)
	glutDisplayFunc(display); //use the GL event functions
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(10, timer, 0); //msecs, timerfunc,input
	glutCloseFunc(closeWin);
	quad = gluNewQuadric(); //init quadric for drawing
		if (quad==0) exit(0);
	gluQuadricDrawStyle(quad, GLU_FILL);
	glScalef(0.5666,1.0,1.0); //make it a circle
	glutMainLoop(); //do it all over and over
return 0;
}
