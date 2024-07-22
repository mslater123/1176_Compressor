#pragma once

#include <JuceHeader.h>

class VuMeter : public juce::Component,
                private juce::Timer
{
public:
    VuMeter()
    {
        startTimerHz(30); // Update 30 times per second
    }

    void setLevel(float newLevel)
    {
        level = newLevel;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white); // Background color

        // Draw the meter outline
        g.setColour(juce::Colours::black);
        g.drawRect(getLocalBounds(), 1);

        // Draw the needle
        g.setColour(juce::Colours::red);
        juce::Path needle;
        float needleLength = getHeight() * 0.8f; // 80% of the component height
        needle.addRectangle(-1.0f, -needleLength, 2.0f, needleLength);

        // Calculate the angle for the needle
        float angle = juce::jmap(level, 0.0f, 1.0f, -juce::MathConstants<float>::pi / 4.0f, juce::MathConstants<float>::pi / 4.0f);

        needle.applyTransform(juce::AffineTransform::rotation(angle)
                                      .translated(getWidth() / 2.0f, getHeight() * 0.9f)); // Pivot point

        g.fillPath(needle);
    }

private:
    void timerCallback() override
    {
        // Optionally, implement any additional functionality for the timer
    }

    float level = 0.0f;
};
