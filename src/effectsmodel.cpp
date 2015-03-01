#include "effectsmodel.h"

EffectsModel::EffectsModel()	
{
}

EffectsModel::EffectsModel(const char* hwConfigFile, const char* mappingConfigFile)
{
    // Create hardware state object
    m_hw = HWState(hwConfigFile);

    // Parse the mapping file (partially)
    Json::Value config;
    parseJSONFile(mappingConfigFile, config);
    int num_profiles = config.get("numprofiles", 1).asInt();

    // Create new profiles and have those parse each of the profiles
    for (int i = 0; i < num_profiles; i++)
    {
        // Make profile and pass in config
        stringstream ss;
        ss << "profile" << i;
        string profileStr = ss.str();
        m_profiles.push_back(Profile(config[profileStr], m_hw));
    }
}

void EffectsModel::updateModel(EventInfo event)
{
    // Update effects using update functions
    switch (event.m_type)
    {
        case TIMER:
            updateSwitchDials();
            break;
        case KEYPRESS:
            updateKeyProfile(event);
            break;
        case MOUSE:
            updateMouse(event);
            break;
    }

    // Update profile
    m_profiles[m_curProfile].update(m_hw);

    // Send updated effects profile to dsp
    updateDSP();

    // Call redisplay?
    // TODO - should this be here, or somewhere in main file that calls this
}

void EffectsModel::updateDSP()
{
    // Get effects message
    EffectUpdateMessage message(m_profiles[m_curProfile]);

    // Get message as char[]
    char msg[1024];
    message.serialize(msg);

    // Send message over IPC
    // TODO
}

bool EffectsModel::updateSwitchDials()
{
    // Update potentiometer values
    int numPot = m_hw.getNumPot();
    for (int idx = 0; idx < numPot; idx++)
    {
        int pin = m_hw.getPinNumber(POTENTIOMETER, idx);
        float value = 0;

        // TODO - Get voltage value on pin       

        //m_hw.setValue(POTENTIOMETER, idx, value);
    }

    // Update switch values
    int numSwitch = m_hw.getNumSwitch();
    for (int idx = 0; idx < numSwitch; idx++)
    {
        int pin = m_hw.getPinNumber(SWITCH, idx);
        float value = 0;

        // TODO - Get voltage value on pin       

        //m_hw.setValue(SWITCH, idx, value);
    }
}

bool EffectsModel::updateMouse(EventInfo event)
{
    // Update hw values
    m_hw.setValue(TOUCHSCREEN, 1, event.m_mouse_x);  // Set x value
    m_hw.setValue(TOUCHSCREEN, 0, event.m_mouse_y);  // Set y value
    
    // TODO - check all these TOUCHSCREEN indices, x as 1 makes no sense really    
}

bool EffectsModel::updateKeyProfile(EventInfo event)
{
    // Check key and switch profiles if necessary
    char ch = event.m_keyvalue;
    int profileIndex = m_hw.getButtonIndex(ch);

    // Update current index
    m_curProfile = profileIndex;
} 
