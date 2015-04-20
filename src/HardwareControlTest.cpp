#include "gpio.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	for (; ;)
	{
		float P33_A = readAI(4);
		float P35_A = readAI(6);
		float P37_A = readAI(2);

		int P21_D = readGPIO(3);
		int P23_D = readGPIO(5);
		int P25_D = readGPIO(117);

		// Print out
		cout << "Pin 33: " << P33_A << endl;
		cout << "Pin 35: " << P35_A << endl;
		cout << "Pin 37: " << P37_A << endl;
		cout << "Pin 21: " << P21_D << endl;
		cout << "Pin 23: " << P23_D << endl;
		cout << "Pin 24: " << P25_D << endl;

		cin.get();
	}
	
}
