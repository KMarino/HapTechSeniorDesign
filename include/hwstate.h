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
    PUSHBUTTON,
    TOUCHSCREEN,
    UNKNOWNCONTROL
};

// Enum for how a parameter scales with hardware
enum ScaleType
{
    LINEAR,
    LOG,
    SWITCHSCALE,
    STATIC,
    UNKNOWNSCALE
};

// Virtual class for controls. Holds information needed for control.
class Control
{
public:
    // Functions
    Control():
        m_value(0), m_pin(0), m_type(UNKNOWNCONTROL) {}
    Control(int pin, ControlType type):
        m_value(0), m_pin(pin), m_type(type) {}

    void updateValue(float value)
    {
        m_value = value; 
    }

    float getValue() 
    {
        return m_value; 
    }

    int getPin() 
    { 
        return m_pin;
    }

    void setPin(int pin)
    {
        m_pin = pin;
    }

    ControlType getType()
    {
        return m_type;
    }

    void setType(ControlType type)
    {
        m_type = type;
    }
          
private:
    float m_value;     // Analog voltage value of the control. Must be interpreted
    int m_pin;          // Pin on microcontroller
    ControlType m_type; // Control type
};

// Classes for each conrol type
class Control_Pot: public Control
{
public:
    // Functions
    Control_Pot();
    Control_Pot(int pin):
        Control(pin, POTENTIOMETER) {}
};

class Control_Switch: public Control
{
public:
    // Functions
    Control_Switch();
    Control_Switch(int pin):
        Control(pin, SWITCH) {}
};

class Control_Push: public Control
{
public:
    // Functions
    Control_Push();
    Control_Push(int pin, char key):
        Control(pin, PUSHBUTTON) {}

    void setKey(char key)
    {
        m_key = key;
    }

    char getKey()
    {
        return m_key;
    }
    
private:
    char m_key;
};

// Class that encapsulates hardware state including mouse, dial and switch settings
// for passing between different classes. Also has pin information for all controls.

class HWState
{
public:
    HWState();
    HWState(const char* configfile);

    int getPinNumber(ControlType type, int index);
    float getValue(ControlType type, int index);
    char getButtonChar(int index);
    int getNumPot();
    int getNumSwitch();
    int getNumPush();
    int getNumDevices();
    int getTouchWidth();
    int getTouchHeight();
    float getLogicLevel();

private:
    int m_num_pot;
    int m_num_switch;
    int m_num_push;
    int m_num_control;
    int m_touchscreen_size[2];
    float m_logic_level;
    vector<Control> m_devices;
    vector<char> m_push_chars;
    vector<ControlType> m_device_types;
};

ControlType getControlType(string controlstr);
ScaleType getScaleType(string scalestr);

#endif // HWSTATE_H
