#include "effectsmodel.h"
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

using namespace std;

int keyboard(){

}

int mouse(){

}

void display(){
	//draw here
	GLfloat radius = 0.5; //circle radius (relative to screen borders)
	glClear(GL_COLOR_BUFFER_BIT); //clear all
	glColor3f(1.0,1.0,1.0); //draw white circle
	glLineWidth(2.0); //thickness of lines
	glBegin(GL_LINE_LOOP); //draw lines where first point connects to last
	for(int i=0; i<360; i++){
		float theta = i*M_PI/180; //each point on a circle
		glVertex2f(radius*cos(theta), radius*sin(theta)); 
	}
	glEnd();
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
	glutMainLoop(); //do it all over and over
	return 0;
}
