/*
  ==============================================================================

    AutoGain.h
    Created: 25 Dec 2021 6:47:07pm
    Author:  thesp

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class AutoGain  : public juce::Component,
                  private juce::Button::Listener
{
public:
    AutoGain(SaturatorTestAudioProcessor&);
    ~AutoGain() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //void buttonStateChanged(juce::Button*) override;
    void buttonClicked(juce::Button*) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SaturatorTestAudioProcessor& audioProcessor;

    juce::TextButton autoGain;
    std::unique_ptr <juce::AudioProcessorValueTreeState::ButtonAttachment> autoGainAttatch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoGain)
};
