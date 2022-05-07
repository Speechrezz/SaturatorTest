/*
  ==============================================================================

    Waveshaper.h
    Created: 25 Dec 2021 2:16:58pm
    Author:  thesp

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Equations.h"
#include "PluginProcessor.h"

#define SENSITIVITY 15

//==============================================================================
/*
*/
class Waveshaper  : public juce::Component,
                    private juce::ComboBox::Listener
{
public:
    Waveshaper(SaturatorTestAudioProcessor&);
    ~Waveshaper() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void comboBoxChanged(juce::ComboBox*) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SaturatorTestAudioProcessor& audioProcessor;

    juce::ComboBox distMenu;
    std::unique_ptr <juce::AudioProcessorValueTreeState::ComboBoxAttachment> distTypeAttatch;
    juce::Slider param1Knob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> param1Attatch;
    juce::Label param1Label;

    juce::Image waveshaperImage;

    float prevMax;
    float prevMin;
    int timerCounter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Waveshaper)
};
