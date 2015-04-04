#include "global.h"
#include "source/generator/SineGenerator.h"
#include "transform/FftFactory.h"
#include "tools/TextPlot.h"
#include "synth/SineSynthesizer.h"
#include "sockServ.h"

int dspManager()
{
	ipcSerSock serSock = new ipcSerSock(); //this is the constructor
	char gimmeData[] = serSock.sockRecv(); //this is where Kenny's stuff will send to
	delete serSock; //destructor
	
	const Aquila::FrequencyType sampleFreq = 2000;
	return 0;
}
