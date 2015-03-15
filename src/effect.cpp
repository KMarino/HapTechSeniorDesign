#include "effect.h"

// Delay
Effect_Delay::Effect_Delay()
{
    m_amount = 0;
    m_speed = 0;
    m_decay = 0;
    m_attack = 0;
    m_on = false;
}

Effect_Delay::Effect_Delay(float amount, float speed, float decay, float attack, bool on)
{
    m_amount = amount;
    m_speed = speed;
    m_decay = decay;
    m_attack = attack;
    m_on = on;
}

Effect_Delay::Effect_Delay(const char* msg)
{
    // Copy char to new object
    Effect_Delay* msgObj = new Effect_Delay();
    memcpy(msgObj, msg, sizeof(Effect_Delay));

    // Copy member variables
    m_amount = msgObj->m_amount;
    m_speed = msgObj->m_speed;
    m_decay = msgObj->m_decay;
    m_attack = msgObj->m_attack;
    m_on = msgObj->m_on;
}

Effect_Delay::Effect_Delay(Json::Value config)
{
    // Read default values into effect
    m_amount = config["amount"].get("default", 0).asDouble();
    m_speed = config["speed"].get("default", 0).asDouble();
    m_decay = config["decay"].get("default", 0).asDouble();
    m_attack = config["attack"].get("default", 0).asDouble();
    m_on = config["on"].get("default", 0).asInt();
}

void Effect_Delay::update(string attribute, float value)
{
    if (attribute.compare("amount") == 0)
    {
        m_amount = value;
    }
    else if (attribute.compare("speed") == 0)
    {
        m_speed = value;
    }
    else if (attribute.compare("decay") == 0)
    {
        m_decay = value;
    }
    else if (attribute.compare("attack") == 0)
    {
        m_attack = value;
    }
    else if (attribute.compare("on") == 0)
    {
        // Say it's off if its below half of logic level
        if (value < 0.5)
        {
            m_on = false;
        }
        else
        {
            m_on = true;
        }
    }
    else
    {
        // Attribute does not exist in effect
        throw std::runtime_error("Error - no attribute " + attribute + " exists in Effect_Delay");
    }
}

EffectInfo::EffectInfo()
{
}

EffectInfo::EffectInfo(Effect* effect, string attributestr, ScaleType scale_type, float min, float max):
        m_effect(effect), m_attributestr(attributestr), m_scale_type(scale_type), m_min(min), m_max(max) 
{ 
}

void Effect_Delay::serialize(char* output)
{
    // Copy object to output char
    memcpy(output, this, sizeof(Effect_Delay));
}

bool Effect_Delay::issame(const Effect* other)
{
    // Cast
    Effect_Delay* otherc = (Effect_Delay*) other;

    // Compare
    bool same = true;
    same &= m_amount == otherc->m_amount;
    same &= m_speed == otherc->m_speed;
    same &= m_decay == otherc->m_decay;
    same &= m_attack == otherc->m_attack;
    same &= m_on == otherc->m_on;
    return same;
}

// Distortion
Effect_Distortion::Effect_Distortion()
{
    m_amount = 0;
    m_on = 0;
}

Effect_Distortion::Effect_Distortion(float amount, bool on)
{
    m_amount = amount;
    m_on = on;
}

Effect_Distortion::Effect_Distortion(const char* msg)
{
    // Copy char to new object
    Effect_Distortion* msgObj = new Effect_Distortion();
    memcpy(msgObj, msg, sizeof(Effect_Distortion));

    // Copy member variables
    m_amount = msgObj->m_amount;
    m_on = msgObj->m_on;
}

Effect_Distortion::Effect_Distortion(Json::Value config)
{
    // Read default values into effect
    m_amount = config["amount"].get("default", 0).asDouble();
    m_on = config["on"].get("default", 0).asInt();
}

void Effect_Distortion::update(string attribute, float value)
{
    if (attribute.compare("amount") == 0)
    {
        m_amount = value;
    }
    else if (attribute.compare("on") == 0)
    {
        // Say it's off if its below half of logic level
        if (value < 0.5)
        {
            m_on = false;
        }
        else
        {
            m_on = true;
        }
    }
    else
    {
        // Attribute does not exist in effect
        throw std::runtime_error("Error - no attribute " + attribute + " exists in Effect_Distortion");
    }
}

void Effect_Distortion::serialize(char* output)
{
    // Copy object to output char
    memcpy(output, this, sizeof(Effect_Distortion));
}

bool Effect_Distortion::issame(const Effect* other)
{
    // Cast
    Effect_Distortion* otherc = (Effect_Distortion*) other;

    // Compare
    bool same = true;
    same &= m_amount == otherc->m_amount;
    same &= m_on == otherc->m_on;
    return same;
}

// Equalizer
Effect_Equalizer::Effect_Equalizer()
{
    m_lowmid = 0;
    m_midhigh = 0;
    m_base_gain = 0;
    m_mid_gain = 0;
    m_treb_gain = 0;
    m_on = 0;
}

Effect_Equalizer::Effect_Equalizer(float lowmid, float midhigh, float base_gain, float mid_gain, float treb_gain, bool on)
{
    m_lowmid = lowmid;
    m_midhigh = midhigh;
    m_base_gain = base_gain;
    m_mid_gain = mid_gain;
    m_treb_gain = treb_gain;
    m_on = on;
}

Effect_Equalizer::Effect_Equalizer(const char* msg)
{
    // Copy char to new object
    Effect_Equalizer* msgObj = new Effect_Equalizer();
    memcpy(msgObj, msg, sizeof(Effect_Equalizer));

    // Copy member variables
    m_lowmid = msgObj->m_lowmid;
    m_midhigh = msgObj->m_midhigh;
    m_base_gain = msgObj->m_base_gain;
    m_mid_gain = msgObj->m_mid_gain;
    m_treb_gain = msgObj->m_treb_gain;
    m_on = msgObj->m_on;
}

Effect_Equalizer::Effect_Equalizer(Json::Value config)
{
    // Read default values into effect
    m_lowmid = config["lowmid"].get("default", 0).asDouble();
    m_midhigh = config["midhigh"].get("default", 0).asDouble();
    m_base_gain = config["base_gain"].get("default", 0).asDouble();
    m_mid_gain = config["mid_gain"].get("default", 0).asDouble();
    m_treb_gain = config["treb_gain"].get("default", 0).asDouble();
    m_on = config["on"].get("default", 0).asInt();
}

void Effect_Equalizer::update(string attribute, float value)
{
    if (attribute.compare("lowmid") == 0)
    {
        m_lowmid = value;
    }
    else if (attribute.compare("midhigh") == 0)
    {
        m_midhigh = value;
    }
    else if (attribute.compare("base_gain") == 0)
    {
        m_base_gain = value;
    }
    else if (attribute.compare("mid_gain") == 0)
    {
        m_mid_gain = value;
    }
    else if (attribute.compare("treb_gain") == 0)
    {
        m_treb_gain = value;
    }
    else if (attribute.compare("on") == 0)
    {
        // Say it's off if its below half of logic level
        if (value < 0.5)
        {
            m_on = false;
        }
        else
        {
            m_on = true;
        }
    }
    else
    {
        // Attribute does not exist in effect
        throw std::runtime_error("Error - no attribute " + attribute + " exists in Effect_Equalizer");
    }
}

void Effect_Equalizer::serialize(char* output)
{
    // Copy object to output char
    memcpy(output, this, sizeof(Effect_Equalizer));
}

bool Effect_Equalizer::issame(const Effect* other)
{
    // Cast
    Effect_Equalizer* otherc = (Effect_Equalizer*) other;

    // Compare
    bool same = true;
    same &= m_lowmid == otherc->m_lowmid;
    same &= m_midhigh == otherc->m_midhigh;
    same &= m_base_gain == otherc->m_base_gain;
    same &= m_mid_gain == otherc->m_mid_gain;
    same &= m_treb_gain == otherc->m_treb_gain;
    same &= m_on == otherc->m_on;
    return same;
}

// Lowpass
Effect_Lowpass::Effect_Lowpass()
{
    m_cutoff = 0;
    m_resonance = 0;
    m_on = false;
}

Effect_Lowpass::Effect_Lowpass(float cutoff, float resonance, bool on)
{
    m_cutoff = cutoff;
    m_resonance = resonance;
    m_on = on;
}

Effect_Lowpass::Effect_Lowpass(const char* msg)
{
    // Copy char to new object
    Effect_Lowpass* msgObj = new Effect_Lowpass();
    memcpy(msgObj, msg, sizeof(Effect_Lowpass));

    // Copy member variables
    m_cutoff = msgObj->m_cutoff;
    m_resonance = msgObj->m_resonance;
    m_on = msgObj->m_on;
}

Effect_Lowpass::Effect_Lowpass(Json::Value config)
{
    // Read default values into effect
    m_cutoff = config["cutoff"].get("default", 0).asDouble();
    m_resonance = config["resonance"].get("default", 0).asDouble();
    m_on = config["on"].get("default", 0).asInt();
}

void Effect_Lowpass::update(string attribute, float value)
{
    if (attribute.compare("cutoff") == 0)
    {
        m_cutoff = value;
    }
    else if (attribute.compare("resonance") == 0)
    {
        m_resonance = value;
    }
    else if (attribute.compare("on") == 0)
    {
        // Say it's off if its below half of logic level
        if (value < 0.5)
        {
            m_on = false;
        }
        else
        {
            m_on = true;
        }
    }
    else
    {
        // Attribute does not exist in effect
        throw std::runtime_error("Error - no attribute " + attribute + " exists in Effect_Lowpass");
    }
}

void Effect_Lowpass::serialize(char* output)
{
    // Copy object to output char
    memcpy(output, this, sizeof(Effect_Lowpass));
}

bool Effect_Lowpass::issame(const Effect* other)
{
    // Cast
    Effect_Lowpass* otherc = (Effect_Lowpass*) other;

    // Compare
    bool same = true;
    same &= m_cutoff == otherc->m_cutoff;
    same &= m_resonance == otherc->m_resonance;
    same &= m_on == otherc->m_on;
    return same;
}

// Reverb
Effect_Reverb::Effect_Reverb()
{
    m_amount = 0;
    m_on = false;
}

Effect_Reverb::Effect_Reverb(float amount, bool on)
{
    m_amount = amount;
    m_on = on;
}

Effect_Reverb::Effect_Reverb(const char* msg)
{
    // Copy char to new object
    Effect_Reverb* msgObj = new Effect_Reverb();
    memcpy(msgObj, msg, sizeof(Effect_Reverb));

    // Copy member variables
    m_amount = msgObj->m_amount;
    m_on = msgObj->m_on;
}

Effect_Reverb::Effect_Reverb(Json::Value config)
{
    // Read default values into effect
    m_amount = config["amount"].get("default", 0).asDouble();
    m_on = config["on"].get("default", 0).asInt();
}

void Effect_Reverb::update(string attribute, float value)
{
    if (attribute.compare("amount") == 0)
    {
        m_amount = value;
    }
    else if (attribute.compare("on") == 0)
    {
        // Say it's off if its below half of logic level
        if (value < 0.5)
        {
            m_on = false;
        }
        else
        {
            m_on = true;
        }
    }
    else
    {
        // Attribute does not exist in effect
        throw std::runtime_error("Error - no attribute " + attribute + " exists in Effect_Reverb");
    }
}

void Effect_Reverb::serialize(char* output)
{
    // Copy object to output char
    memcpy(output, this, sizeof(Effect_Reverb));
}

bool Effect_Reverb::issame(const Effect* other)
{
    // Cast
    Effect_Reverb* otherc = (Effect_Reverb*) other;    

    // Compare
    bool same = true;
    same &= m_amount == otherc->m_amount;
    same &= m_on == otherc->m_on;
    return same;
}

// Unknown
Effect_Unknown::Effect_Unknown()
{
}

Effect_Unknown::Effect_Unknown(const char* output)
{
}

void Effect_Unknown::update(string attribute, float value)
{
}

void Effect_Unknown::serialize(char* output)
{
}

bool Effect_Unknown::issame(const Effect* other)
{
    // Compare
    return true;
}

EffectType getEffectType(string effectstr)
{
    if (effectstr.compare("delay") == 0)
    {
        return DELAY;
    }
    else if (effectstr.compare("distortion") == 0)
    {
        return DISTORTION;
    }
    else if (effectstr.compare("equalizer") == 0)
    {
        return EQUALIZER;
    }
    else if (effectstr.compare("lowpass") == 0)
    {
        return LOWPASS;
    }
    else if (effectstr.compare("reverb") == 0)
    {
        return REVERB;
    }
    else
    {
        // Does not match a class
        return UNKNOWNEFFECT;
    }
}

int getEffectSize(EffectType type)
{
    switch (type)
    {
        case DELAY:
            return sizeof(Effect_Delay);

        case DISTORTION:
            return sizeof(Effect_Distortion);
        
        case EQUALIZER:
            return sizeof(Effect_Equalizer);

        case LOWPASS:
            return sizeof(Effect_Lowpass);

        case REVERB:
            return sizeof(Effect_Reverb);
        
        case UNKNOWNEFFECT:
            return sizeof(Effect_Unknown);
    }
}
