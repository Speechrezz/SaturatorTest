/*
  ==============================================================================

    Equations.h
    Created: 28 Dec 2021 12:01:27pm
    Author:  thesp

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


float analogClip(float sample, float param1);

float digitalClip(float sample, float param1);

float sinoidFold(float sample, float param1);

float softSine(float sample, float param1);

float mediumCurve(float sample, float param1);

float hardCurve(float sample, float param1);

float customCurve(float sample, float param1);

float decideEquation(float sample, int selectedItem, float param1);

float getPartialPoint(const float n1, const float n2, const float perc);

juce::Point<float> getBezierPoint(juce::Point<float>, juce::Point<float>, juce::Point<float>, float);