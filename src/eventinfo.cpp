#include "eventinfo.h"

using namespace std;

// Empty constructor for timer event
EventInfo::EventInfo()	
{
    m_type = TIMER;
    m_keyvalue = 0;
    m_mouse_x = 0;
    m_mouse_y = 0;
}

// Constructor for keypress event
EventInfo::EventInfo(char keyvalue)
{
    m_type = KEYPRESS;
    m_keyvalue = keyvalue;
    m_mouse_x = 0;
    m_mouse_y = 0;
}

// Constructor for mouse event
EventInfo::EventInfo(int mouse_x, int mouse_y)
{
    m_type = MOUSE;
    m_keyvalue = 0;
    m_mouse_x = mouse_x;
    m_mouse_y = mouse_y;
}
   
