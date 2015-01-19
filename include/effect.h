#ifndef EFFECT_H
#define EFFECT_H

#include <string.h>

// Enum that lists all of the types of effects
enum EffectType
{
    DELAY,
    DISTORTION,
    REVERB,
    HIGHPASS,
    LOWPASS,
    BANDPASS,
    UNKNOWN
};

// Virtual class for effects. Holds all of the parameters of the effect
class Effect
{
public:
    virtual void serialize(char* output)=0;
};

class Effect_Delay: public Effect
{
public:
    // Functions
    Effect_Delay();
    Effect_Delay(double delay_time);
    Effect_Delay(const char* msg);
    void serialize(char* output);  

    // Member variables
    double m_delay_time;
};

class Effect_Distortion: public Effect
{
public:
    Effect_Distortion();
    Effect_Distortion(const char* msg);
    void serialize(char* output);
};

class Effect_Reverb: public Effect
{
 public:
    Effect_Reverb();
    Effect_Reverb(const char* msg);
    void serialize(char* output);
};

class Effect_Highpass: public Effect
{
public:
    Effect_Highpass();
    Effect_Highpass(const char* msg);
    void serialize(char* output);
};    

class Effect_Lowpass: public Effect
{
public:
    Effect_Lowpass();
    Effect_Lowpass(const char* msg);
    void serialize(char* output);
};

class Effect_Bandpass: public Effect
{
public:
    Effect_Bandpass();
    Effect_Bandpass(const char* msg);
    void serialize(char* output);
};

class Effect_Unknown: public Effect
{
public:
    Effect_Unknown();
    Effect_Unknown(const char* msg);
    void serialize(char* output);
};

#endif // EFFECT_H
