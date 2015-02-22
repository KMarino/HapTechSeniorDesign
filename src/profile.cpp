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

void Profile::update(HWState hw)
{
    // TODO

    // Go through all of the devices and use m_mapping to do update

    // Don't forget about the touchscreen    
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
        control_info control(hwtype, index);
        EffectInfo effectData(effect, attributestr, scaletype, min, max);
        m_mapping[control] = effectData;
    }
}
