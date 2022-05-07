/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define SIZE 100

#define DRIVE_ID "drive"
#define DRIVE_NAME "Drive"
#define GAIN_ID "gain"
#define GAIN_NAME "Gain"
#define MIX_ID "mix"
#define MIX_NAME "Mix"

#define BASE_ID "base"
#define BASE_NAME "Base"
#define FREQ_ID "freq"
#define FREQ_NAME "Freq"
#define WIDTH_ID "width"
#define WIDTH_NAME "Width"
#define DEPTH_ID "depth"
#define DEPTH_NAME "Depth"
#define SOFT_ID "soft"
#define SOFT_NAME "Soft"

#define DISTTYPE_ID "distType"
#define DISTTYPE_NAME "Distortion Type"

#define AUTOGAIN_ID "autogain"
#define AUTOGAIN_NAME "Auto Gain"

#define PARAM1_ID "param1"
#define PARAM1_NAME "Param1"
#define PARAM2_ID "param2"
#define PARAM2_NAME "Param2"
#define PARAM3_ID "param3"
#define PARAM3_NAME "Param3"

#define MIN_GAIN -36.0
#define MAX_GAIN 36.0
#define MIN_FREQ 30.0
#define MAX_FREQ 18500.0
#define MIN_DEPTH -24.0
#define MAX_DEPTH 24.0
#define MIN_WIDTH 0.0f
#define MAX_WIDTH 100.0f
#define MIN_SOFT 0.0f
#define MAX_SOFT 24.0f

#define LOW_SHELF_FREQ 80.0f
#define HIGH_SHELF_FREQ 15000.0f
#define LOW_SHELF_Q 0.1f

#define ANALOG_CLIP 0
#define SOFT_SINE 1
#define MEDIUM_CURVE 2
#define HARD_CURVE 3
#define SINOID_FOLD 3
#define DIGITAL_CLIP 4
#define CUSTOM 5

#define BUFFER_SIZE 1024

#include <JuceHeader.h>
#include "Equations.h"
//#include "CircularBuffer.hpp"

//==============================================================================
/**
*/
class SaturatorTestAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SaturatorTestAudioProcessor();
    ~SaturatorTestAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void customSoftclip(juce::dsp::ProcessContextReplacing<float>);
    void processDigitalClip(juce::dsp::ProcessContextReplacing<float>);
    void processSinoidFold(juce::dsp::ProcessContextReplacing<float>);
    void processSoftSine(juce::dsp::ProcessContextReplacing<float>);
    void processMediumCurve(juce::dsp::ProcessContextReplacing<float>);
    void processHardCurve(juce::dsp::ProcessContextReplacing<float>);
    void processCustomCurve(juce::dsp::ProcessContextReplacing<float>);
    void processWaveshaper(juce::dsp::ProcessContextReplacing<float>, float (*func)(float));
    void processWaveshaper(juce::dsp::ProcessContextReplacing<float>, float (*func)(float, float));

    void process(juce::dsp::ProcessContextReplacing<float> context);
    void processAfter(juce::dsp::ProcessContextReplacing<float> context);
    void updateParameters();

    void setDistType(float* selection);

    std::array<float, BUFFER_SIZE> getWetArray();
    std::array<float, BUFFER_SIZE> getDryArray();
    

    juce::AudioProcessorValueTreeState::ParameterLayout SaturatorTestAudioProcessor::createParameterLayout();
    juce::AudioProcessorValueTreeState treeState;

    //CircularBuffer<float, CBUFFER_SIZE> cbuf;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturatorTestAudioProcessor)

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peakFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peakFilterAfter;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelfFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelfFilterAfter;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelfFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelfFilterAfter;

    //juce::dsp::ProcessorDuplicator<juce::dsp::WaveShaper<float>, juce::dsp::CreateWaveShaper<float>)

    //std::unique_ptr<float[]> dryAudioArray;
    std::array<float, BUFFER_SIZE> dryAudioArray;
    std::array<float, BUFFER_SIZE> wetAudioArray;
    //std::unique_ptr<float[]> wetAudioArray;

    juce::AudioBuffer<float> dryBuffer;

    float prevDrive;
    float prevDriveInv;
    float prevGain;
    float prevMix;

};
