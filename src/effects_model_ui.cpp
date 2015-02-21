#include "effectsmodel.h"
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

using namespace std;

GLUquadric* quad;
float transx, transy, transz = 0;

void keyboard(unsigned char key, int x, int y){
	//will map touchscreen buttons to keys
	//key is input as char, e.g. key=='a' = true
	switch (key){
		case 'a': transx += .001; break;
		case 'b': transy += .001; break;
	}
	glutPostRedisplay();
	
}

void mouse(int button, int state, int x, int y){
	//button should not matter, just state
	//state is GLUT_DOWN or GLUT_UP
}

void display(){
	//draw here
	/*GLfloat radius = 0.5; //circle radius (relative to screen borders)
	glClear(GL_COLOR_BUFFER_BIT); //clear all
	glColor3f(1.0,1.0,1.0); //draw white circle
	glLineWidth(2.0); //thickness of lines
	glBegin(GL_LINE_LOOP); //draw lines where first point connects to last
	for(int i=0; i<360; i++){
		float theta = i*M_PI/180; //each point on a circle
		glVertex2f(radius*cos(theta), radius*sin(theta)); 
	}
	glEnd();*/
	glClear(GL_COLOR_BUFFER_BIT); //clear all
	glColor3f(1.0,1.0,1.0); //draw white circle
	GLdouble innerRad = .01;
	GLdouble outerRad = .2;
	GLint slices = 100;
	GLint loops = 100;
	GLdouble start = 0;
	GLdouble sweep = 360;
	gluPartialDisk(quad, innerRad, outerRad, slices, loops, start, sweep);
	//GLUquadric*, GLdouble, GLdouble, GLint (subdivisions around z axis), GLint (concentric rings about origin for subdivision), GLdouble (start angle in deg), GLdouble (sweep angle in deg)
	glTranslatef(transx, transy, transz);
	//swap buffers and redraw
	glutSwapBuffers();
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //double buffering, rgba
	glutInitWindowSize(480, 272); //size of BBB touchscreen
	glutInitWindowPosition(0,0); //aligned to corner
	glutCreateWindow("HapTech Guitar Effects"); //open a window
	glClearColor(0.0,0.0,0.0,0.0); //clear screen in black
	//glutFullScreen(); //fullscreen (no window border)
	glutDisplayFunc(display); //use the display func to draw
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	quad = gluNewQuadric();
		if (quad==0) exit(0);
	gluQuadricDrawStyle(quad, GLU_FILL);
	glutMainLoop(); //do it all over and over
	gluDeleteQuadric(quad);
return 0;
}
