#include "effectupdatemessage.h"

EffectUpdateMessage::EffectUpdateMessage(vector<Effect*> effects, vector<EffectType> types)
{
    // Set environmental variables
    assert(effects.size() == types.size());   
    m_num_effects = effects.size();
    m_effect_types = types;
    m_effects = effects;

    // Figure out message size
    m_msg_sz = sizeof(int) + sizeof(int) + m_num_effects * sizeof(EffectType);
    for (int i = 0; i < m_num_effects; i++)
    {
        m_msg_sz += getEffectSize(types[i]);
    }

    // Allocate m_msg
    m_msg = new char[m_msg_sz];
    
    // Copy data to m_msg
    int offset = 0;
    memcpy(m_msg + offset, &m_msg_sz, sizeof(int));
    offset += sizeof(int);
    memcpy(m_msg + offset, &m_num_effects, sizeof(int));
    offset += sizeof(int);
    for (int i = 0; i < m_num_effects; i++)
    {
        memcpy(m_msg + offset, &m_effect_types[i], sizeof(EffectType));
        offset += sizeof(EffectType);
        memcpy(m_msg + offset, m_effects[i], getEffectSize(m_effect_types[i]));
        offset += getEffectSize(m_effect_types[i]);
    }
    assert(offset == m_msg_sz);
}

EffectUpdateMessage::EffectUpdateMessage(char* msg)
{
    // Copy actual message
    m_msg = msg;

    // Start reading
    int offset = 0;
    memcpy(&m_msg_sz, m_msg + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&m_num_effects, m_msg + offset, sizeof(int));
    offset += sizeof(int);   
    for (int i = 0; i < m_num_effects; i++)
    {
        EffectType type;
        memcpy(&type, m_msg + offset, sizeof(EffectType));
        m_effect_types.push_back(type);
        offset += sizeof(EffectType);
        Effect* effect;
        switch(type)
        {
            case DELAY:
                effect = new Effect_Delay();
                break;

            case DISTORTION:
                effect = new Effect_Distortion();
                break;

            case REVERB:
                effect = new Effect_Reverb();
                break;
            
            case HIGHPASS:
                effect = new Effect_Highpass();
                break;

            case LOWPASS:
                effect = new Effect_Lowpass();
                break;
            
            case BANDPASS:
                effect = new Effect_Bandpass();
                break;
            
            case UNKNOWN:
                effect = new Effect_Unknown();
                break;
        }
        memcpy(effect, m_msg + offset, getEffectSize(type));
        m_effects.push_back(effect);
        offset += getEffectSize(type);
    }
    assert(offset == m_msg_sz);        
}

EffectUpdateMessage::~EffectUpdateMessage()
{
    delete m_msg;
    for (int i = 0; i < m_num_effects; i++)
    {
        delete m_effects[i];
    }
}

void EffectUpdateMessage::serialize(char* output)
{
}

int EffectUpdateMessage::getEffectSize(EffectType type)
{
    switch (type)
    {
        case DELAY:
            return sizeof(Effect_Delay);

        case DISTORTION:
            return sizeof(Effect_Distortion);

        case REVERB:
            return sizeof(Effect_Reverb);
        
        case HIGHPASS:
            return sizeof(Effect_Highpass);

        case LOWPASS:
            return sizeof(Effect_Lowpass);
        
        case BANDPASS:
            return sizeof(Effect_Bandpass);
        
        case UNKNOWN:
            return sizeof(Effect_Unknown);
    }
}

