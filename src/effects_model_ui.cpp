#include "effectsmodel.h"
#include "eventinfo.h"
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

using namespace std;

//global inits
GLUquadric* quad;
float transx, transy, transz = 0;
EffectsModel* model;
int screenWidth, screenHeight;

void keyboard(unsigned char key, int x, int y){
	//will map touchscreen buttons to keys
	//key is input as char, e.g. key=='a' = true
	if(key=='q') glutReshapeWindow(screenWidth, screenHeight);
	EventInfo event(key);
	model->updateModel(event);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
	//adjust x, y position
	transx = ((float)x-(screenWidth/2))/(screenWidth/2);
	transy = ((screenHeight/2)-(float)y)/(screenHeight/2);
	EventInfo event(x+(screenWidth/2),y+(screenHeight/2));
	model->updateModel(event);
	glutPostRedisplay();
}

void mouseMove(int x, int y){
	//adjust x, y position
	transx = ((float)x-(screenWidth/2))/(screenWidth/2);
	transy = ((screenHeight/2)-(float)y)/(screenHeight/2);
	EventInfo event(x+(screenWidth/2),y+(screenHeight/2));
	model->updateModel(event);
	glutPostRedisplay();
}

void timer(int n){
	EventInfo event;
	model->updateModel(event);
	glutPostRedisplay();
}

void closeWin(){
	//on window close (program end)
	printf("Exiting.\n");
	delete model;
	gluDeleteQuadric(quad);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT); //clear all
	glColor3f(1.0,1.0,1.0); //draw white circle
	glBegin(GL_POLYGON);
	float theta;
	for(int i = 0; i<360; i++){
	    theta = i*M_PI/180;
        glVertex2f(0.1*cos(theta),0.1*sin(theta));
	}
	glEnd();
	glLoadIdentity(); //need to reset translation
	glTranslatef(0,0,0);
	glTranslatef(transx, transy, transz); //translate
    glScalef(0.5666,1.0,1.0);
	//GLUPartialDisk variables for drawing	
	/*GLdouble innerRad = 0.0; 
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
	*///swap buffers and redraw
	glutSwapBuffers();
	//glutPostRedisplay();
}

int main(int argc, char** argv){
	setenv("DISPLAY", ":0.0", true);
	glutInit(&argc, argv);
	if(argc!=3){ //taking inputs
		printf("Usage: %s hwConfigFile mappingConfigFile\n", argv[0]);
		exit(0);
	}
	model = new EffectsModel(argv[1], argv[2]); //pass file names to config
	//get touchscreen width and height from config
	screenHeight = model->getTouchHeight(); 
	screenWidth = model->getTouchWidth();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //double buffering, rgba
	glutInitWindowSize(screenWidth, screenHeight); //size of BBB touchscreen
	glutInitWindowPosition(0,0); //aligned to corner
	glutCreateWindow("HapTech Guitar Effects"); //open a window
	glClearColor(0.0,0.0,0.0,0.0); //clear screen in black
	glutFullScreen(); //fullscreen (no window border)
	//use the GL event functions
	glutDisplayFunc(display); 
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboard);
	//glutTimerFunc(10, timer, 0); //msecs, timerfunc,input
	glutCloseFunc(closeWin);
	//init quadric for drawing
	quad = gluNewQuadric(); 
		if (quad==0) exit(0);
	gluQuadricDrawStyle(quad, GLU_FILL);
	glScalef(0.5666,1.0,1.0); //make it a circle
	glutMainLoop(); //do it all over and over
	return 0;
}
