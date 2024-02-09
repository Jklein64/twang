#pragma once

#include <bit>
#include <complex>
#include <fftw3.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>

#if (MSVC)
    #include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // whether a note is playing, based on rms
    std::atomic<bool> playingNote;

    struct ui_event
    {
        // bar activation left to right
        std::atomic<std::bitset<9>> bars;
        // note index; C is 0, B is 11
        std::atomic<size_t> note;
    };

    ui_event uiEvent;

private:
    size_t fft_size;
    size_t hop_size;
    float sample_rate;

    struct fftw_data
    {
        fftwf_plan plan;
        std::vector<float> in;
        std::vector<std::complex<float>> out;
    } fftw;

    float rms;
    // next index to write to
    size_t head;
    // hann window
    std::vector<float> window;
    // magnitude spectrum
    std::vector<float> spectrum;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
