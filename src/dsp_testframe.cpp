#include "effectsmodel.h"
#include "eventinfo.h"
#include <math.h>

using namespace std;

int main(int argc, char** argv)
{
	if(argc!=3) //taking inputs
	{ 
		printf("Usage: %s hwConfigFile mappingConfigFile\n", argv[0]);
		exit(0);
	}
	EffectsModel model(argv[1], argv[2]); //pass file names to config
	int screenHeight = model.getTouchHeight(); 
	int screenWidth = model.getTouchWidth();
	int x = screenWidth - 1;
	int y = 0;
	
	// Infinite for loop changing values
	for ( ; ; )
	{
		// Update mouse
		y++;
		if (y >= screenHeight)
			y = 0;
		x--;
		if (x <= 0)
			x = screenWidth - 1;
	
		// New arrow key
		//EventInfo event(val);
		//model->updateModel(event);

		// New mouse event
		EventInfo event(x, y);
		model.updateModel(event);

		// New timer event
		//EventInfo event;
		//model->updateModel(event);
	}
	
	return 0;
}
