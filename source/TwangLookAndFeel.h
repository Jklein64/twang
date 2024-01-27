#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

class TwangLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Typeface::Ptr alarmClockFont;

    TwangLookAndFeel()
    {
        juce::Typeface::Ptr alarmClock = juce::Typeface::createSystemTypefaceFor (
            BinaryData::alarm_clock_ttf,
            BinaryData::alarm_clock_ttfSize);
        setDefaultSansSerifTypeface (alarmClock);
        alarmClockFont = alarmClock;
    }
};