#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
        : AudioProcessor (BusesProperties()
                                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
          parameters (*this, nullptr, juce::Identifier("Parameters"),
                      {
                              std::make_unique<juce::AudioParameterFloat>("inputGain", "Input Gain", 0.0f, 1.0f, 0.5f),
                              std::make_unique<juce::AudioParameterFloat>("outputGain", "Output Gain", 0.0f, 1.0f, 1.0f),
                              std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.0002f, 0.02f, 0.008f),
                              std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.05f, 1.1f, 0.1f),
                              std::make_unique<juce::AudioParameterChoice>("ratio", "Ratio", juce::StringArray{ "4:1", "8:1", "12:1", "20:1" }, 0),
                              std::make_unique<juce::AudioParameterInt>("slam", "Slam", 0, 1, 0),
                              std::make_unique<juce::AudioParameterInt>("power", "Power", 0, 1, 1),
                              std::make_unique<juce::AudioParameterInt>("vuMeter", "VU Meter", 0, 1, 1),
                              std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", -60.0f, 0.0f, -20.0f)
                      })
{
}
#else
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}
#endif

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

void AudioPluginAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/) {
    // Initialize or reset variables here
}

void AudioPluginAudioProcessor::releaseResources() {
    // Release resources if necessary
}

float AudioPluginAudioProcessor::calculateGainReduction(float inputSample) {
    float inputLevel = std::fabs(inputSample);
    float dbInputLevel = juce::Decibels::gainToDecibels(inputLevel);

    float ratio = 4.0f;  // Default to 4:1
    if (isSlamModeOn()) {
        ratio = std::numeric_limits<float>::infinity(); // Infinity ratio for slam mode
    } else {
        switch (ratioIndex) {
            case 0: ratio = 4.0f; break;
            case 1: ratio = 8.0f; break;
            case 2: ratio = 12.0f; break;
            case 3: ratio = 20.0f; break;
        }
    }

    if (dbInputLevel > threshold) {
        float dbGainReduction = (dbInputLevel - threshold) * (1.0f - 1.0f / ratio);
        return juce::Decibels::decibelsToGain(-dbGainReduction);
    }

    return 1.0f;
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    inputGain = *parameters.getRawParameterValue("inputGain") * 3.0f;
    outputGain = *parameters.getRawParameterValue("outputGain") * 3.0f;
    attackTime = *parameters.getRawParameterValue("attack") * 2.0f;
    releaseTime = *parameters.getRawParameterValue("release") * 2.0f;
    ratioIndex = static_cast<int>(parameters.getRawParameterValue("ratio")->load());
    threshold = *parameters.getRawParameterValue("threshold");

    int powerState = static_cast<int>(parameters.getRawParameterValue("power")->load());

    if (powerState == 0) {
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            const auto* inputChannelData = buffer.getReadPointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                channelData[sample] = inputChannelData[sample];
            }
        }
        return;
    }

    int vuMeterState = static_cast<int>(parameters.getRawParameterValue("vuMeter")->load());
    bool slamMode = static_cast<int>(parameters.getRawParameterValue("slam")->load()) > 0;

    float attackCoefficient = static_cast<float>(std::exp(-1.0 / (getSampleRate() * attackTime)));
    float releaseCoefficient = static_cast<float>(std::exp(-1.0 / (getSampleRate() * releaseTime)));

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float inputSample = channelData[sample] * inputGain;
            float absSample = std::fabs(inputSample);

            float gainReduction = slamMode ? std::numeric_limits<float>::infinity() : calculateGainReduction(inputSample);

            if (gainReduction < envelope) {
                envelope = attackCoefficient * (envelope - gainReduction) + gainReduction;
            } else {
                envelope = releaseCoefficient * (envelope - gainReduction) + gainReduction;
            }

            float outputSample = inputSample * envelope * outputGain;
            channelData[sample] = outputSample;

            if (vuMeterState == 1) {
                vuLevel = juce::jmax(vuLevel, absSample);
            }
        }
    }

    vuLevel = vuLevel * 0.9f;
}

const juce::String AudioPluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool AudioPluginAudioProcessor::acceptsMidi() const { return false; }
bool AudioPluginAudioProcessor::producesMidi() const { return false; }
double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

bool AudioPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor() { return new AudioPluginAudioProcessorEditor (*this); }

void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // Store your parameters in the memory block
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // Restore your parameters from the memory block
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AudioPluginAudioProcessor();
}
