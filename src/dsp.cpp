#include "source/generator/SineGenerator.h"
#include <SFML/Audio.hpp>
#include "global.h"
#include "transform/FftFactory.h"
#include "tools/TextPlot.h"
#include "synth/SineSynthesizer.h"
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <thread>
#include <mutex>
#include "sockServ.h"
#include <string.h>
#include "effectupdatemessage.h"

std::vector <sf::SoundBuffer> buffers(2);
std::mutex BufferStateMutex;
int recorderBufferState = 0; //buffer for recorder
int playerBufferState = 0;//buffer for player
bool CONT_LISTEN = true;//halt boolean
bool firstTime = true;//first buffer edge case handler

unsigned int channels;
unsigned int sampleRate;

class inputAudioStream : public sf::SoundStream
{
public:
	void load()//loads initial buffer and calls init to set smpRate
	{
		samples.assign(buffers[playerBufferState].getSamples(), buffers[playerBufferState].getSamples() + buffers[playerBufferState].getSampleCount());
		currentSample = 0;
		channels = buffers[playerBufferState].getChannelCount();
		sampleRate = buffers[playerBufferState].getSampleRate();
		initialize(channels, sampleRate);//inherited func
	}
private:
	std::vector<sf::Int16> samples;
	std::size_t currentSample;
	virtual bool onGetData(Chunk& data)
	{
		const int samplesToStream = 4410; 
		//arbritary number tweak for preformance
		//samples/SampleRate = time
		if(currentSample == samples.size())
		{
			auto sample_start = buffers[playerBufferState].getSamples();
			auto sample_Count = buffers[playerBufferState].getSampleCount();
			auto sample_end = buffers[playerBufferState].getSamples() + sample_Count;
			samples.assign(sample_start, sample_end);
			currentSample = 0;
		}
		data.samples = &samples[currentSample];
		if(currentSample + samplesToStream <= samples.size())
		{
			//data struct is inherited from soundstream
			data.sampleCount = samplesToStream;
			currentSample+=samplesToStream;
		}
		else
		{
			//player remain sampes and set next look to reset
			data.sampleCount = samples.size() - currentSample;
			currentSample = samples.size();
		}
	}
	virtual void onSeek(sf::Time timeOffset)
	{
		//required virtual func, we're not using it
		int dummy = 0;
		return;
	}
	
};
void recorder()
{
	//this function handles recording of audio, runs in own thread
	while(CONT_LISTEN)//global shutdownHandler
	{
		unsigned int recordTime = 400;
		sf::SoundBufferRecorder recorder;
		recorder.start();
		sf::sleep(sf::milliseconds(recordTime));
		recorder.stop();
		buffers[recorderBufferState] = recorder.getBuffer();
		if(firstTime)
		{
			firstTime =false;
			playerBufferState = 1;
		}
		if(recorderBufferState == 0)
		{
			recorderBufferState = 1;
			playerBufferState = 0;
		}
		else
		{
			recorderBufferState = 0;
			playerBufferState = 1;
		}
	}
}
void play()
{//plays using custom audio stream from double buffer
//run in own thread
	while(firstTime)
		sf::sleep(sf::milliseconds(10));//spin until first buffer
	inputAudioStream stream;
	stream.load();
	stream.play();
	while(CONT_LISTEN)
		sf::sleep(sf::milliseconds(100));
	sf::sleep(sf::milliseconds(100));
}
void closeProgram()
{
	//thread that ends program
	//right now timer, eventually will read from something I supose
	sf::sleep(sf::milliseconds(100000));
	CONT_LISTEN = false;
}
int main()
{
	const Aquila::FrequencyType sampleFreq = sampleRate;
	std::thread listener (recorder);
	std::thread player (play);
	std::thread endControl (closeProgram);
	
	ipcSerSock* serSock = new ipcSerSock(); //this is the constructor
    char gimmeData[1024];
    char msgSizeC[sizeof(int)];
    while(CONT_LISTEN)
	{
		//blocking recieve here
        char* recvStrm = serSock->sockRecv();
        strncpy(msgSizeC, recvStrm, sizeof(int));
        int msgSize = atoi(msgSizeC);
	    strncpy(recvStrm, gimmeData, msgSize); //this is where Kenny's stuff will send to
        EffectUpdateMessage profile(gimmeData);
	}
	delete recvStrm, serSock; //destructor
	
	listener.join();
	player.join();
	endControl.join();
	return 0;
}
