/******************************************/
/*
  Code adapted from duplex.cpp
  by Gary P. Scavone, 2006-2007.

  This program opens a duplex stream and applies
  effects as defined by config and 
  parameterized  by touch screen and 
  gpio to the input before  output.
*/
/******************************************/

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include "sockServ.h"
#include <string.h>
#include "effectupdatemessage.h"

bool CONT_LISTEN = true;//global variable that controls stream
//typedef signed short MY_TYPE;
//#define FORMAT RTAUDIO_SINT16
typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

//std::vector<*Effect> effects;
//std::vector<EffectType> types;

int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data )
{
  // Since the number of input and output channels is equal, we can do
  // a simple buffer copy operation here.
  if ( status ) std::cout << "Stream over/underflow detected." << std::endl;
//  std::cout<< *((short*)inputBuffer) << "\n";
  uint32_t *bytes = (uint32_t *) data;
  float *obuffer = (float *) outputBuffer;
  float *ibuffer = (float *) inputBuffer;
  std::deque<float> delayBuffer;
  unsigned int i, j;
  int amp = 4;

//   memcpy( outputBuffer, inputBuffer, *bytes );
  return 0;
}

void closeProgram()
{
	//TODO make read from GPIO
	std::this_thread::sleep_for(std::chrono::milliseconds(1000000));
}
int main( int argc, char *argv[] )
{
  unsigned int channels, fs, bufferBytes, oDevice = 0, iDevice = 0, iOffset = 0, oOffset = 0;
  

  RtAudio adac;
  if ( adac.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 1 );
  }

  channels = 2;//guitar ouputs mono TODO make configurable
  fs = 48000;//cd quality and native to codex
   // Let RtAudio print messages to stderr.
  adac.showWarnings( true );

  // Set the same number of channels for both input and output.
  unsigned int bufferFrames = 128; //tweak for lag
  RtAudio::StreamParameters iParams, oParams;
  iParams.deviceId = iDevice;
  iParams.nChannels = channels;
  iParams.firstChannel = iOffset;
  oParams.deviceId = oDevice;
  oParams.nChannels = channels;
  oParams.firstChannel = oOffset;

  if ( iDevice == 0 )
    iParams.deviceId = adac.getDefaultInputDevice();
  if ( oDevice == 0 )
    oParams.deviceId = adac.getDefaultOutputDevice();

  RtAudio::StreamOptions options;
  //options.flags |= RTAUDIO_NONINTERLEAVED;

  try 
  {
    adac.openStream( &oParams, &iParams, FORMAT, fs, &bufferFrames, &inout, (void *)&bufferBytes, &options );
  }
  catch ( RtAudioError& e ) 
  {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    exit( 1 );
  }

  bufferBytes = bufferFrames * channels * sizeof( MY_TYPE );

  // Test RtAudio functionality for reporting latency.
  std::cout << "\nStream latency = " << adac.getStreamLatency() << " frames" << std::endl;

  try 
  {
    adac.startStream();
    std::thread endControl(closeProgram);
    ipcSerSock* serSock = new ipcSerSock();
    char gimmeData[1024];
    char msgSizeC[sizeof(int)];
    char *recvStrm;
    while(CONT_LISTEN)
	{
		recvStrm = serSock->sockRecv();
		if(effects.size() == 0 )
		{
			for(int i = 0; i < effects.size(); i++)
			{
				delete effects[i];
			}
		}
		strncpy(msgSizeC, recvStrm, sizeof(int));
		int msgSize = atoi(msgSizeC);
		strncpy(recvStrm, gimmeData, msgSize);
		EffectUpdateMessage recievedEffects(gimmeData);
		effects = recievedEffects.getEffectCopy();
		//debug
		
	}
	delete recvStrm, serSock;
	for(int i = 0; i < effects.size(); i++)
	{
		delete effects[i];
	}
	endControl.join();
    // Stop the stream.
	 adac.stopStream();
  }
  catch ( RtAudioError& e ) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    goto cleanup;
  }

 cleanup:
  if ( adac.isStreamOpen() ) adac.closeStream();

  return 0;
}
