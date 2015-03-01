#include "profile.h"

using namespace std;

Profile::Profile()	
{
}

Profile::Profile(Json::Value config, HWState hw)
{
    // Get logic level from hw
    m_logic_level = hw.getLogicLevel();

    // Parse the config
    int num_effects = config.get("num_effects", 1).asInt();
    Json::Value effect_types = config["effect_types"];
    for (int i = 0; i < num_effects; i++)
    {
        // Get info for the effect
        string effectType = effect_types.get(i, "").asString();
        Effect* effect;
        Json::Value effectConfig;
        if (effectType.compare("delay") == 0)
        {
            effectConfig = config["delay"];
            effect = new Effect_Delay(effectConfig);
            updateMapping(effectConfig, effect, "delay");
        }
        else if (effectType.compare("distortion") == 0)
        {
            effectConfig = config["distortion"];
            effect = new Effect_Distortion(effectConfig);
            updateMapping(effectConfig, effect, "distortion");
        }
        else if (effectType.compare("equalizer") == 0)
        {
            effectConfig = config["equalizer"];
            effect = new Effect_Equalizer(effectConfig);
            updateMapping(effectConfig, effect, "equalizer");
        }
        else if (effectType.compare("lowpass") == 0)
        {
            effectConfig = config["lowpass"];
            effect = new Effect_Lowpass(effectConfig);
            updateMapping(effectConfig, effect, "lowpass");
        }
        else if (effectType.compare("reverb") == 0)
        {
            effectConfig = config["reverb"];
            effect = new Effect_Reverb(effectConfig);
            updateMapping(effectConfig, effect, "reverb");
        }
        else
        {
            // Didn't compare anything
            throw std::runtime_error("Error in parsing - unknown effect type");
        }
    }
}

Profile::~Profile()
{
    // Delete effects pointers
    for (int i = 0; i < m_effects.size(); i++)
    {
        delete m_effects[i];
    }
}

void Profile::update(HWState hw)
{
    // Get necessary variables and constants
    double logic_level = hw.getLogicLevel();

    // Update potentiometers
    for (int idx = 0; idx < hw.getNumPot(); idx++)
    {
        // Get mapping information
        ControlInfo key(POTENTIOMETER, idx);
        if (m_mapping.count(key) == 0)
        {
            continue;   // This control doesn't map to anything
        }
        EffectInfo effect_info = m_mapping[key];
        Effect* effect = effect_info.m_effect;
        string attr = effect_info.m_attributestr;
        ScaleType scaletype = effect_info.m_scale_type;
        float min = effect_info.m_min;
        float max = effect_info.m_max;

        // Get voltage level from control
        float control_value = hw.getValue(POTENTIOMETER, idx);
        float attr_value = 0;

        // Convert to attribute value and call effect's update
        if (scaletype == LINEAR)
        {
            // Scale pot value linearly between min and max
            attr_value = min + (control_value/logic_level)*(max-min);
        }
        else if (scaletype == LOG)
        {
            
            // Scale pot value linearly between min and max
            attr_value = log((control_value/logic_level) * (logic_level - (1/exp(2)))) + 2; // Value that scales log between 0 and 2
            attr_value = min + 0.5*attr_value*(min-max);    // Now scales between min and max
        }
        else
        {
            // Scale type is not valid for this type of control
            throw std::runtime_error("Invalid scale type for potentiometer control");            
        }
        effect->update(attr, attr_value);
    }

    // Update switches
    for (int idx = 0; idx < hw.getNumSwitch(); idx++)
    {
        // Get mapping information
        ControlInfo key(SWITCH, idx);
        if (m_mapping.count(key) == 0)
        {
            continue;   // This control doesn't map to anything
        }
        EffectInfo effect_info = m_mapping[key];
        Effect* effect = effect_info.m_effect;
        string attr = effect_info.m_attributestr;
        ScaleType scaletype = effect_info.m_scale_type;

        // Get voltage level from control
        float control_value = hw.getValue(SWITCH, idx);
        float attr_value = 0;

        // Convert to attribute value and call effect's update
        if (scaletype == SWITCHSCALE)
        {
            // If less than 1/2 logic level, it's 0. Otherwise 1.
            if (control_value < 0.5 * logic_level)
            {
                attr_value = 0.0;
            }
            else
            {
                attr_value = 1.0;
            }
        }
        else
        {
            // Scale type is not valid for this type of control
            throw std::runtime_error("Invalid scale type for switch control");            
        }
        effect->update(attr, attr_value);
    }

    // Update touchscreen
    for (int idx = 0; idx < 2; idx++)
    {
        // Get mapping information
        ControlInfo key(TOUCHSCREEN, idx);
        if (m_mapping.count(key) == 0)
        {
            continue;   // This control doesn't map to anything
        }
        EffectInfo effect_info = m_mapping[key];
        Effect* effect = effect_info.m_effect;
        string attr = effect_info.m_attributestr;
        ScaleType scaletype = effect_info.m_scale_type;
        float min = effect_info.m_min;
        float max = effect_info.m_max;

        // Get voltage level from control
        float control_value = hw.getValue(TOUCHSCREEN, idx);
        float attr_value = 0;
        int num_pixels = 1;
        if (idx == 0)
        {
            num_pixels = hw.getTouchWidth();    // Index 0 of touchscreen is x control
        }
        else
        {
            num_pixels = hw.getTouchHeight();   // Index 1 of touchscreen is y control    
        }

        // Convert to attribute value and call effect's update
        if (scaletype == LINEAR)
        {
            // Scale pot value linearly between min and max
            attr_value = min + (control_value/(double)num_pixels)*(max-min);
        }
        else if (scaletype == LOG)
        {
            
            // Scale pot value linearly between min and max
            attr_value = log((control_value/(double)num_pixels) * ((double)num_pixels - (1/exp(2)))) + 2; // Value that scales log between 0 and 2
            attr_value = min + 0.5*attr_value*(min-max);    // Now scales between min and max
        }
        else
        {
            // Scale type is not valid for this type of control
            throw std::runtime_error("Invalid scale type for potentiometer control");            
        }
        effect->update(attr, attr_value);
    }
}

vector<Effect*> Profile::getEffectCopy()
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

vector<EffectType> Profile::getEffectTypes()
{
    return m_effect_types;
}

void Profile::updateMapping(Json::Value config, Effect* effect, string effectstr)
{
    // Update m_mapping so that it correctly hooks hardware and effect attributes
    EffectType type = getEffectType(effectstr); 
    switch(type)
    {
        case DELAY:
            {
                // Update for each attribute
                Json::Value amountConfig = config["amount"];
                updateMappingAttr(amountConfig, effect, "amount");
                Json::Value speedConfig = config["speed"];
                updateMappingAttr(speedConfig, effect, "speed");
                Json::Value decayConfig = config["decay"];
                updateMappingAttr(decayConfig, effect, "decay");
                Json::Value attackConfig = config["attack"];
                updateMappingAttr(attackConfig, effect, "attack");
                Json::Value onConfig = config["on"];
                updateMappingAttr(onConfig, effect, "on");
            }
            break;
        case DISTORTION:
            {
                // Update for each attribute
                Json::Value amountConfig = config["amount"];
                updateMappingAttr(amountConfig, effect, "amount");
                Json::Value onConfig = config["on"];
                updateMappingAttr(onConfig, effect, "on");
            }
            break;
        case EQUALIZER:
            {
                // Update for each attribute
                Json::Value lowmidConfig = config["lowmid"];
                updateMappingAttr(lowmidConfig, effect, "lowmid");
                Json::Value midhighConfig = config["midhigh"];
                updateMappingAttr(midhighConfig, effect, "midhigh");
                Json::Value base_gainConfig = config["base_gain"];
                updateMappingAttr(base_gainConfig, effect, "base_gain");
                Json::Value mid_gainConfig = config["mid_gain"];
                updateMappingAttr(mid_gainConfig, effect, "mid_gain");
                Json::Value treb_gainConfig = config["treb_gain"];
                updateMappingAttr(treb_gainConfig, effect, "midhigh");
                Json::Value onConfig = config["on"];
                updateMappingAttr(onConfig, effect, "on");
            }
            break;

        case LOWPASS:
            {
                // Update for each attribute
                Json::Value cutoffConfig = config["cutoff"];
                updateMappingAttr(cutoffConfig, effect, "cutoff");
                Json::Value resonanceConfig = config["resonance"];
                updateMappingAttr(resonanceConfig, effect, "resonance");
                Json::Value onConfig = config["on"];
                updateMappingAttr(onConfig, effect, "on");
            }
            break;

        case REVERB:
            {
                // Update for each attribute
                Json::Value amountConfig = config["amount"];
                updateMappingAttr(amountConfig, effect, "amount");
                Json::Value onConfig = config["on"];
                updateMappingAttr(onConfig, effect, "on");
            }
            break;
        case UNKNOWNEFFECT:
        default:
            throw std::runtime_error("Could not parse effect " + effectstr + " - no such effect");
            break;
    }
}

void Profile::updateMappingAttr(Json::Value config, Effect* effect, string attributestr)
{
    string scalestr = config["control"].asString();
    ScaleType scaletype = getScaleType(scalestr);
    if (scaletype == UNKNOWNSCALE)
    {
        throw std::runtime_error("Could not find control type " + scalestr + " - no such");
    }
    // If not static, put in mapping table
    else if (scaletype != STATIC)
    {
        // Get min and max values
        float min = 0, max = 0;
        if (scaletype == SWITCHSCALE)
        {
            min = 0;
            max = 1;
        }
        else
        {
            min = config.get("min", 0).asDouble();
            max = config.get("max", 0).asDouble();
        }
    
        // Get hardware control info
        string hwstr = config.get("hwtype", "").asString();
        ControlType hwtype = getControlType(hwstr);
        if (hwtype == UNKNOWNCONTROL)
        {
            throw std::runtime_error("Could not control type " + hwstr + " - no such");
        }
        int index = config.get("hwidx", 0).asInt();

        // Populate mapping table
        ControlInfo control(hwtype, index);
        EffectInfo effectData(effect, attributestr, scaletype, min, max);
        m_mapping[control] = effectData;
    }
}
