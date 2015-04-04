#ifndef EFFECTS_MODEL_H
#define EFFECTS_MODEL_H

#include <string>
#include "eventinfo.h"
#include "hwstate.h"
#include "profile.h"
#include "effectupdatemessage.h"
#include <sstream>
#include "sockClient.h"

using namespace std;

// Class that defines the model for the dsp effects and updates those effects based on hardware inputs

class EffectsModel
{
public:
    EffectsModel();
    EffectsModel(const char* hwConfigFile, const char* mappingConfigFile);
    void updateModel(EventInfo event);
    int getTouchWidth();
    int getTouchHeight();
    ~EffectsModel();
private:
    void updateDSP();
    bool updateSwitchDials();
    bool updateMouse(EventInfo event);
    bool updateKeyProfile(EventInfo event);
    
    ipcCliSock* cliSock;    
    HWState m_hw;
    vector<Profile> m_profiles;
    int m_curProfile;
};

#endif // EFFECTS_MODEL_H
