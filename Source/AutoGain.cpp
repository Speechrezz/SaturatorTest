/*
  ==============================================================================

    AutoGain.cpp
    Created: 25 Dec 2021 6:47:07pm
    Author:  thesp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AutoGain.h"

//==============================================================================
AutoGain::AutoGain(SaturatorTestAudioProcessor& p) : audioProcessor(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    setSize(50, 50);

    // Autogain

    autoGain.setButtonText(AUTOGAIN_NAME);
    //bool toggleValue = audioProcessor.treeState.getRawParameterValue(AUTOGAIN_ID)->load();;
    //autoGain.setToggleState(toggleValue, juce::NotificationType::dontSendNotification);
    autoGain.setToggleable(true);
    autoGain.setClickingTogglesState(true);

    addAndMakeVisible(autoGain);

    autoGain.addListener(this);

    autoGainAttatch = std::make_unique <juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, AUTOGAIN_ID, autoGain);

}

AutoGain::~AutoGain()
{
}

void AutoGain::paint (juce::Graphics& g)
{
    
}

void AutoGain::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    juce::Rectangle<int> area = getLocalBounds();

    autoGain.setBounds(area);

}

void AutoGain::buttonClicked(juce::Button* button) {
    /*
    if (button == &autoGain) {
        if (button->getToggleState() == false) {
            button->setToggleState(true, juce::NotificationType::dontSendNotification);
        }
        else {
            button->setToggleState(false, juce::NotificationType::dontSendNotification);
        }
    }
    */
}