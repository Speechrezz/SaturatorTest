/*
  ==============================================================================

    Equations.cpp
    Created: 28 Dec 2021 12:01:27pm
    Author:  thesp

  ==============================================================================
*/

#include "Equations.h"
#include "PluginProcessor.h"
#include <cmath>

#define PI_HALF 1.5708

float analogClip(float sample, float param1) {
    //const float saturation = param1;
    //float newSample = sample;

    int negative = (sample > 0) - (sample < 0);
    sample = fabsf(sample); //newSample * negative;
    if (sample > param1)
        sample = param1 + (sample - param1) / (1 + pow((sample - param1) / (1 - param1), 2));

    return negative * sample * (1 / ((param1 + 1) / 2)); //normalization AND puts the negative sign back
}

/*float digitalClip(float sample) {
    sample = fminf(1.0f, sample);
    sample = fmaxf(-1.0f, sample);

    return sample;
} */

float sinoidFold(float sample, float param1) {

    float coef = fabsf(sample) * param1 + (1 - param1);
    float time = 1.f + param1 * 2.f;
    
    param1 *= 0.5;

    return sinf(PI_HALF * sample * time) * (1.f - param1) + sample/2 * param1;
}

float softSine(float sample, float param1) {
    sample = fminf(1.f + param1, sample);
    sample = fmaxf(-1.f - param1, sample);

    return sinf(PI_HALF * sample);
}

float mediumCurve(float sample, float param1) {
    const float negative = (sample > 0) - (sample < 0);
    sample = fabsf(sample);

    return powf(tanhf(sample * (1 + param1)), param1 * 2.f + 1.f) * negative;
}

// rename crazy curve
float hardCurve(float sample, float param1) {
    return 1.5 * sample - 0.5 * sample * sample * sample;
}

float digitalClip(float sample, float param1) {
    param1 = 1.0 - param1;
    const float negative = (sample > 0) - (sample < 0);
    sample = fabsf(sample);

    //if (sample < 0)
    //    return 0.0;
    if (sample >= (1 - param1) + 1.0)
        return negative;
    if (sample <= param1)
        return sample * negative;

    const juce::Point<float> p1(param1, param1);
    const juce::Point<float> p2(1.0, 1.0);
    const juce::Point<float> p3((1 - param1) + 1.0, 1.0);

    return getBezierPoint(p1, p2, p3, sample).getY() * negative;
}

float customCurve(float sample, float param1) {
    return sample;
}

float decideEquation(float sample, int selectedItem, float param1) {
    float point;
    if (selectedItem == ANALOG_CLIP)
        point = analogClip(sample, param1);
    else if (selectedItem == DIGITAL_CLIP)
        point = digitalClip(sample, param1);
    else if (selectedItem == SINOID_FOLD)
        point = sinoidFold(sample, param1);
    else if (selectedItem == SOFT_SINE)
        point = softSine(sample, param1);
    else if (selectedItem == HARD_CURVE)
        point = hardCurve(sample, param1);
    else if (selectedItem == MEDIUM_CURVE)
        point = mediumCurve(sample, param1);
    else if (selectedItem == CUSTOM)
        point = customCurve(sample, param1);
    else
        point = sample;

    return point;
}

// necessary to compute the bezier curve
float getPartialPoint(const float n1, const float n2, const float perc) {
    const float diff = n2 - n1;

    return n1 + (diff * perc);
}

// creates a bezier point between p1 and p3, using p2 as the amount of curve.
// i is a value between 0 and 1, 0 being at p1 and 1 being p3
juce::Point<float> getBezierPoint(juce::Point<float> p1, juce::Point<float> p2, juce::Point<float> p3, float x) {

    const float i = (x - p1.getX()) / (p3.getX() - p1.getX());

    // A line representing the slope of the point
    const float xa = getPartialPoint(p1.getX(), p2.getX(), i);
    const float ya = getPartialPoint(p1.getY(), p2.getY(), i);
    const float xb = getPartialPoint(p2.getX(), p3.getX(), i);
    const float yb = getPartialPoint(p2.getY(), p3.getY(), i);

    // The actual point
    juce::Point<float> bezierPoint;
    bezierPoint.setX(getPartialPoint(xa, xb, i));
    bezierPoint.setY(getPartialPoint(ya, yb, i));

    return bezierPoint;
}
