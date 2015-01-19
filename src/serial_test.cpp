#include "effect.h"
#include <iostream>

using namespace std;

int main()
{
    // Make new Effect_Delay object
    Effect_Delay delay(9.999901);
    
    // Serialize
    char delay_serial[256];
    delay.serialize(delay_serial);

    // Make a new delay object
    Effect_Delay delay2(delay_serial); 

    // Compare them
    cout << "delay_time orig: " << delay.m_delay_time << endl;
    cout << "delay_time copy: " << delay2.m_delay_time << endl;
}
