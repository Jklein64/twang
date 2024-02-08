#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "TwangLookAndFeel.h"
#include <string>

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    TwangLookAndFeel twangLookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
