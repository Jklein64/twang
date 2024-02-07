#include "PluginProcessor.h"
#include "Notes.h"
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
    sample_rate = (float) sampleRate;
    hop_size = 4 * samplesPerBlock;
    // achieves min bin resolution to tune a bass guitar (really big though...)
    // https://electronics.stackexchange.com/a/12408
    fft_size = (size_t) std::bit_ceil ((uint32_t) (sampleRate / (Notes::E1 - Notes::Eb1)));
    // initialize in/out block memory
    size_t in_size = fft_size;
    size_t out_size = (fft_size / 2 + 1);
    fftw.in.resize (in_size, 0);
    fftw.out.resize (out_size, 0);
    spectrum.resize (out_size, 0);
    // see https://stackoverflow.com/a/75561253 and https://www.fftw.org/fftw3_doc/Complex-numbers.html
    fftw.plan = fftwf_plan_dft_r2c_1d ((int) fft_size, fftw.in.data(), reinterpret_cast<fftwf_complex*> (fftw.out.data()), FFTW_MEASURE);
    // create and fill the hann window. weird trick to make it equivalent to scipy sym=False
    // basically a window of length n with sym=False is the same as one of length n+1 with sym=True,
    // which is the behavior of this function.
    window.resize (fft_size + 1, 0);
    juce::dsp::WindowingFunction<float>::fillWindowingTables (window.data(), fft_size + 1, juce::dsp::WindowingFunction<float>::hann, false);
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
        if (head == fftw.in.size())
        {
            rms = std::sqrtf (rms / fftw.in.size());
            // circular buffer filled with sound
            if (rms >= 1e-4)
            {
                // apply windowing function
                for (size_t j = 0; j < fftw.in.size(); j++)
                    fftw.in[j] *= window[j];

                // execute fft
                fftwf_execute_dft_r2c (fftw.plan, fftw.in.data(), reinterpret_cast<fftwf_complex*> (fftw.out.data()));

                // convert to power spectrum
                for (size_t j = 0; j < fftw.out.size(); j++)
                {
                    float re = fftw.out[j].real();
                    float im = fftw.out[j].imag();
                    spectrum[j] = std::sqrtf (re * re + im * im);
                }

                // run hps. see http://musicweb.ucsd.edu/~trsmyth/analysis/Harmonic_Product_Spectrum.html
                for (size_t order = 1; order < 5; order++)
                {
                    // order means "skipping _ samples after each one"
                    for (size_t j = 0, k = 0; k < spectrum.size(); j++, k += (order + 1))
                        // k indexes into the resampled version, j to the original
                        spectrum[k] *= spectrum[j];
                }

                // get max reasonable peak. can ignore octave errors because I only care about note classification
                ptrdiff_t peak = std::distance (spectrum.begin(), std::max_element (spectrum.begin(), spectrum.end()));
                float frequency = ((float) peak) / fft_size * sample_rate / 2;

                // only continue if frequency is reasonable
                if (frequency > Notes::C1)
                {
                    // find closest note, how far off, and which direction
                    Notes::note_event e = Notes::freq_to_note (frequency);
                    bool swap = e.frequency > frequency;
                    float f1 = e.frequency / (swap ? std::powf (2, 1 / 12.) : 1);
                    float f2 = e.frequency * (!swap ? std::powf (2, 1 / 12.) : 1);
                    float t = 12 * std::log2f (frequency / f1);
                    // f1 < frequency < f2
                    printf ("%f < %f < %f", f1, frequency, f2);
                    printf ("\t %s %s by %.2f%%\n", swap ? "under" : "over", e.name.c_str(), swap ? (1 - t) * 100 : t * 100);
                    // printf ("%s\n", e.name.c_str());

                    // update UI
                }
            }

            // reset
            head -= hop_size;
            // set input array range [h, n) -> [0, n-h), rest will be overwritten
            memcpy (&fftw.in[0], &fftw.in[hop_size], fft_size - hop_size);
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
