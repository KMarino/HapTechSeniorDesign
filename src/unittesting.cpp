#include <string>
#include <iostream>
#include <cmath>
#include "gtest/gtest.h"
#include "optparser.h"
#include "effect.h"
#include "effectupdatemessage.h"
#include "hwstate.h"
#include "eventinfo.h"
#include "effectsmodel.h" 

using namespace std;

string configLocation;

/*---------------------------------
------------- effects -------------
-----------------------------------*/
// Effect_Delay
TEST(EffectTest, Effect_Delay)
{
    // Check default constructor
    Effect_Delay delay;
    EXPECT_EQ(delay.m_amount, 0);
    EXPECT_EQ(delay.m_speed, 0);
    EXPECT_EQ(delay.m_decay, 0);
    EXPECT_EQ(delay.m_attack, 0);
    EXPECT_EQ(delay.m_on, false);

    // Check parameter constructor
    srand(0);
    float amount = (float)rand() / RAND_MAX;
    float speed = (float)rand() / RAND_MAX;
    float decay = (float)rand() / RAND_MAX;
    float attack = (float)rand() / RAND_MAX;
    bool on = true;
    Effect_Delay delay2(amount, speed, decay, attack, on);
    EXPECT_EQ(delay2.m_amount, amount);
    EXPECT_EQ(delay2.m_speed, speed);
    EXPECT_EQ(delay2.m_decay, decay);
    EXPECT_EQ(delay2.m_attack, attack);
    EXPECT_EQ(delay2.m_on, on);

    // Check serialization
    char delay_serial[256];
    delay2.serialize(delay_serial);
    Effect_Delay delay3(delay_serial);
    EXPECT_EQ(delay3.m_amount, amount);
    EXPECT_EQ(delay3.m_speed, speed);
    EXPECT_EQ(delay3.m_decay, decay);
    EXPECT_EQ(delay3.m_attack, attack);
    EXPECT_EQ(delay3.m_on, on);

    // Check update
    delay2.update("amount", 2*amount);
    delay2.update("speed", 2*speed);
    delay2.update("decay", 2*decay);
    delay2.update("attack", 2*attack);
    delay2.update("on", !on);
    EXPECT_EQ(delay2.m_amount, 2*amount);
    EXPECT_EQ(delay2.m_speed, 2*speed);
    EXPECT_EQ(delay2.m_decay, 2*decay);
    EXPECT_EQ(delay2.m_attack, 2*attack);
    EXPECT_EQ(delay2.m_on, !on);
}

// Effect_Distortion
TEST(EffectTest, Effect_Distortion)
{
    // Check default constructor
    Effect_Distortion distort;
    EXPECT_EQ(distort.m_amount, 0);
    EXPECT_EQ(distort.m_on, false);

    // Check parameter constructor
    srand(1);
    float amount = (float)rand() / RAND_MAX;
    bool on = true;
    Effect_Distortion distort2(amount, on);
    EXPECT_EQ(distort2.m_amount, amount);
    EXPECT_EQ(distort2.m_on, on);

    // Check serialization
    char distort_serial[256];
    distort2.serialize(distort_serial);
    Effect_Distortion distort3(distort_serial);
    EXPECT_EQ(distort3.m_amount, amount);
    EXPECT_EQ(distort3.m_on, on);

    // Check update
    distort2.update("amount", 2*amount);
    distort2.update("on", !on);
    EXPECT_EQ(distort2.m_amount, 2*amount);
    EXPECT_EQ(distort2.m_on, !on);
}

// Effect_Equalizer
TEST(EffectTest, Effect_Equalizer)
{
    // Check default constructor
    Effect_Equalizer eq;
    EXPECT_EQ(eq.m_lowmid, 0);
    EXPECT_EQ(eq.m_midhigh, 0);
    EXPECT_EQ(eq.m_base_gain, 0);
    EXPECT_EQ(eq.m_mid_gain, 0);
    EXPECT_EQ(eq.m_treb_gain, 0);
    EXPECT_EQ(eq.m_on, false);

    // Check parameter constructor
    srand(2);
    float lowmid = (float)rand() / RAND_MAX;
    float midhigh = (float)rand() / RAND_MAX;
    float base_gain = (float)rand() / RAND_MAX;
    float mid_gain = (float)rand() / RAND_MAX;
    float treb_gain = (float)rand() / RAND_MAX;
    bool on = true;
    Effect_Equalizer eq2(lowmid, midhigh, base_gain, mid_gain, treb_gain, on);
    EXPECT_EQ(eq2.m_lowmid, lowmid);
    EXPECT_EQ(eq2.m_midhigh, midhigh);
    EXPECT_EQ(eq2.m_base_gain, base_gain);
    EXPECT_EQ(eq2.m_mid_gain, mid_gain);
    EXPECT_EQ(eq2.m_treb_gain, treb_gain);
    EXPECT_EQ(eq2.m_on, on);

    // Check serialization
    char eq_serial[256];
    eq2.serialize(eq_serial);
    Effect_Equalizer eq3(eq_serial);
    EXPECT_EQ(eq3.m_lowmid, lowmid);
    EXPECT_EQ(eq3.m_midhigh, midhigh);
    EXPECT_EQ(eq3.m_base_gain, base_gain);
    EXPECT_EQ(eq3.m_mid_gain, mid_gain);
    EXPECT_EQ(eq3.m_treb_gain, treb_gain);
    EXPECT_EQ(eq3.m_on, on);

    // Check update
    eq2.update("lowmid", 2*lowmid);
    eq2.update("midhigh", 2*midhigh);
    eq2.update("base_gain", 2*base_gain);
    eq2.update("mid_gain", 2*mid_gain);
    eq2.update("treb_gain", 2*treb_gain);
    eq2.update("on", !on);
    EXPECT_EQ(eq2.m_lowmid, 2*lowmid);
    EXPECT_EQ(eq2.m_midhigh, 2*midhigh);
    EXPECT_EQ(eq2.m_base_gain, 2*base_gain);
    EXPECT_EQ(eq2.m_mid_gain, 2*mid_gain);
    EXPECT_EQ(eq2.m_treb_gain, 2*treb_gain);
    EXPECT_EQ(eq2.m_on, !on);
}

// Effect_Lowpass
TEST(EffectTest, Effect_Lowpass)
{
    // Check default constructor
    Effect_Lowpass low;
    EXPECT_EQ(low.m_cutoff, 0);
    EXPECT_EQ(low.m_resonance, 0);
    EXPECT_EQ(low.m_on, false);

    // Check parameter constructor
    srand(3);
    float cutoff = (float)rand() / RAND_MAX;
    float resonance = (float)rand() / RAND_MAX;
    bool on = true;
    Effect_Lowpass low2(cutoff, resonance, on);
    EXPECT_EQ(low2.m_cutoff, cutoff);
    EXPECT_EQ(low2.m_resonance, resonance);
    EXPECT_EQ(low2.m_on, on);

    // Check serialization
    char low_serial[256];
    low2.serialize(low_serial);
    Effect_Lowpass low3(low_serial);
    EXPECT_EQ(low3.m_cutoff, cutoff);
    EXPECT_EQ(low3.m_resonance, resonance);
    EXPECT_EQ(low3.m_on, on);

    // Check update
    low2.update("cutoff", 2*cutoff);
    low2.update("resonance", 2*resonance);
    low2.update("on", !on);
    EXPECT_EQ(low2.m_cutoff, 2*cutoff);
    EXPECT_EQ(low2.m_resonance, 2*resonance);
    EXPECT_EQ(low2.m_on, !on);
}

// Effect_Reverb
TEST(EffectTest, Effect_Reverb)
{
    // Check default constructor
    Effect_Reverb reverb;
    EXPECT_EQ(reverb.m_amount, 0);
    EXPECT_EQ(reverb.m_on, false);

    // Check parameter constructor
    srand(4);
    float amount = (float)rand() / RAND_MAX;
    bool on = true;
    Effect_Reverb reverb2(amount, on);
    EXPECT_EQ(reverb2.m_amount, amount);
    EXPECT_EQ(reverb2.m_on, on);

    // Check serialization
    char reverb_serial[256];
    reverb2.serialize(reverb_serial);
    Effect_Reverb reverb3(reverb_serial);
    EXPECT_EQ(reverb3.m_amount, amount);
    EXPECT_EQ(reverb3.m_on, on);

    // Check update
    reverb2.update("amount", 2*amount);
    reverb2.update("on", !on);
    EXPECT_EQ(reverb2.m_amount, 2*amount);
    EXPECT_EQ(reverb2.m_on, !on);
}

// Effect_Unknown
TEST(EffectTest, Effect_Unknown)
{
    // Check default constructor
    Effect_Unknown unk;

    // Check serialization
    char unk_serial[256];
    unk.serialize(unk_serial);
    Effect_Unknown unk2(unk_serial);
}

// Test getEffectSize
TEST(EffectTest, getEffectSize)
{
    // Test size of various effects
    EXPECT_EQ(getEffectSize(DELAY), sizeof(Effect_Delay));
    EXPECT_EQ(getEffectSize(DISTORTION), sizeof(Effect_Distortion));
    EXPECT_EQ(getEffectSize(EQUALIZER), sizeof(Effect_Equalizer));
    EXPECT_EQ(getEffectSize(LOWPASS), sizeof(Effect_Lowpass));
    EXPECT_EQ(getEffectSize(REVERB), sizeof(Effect_Reverb));
    EXPECT_EQ(getEffectSize(UNKNOWNEFFECT), sizeof(Effect_Unknown));
}

// Test getEffectType
TEST(EffectTest, getEffectType)
{
    EXPECT_EQ(getEffectType("delay"), DELAY);
    EXPECT_EQ(getEffectType("distortion"), DISTORTION);
    EXPECT_EQ(getEffectType("equalizer"), EQUALIZER);
    EXPECT_EQ(getEffectType("lowpass"), LOWPASS);
    EXPECT_EQ(getEffectType("reverb"), REVERB);
    EXPECT_EQ(getEffectType("asdfadsf"), UNKNOWNEFFECT);
}

// All effects, checking parsing
TEST(EffectTest, JSONParse)
{
    // Get config file and parse
    char configFilepath[1024];
    strcpy(configFilepath, string(configLocation + "mapping.json").c_str());
    Json::Value config;
    parseJSONFile(configFilepath, config);

    // Now make sure constructors properly get default values
    // Get all the configs
    Json::Value configP0 = config["profile0"];
    Json::Value configDistortion = configP0["distortion"];
    Json::Value configP1 = config["profile1"];
    Json::Value configEqualizer = configP1["equalizer"];  
    Json::Value configP2 = config["profile2"];
    Json::Value configLowpass = configP2["lowpass"]; 
    Json::Value configDelay = configP2["delay"];
    Json::Value configP3 = config["profile3"];
    Json::Value configReverb = configP3["reverb"];

    // Make effects
    Effect_Delay delay(configDelay);
    Effect_Distortion distortion(configDistortion);
    Effect_Equalizer equalizer(configEqualizer);
    Effect_Lowpass lowpass(configLowpass);
    Effect_Reverb reverb(configReverb);    

    // Check values are same as defaults in config file
    EXPECT_EQ(delay.m_amount, 0);
    EXPECT_EQ(delay.m_speed, 0);
    EXPECT_EQ(delay.m_decay, 0);
    EXPECT_EQ(delay.m_attack, 0);
    EXPECT_EQ(delay.m_on, 0);
    EXPECT_EQ(distortion.m_amount, 0);
    EXPECT_EQ(distortion.m_on, 0);
    EXPECT_EQ(equalizer.m_lowmid, 600);
    EXPECT_EQ(equalizer.m_midhigh, 6000);
    EXPECT_EQ(equalizer.m_base_gain, 0);
    EXPECT_EQ(equalizer.m_mid_gain, 0);
    EXPECT_EQ(equalizer.m_treb_gain, 0);
    EXPECT_EQ(equalizer.m_on, 0);
    EXPECT_EQ(lowpass.m_cutoff, 12000);
    EXPECT_EQ(lowpass.m_resonance, 0);
    EXPECT_EQ(lowpass.m_on, 0);
    EXPECT_EQ(reverb.m_amount, 0);
    EXPECT_EQ(reverb.m_on, 0);
}

/*---------------------------------
------------- hwstate -------------
-----------------------------------*/
// Test parsing and constructor
TEST(hwstate, testConfigParse)
{
    // Make hwstate object
    char configFilepath[1024];
    strcpy(configFilepath, string(configLocation + "/hardware.json").c_str());
    HWState hwstate(configFilepath);

    // Check values are same as in config
    EXPECT_EQ(hwstate.getNumDevices(), 12);
    EXPECT_EQ(hwstate.getNumPot(), 5);
    EXPECT_EQ(hwstate.getNumSwitch(), 3);
    EXPECT_EQ(hwstate.getNumPush(), 4);
    EXPECT_EQ(hwstate.getTouchWidth(), 480);
    EXPECT_EQ(hwstate.getTouchHeight(), 560);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 0), 0);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 0), 0);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 1), 1);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 1), 0);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 2), 2);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 2), 0);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 3), 3);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 3), 0);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 4), 4);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 4), 0);
    EXPECT_EQ(hwstate.getPinNumber(SWITCH, 0), 5);
    EXPECT_EQ(hwstate.getValue(SWITCH, 0), 0);
    EXPECT_EQ(hwstate.getPinNumber(SWITCH, 1), 6);
    EXPECT_EQ(hwstate.getValue(SWITCH, 1), 0);
    EXPECT_EQ(hwstate.getPinNumber(SWITCH, 2), 7);
    EXPECT_EQ(hwstate.getValue(SWITCH, 2), 0);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 0), 8);
    EXPECT_EQ(hwstate.getValue(PUSHBUTTON, 0), 0);
    EXPECT_EQ(hwstate.getButtonIndex('a'), 0);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 1), 9);
    EXPECT_EQ(hwstate.getValue(PUSHBUTTON, 1), 0);
    EXPECT_EQ(hwstate.getButtonIndex('s'), 1);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 2), 10);
    EXPECT_EQ(hwstate.getValue(PUSHBUTTON, 2), 0);
    EXPECT_EQ(hwstate.getButtonIndex('d'), 2);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 3), 11);
    EXPECT_EQ(hwstate.getValue(PUSHBUTTON, 3), 0);    
    EXPECT_EQ(hwstate.getButtonIndex('f'), 3);
    EXPECT_EQ(hwstate.getLogicLevel(), 5.0);
}

// Test get and set value
TEST(hwstate, testGetSet)
{
    // Make hwstate object
    char configFilepath[1024];
    strcpy(configFilepath, string(configLocation + "/hardware.json").c_str());
    HWState hwstate(configFilepath);

    // Set new values for attributes
    hwstate.setValue(POTENTIOMETER, 0, 1.0);
    hwstate.setValue(POTENTIOMETER, 1, 2.0);
    hwstate.setValue(POTENTIOMETER, 2, 3.0);
    hwstate.setValue(POTENTIOMETER, 3, 4.0);
    hwstate.setValue(POTENTIOMETER, 4, 5.0);
    hwstate.setValue(SWITCH, 0, 0.0);
    hwstate.setValue(SWITCH, 1, 1.0);
    hwstate.setValue(SWITCH, 2, 1.0);
    hwstate.setValue(TOUCHSCREEN, 0, 132);
    hwstate.setValue(TOUCHSCREEN, 1, 245);

    // Get values back
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 0), 1.0);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 1), 2.0);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 2), 3.0);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 3), 4.0);
    EXPECT_EQ(hwstate.getValue(POTENTIOMETER, 4), 5.0);
    EXPECT_EQ(hwstate.getValue(SWITCH, 0), 0.0);
    EXPECT_EQ(hwstate.getValue(SWITCH, 1), 1.0);
    EXPECT_EQ(hwstate.getValue(SWITCH, 2), 1.0);
    EXPECT_EQ(hwstate.getValue(TOUCHSCREEN, 0), 132);
    EXPECT_EQ(hwstate.getValue(TOUCHSCREEN, 1), 245);
}

// Test getControlType
TEST(hwstate, getControlType)
{
    EXPECT_EQ(getControlType("pot"), POTENTIOMETER);
    EXPECT_EQ(getControlType("switch"), SWITCH);
    EXPECT_EQ(getControlType("push"), PUSHBUTTON);
    EXPECT_EQ(getControlType("touch"), TOUCHSCREEN);
    EXPECT_EQ(getControlType("hjsfdgas"), UNKNOWNCONTROL);
}

// Test getScaleType
TEST(hwstate, getScaleType)
{
    EXPECT_EQ(getScaleType("linear"), LINEAR);
    EXPECT_EQ(getScaleType("log"), LOG);
    EXPECT_EQ(getScaleType("switch"), SWITCHSCALE);
    EXPECT_EQ(getScaleType("static"), STATIC);
    EXPECT_EQ(getScaleType("dfhdghd"), UNKNOWNSCALE);   
}

/*---------------------------------
------------- profile -------------
-----------------------------------*/
TEST(Profile, testProfile)
{
    // Get the config files
    char hardwareConfigFilepath[1024];
    strcpy(hardwareConfigFilepath, string(configLocation + "/hardware.json").c_str());
    char mappingConfigFilepath[1024];
    strcpy(mappingConfigFilepath, string(configLocation + "/mapping.json").c_str());

    // Make hwstate object
    HWState hwstate(hardwareConfigFilepath);

    // Parse the mapping file (partially)
    Json::Value mapConfig;
    parseJSONFile(mappingConfigFilepath, mapConfig);
    int num_profiles = mapConfig.get("numprofiles", 1).asInt();
    EXPECT_EQ(num_profiles, 4);

    // Create new profiles and have those parse each of the profiles
    vector<Profile> profiles;
    for (int i = 0; i < num_profiles; i++)
    {
        // Make profile and pass in config
        stringstream ss;
        ss << "profile" << i;
        string profileStr = ss.str();
        Json::Value configProfile = mapConfig[profileStr];
        Profile prof(configProfile, hwstate);
        profiles.push_back(Profile(mapConfig[profileStr], hwstate));
    }

    // Check each profile contains the right stuff
    // Profile 0    
    Profile profile0 = profiles[0];
    vector<Effect*> effects0 = profile0.getEffectCopy();
    vector<EffectType> types0 = profile0.getEffectTypes();
    EXPECT_EQ(effects0.size(), 1);
    EXPECT_EQ(types0.size(), 1);
    EXPECT_EQ(types0[0], DISTORTION);
    Effect_Distortion distortion = *((Effect_Distortion*) effects0[0]);
    EXPECT_EQ(distortion.m_amount, 0);
    EXPECT_EQ(distortion.m_on, 0);

    // Profile 1
    Profile profile1 = profiles[1];
    vector<Effect*> effects1 = profile1.getEffectCopy();
    vector<EffectType> types1 = profile1.getEffectTypes();
    EXPECT_EQ(effects1.size(), 1);
    EXPECT_EQ(types1.size(), 1);
    EXPECT_EQ(types1[0], EQUALIZER);
    Effect_Equalizer equalizer = *((Effect_Equalizer*) effects1[0]);
    EXPECT_EQ(equalizer.m_lowmid, 600);
    EXPECT_EQ(equalizer.m_midhigh, 6000);
    EXPECT_EQ(equalizer.m_base_gain, 0);
    EXPECT_EQ(equalizer.m_mid_gain, 0);
    EXPECT_EQ(equalizer.m_treb_gain, 0);
    EXPECT_EQ(equalizer.m_on, 0);

    // Profile 2
    Profile profile2 = profiles[2];
    vector<Effect*> effects2 = profile2.getEffectCopy();
    vector<EffectType> types2 = profile2.getEffectTypes();
    EXPECT_EQ(effects2.size(), 2);
    EXPECT_EQ(types2.size(), 2);
    EXPECT_EQ(types2[0], LOWPASS);
    EXPECT_EQ(types2[1], DELAY);
    Effect_Lowpass lowpass = *((Effect_Lowpass*) effects2[0]);
    Effect_Delay delay = *((Effect_Delay*) effects2[1]);
    EXPECT_EQ(lowpass.m_cutoff, 12000);
    EXPECT_EQ(lowpass.m_resonance, 0);
    EXPECT_EQ(lowpass.m_on, 0);
    EXPECT_EQ(delay.m_amount, 0);
    EXPECT_EQ(delay.m_speed, 0);
    EXPECT_EQ(delay.m_decay, 0);
    EXPECT_EQ(delay.m_attack, 0);
    EXPECT_EQ(delay.m_on, 0);

    // Profile 3
    Profile profile3 = profiles[3];
    vector<Effect*> effects3 = profile3.getEffectCopy();
    vector<EffectType> types3 = profile3.getEffectTypes();
    EXPECT_EQ(effects3.size(), 1);
    EXPECT_EQ(types3.size(), 1);
    EXPECT_EQ(types3[0], REVERB);
    Effect_Reverb reverb = *((Effect_Reverb*) effects3[0]);
    EXPECT_EQ(reverb.m_amount, 0);
    EXPECT_EQ(reverb.m_on, 0);
}

// Test that the update function of Profile works correctly
TEST(Profile, testProfileUpdate)
{
    // Get the config files
    char hardwareConfigFilepath[1024];
    strcpy(hardwareConfigFilepath, string(configLocation + "/hardware.json").c_str());
    char mappingConfigFilepath[1024];
    strcpy(mappingConfigFilepath, string(configLocation + "/mapping.json").c_str());

    // Make hwstate object
    HWState hwstate(hardwareConfigFilepath);

    // Set new values for attributes
    hwstate.setValue(POTENTIOMETER, 0, 1.0);
    hwstate.setValue(POTENTIOMETER, 1, 2.0);
    hwstate.setValue(POTENTIOMETER, 2, 3.0);
    hwstate.setValue(POTENTIOMETER, 3, 4.0);
    hwstate.setValue(POTENTIOMETER, 4, 5.0);
    hwstate.setValue(SWITCH, 0, 0.0);
    hwstate.setValue(SWITCH, 1, 1.0);
    hwstate.setValue(SWITCH, 2, 1.0);
    hwstate.setValue(TOUCHSCREEN, 0, 132);
    hwstate.setValue(TOUCHSCREEN, 1, 245);

    // Parse the mapping file (partially)
    Json::Value mapConfig;
    parseJSONFile(mappingConfigFilepath, mapConfig);
    int num_profiles = mapConfig.get("numprofiles", 1).asInt();
    EXPECT_EQ(num_profiles, 4);

    // Create new profiles and have those parse each of the profiles
    vector<Profile> profiles;
    profiles.reserve(5);
    for (int i = 0; i < num_profiles; i++)
    {
        // Make profile and pass in config
        stringstream ss;
        ss << "profile" << i;
        string profileStr = ss.str();
        profiles.push_back(Profile(mapConfig[profileStr], hwstate));
    }

    // Update profiles
    for (int i = 0; i < num_profiles; i++)
    {
        profiles[i].update(hwstate);
    }

    // Check each profile contains the right stuff
    // Profile 0    
    Profile profile0 = profiles[0];
    vector<Effect*> effects0 = profile0.getEffectCopy();
    vector<EffectType> types0 = profile0.getEffectTypes();
    EXPECT_EQ(effects0.size(), 1);
    EXPECT_EQ(types0.size(), 1);
    EXPECT_EQ(types0[0], DISTORTION);
    Effect_Distortion distortion = *((Effect_Distortion*) effects0[0]);
    EXPECT_EQ(distortion.m_amount, 2);
    EXPECT_EQ(distortion.m_on, 0);

    // Profile 1
    Profile profile1 = profiles[1];
    vector<Effect*> effects1 = profile1.getEffectCopy();
    vector<EffectType> types1 = profile1.getEffectTypes();
    EXPECT_EQ(effects1.size(), 1);
    EXPECT_EQ(types1.size(), 1);
    EXPECT_EQ(types1[0], EQUALIZER);
    Effect_Equalizer equalizer = *((Effect_Equalizer*) effects1[0]);
    EXPECT_EQ(equalizer.m_lowmid, 600);
    EXPECT_EQ(equalizer.m_midhigh, 6000);
    EXPECT_EQ(equalizer.m_base_gain, -12);
    EXPECT_EQ(equalizer.m_mid_gain, -4);
    EXPECT_EQ(equalizer.m_treb_gain, 4);
    EXPECT_EQ(equalizer.m_on, 0);

    // Profile 2
    Profile profile2 = profiles[2];
    vector<Effect*> effects2 = profile2.getEffectCopy();
    vector<EffectType> types2 = profile2.getEffectTypes();
    EXPECT_EQ(effects2.size(), 2);
    EXPECT_EQ(types2.size(), 2);
    EXPECT_EQ(types2[0], LOWPASS);
    EXPECT_EQ(types2[1], DELAY);
    Effect_Lowpass lowpass = *((Effect_Lowpass*) effects2[0]);
    Effect_Delay delay = *((Effect_Delay*) effects2[1]);
    EXPECT_LT(abs(lowpass.m_cutoff - (60 + 0.5*(log((1.0/5.0) * (5.0 - (1/exp(2)))) + 2)*(11940))), 1e-3);
    EXPECT_EQ(lowpass.m_resonance, 6);
    EXPECT_EQ(lowpass.m_on, 0);
    EXPECT_EQ(delay.m_amount, 4);
    EXPECT_EQ(delay.m_speed, 6);
    EXPECT_EQ(delay.m_decay, 2.75);
    EXPECT_EQ(delay.m_attack, 8);
    EXPECT_EQ(delay.m_on, 1);

    // Profile 3
    Profile profile3 = profiles[3];
    vector<Effect*> effects3 = profile3.getEffectCopy();
    vector<EffectType> types3 = profile3.getEffectTypes();
    EXPECT_EQ(effects3.size(), 1);
    EXPECT_EQ(types3.size(), 1);
    EXPECT_EQ(types3[0], REVERB);
    Effect_Reverb reverb = *((Effect_Reverb*) effects3[0]);
    EXPECT_EQ(reverb.m_amount, 2);
    EXPECT_EQ(reverb.m_on, 0);
}

/*---------------------------------
------- effectupdatemessage -------
-----------------------------------*/
TEST(UpdateMessage, testSerialization)
{
    // Get the config files
    char hardwareConfigFilepath[1024];
    strcpy(hardwareConfigFilepath, string(configLocation + "/hardware.json").c_str());
    char mappingConfigFilepath[1024];
    strcpy(mappingConfigFilepath, string(configLocation + "/mapping.json").c_str());

    // Make hwstate object
    HWState hwstate(hardwareConfigFilepath);
    
    // Parse the mapping file (partially)
    Json::Value mapConfig;
    parseJSONFile(mappingConfigFilepath, mapConfig);
    int num_profiles = mapConfig.get("numprofiles", 1).asInt();
    EXPECT_EQ(num_profiles, 4);

    // Create new profiles and have those parse each of the profiles
    vector<Profile> profiles;
    profiles.reserve(5);
    for (int i = 0; i < num_profiles; i++)
    {
        // Make profile and pass in config
        stringstream ss;
        ss << "profile" << i;
        string profileStr = ss.str();
        profiles.push_back(Profile(mapConfig[profileStr], hwstate));
    }
    
    // Make update messages
    vector<EffectUpdateMessage> messages;
    for (int i = 0; i < num_profiles; i++)
    {
        EffectUpdateMessage msg(profiles[i]);
        messages.push_back(msg);
    }
    
    // Serialize it and see if we get it back
    for (int i = 0; i < num_profiles; i++)
    {
        char output[messages[i].getMessageSize()];
        messages[i].serialize(output);
        EffectUpdateMessage serialize_msg(output);
        EXPECT_EQ(messages[i].issame(serialize_msg), true);
    }
}

/*---------------------------------
------------ eventinfo ------------
-----------------------------------*/
TEST(EventInfo, testTimerEvent)
{
    EventInfo event;
    EXPECT_EQ(event.m_type, TIMER);    
}

TEST(EventInfo, testKeyEvent)
{
    EventInfo e0('a');
    EventInfo e1('b');
    EventInfo e2('c');
    EXPECT_EQ(e0.m_type, KEYPRESS);
    EXPECT_EQ(e1.m_type, KEYPRESS);
    EXPECT_EQ(e2.m_type, KEYPRESS);
    EXPECT_EQ(e0.m_keyvalue, 'a');
    EXPECT_EQ(e1.m_keyvalue, 'b');
    EXPECT_EQ(e2.m_keyvalue, 'c');
}

TEST(EventInfo, testMouseEvent)
{
    EventInfo e0(0, 0);
    EventInfo e1(250, 100);
    EventInfo e2(199, 372);
    EXPECT_EQ(e0.m_type, MOUSE);
    EXPECT_EQ(e1.m_type, MOUSE);
    EXPECT_EQ(e2.m_type, MOUSE);
    
    EXPECT_EQ(e0.m_mouse_x, 0);
    EXPECT_EQ(e0.m_mouse_y, 0);
    EXPECT_EQ(e1.m_mouse_x, 250);
    EXPECT_EQ(e1.m_mouse_y, 100);
    EXPECT_EQ(e2.m_mouse_x, 199);
    EXPECT_EQ(e2.m_mouse_y, 372);
}

/*---------------------------------
----------- effectsmodel ----------
-----------------------------------*/
TEST(EffectsModel, updateModel)
{
    // Get the config files
    char hardwareConfigFilepath[1024];
    strcpy(hardwareConfigFilepath, string(configLocation + "/hardware.json").c_str());
    char mappingConfigFilepath[1024];
    strcpy(mappingConfigFilepath, string(configLocation + "/mapping.json").c_str());

    // Construct the effects model
    EffectsModel model(hardwareConfigFilepath, mappingConfigFilepath);

    // TODO - think I need to test with IPC
}

TEST(EffectsModel, checkTouchSize)
{
    // Get the config files
    char hardwareConfigFilepath[1024];
    strcpy(hardwareConfigFilepath, string(configLocation + "/hardware.json").c_str());
    char mappingConfigFilepath[1024];
    strcpy(mappingConfigFilepath, string(configLocation + "/mapping.json").c_str());

    // Construct the effects model
    EffectsModel model(hardwareConfigFilepath, mappingConfigFilepath);

    // Get width and height
    int width = model.getTouchWidth();
    int height = model.getTouchHeight();

    // Check values
    EXPECT_EQ(width, 480);
    EXPECT_EQ(height, 560);
}
TEST(IPCSocket, sendReceive)
{
    //create
    cliSock = new ipcCliSock();
    serSock = new ipcSerSock();

    char[4] message = "ohai";

    bool ok = cliSock.sockSend(message, sizeof(message));
    char * response = serSock.sockRecv();

    EXPECT_EQ(ok, true);
    EXPECT_EQ("o", response[0]);
    EXPECT_EQ("h", response[1]);
    EXPECT_EQ("a", response[2]);
    EXPECT_EQ("i", response[3]);

    delete cliSock;
    delete serSock;
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);

    // Argument is filename
	if (argc < 2)
	{
		cout << "usage unit_test config_unitTest_Directory" << endl;
		return 1;
	}
	configLocation = string(argv[1]);

	return RUN_ALL_TESTS();
}

