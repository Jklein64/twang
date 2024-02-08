#include "PluginEditor.h"
#include "Colors.h"
#include "Notes.h"

PluginEditor::PluginEditor (PluginProcessor& p) : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    setSize (400, 300);
    setResizeLimits (240, 304, 500, 400);

    setOpaque (true);
    setResizable (true, true);
    setLookAndFeel (&twangLookAndFeel);
    // repaint at 20 Hz
    startTimerHz (20);
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel (nullptr);
}

void PluginEditor::timerCallback()
{
    repaint();
    for (size_t i = 0; i < 9; i++)
    {
        meterBarPercentRemaining[i] -= 0.6;
        if (meterBarPercentRemaining[i] < 0)
            meterBarPercentRemaining[i] = 0;
    }
}

void PluginEditor::paint (juce::Graphics& g)
{
    std::vector<juce::Colour> meterBarFillColors;
    auto isBarEnabled = processorRef.uiEvent.bars.load();
    for (size_t i = 0; i < 9; i++)
        if (processorRef.playingNote.load() && isBarEnabled[i])
            meterBarPercentRemaining[i] = 1;
    for (size_t i = 0; i < 9; i++)
        meterBarFillColors.push_back (Colors::secondary.interpolatedWith (Colors::primary, meterBarPercentRemaining[i]));

    // opaque -> must fill background
    g.fillAll (Colors::background);

    auto area = getLocalBounds();

    juce::Path border;
    int w = 208, h = 272;
    auto borderArea = area.withSizeKeepingCentre (w, h);
    auto x = borderArea.getX(), y = borderArea.getY();
    // start with top left corner bottom section and go clockwise
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
    meterArea.setY (borderArea.getY() + borderArea.getHeight() - h - 56);
    x = meterArea.getX(), y = meterArea.getY();
    juce::Path leftMeterBar;
    leftMeterBar.startNewSubPath (x, y + 12);
    leftMeterBar.lineTo (x + 4, y + 8);
    leftMeterBar.lineTo (x + 8, y + 8);
    leftMeterBar.lineTo (x + 8, y + 8 + 32);
    leftMeterBar.lineTo (x + 4, y + 8 + 32);
    leftMeterBar.lineTo (x, y + 8 + 32 - 4);
    leftMeterBar.closeSubPath();
    g.setColour (meterBarFillColors[0]);
    g.fillPath (leftMeterBar);

    juce::Path meterBar;
    for (int i = 0; i < 7; i++)
    {
        if (i == 3)
            continue;
        meterBar.addRectangle (x + 16 + i * (16), y + 8, 8, 32);
        g.setColour (meterBarFillColors[i + 1]);
        g.fillPath (meterBar);
    }

    juce::Path rightMeterBar;
    rightMeterBar.startNewSubPath (x + w - 8, y + 8);
    rightMeterBar.lineTo (x + w - 4, y + 8);
    rightMeterBar.lineTo (x + w, y + 12);
    rightMeterBar.lineTo (x + w, y + 8 + 32 - 4);
    rightMeterBar.lineTo (x + w - 4, y + 8 + 32);
    rightMeterBar.lineTo (x + w - 8, y + 8 + 32);
    g.setColour (meterBarFillColors[8]);
    g.fillPath (rightMeterBar);

    juce::Path centerMeterBar;
    centerMeterBar.startNewSubPath (x + w / 2, y);
    centerMeterBar.lineTo (x + w / 2 + 4, y + 4);
    centerMeterBar.lineTo (x + w / 2 + 4, y + h - 4);
    centerMeterBar.lineTo (x + w / 2, y + h);
    centerMeterBar.lineTo (x + w / 2 - 4, y + h - 4);
    centerMeterBar.lineTo (x + w / 2 - 4, y + 4);
    centerMeterBar.closeSubPath();
    g.setColour (meterBarFillColors[4]);
    g.fillPath (centerMeterBar);

    // auto noteText = juce::String ("D");
    if (processorRef.playingNote.load())
        noteText = juce::String (Notes::index_to_name (processorRef.uiEvent.note.load())->c_str());
    auto textArea = borderArea
                        .withTop (borderArea.getY() + 48)
                        // centers the font a bit better
                        .withTrimmedLeft (12)
                        .withHeight (64);
    auto font = twangLookAndFeel.alarmClockFont;
    g.setFont (juce::Font (font));
    g.setFont (96.f); // set size
    g.setColour (Colors::primary);
    g.drawText (noteText, textArea, juce::Justification::centred, false);

    w = 64, h = 6;
    juce::Path underline;
    auto underlineArea = textArea
                             .withWidth (w)
                             .withCentre (area.getCentre())
                             .withTop (textArea.getBottom() + 10)
                             .withHeight (h);
    x = underlineArea.getX(), y = underlineArea.getY();
    underline.startNewSubPath (x + 2, y);
    underline.lineTo (x + w - 2, y);
    underline.lineTo (x + w, y + 2);
    underline.lineTo (x + w - 4, y + h);
    underline.lineTo (x + 4, y + h);
    underline.lineTo (x, y + 2);
    underline.closeSubPath();
    underline.scaleToFit (x + 2, y + 2, w, h, true);
    g.setColour (Colors::secondary);
    g.fillPath (underline);
    underline.scaleToFit (x, y, w, h, true);
    g.setColour (Colors::primary);
    g.fillPath (underline);
}

void PluginEditor::resized()
{
}
