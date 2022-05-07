/*
  ==============================================================================

	MyKnobs.h
	Created: 26 Feb 2019 8:30:04pm
	Author:  Joonas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define _USE_MATH_DEFINES
#include <math.h>

//==============================================================================
/*
*/
class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
	{
		const float thickness = 4.f;

		// Radius of knob
		float radius = juce::jmin(width / 2, height / 2) - 5;
		// Centre point (centreX, centreY) of knob
		float centreX = x + width * 0.5f;
		float centreY = y + radius + 12.f;

		// current angle of the slider
		float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		// Draw path of the slider backgound (in darker background colour)
		juce::Path backgroundArc;
		backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
		juce::Colour myColour = findColour(0x1005700);
		myColour = myColour.darker(.8f);
		g.setColour(myColour);
		g.strokePath(backgroundArc, juce::PathStrokeType(thickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
		// Draw path of slider foreground (in white)
		juce::Path foregroundArc;
		foregroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
		g.setColour(juce::Colours::lightskyblue);
		g.strokePath(foregroundArc, juce::PathStrokeType(thickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

		// Pointer
		juce::Path p;
		float pointerLength = radius * 1.f;
		p.addRectangle(-thickness * 0.5f, -radius, thickness, pointerLength);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
		//g.setColour(juce::Colours::lightskyblue);
		g.fillPath(p);
	}

	// Slider textbox
	void drawLabel(juce::Graphics& g, juce::Label& label) override
	{
		g.setColour(juce::Colours::white);

		juce::String text = label.getText();
		//text.append(text.getFloatValue(), );
		int width = label.getWidth();
		int height = label.getHeight();
		g.setFont(juce::Font(height - 8.f, juce::Font::plain));
		g.drawText(text, 0, 0, width, height, juce::Justification::centredBottom);
		//g.drawFittedText(text, 0, 0, width, height, juce::Justification::centred, 1);
	}
};