#ifndef EFFECT_H
#define EFFECT_H

#include <string.h>
#include <iostream>

using namespace std;

// Enum that lists all of the types of effects
enum EffectType
{
    DELAY,
    DISTORTION,
    EQUALIZER,
    LOWPASS,
    REVERB,
    UNKNOWNEFFECT
};

// Virtual class for effects. Holds all of the parameters of the effect
class Effect
{
public:
    virtual void serialize(char* output)=0;
    virtual bool issame(const Effect* other)=0;
};

class Effect_Delay: public Effect
{
public:
    // Functions
    Effect_Delay();
    Effect_Delay(float delay_time, float speed, float decay, float attack, bool on);
    Effect_Delay(const char* msg);
    void serialize(char* output);
    bool issame(const Effect* other);

    // Member variables
    float m_delay_time;
    float m_speed;
    float m_decay;
    float m_attack;
    bool m_on;
};

class Effect_Distortion: public Effect
{
public:
    Effect_Distortion();
    Effect_Distortion(float amount, bool on);
    Effect_Distortion(const char* msg);
    void serialize(char* output);
    bool issame(const Effect* other);

    // Member variables
    float m_amount;
    bool m_on;
};

class Effect_Equalizer: public Effect
{
public:
    Effect_Equalizer();
    Effect_Equalizer(float lowmid, float midhigh, float base_gain, float mid_gain, float treb_gain, bool on);
    Effect_Equalizer(const char* msg);
    void serialize(char* output);
    bool issame(const Effect* other);    
    
    // Member variables
    float m_lowmid;
    float m_midhigh;
    float m_base_gain;
    float m_mid_gain;
    float m_treb_gain;
    bool m_on;
};

class Effect_Lowpass: public Effect
{
public:
    Effect_Lowpass();
    Effect_Lowpass(float cutoff, float resonance, bool on);
    Effect_Lowpass(const char* msg);
    void serialize(char* output);
    bool issame(const Effect* other);    

    // Member variables
    float m_cutoff;
    float m_resonance;
    bool m_on;
};

class Effect_Reverb: public Effect
{
 public:
    Effect_Reverb();
    Effect_Reverb(float amount, bool on);
    Effect_Reverb(const char* msg);
    void serialize(char* output);
    bool issame(const Effect* other); 

    // Member variables
    float m_amount;
    bool m_on;
};

class Effect_Unknown: public Effect
{
public:
    Effect_Unknown();
    Effect_Unknown(const char* msg);
    void serialize(char* output);
    bool issame(const Effect* other); 
};

#endif // EFFECT_H
