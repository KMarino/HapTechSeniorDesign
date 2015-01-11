#ifndef EFFECTSMODEL_H
#define EFFECTSMODEL_H

#include <string>
#include "eventinfo.h"
#include "hwstate.h"

using namespace std;

// Class that defines the model for the dsp effects and updates those effects based on hardware inputs

class EffectsModel
{
public:
    EffectsModel();
    EffectsModel(string configFile);
    void updateModel(EventInfo event);

private:
    void updateDSP();
    bool updateSwitchDials(EventInfo event);
    bool updateMouse(EventInfo event);
    bool updateKeyProfile(EventInfo event);    

    HWState hw;
};

#endif // EFFECTSMODEL_H
