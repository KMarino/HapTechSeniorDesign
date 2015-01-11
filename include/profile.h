#ifndef PROFILE_H
#define PROFILE_H

#include "hwstate.h"

// Class that defines the profile for an effect including what effects are active, 
// what hardware is tied to what effect parameter, and any specific UI information

class Profile
{
public:
    Profile();
    void update(HWState hw);
};

#endif // PROFILE_H
