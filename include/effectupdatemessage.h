#ifndef EFFECT_UPDATE_MESSAGE_H
#define EFFECT_UPDATE_MESSAGE_H

#include <vector>
#include <assert.h>
#include <iostream>
#include "effectsmodel.h"
#include "effect.h"

using namespace std;

// Class that defines the message to update the DSP program
class EffectUpdateMessage
{
public:
    EffectUpdateMessage(vector<Effect*> effects, vector<EffectType> types); // Note, effectupdatemessage will keep track of and delete the pointers in effects now, so do not delete outside the function
    EffectUpdateMessage(char* msg);
    ~EffectUpdateMessage();
    void serialize(char* output);
    bool issame(const EffectUpdateMessage& other);

private:
    vector<EffectType> m_effect_types;
    vector<Effect*> m_effects;
    int m_msg_sz;
    int m_num_effects;
    char* m_msg;

    int getEffectSize(EffectType type);
};

#endif // EFFECT_UPDATE_MESSAGE_H
