#ifndef HWSTATE_H
#define HWSTATE_H

#include "optparser.h"
#include <cassert>
#include <vector>

using namespace std;

// Enums for hardware
enum ControlType
{
    POTENTIOMETER,
    SWITCH,
    PUSHBUTTON
};

// Class that encapsulates hardware state including mouse, dial and switch settings
// for passing between different classes

class HWState
{
public:
    HWState();
    HWState(const char* configfile);

    int getPinNumber(ControlType type, int index);
    int getNumPot();
    int getNumSwitch();
    int getNumPush();
    int getNumDevices();
    int getTouchWidth();
    int getTouchHeight();

private:
    int m_num_pot;
    int m_num_switch;
    int m_num_push;
    int m_num_control;
    int m_touchscreen_size[2];
    vector<int> m_device_pins;
    vector<ControlType> m_device_types;
};

#endif // HWSTATE_H
