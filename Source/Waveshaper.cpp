/*
  ==============================================================================

    Waveshaper.cpp
    Created: 25 Dec 2021 2:16:58pm
    Author:  thesp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Waveshaper.h"

//==============================================================================
Waveshaper::Waveshaper(SaturatorTestAudioProcessor& p) : audioProcessor(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(SIZE * 5, SIZE * 3);
    prevMax = 0;
    prevMin = 0;
    timerCounter = 0;


    // Distortion type drop-down menu

    distMenu.addItem("Analog Clip", 1);
    distMenu.addItem("Soft Sine", 2);
    distMenu.addItem("Medium Curve", 3);
    //distMenu.addItem("Hard Curve", 4);
    distMenu.addItem("Sinoid Fold", 4);
    distMenu.addItem("Digital Clip", 5);
    //distMenu.addItem("Custom", 7);

    distMenu.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(distMenu);
    distMenu.addListener(this);

    distTypeAttatch = std::make_unique <juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.treeState, DISTTYPE_ID, distMenu);


    // Param1 knob

    param1Attatch = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, PARAM1_ID, param1Knob);

    param1Knob.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    //param1Knob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 25);
    //param1Knob.setTextValueSuffix(" dB");
    param1Knob.setTextBoxIsEditable(true);
    param1Knob.setRange(0.f, 1.f);
    param1Knob.setSliderSnapsToMousePosition(false);

    param1Label.setText("Morph", juce::dontSendNotification);
    param1Label.setJustificationType(juce::Justification::centred);
    param1Label.attachToComponent(&param1Knob, false);

    addAndMakeVisible(param1Knob);

}

// Destructor
Waveshaper::~Waveshaper()
{
}

void Waveshaper::paint (juce::Graphics& g)
{
    // Draw the waveshaper graph

    juce::Path p; // path for the waveshaper
    const int numPoints = 200;
    const float delta = static_cast<float>(getWidth()) / static_cast<float>(numPoints);

    //p.startNewSubPath(0, static_cast<float>(getHeight()));
    float point = 0;
    float index;
    const float stretch = 100.0f;

    // Plot the points
    for (int i = 0; i < numPoints; i++) {
        index = (delta * (i - numPoints / 2)) / stretch;

        point = decideEquation(index, distMenu.getSelectedItemIndex(), audioProcessor.treeState.getRawParameterValue(PARAM1_ID)->load());

        if (i == 0) {
            p.startNewSubPath(0, getHeight() / 2 * (1 - 1));
            p.startNewSubPath(0, getHeight() / 2 * (1 + 1));
            p.startNewSubPath(0, getHeight() / 2 * (1 - point));
        }
        else
            p.lineTo(delta * i, getHeight() / 2 * (1 - point));
    }

    // ==================================================================================
    // Draw the graph
    juce::Rectangle<float> shaperArea(SIZE / 2.0, SIZE / 4.0, SIZE * 3.0, SIZE * 2.0);

    p.scaleToFit(shaperArea.getX(), shaperArea.getY(), shaperArea.getWidth(), shaperArea.getHeight(), false);

    g.setColour(juce::Colour(30, 30, 30));
    g.fillRect(shaperArea.expanded(SIZE / 10, SIZE / 10));
    g.setColour(juce::Colours::dimgrey);
    g.drawHorizontalLine(static_cast<int>(shaperArea.getCentreY()), shaperArea.getX(), shaperArea.getRight());
    g.drawVerticalLine(static_cast<int>(shaperArea.getCentreX()), shaperArea.getY(), shaperArea.getBottom());

    g.setColour(juce::Colours::black);
    g.drawRect(shaperArea.expanded(SIZE / 10, SIZE / 10));

    g.setColour(juce::Colours::lightskyblue);
    g.strokePath(p, juce::PathStrokeType(2.0));

    // ==================================================================================
    // Draw the real-time maximum point

    const float slope = 2.0 / ((delta * numPoints) / stretch);
    std::array<float, BUFFER_SIZE> audioData = audioProcessor.getDryArray();
    float pointX = *std::max_element(audioData.begin(), audioData.end());
    pointX = fminf(pointX, ((delta * numPoints) / stretch) / 2);

    if (pointX < prevMax)
        timerCounter++;
    else
        timerCounter = 0;

    pointX = fmaxf(pointX, prevMax);
    prevMax = pointX;

    if (timerCounter >= SENSITIVITY) {
        prevMax *= 0.9;
    }
    float pointY = decideEquation(pointX, distMenu.getSelectedItemIndex(), audioProcessor.treeState.getRawParameterValue(PARAM1_ID)->load());

    pointX *= slope;
    pointX = pointX * shaperArea.getWidth()/2 + shaperArea.getCentreX();

    pointY *= -1;
    pointY = pointY * shaperArea.getHeight()/2 + shaperArea.getCentreY();

    juce::Rectangle<float> pointArea(pointX, pointY, 1, 1);
    pointArea.expand(1, 1);

    g.setColour(juce::Colours::orange);
    g.drawEllipse(pointArea, 4);

    // ==================================================================================
    // Draw the real-time minimum point

    pointX = *std::min_element(audioData.begin(), audioData.end());
    pointX = fmaxf(pointX, -((delta * numPoints) / stretch) / 2);

    pointX = fminf(pointX, prevMin);
    prevMin = pointX;

    if (timerCounter >= SENSITIVITY) {
        prevMin *= 0.9;
    }
    pointY = decideEquation(pointX, distMenu.getSelectedItemIndex(), audioProcessor.treeState.getRawParameterValue(PARAM1_ID)->load());

    pointX *= slope;
    pointX = pointX * shaperArea.getWidth() / 2 + shaperArea.getCentreX();

    pointY *= -1;
    pointY = pointY * shaperArea.getHeight() / 2 + shaperArea.getCentreY();

    pointArea = juce::Rectangle<float>(pointX, pointY, 1, 1);
    pointArea.expand(1, 1);

    g.setColour(juce::Colours::orange);
    g.drawEllipse(pointArea, 4);

}

// Set bounds of stuff
void Waveshaper::resized()
{
    // the .reduced() function shrinks the area by 25 pixels on each side
    // basically adds a margin
    //juce::Rectangle<int> area = getLocalBounds().reduced(25); //draw the menu another 25 pixels down

    distMenu.setBounds(25, getHeight() - 50, SIZE * 4 - 50, 25);
    param1Knob.setBounds(getWidth() - SIZE * 1.25, SIZE * 0.25, SIZE/2, SIZE * 2);

}

// have to implement this function (even with if you put nothing in it)
void Waveshaper::comboBoxChanged(juce::ComboBox*) {

}
