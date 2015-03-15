#include "effectupdatemessage.h"

EffectUpdateMessage::EffectUpdateMessage(Profile profile)
{
    vector<Effect*> effects = profile.getEffectCopy();
    vector<EffectType> types = profile.getEffectTypes();

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
    // Start reading
    int offset = 0;
    memcpy(&m_msg_sz, msg + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&m_num_effects, msg + offset, sizeof(int));
    offset += sizeof(int);   
    for (int i = 0; i < m_num_effects; i++)
    {
        EffectType type;
        memcpy(&type, msg + offset, sizeof(EffectType));
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
            
            case EQUALIZER:
                effect = new Effect_Equalizer();
                break;

            case LOWPASS:
                effect = new Effect_Lowpass();
                break;

            case REVERB:
                effect = new Effect_Reverb();
                break;
            
            case UNKNOWNEFFECT:
                effect = new Effect_Unknown();
                break;
        }
        memcpy(effect, msg + offset, getEffectSize(type));
        m_effects.push_back(effect);
        offset += getEffectSize(type);
    }
    assert(offset == m_msg_sz);   

    // Copy actual message
    m_msg = new char[m_msg_sz];
    memcpy(m_msg, msg, m_msg_sz);
}

EffectUpdateMessage::~EffectUpdateMessage()
{
    delete m_msg;
    for (int i = 0; i < m_num_effects; i++)
    {
        delete m_effects[i];
    }
}

EffectUpdateMessage::EffectUpdateMessage(EffectUpdateMessage const& copy)
{
    m_msg_sz = copy.m_msg_sz;
    m_num_effects = copy.m_num_effects;
    m_effect_types = copy.m_effect_types;

    // Get copy of messages    
    m_effects = copy.getEffectCopy();

    // Copy actual message
    m_msg = new char[m_msg_sz];
    memcpy(m_msg, copy.m_msg, m_msg_sz);
}

void EffectUpdateMessage::serialize(char* output)
{
    // Start reading
    int offset = 0;
    memcpy(output + offset, &m_msg_sz, sizeof(int));
    offset += sizeof(int);
    memcpy(output + offset, &m_num_effects, sizeof(int));
    offset += sizeof(int);   
    for (int i = 0; i < m_num_effects; i++)
    {
        EffectType type = m_effect_types[i];
        memcpy(output + offset, &type, sizeof(EffectType));
        offset += sizeof(EffectType);
        Effect* effect = m_effects[i];
        memcpy(output + offset, effect, getEffectSize(type));
        offset += getEffectSize(type);
    }
    assert(offset == m_msg_sz); 

    // Copy actual message
    memcpy(m_msg, output, m_msg_sz);
}

bool EffectUpdateMessage::issame(const EffectUpdateMessage& other)
{
    // Compare everything
    bool same = true;
    same &= m_msg_sz == other.m_msg_sz;
    if (m_num_effects != other.m_num_effects)
        return false;
    for (int i = 0; i < m_num_effects; i++)
    {
        same &= m_effect_types[i] == other.m_effect_types[i];
        same &= m_effects[i]->issame(other.m_effects[i]);
    }
    return same;
}

vector<EffectType> EffectUpdateMessage::getEffectTypes()
{
    return m_effect_types;
}
    
vector<Effect*> EffectUpdateMessage::getEffectCopy() const
{
    // Get values from profile
    vector<Effect*> effects;
    
    // Copy over effects
    for (int i = 0; i < m_effects.size(); i++)
    {
        EffectType type = m_effect_types[i];
        Effect* effect;
        switch(type)
        {
            case DELAY:
                effect = new Effect_Delay();
                break;

            case DISTORTION:
                effect = new Effect_Distortion();
                break;
            
            case EQUALIZER:
                effect = new Effect_Equalizer();
                break;

            case LOWPASS:
                effect = new Effect_Lowpass();
                break;

            case REVERB:
                effect = new Effect_Reverb();
                break;
            
            case UNKNOWNEFFECT:
                effect = new Effect_Unknown();
                break;
        }
        memcpy(effect, m_effects[i], getEffectSize(type));
        effects.push_back(effect);
    }

    return effects;
}

int EffectUpdateMessage::getMessageSize()
{
    return m_msg_sz;
}

