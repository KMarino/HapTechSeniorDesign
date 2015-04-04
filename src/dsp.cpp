#include "global.h"
#include "source/generator/SineGenerator.h"
#include "transform/FftFactory.h"
#include "tools/TextPlot.h"
#include "synth/SineSynthesizer.h"
#include "sockServ.h"
#include <string.h>
#include "effectupdatemessage.h"

int main()
{
	ipcSerSock* serSock = new ipcSerSock(); //this is the constructor
	char gimmeData[1024];
    char msgSizeC[sizeof(int)];
    char* recvStrm = serSock->sockRecv();
    strncpy(msgSizeC, recvStrm, sizeof(int));
    int msgSize = atoi(msgSizeC);
	strncpy(recvStrm, gimmeData, msgSize); //this is where Kenny's stuff will send to
    EffectUpdateMessage profile(gimmeData);
	delete serSock; //destructor
	
	const Aquila::FrequencyType sampleFreq = 2000;
	return 0;
}
