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
    EffectUpdateMessage(Profile profile); 
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
};

#endif // EFFECT_UPDATE_MESSAGE_H
