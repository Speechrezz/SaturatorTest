/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SaturatorTestAudioProcessorEditor::SaturatorTestAudioProcessorEditor (SaturatorTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), distGui(p), autoGainGui(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    Timer::startTimerHz(60.0f);
    setSize(SIZE * 7, SIZE * 5);

    // Drive knob

    driveAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, DRIVE_ID, driveKnob);

    driveKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    driveKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    driveKnob.setTextValueSuffix(" dB");
    driveKnob.setTextBoxIsEditable(true);
    driveKnob.setRange(MIN_GAIN, MAX_GAIN);
    driveKnob.setLookAndFeel(&knobLook);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.attachToComponent(&driveKnob, false);

    addAndMakeVisible(driveKnob);


    // Output gain knob

    gainAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, GAIN_ID, gainKnob);

    gainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    gainKnob.setTextValueSuffix(" dB");
    gainKnob.setTextBoxIsEditable(true);
    gainKnob.setRange(MIN_GAIN, MAX_GAIN);
    gainKnob.setLookAndFeel(&knobLook);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainKnob, false);

    addAndMakeVisible(gainKnob);


    // Mix knob

    mixAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, MIX_ID, mixKnob);

    mixKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    mixKnob.setTextValueSuffix(" %");
    mixKnob.setTextBoxIsEditable(true);
    mixKnob.setRange(0, 100);
    mixKnob.setLookAndFeel(&knobLook);

    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixKnob, false);

    addAndMakeVisible(mixKnob);


    // Freq knob

    freqAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, FREQ_ID, freqKnob);

    freqKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    freqKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    freqKnob.setTextValueSuffix(" Hz");
    freqKnob.setTextBoxIsEditable(true);
    freqKnob.setRange(MIN_FREQ, MAX_FREQ);
    freqKnob.setSkewFactorFromMidPoint(710.0);
    freqKnob.setLookAndFeel(&knobLook);

    freqLabel.setText("Freq", juce::dontSendNotification);
    freqLabel.setJustificationType(juce::Justification::centred);
    freqLabel.attachToComponent(&freqKnob, false);

    addAndMakeVisible(freqKnob);


    // Width knob

    widthAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, WIDTH_ID, widthKnob);

    widthKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    widthKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    widthKnob.setTextValueSuffix(" %");
    widthKnob.setTextBoxIsEditable(true);
    widthKnob.setRange(MIN_WIDTH, MAX_WIDTH);
    widthKnob.setLookAndFeel(&knobLook);

    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    widthLabel.attachToComponent(&widthKnob, false);

    addAndMakeVisible(widthKnob);


    // Depth knob

    depthAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, DEPTH_ID, depthKnob);

    depthKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    depthKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    depthKnob.setTextValueSuffix(" dB");
    depthKnob.setTextBoxIsEditable(true);
    depthKnob.setRange(MIN_DEPTH, MAX_DEPTH);
    depthKnob.setLookAndFeel(&knobLook);

    depthLabel.setText("Depth", juce::dontSendNotification);
    depthLabel.setJustificationType(juce::Justification::centred);
    depthLabel.attachToComponent(&depthKnob, false);

    addAndMakeVisible(depthKnob);


    // Base knob

    baseAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, BASE_ID, baseKnob);

    baseKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    baseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    baseKnob.setTextValueSuffix(" dB");
    baseKnob.setTextBoxIsEditable(true);
    baseKnob.setRange(MIN_GAIN, MAX_GAIN);
    baseKnob.setLookAndFeel(&knobLook);

    baseLabel.setText("Base", juce::dontSendNotification);
    baseLabel.setJustificationType(juce::Justification::centred);
    baseLabel.attachToComponent(&baseKnob, false);

    addAndMakeVisible(baseKnob);


    // Soft knob

    softAttatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, SOFT_ID, softKnob);

    softKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    softKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    softKnob.setTextValueSuffix(" dB");
    softKnob.setTextBoxIsEditable(true);
    softKnob.setRange(MIN_SOFT, MAX_SOFT);
    softKnob.setLookAndFeel(&knobLook);

    softLabel.setText("Soft", juce::dontSendNotification);
    softLabel.setJustificationType(juce::Justification::centred);
    softLabel.attachToComponent(&softKnob, false);

    addAndMakeVisible(softKnob);


    // Waveshaper GUI
    addAndMakeVisible(distGui);


    // Waveshaper GUI
    addAndMakeVisible(autoGainGui);

}

SaturatorTestAudioProcessorEditor::~SaturatorTestAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void SaturatorTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.drawVerticalLine(120, 20, audioProcessor.getWetArray()[0] * 100);

    juce::Rectangle<float> bottomRect(SIZE, getHeight() - SIZE - 14.f, SIZE * 3, SIZE + 8.f);
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(bottomRect, 10.f, 1.f);

}

void SaturatorTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const int size = SIZE;
    const int margin = SIZE / 8;

    driveKnob.setBounds(juce::Rectangle < int>(0, size, size, size).reduced(margin));
    gainKnob.setBounds(juce::Rectangle < int>(getWidth() - size, size, size, size).reduced(margin));
    mixKnob.setBounds(juce::Rectangle < int>(getWidth() - size, getHeight() - size, size, size).reduced(margin));

    juce::Rectangle<int> knobRect(0, getHeight() - size, size, size);

    baseKnob.setBounds(knobRect.reduced(margin));
    knobRect.translate(SIZE, 0);
    freqKnob.setBounds(knobRect.reduced(margin));
    knobRect.translate(SIZE, 0);
    widthKnob.setBounds(knobRect.reduced(margin));
    knobRect.translate(SIZE, 0);
    depthKnob.setBounds(knobRect.reduced(margin));
    knobRect.translate(SIZE, 0);
    softKnob.setBounds(knobRect.reduced(margin));

    autoGainGui.setBounds(size*0.25, size * 2.25, size / 2, size / 2);
    
    //distType.setBounds(size*1.5, size * 3, size*1.5, size / 4);

    //juce::Rectangle<int> area = getLocalBounds();

    //distGui.setBounds(area.removeFromLeft(size*3).removeFromTop(size*3));

    distGui.setBounds(size, size, size * 5, size * 3);

}

void SaturatorTestAudioProcessorEditor::timerCallback() {
    repaint();
}