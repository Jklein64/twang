#include "PluginEditor.h"
#include "Colors.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

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
    int w = 208, h = 272;
    auto borderArea = area.withSizeKeepingCentre (w, h);
    auto x = borderArea.getX(), y = borderArea.getY();
    // start with top left corner bottom section and go clockwise
    // todo: the path below should be the outer limits
    border.startNewSubPath (x, y + 16);
    border.lineTo (x + 16, y);
    border.lineTo (x + w - 16, y);
    border.lineTo (x + w, y + 16);
    border.lineTo (x + w, y + h - 16);
    border.lineTo (x + w - 16, y + h);
    border.lineTo (x + 16, y + h);
    border.lineTo (x, y + h - 16);
    border.closeSubPath();
    border.scaleToFit (x + 4, y + 4, w, h, true);
    g.setColour (Colors::secondary);
    g.strokePath (border, juce::PathStrokeType (8.0f));
    border.scaleToFit (x, y, w, h, true);
    g.setColour (Colors::primary);
    g.strokePath (border, juce::PathStrokeType (8.0f));
}

void PluginEditor::resized()
{
}
