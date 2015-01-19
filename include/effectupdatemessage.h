#ifndef EFFECT_UPDATE_MESSAGE_H
#define EFFECT_UPDATE_MESSAGE_H

#include <vector>
#include "effectsmodel.h"
#include "effect.h"

using namespace std;

// Class that defines the message to update the DSP program
class EffectUpdateMessage
{
public:
    EffectUpdateMessage(vector<Effect> effects, vector<EffectType> types);
    void serialize(const char* output);   

private:
    vector<EffectType> m_effect_types;
    vector<Effect> m_effects;
    int m_msg_sz;
    int m_num_effects;
    char* msg;
};

#endif // EFFECT_UPDATE_MESSAGE_H
