#include <iostream>
#include "GPIO.h"
#include <time.h>

#include <sys/time.h>



int main()
{


	struct timeval stop, start;

	int value;

	long curtime, wait;
	curtime = 0;
	std::cout << "\n\nRunning...\n\n";
	for(wait = 10; wait >= 0; wait--){


		for (int i = 0; i < 1022; i++) { //can only have 1021 files open by default
			gettimeofday(&start, NULL);
			curtime = start.tv_usec;
			while(curtime - start.tv_usec <= wait) { //wait for specified amount of time before read
				gettimeofday(&stop, NULL);
				curtime = stop.tv_usec;
			}
			value = readAI(0);
			if (value < 0) {
				std::cout << "It didn't fucking work!\nOpened " << i << " files  and waited " << wait << " usecs.\n"
				 << value << "!\n";

				return 0;
			}
			else {
				std::cout << "Pin 1: " << value << "\n";
			}
		}		
	}
	
	return 0;
}

