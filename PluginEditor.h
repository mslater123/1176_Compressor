#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "VuMeter.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // Binary resources:
    static const char* background_png;
    static const int background_pngSize;

private:
    AudioPluginAudioProcessor& audioProcessor;

    CustomLookAndFeel customLookAndFeel;

    juce::Slider inputSlider;
    juce::Slider outputSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::TextButton slamButton;
    juce::Slider ratioSlider;     // Ratio control as rotary knob
    juce::Slider powerSlider;     // Power control as rotary knob
    juce::Slider bypassSlider;   // VU meter control as rotary knob

    VuMeter vuMeter;

    juce::Image cachedImage_background_png; // Background image

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
