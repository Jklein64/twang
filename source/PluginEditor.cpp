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

    w = 136, h = 48;
    auto meterArea = area.withSizeKeepingCentre (w, h);
    // the 2 is to offset the stroke being in the center instead of edge
    meterArea.setY (borderArea.getY() + 2 + borderArea.getHeight() - h - 48);
    x = meterArea.getX(), y = meterArea.getY();
    juce::Path leftMeterBar;
    leftMeterBar.startNewSubPath (x, y + 12);
    leftMeterBar.lineTo (x + 4, y + 8);
    leftMeterBar.lineTo (x + 8, y + 8);
    leftMeterBar.lineTo (x + 8, y + 8 + 32);
    leftMeterBar.lineTo (x + 4, y + 8 + 32);
    leftMeterBar.lineTo (x, y + 8 + 32 - 4);
    leftMeterBar.closeSubPath();
    g.setColour (Colors::secondary);
    g.fillPath (leftMeterBar);

    juce::Path meterBar;
    for (int i = 0; i < 7; i++)
    {
        if (i == 3)
            continue;
        meterBar.addRectangle (x + 16 + i * (16), y + 8, 8, 32);
        g.setColour (Colors::secondary);
        g.fillPath (meterBar);
    }

    juce::Path rightMeterBar;
    rightMeterBar.startNewSubPath (x + w - 8, y + 8);
    rightMeterBar.lineTo (x + w - 4, y + 8);
    rightMeterBar.lineTo (x + w, y + 12);
    rightMeterBar.lineTo (x + w, y + 8 + 32 - 4);
    rightMeterBar.lineTo (x + w - 4, y + 8 + 32);
    rightMeterBar.lineTo (x + w - 8, y + 8 + 32);
    g.setColour (Colors::secondary);
    g.fillPath (rightMeterBar);

    juce::Path centerMeterBar;
    centerMeterBar.startNewSubPath (x + w / 2, y);
    centerMeterBar.lineTo (x + w / 2 + 4, y + 4);
    centerMeterBar.lineTo (x + w / 2 + 4, y + h - 4);
    centerMeterBar.lineTo (x + w / 2, y + h);
    centerMeterBar.lineTo (x + w / 2 - 4, y + h - 4);
    centerMeterBar.lineTo (x + w / 2 - 4, y + 4);
    centerMeterBar.closeSubPath();
    g.setColour (Colors::secondary);
    g.fillPath (centerMeterBar);
}

void PluginEditor::resized()
{
}
