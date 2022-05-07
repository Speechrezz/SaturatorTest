/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "KnobLookAndFeel.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Waveshaper.h"
#include "AutoGain.h"

//==============================================================================
/**
*/
class SaturatorTestAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SaturatorTestAudioProcessorEditor (SaturatorTestAudioProcessor&);
    ~SaturatorTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SaturatorTestAudioProcessor& audioProcessor;

    Waveshaper distGui;
    AutoGain autoGainGui;

    juce::Slider driveKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> driveAttatch;
    juce::Label driveLabel;
    juce::Slider gainKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> gainAttatch;
    juce::Label gainLabel;
    juce::Slider mixKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> mixAttatch;
    juce::Label mixLabel;

    juce::Slider freqKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> freqAttatch;
    juce::Label freqLabel;
    juce::Slider widthKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> widthAttatch;
    juce::Label widthLabel;
    juce::Slider depthKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> depthAttatch;
    juce::Label depthLabel;
    juce::Slider baseKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> baseAttatch;
    juce::Label baseLabel;
    juce::Slider softKnob;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> softAttatch;
    juce::Label softLabel;

    //juce::ComboBox distType;
    //std::unique_ptr <juce::AudioProcessorValueTreeState::ComboBoxAttachment> distTypeAttatch;

    KnobLookAndFeel knobLook;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaturatorTestAudioProcessorEditor)
};
