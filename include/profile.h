#ifndef PROFILE_H
#define PROFILE_H

#include "hwstate.h"
#include "effect.h"
#include <stdexcept>

using namespace std;

// Class that defines the profile for an effect including what effects are active, 
// what hardware is tied to what effect parameter, and any specific UI information
class Profile
{
public:
    Profile();
    Profile(Json::Value config, HWState hw);
    void update(HWState hw);

private:
    void updateMapping(Json::Value config, Effect* effect, string attribute); 
    void updateMappingAttr(Json::Value config, Effect* effect, string attributestr);
    vector<EffectType> m_effect_types;
    vector<Effect*> m_effects;
    map<control_info, EffectInfo> m_mapping;
    float m_logic_level;    // logic level from HWState
};

#endif // PROFILE_H
