#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    bool isSlamModeOn() const { return *parameters.getRawParameterValue("slam") == 1; }
    void setSlamMode(bool isOn) { parameters.getRawParameterValue("slam")->store(isOn ? 1.0f : 0.0f); }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Add getters for vuLevel and parameters
    float getVuLevel() const { return vuLevel; }
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

private:
    float inputGain = 1.0f;
    float outputGain = 1.0f;
    float attackTime = 0.01f;
    float releaseTime = 0.3f;
    int ratioIndex = 0;  // Updated to store the index for the ratio choice
    float threshold = 0.0f;

    float vuLevel = 0.0f;

    float envelope = 0.0f;

    float calculateGainReduction(float inputSample);

    juce::AudioProcessorValueTreeState parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
