#include "PluginEditor.h"
#include "Colors.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    setOpaque (true);
    setResizable (true, true);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // opaque -> must fill background
    g.fillAll (Colors::background);

    auto area = getLocalBounds();

    juce::Path border;
    int w = 104, h = 136;
    auto borderArea = area.withSizeKeepingCentre (w, h);
    auto x = borderArea.getX(), y = borderArea.getY();
    // start with top left corner bottom section and go clockwise
    // todo: the path below should be the outer limits
    border.startNewSubPath (x, y + 8);
    border.lineTo (x + 8, y);
    border.lineTo (x + w - 8, y);
    border.lineTo (x + w, y + 8);
    border.lineTo (x + w, y + h - 8);
    border.lineTo (x + w - 8, y + h);
    border.lineTo (x + 8, y + h);
    border.lineTo (x, y + h - 8);
    border.closeSubPath();
    border.scaleToFit (x + 2, y + 2, w, h, true);
    g.setColour (Colors::secondary);
    g.strokePath (border, juce::PathStrokeType (4.0f));
    border.scaleToFit (x, y, w, h, true);
    g.setColour (Colors::primary);
    g.strokePath (border, juce::PathStrokeType (4.0f));

    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    g.drawText (helloWorld, area.removeFromTop (150), juce::Justification::centred, false);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    area.removeFromBottom (50);
    inspectButton.setBounds (getLocalBounds().withSizeKeepingCentre (100, 50));
}
