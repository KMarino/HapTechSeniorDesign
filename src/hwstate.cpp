#include "hwstate.h"

using namespace std;

HWState::HWState()	
{
}

HWState::HWState(const char* configfile)	
{
    // Parse the config file
    Json::Value config;
    parseJSONFile(configfile, config);

    // Get data
    m_num_control = config.get("num_dev", 0).asInt();
    m_num_pot = config.get("num_pot", 0).asInt();
    m_num_switch = config.get("num_switch", 0).asInt();
    m_num_push = config.get("num_push", 0).asInt();
    m_logic_level = (float) config.get("logic_level", 0).asDouble();
    assert(m_num_control == (m_num_pot + m_num_switch + m_num_push));
    Json::Value touchscreen_size = config["touch_size"];
    for (int i = 0; i < 2; i++)
    {
        m_touchscreen_size[i] = touchscreen_size.get(i, 0).asInt();
    }
    Json::Value potpins = config["pot_pins"];
    for (int i = 0; i < m_num_pot; i++)
    {
        int pot_pin = potpins.get(i, 0).asInt();
        m_devices.push_back(Control_Pot(pot_pin));
        m_device_types.push_back(POTENTIOMETER);
    }
    Json::Value switchpins = config["switch_pins"];
    for (int i = 0; i < m_num_pot; i++)
    {
        int switch_pin = switchpins.get(i, 0).asInt();
        m_devices.push_back(Control_Switch(switch_pin));
        m_device_types.push_back(SWITCH);
    }
    Json::Value pushpins = config["push_pins"];
    Json::Value keys = config["keys"];
    for (int i = 0; i < m_num_push; i++)
    {
        int push_pin = pushpins.get(i, 0).asInt();
        char key = keys.get(i, "?").asString().at(0);
        m_devices.push_back(Control_Push(push_pin, key));
        m_device_types.push_back(PUSHBUTTON);
        m_push_chars[key] = i;
        //m_push_chars.push_back(key);
    }

    // Touchscreen starts at (0, 0)
    m_touch_x = 0;
    m_touch_y = 0;
}

int HWState::getPinNumber(ControlType type, int index)
{
    int pin = -1;
    int device_idx = 0;
    for (int i = 0; i < m_devices.size(); i++)
    {        
        if (m_device_types[i] == type)
        {
            if (device_idx == index)
            {
                return m_devices[i].getPin();
            }
            else
            {
                device_idx++;
            }
        }
    }

    // Return -1 (failure)
    return pin;
}

float HWState::getValue(ControlType type, int index)
{
    if (type == TOUCHSCREEN)
    {
        if (index == 0)
        {
            // Index 0 of touchscreen is x control. Returns x position
            return m_touch_x;
        }
        else if (index == 1)
        {
            // Index 1 of touchscreen is y control. Returns y position
            return m_touch_y;
        }
        else
        {
            // Makes no sense. Failure
            throw std::runtime_error("Invalid index for touchscreen control. Only have 0 (x) and 1 (y)");
            return -1;
        }
    }

    float value = -1;
    int device_idx = 0;
    for (int i = 0; i < m_devices.size(); i++)
    {        
        if (m_device_types[i] == type)
        {
            if (device_idx == index)
            {
                return m_devices[i].getValue();
            }
            else
            {
                device_idx++;
            }
        }
    }

    // Return -1 (failure)
    return value;
}

void HWState::setValue(ControlType type, int index, float value)
{
    if (type == TOUCHSCREEN)
    {
        if (index == 0)
        {
            // Index 0 of touchscreen is x control. Set x position
            m_touch_x = (int)value;
        }
        else if (index == 1)
        {
            // Index 1 of touchscreen is y control. Set y position
            m_touch_y = (int)value;
        }
        else
        {
            // Makes no sense. Failure
            throw std::runtime_error("Invalid index for touchscreen control. Only have 0 (y) and 1 (x)");
        }
        return;
    }

    // Other devices
    int device_idx = 0;
    for (int i = 0; i < m_devices.size(); i++)
    {        
        if (m_device_types[i] == type)
        {
            if (device_idx == index)
            {
                m_devices[i].updateValue(value);
            }
            else
            {
                device_idx++;
            }
        }
    }
}

int HWState::getButtonIndex(char c)
{
    return m_push_chars[c];
}

int HWState::getNumPot()
{
    return m_num_pot;
}

int HWState::getNumSwitch()
{
    return m_num_switch;
}

int HWState::getNumPush()
{
    return m_num_push;
}

int HWState::getNumDevices()
{
    return m_num_control;
}

int HWState::getTouchWidth()
{
    return m_touchscreen_size[0];
}

int HWState::getTouchHeight()
{
    return m_touchscreen_size[1];
}

float HWState::getLogicLevel()
{
    return m_logic_level;
}

ControlType getControlType(string controlstr)
{
    if (controlstr.compare("pot") == 0)
    {
        return POTENTIOMETER;
    }
    else if (controlstr.compare("switch") == 0)
    {
        return SWITCH;
    }
    else if (controlstr.compare("push") == 0)
    {
        return PUSHBUTTON;
    }
    else if (controlstr.compare("touch") == 0)
    {
        return TOUCHSCREEN;
    }
    else
    {
        return UNKNOWNCONTROL;
    }
}    

ScaleType getScaleType(string scalestr)
{
    if (scalestr.compare("linear") == 0)
    {
        return LINEAR;
    }
    else if (scalestr.compare("log") == 0)
    {
        return LOG;
    }
    else if (scalestr.compare("switch") == 0)
    {
        return SWITCHSCALE;
    }
    else if (scalestr.compare("static") == 0)
    {
        return STATIC;
    }
    else
    {
        // Does not match a class
        return UNKNOWNSCALE;
    }    
}
