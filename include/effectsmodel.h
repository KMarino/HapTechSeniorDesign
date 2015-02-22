#ifndef EFFECTS_MODEL_H
#define EFFECTS_MODEL_H

#include <string>
#include "eventinfo.h"
#include "hwstate.h"
#include "profile.h"
#include <sstream>

using namespace std;

// Class that defines the model for the dsp effects and updates those effects based on hardware inputs

class EffectsModel
{
public:
    EffectsModel();
    EffectsModel(char* hwConfigFile, char* mappingConfigFile);
    void updateModel(EventInfo event);

private:
    void updateDSP();
    bool updateSwitchDials(EventInfo event);
    bool updateMouse(EventInfo event);
    bool updateKeyProfile(EventInfo event);    

    HWState m_hw;
    vector<Profile> m_profiles;
};

#endif // EFFECTS_MODEL_H
