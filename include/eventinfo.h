#ifndef EVENTINFO_H
#define EVENTINFO_H

enum EventType
{
    TIMER,
    KEYPRESS,
    MOUSE
};

// Class that encapsulates hardware events for EffectsModel to update
class EventInfo
{
public:
    EventInfo();
    EventInfo(char keyvalue);
    EventInfo(int mouse_x, int mouse_y);

    EventType m_type;
    int m_mouse_x;
    int m_mouse_y;
    char m_keyvalue;
};

#endif // EVENTINFO_H
