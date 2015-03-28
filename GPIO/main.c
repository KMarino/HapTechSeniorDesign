#include <iostream>
//#include <time.h>
#include "GPIO.h"
//using namespace std;

int main()
{
	std::cout << "Pin 1: " << readAI(0) << "\n";
	return 0;
}

