#include "effectsmodel.h"

EffectsModel::EffectsModel()	
{
}

EffectsModel::EffectsModel(char* hwConfigFile, char* mappingConfigFile)
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
}

void EffectsModel::updateDSP()
{
}

bool EffectsModel::updateSwitchDials(EventInfo event)
{
}

bool EffectsModel::updateMouse(EventInfo event)
{
}

bool EffectsModel::updateKeyProfile(EventInfo event)
{
} 
