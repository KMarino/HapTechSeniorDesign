#include "effectsmodel.h"
#include "eventinfo.h"
#include "effectupdatemessage.h"
#include "RtAudio.h"
#include <iostream>
#include <cstdint>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

using namespace std;
typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

//global inits
GLUquadric* quad;
float transx, transy, transz = 0;
EffectsModel* model;
int screenWidth, screenHeight;
bool ContList = true;
bool passThrough = false;
//DSP effect vectors
std::mutex effectUpdateMutex;
std::vector<Effect*> effectObjectCopies;
std::vector<EffectType> recievedEffectTypes;
int gX = 0;
int gY = 0;//hacky fake for demo
std::mutex updateMutex;
int audioHandler(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
			double streamTime, RtAudioStreamStatus status, void *data)
{
	if(status) std::cout << "Stream over/underflow detected \n";
	uint32_t *bytes = (uint32_t *) data;
	float *obuffer = (float *) outputBuffer;
	float *ibuffer = (float *) inputBuffer;
//	Effect_Delay activeE =(Effect_Delay) effectObjectCopies[0];
	if(!passThrough)
	{	
	float distort = 5;
//	distort = ((activeE.m_attack) * .05);
	float para1 = 0;
	float para2 = 0;
	updateMutex.lock();
	para1 =gX/800.0;
	para2 =gY/600.0;
	updateMutex.unlock();
	for(int i = 0; i < nBufferFrames*2; i++)
	{
		if(i > 0)
			obuffer[i] = ibuffer[i] + (para1*ibuffer[i] * ibuffer[i-1]);
		else
			obuffer[i] = ibuffer[i] * ibuffer[i];
	}
	int j = 0;
	for(int i = 0; i<nBufferFrames*2; i++)
	{
		if(i>para2)
		{
			for(j = 1; j<para2; j++)
			{
				obuffer[i] += obuffer[i-j];
			} 
			obuffer[i] /=std::floor(para2+1);
		}
	}
	}
	else{
	memcpy(outputBuffer, inputBuffer, *bytes);
	}
	return 0;
}
void dsp()//thread fucntion for dsp
{
	 RtAudio adac;
  	if ( adac.getDeviceCount() < 1 ) 
	{
    		std::cout << "\nNo audio devices found!\n";
    		exit( 0 );
  	}
  	// Set the same number of channels for both input and output.
  	unsigned int bufferBytes, bufferFrames = 128;
	unsigned int fs, channels;
	fs = 48000;
	channels = 2;
  	RtAudio::StreamParameters iParams, oParams;
  	iParams.deviceId = 0; // first available device
  	iParams.nChannels = 2;
  	oParams.deviceId = 0; // first available device
  	oParams.nChannels = 2;
  	try {
    		adac.openStream( &oParams, &iParams, FORMAT, fs, &bufferFrames, &audioHandler, (void *)&bufferBytes );
  	}
  	catch ( RtAudioError& e ) {
    		e.printMessage();
    		exit( 0 );
  	}
  	bufferBytes = bufferFrames * channels * sizeof(MY_TYPE);
  	try 
	{
		adac.startStream();
		while (ContList)
		{
			 std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
         	adac.stopStream();
  	}
  	catch ( RtAudioError& e ) 
	{
    		e.printMessage();
    		goto cleanup;
  	}
 cleanup:
  if ( adac.isStreamOpen() ) adac.closeStream();

}
void setEffects(EffectUpdateMessage m)
{
	effectUpdateMutex.lock();
	recievedEffectTypes = m.getEffectTypes(); //vector of enums
	for(int i = 0; i<effectObjectCopies.size(); i++)
	{
		delete effectObjectCopies[i];
		/*switch (recievedEffectTypes[i])
		{
			case 1:
				delete (Effect_Delay*)effectObjectCopies[i]; //free pointers
				break;
			case 2:
				delete (Effect_Distortion*)effectObjectCopies[i];
				break;
			case 3:
				delete (Effect_Equalizer*)effectObjectCopies[i];
				break;
			case 4:
				delete (Effect_Lowpass*)effectObjectCopies[i];
				break;
			case 5:
				delete (Effect_Reverb*)effectObjectCopies[i];
				break;
			default:
				delete (Effect_Unknown*)effectObjectCopies[i];
		}*/
		
	}
	effectObjectCopies = m.getEffectCopy();
	effectUpdateMutex.unlock();

}
void keyboard(unsigned char key, int x, int y){
	//will map touchscreen buttons to keys
	passThrough = !passThrough;
	 std::this_thread::sleep_for(std::chrono::milliseconds(10));
	cout<< "flip  bit \n";
	//key is input as char, e.g. key=='a' = true
	if(key=='q') glutReshapeWindow(screenWidth, screenHeight);
	EventInfo event(key);
	EffectUpdateMessage message = model->updateModel(event);
	setEffects(message);
	glutPostRedisplay();
}

void specialkey(int key, int x, int y) {
	//will map touchscreen buttons to keys
	//key is input as char, e.g. key=='a' = true
	passThrough = !passThrough;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	cout<< "flip  bit \n";
	
	if(key=='q') glutReshapeWindow(screenWidth, screenHeight);

	char val = 0;

	// Check what special key it is
	switch (key)
	{
		case GLUT_KEY_LEFT:
			val = 1;

			break;
		case GLUT_KEY_RIGHT:
			val = 2;
			break;
		case GLUT_KEY_UP:
			val = 3;
			break;
		case GLUT_KEY_DOWN:
			val = 4;
			break;
	}

	EventInfo event(val);
	EffectUpdateMessage message= model->updateModel(event);
	setEffects(message);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
	//adjust x, y position
	transx = ((float)x-(screenWidth/2))/(screenWidth/2);
	transy = ((screenHeight/2)-(float)y)/(screenHeight/2);
	EventInfo event(x+(screenWidth/2),y+(screenHeight/2));
	EffectUpdateMessage message = model->updateModel(event);
	updateMutex.lock();
	gX = x;
	gY = y;
	updateMutex.unlock();
	setEffects(message);
	glutPostRedisplay();
}

void mouseMove(int x, int y){
	//adjust x, y position
	transx = ((float)x-(screenWidth/2))/(screenWidth/2);
	transy = ((screenHeight/2)-(float)y)/(screenHeight/2);
	EventInfo event(x+(screenWidth/2),y+(screenHeight/2));
	EffectUpdateMessage message = model->updateModel(event);
	setEffects(message);
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
	ContList = false;
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
	glutSpecialFunc(specialkey);
	//glutTimerFunc(10, timer, 0); //msecs, timerfunc,input
	glutCloseFunc(closeWin);
	//init quadric for drawing
	quad = gluNewQuadric(); 
		if (quad==0) exit(0);
	gluQuadricDrawStyle(quad, GLU_FILL);
	glScalef(0.5666,1.0,1.0); //make it a circle
	std::thread audioControl(dsp);
	glutMainLoop(); //do it all over and over
	audioControl.join();	
	return 0;
}
