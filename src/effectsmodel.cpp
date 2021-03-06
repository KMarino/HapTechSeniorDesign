#include "effectsmodel.h"
#include "sockClient.h"

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

    // Set current profile to 0 as default
    m_curProfile = 0;
}

EffectUpdateMessage EffectsModel::updateModel(EventInfo event)
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
    return updateDSP();
}

int EffectsModel::getTouchWidth()
{
    return m_hw.getTouchWidth();
}
    
int EffectsModel::getTouchHeight()
{
    return m_hw.getTouchHeight();
}

EffectUpdateMessage EffectsModel::updateDSP()
{
    // Get effects message
    EffectUpdateMessage message(m_profiles[m_curProfile]);
	return message;
 //debug	cout << message.getEffectCopy().size() << " num effects \n";

    // Get message as char[]
   // int msgSize = message.getMessageSize();
//debug	std::cout << msgSize<< " message size \n";
  //  char msg[msgSize];
   // message.serialize(msg);

    // Send msg over IPC
    // TODO
//    cliSock.sockSend(msg, msgSize);

	
}

bool EffectsModel::updateSwitchDials()
{
    // Update potentiometer values
    int numPot = m_hw.getNumPot();
    for (int idx = 0; idx < numPot; idx++)
    {
        int pin = m_hw.getPinNumber(POTENTIOMETER, idx);
        float value = 0;

		if (!HW_SIM)
		{
			// Read voltage from hardware
			value = readAI(pin); 
		}
		else
		{
			// Dummy valu
			value = 1;
		}    

		// Set value for update
        m_hw.setValue(POTENTIOMETER, idx, value);
    }

    // Update switch values
    int numSwitch = m_hw.getNumSwitch();
    for (int idx = 0; idx < numSwitch; idx++)
    {
        int pin = m_hw.getPinNumber(SWITCH, idx);
        float value = 0;

		if (!HW_SIM)
		{
			// Read logic level from hardware
			value = (float) readGPIO(pin);
		}
		else
		{
	// Dummy valu
			value = 1;
		}

		// Set value for update
		m_hw.setValue(SWITCH, idx, value);
    }
}

bool EffectsModel::updateMouse(EventInfo event)
{
    // Update hw values
    m_hw.setValue(TOUCHSCREEN, 0, event.m_mouse_x);  // Set x value
    m_hw.setValue(TOUCHSCREEN, 1, event.m_mouse_y);  // Set y value  
}

bool EffectsModel::updateKeyProfile(EventInfo event)
{
    // Check key and switch profiles if necessary
    char ch = event.m_keyvalue;
    int profileIndex = m_hw.getButtonIndex(ch);

    // Update current index
    m_curProfile = profileIndex;
}
