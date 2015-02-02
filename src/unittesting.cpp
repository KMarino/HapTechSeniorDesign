#include <string>
#include <iostream>
#include "gtest/gtest.h"
#include "optparser.h"
#include "effect.h"
#include "effectupdatemessage.h"
#include "hwstate.h"

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
    EXPECT_EQ(delay.m_delay_time, 0);
    EXPECT_EQ(delay.m_speed, 0);
    EXPECT_EQ(delay.m_decay, 0);
    EXPECT_EQ(delay.m_attack, 0);
    EXPECT_EQ(delay.m_on, false);

    // Check parameter constructor
    srand(0);
    float delay_time = (float)rand() / RAND_MAX;
    float speed = (float)rand() / RAND_MAX;
    float decay = (float)rand() / RAND_MAX;
    float attack = (float)rand() / RAND_MAX;
    bool on = true;
    Effect_Delay delay2(delay_time, speed, decay, attack, on);
    EXPECT_EQ(delay2.m_delay_time, delay_time);
    EXPECT_EQ(delay2.m_speed, speed);
    EXPECT_EQ(delay2.m_decay, decay);
    EXPECT_EQ(delay2.m_attack, attack);
    EXPECT_EQ(delay2.m_on, on);

    // Check serialization
    char delay_serial[256];
    delay2.serialize(delay_serial);
    Effect_Delay delay3(delay_serial);
    EXPECT_EQ(delay3.m_delay_time, delay_time);
    EXPECT_EQ(delay3.m_speed, speed);
    EXPECT_EQ(delay3.m_decay, decay);
    EXPECT_EQ(delay3.m_attack, attack);
    EXPECT_EQ(delay3.m_on, on);
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

/*---------------------------------
------- effectupdatemessage -------
-----------------------------------*/
TEST(UpdateMessage, testSerialization)
{
    // Create a new message
    srand(10);
    float cutoff = (float)rand() / RAND_MAX;
    float resonance = (float)rand() / RAND_MAX;
    float amount = (float)rand() / RAND_MAX;
    bool on = true;
    Effect* low = new Effect_Lowpass(cutoff, resonance, on);
    Effect* distort = new Effect_Distortion(amount, on);
    vector<Effect*> effects;
    effects.push_back(low);
    effects.push_back(distort);
    vector<EffectType> types;
    types.push_back(LOWPASS);
    types.push_back(DISTORTION);
    EffectUpdateMessage message(effects, types);

    // Serialize it and see if we get it back
    char output[1024];
    message.serialize(output);
    EffectUpdateMessage serialize_msg(output);
    EXPECT_EQ(message.issame(serialize_msg), true);
}

/*---------------------------------
------------- hwstate -------------
-----------------------------------*/
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
    EXPECT_EQ(hwstate.getTouchWidth(), 560);
    EXPECT_EQ(hwstate.getTouchHeight(), 480);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 0), 0);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 1), 1);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 2), 2);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 3), 3);
    EXPECT_EQ(hwstate.getPinNumber(POTENTIOMETER, 4), 4);
    EXPECT_EQ(hwstate.getPinNumber(SWITCH, 0), 5);
    EXPECT_EQ(hwstate.getPinNumber(SWITCH, 1), 6);
    EXPECT_EQ(hwstate.getPinNumber(SWITCH, 2), 7);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 0), 8);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 1), 9);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 2), 10);
    EXPECT_EQ(hwstate.getPinNumber(PUSHBUTTON, 3), 11);    
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

