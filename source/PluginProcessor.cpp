#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
#if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
    #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
{
}

PluginProcessor::~PluginProcessor()
{
    fftwf_destroy_plan (fftw.plan);
    fftwf_free (fftw.in);
    fftwf_free (fftw.out);
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    // get the next power of two: https://stackoverflow.com/a/66975605
    size_t n = (size_t) std::bit_ceil ((uint32_t) samplesPerBlock);
    n = std::max ((size_t) 2048, n);
    // initialize in/out block memory
    fftw.in_size = n;
    fftw.out_size = (n / 2 + 1);
    fftw.in = (float*) fftwf_malloc (sizeof (float) * fftw.in_size);
    fftw.out = (fftwf_complex*) fftwf_malloc (sizeof (fftwf_complex) * fftw.out_size);
    fftw.plan = fftwf_plan_dft_r2c_1d ((int) n, fftw.in, fftw.out, FFTW_MEASURE);
    // create and fill the hann window. weird trick to make it equivalent to scipy sym=False
    // basically a window of length n with sym=False is the same as one of length n+1 with sym=True,
    // which is the behavior of this function.
    window.resize (n + 1, 0);
    juce::dsp::WindowingFunction<float>::fillWindowingTables (window.data(), n + 1, juce::dsp::WindowingFunction<float>::hann, false);
    window.pop_back();
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
    #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
#endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const float* leftChannelPointer = buffer.getReadPointer (0);
    const float* rightChannelPointer = buffer.getReadPointer (1);

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        // circular buffer full
        if (head == fftw.in_size)
        {
            rms = std::sqrtf (rms / fftw.in_size);
            // circular buffer filled with sound
            if (rms >= 1e-4)
            {
                // apply windowing function
                for (size_t j = 0; j < fftw.in_size; j++)
                    fftw.in[j] *= window[j];

                fftwf_execute_dft_r2c (fftw.plan, fftw.in, fftw.out);

                // run hps on fftw.out

                // threshold highest peak

                // compare against lookup table

                // update UI
            }

            // reset
            head = 0;
            rms = 0;
        }

        else
        {
            // merge stereo into mono: https://www.dsprelated.com/showthread/comp.dsp/106421-1.php
            float merged = (leftChannelPointer[i] + rightChannelPointer[i]) / 2;
            fftw.in[head++] = merged;
            rms += merged;
        }
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
