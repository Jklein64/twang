#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "TwangLookAndFeel.h"
#include <string>

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    struct ui_event
    {
        // bar activation left to right
        std::bitset<9> bars;
        // note without octave number
        std::string note;
    };

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    TwangLookAndFeel twangLookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
