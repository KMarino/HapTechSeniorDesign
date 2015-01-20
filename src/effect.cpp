#include "effect.h"

// Delay
Effect_Delay::Effect_Delay()
{
}

Effect_Delay::Effect_Delay(const Effect_Delay& other)
{
    m_delay_time = other.m_delay_time;
}

Effect_Delay::Effect_Delay(double delay_time)
{
    m_delay_time = delay_time;
}

Effect_Delay::Effect_Delay(const char* msg)
{
    // Copy char to new object
    Effect_Delay* msgObj = new Effect_Delay();
    memcpy(msgObj, msg, sizeof(Effect_Delay));

    // Copy member variables
    m_delay_time = msgObj->m_delay_time;
}

void Effect_Delay::serialize(char* output)
{
    // Copy object to output char
    memcpy(output, this, sizeof(Effect_Delay));
}

// Distortion
Effect_Distortion::Effect_Distortion()
{
}

Effect_Distortion::Effect_Distortion(const char* msg)
{
}

void Effect_Distortion::serialize(char* output)
{
}

// Reverb
Effect_Reverb::Effect_Reverb()
{
}

Effect_Reverb::Effect_Reverb(const char* msg)
{
}

void Effect_Reverb::serialize(char* output)
{
}

// Highpass
Effect_Highpass::Effect_Highpass()
{
}

Effect_Highpass::Effect_Highpass(const char* msg)
{
}

void Effect_Highpass::serialize(char* output)
{
}

// Lowpass
Effect_Lowpass::Effect_Lowpass()
{
}

Effect_Lowpass::Effect_Lowpass(const char* msg)
{
}

void Effect_Lowpass::serialize(char* output)
{
}

// Bandpass
Effect_Bandpass::Effect_Bandpass()
{
}

Effect_Bandpass::Effect_Bandpass(const char* msg)
{
}

void Effect_Bandpass::serialize(char* output)
{
}

// Unknown
Effect_Unknown::Effect_Unknown()
{
}

Effect_Unknown::Effect_Unknown(const char* output)
{
}

void Effect_Unknown::serialize(char* output)
{
}

