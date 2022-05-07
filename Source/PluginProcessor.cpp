/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SaturatorTestAudioProcessor::SaturatorTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                    treeState(*this, nullptr, "PARAMETER", { createParameterLayout() }),
                    peakFilter(juce::dsp::IIR::Coefficients<float>::makePeakFilter(44100, 1000.0f, 0.1f, 0.0f)),
                    peakFilterAfter(juce::dsp::IIR::Coefficients<float>::makePeakFilter(44100, 1000.0f, 0.1f, 0.0f)),
                    lowShelfFilter(juce::dsp::IIR::Coefficients<float>::makeLowShelf(44100, LOW_SHELF_FREQ, LOW_SHELF_Q, 0.0f)),
                    lowShelfFilterAfter(juce::dsp::IIR::Coefficients<float>::makeLowShelf(44100, LOW_SHELF_FREQ, LOW_SHELF_Q, 0.0f))
#endif
{
    treeState.state = juce::ValueTree("savedParams");

    //dryAudioArray = std::make_unique<float[]>(BUFFER_SIZE);
    //wetAudioArray = std::make_unique<float[]>(BUFFER_SIZE);

    dryAudioArray.fill(0.0f);
    wetAudioArray.fill(0.0f);

    //setLatencySamples(1);
}

SaturatorTestAudioProcessor::~SaturatorTestAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SaturatorTestAudioProcessor::createParameterLayout()
{
    // Add parameters to the paramater tree. Allows for saving/loading and automating of paramaters
    std::vector < std::unique_ptr <juce::RangedAudioParameter> > params;

    auto driveParam = std::make_unique<juce::AudioParameterFloat>(DRIVE_ID, DRIVE_NAME, MIN_GAIN, MAX_GAIN, 0.0f);
    params.push_back(std::move(driveParam));

    auto gainParam = std::make_unique<juce::AudioParameterFloat>(GAIN_ID, GAIN_NAME, MIN_GAIN, MAX_GAIN, 0.0f);
    params.push_back(std::move(gainParam));

    auto mixParam = std::make_unique<juce::AudioParameterFloat>(MIX_ID, MIX_NAME, 0.0f, 100.0f, 100.0f);
    params.push_back(std::move(mixParam));

    auto freqParam = std::make_unique<juce::AudioParameterFloat>(FREQ_ID, FREQ_NAME, MIN_FREQ, MAX_FREQ, 1000.0f);
    params.push_back(std::move(freqParam));

    auto widthParam = std::make_unique<juce::AudioParameterFloat>(WIDTH_ID, WIDTH_NAME, MIN_WIDTH, MAX_WIDTH, 30.0f);
    params.push_back(std::move(widthParam));

    auto depthParam = std::make_unique<juce::AudioParameterFloat>(DEPTH_ID, DEPTH_NAME, MIN_DEPTH, MAX_DEPTH, 0.0f);
    params.push_back(std::move(depthParam));

    auto baseParam = std::make_unique<juce::AudioParameterFloat>(BASE_ID, BASE_NAME, MIN_GAIN, MAX_GAIN, 0.0f);
    params.push_back(std::move(baseParam));

    auto autoGainParam = std::make_unique<juce::AudioParameterBool>(AUTOGAIN_ID, AUTOGAIN_NAME, true);
    params.push_back(std::move(autoGainParam));

    auto softParam = std::make_unique<juce::AudioParameterFloat>(SOFT_ID, SOFT_NAME, MIN_SOFT, MAX_SOFT, MIN_SOFT);
    params.push_back(std::move(softParam));

    auto param1Param = std::make_unique<juce::AudioParameterFloat>(PARAM1_ID, PARAM1_NAME, 0.0f, 1.0f, 0.0f);
    params.push_back(std::move(param1Param));

    /*
    juce::StringArray types { "Analog Clip", "Soft Sine", ... };
    std::make_unique<juce::AudioParameterChoice>(DISTTYPE_ID, DISTTYPE_NAME, types, 0);

    distMenu.addItems(parameter->getAllValueStrings(), 1);
    distTypeAttach = ...
    */

    // values of the ComboBox internally start at 0, while when declaring it, they start at 1
    auto distTypeParam = std::make_unique<juce::AudioParameterFloat>(DISTTYPE_ID, DISTTYPE_NAME, 0, 4, 0);
    params.push_back(std::move(distTypeParam));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String SaturatorTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SaturatorTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SaturatorTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SaturatorTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SaturatorTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SaturatorTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SaturatorTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SaturatorTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SaturatorTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void SaturatorTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SaturatorTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // Setup the dsp module initializations
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // resets filter every time we stop and play music
    peakFilter.prepare(spec);
    peakFilter.reset();

    peakFilterAfter.prepare(spec);
    peakFilterAfter.reset();

    lowShelfFilter.prepare(spec);
    lowShelfFilter.reset();

    lowShelfFilterAfter.prepare(spec);
    lowShelfFilterAfter.reset();

    highShelfFilter.prepare(spec);
    highShelfFilter.reset();

    highShelfFilterAfter.prepare(spec);
    highShelfFilterAfter.reset();

    dryBuffer.setSize(spec.numChannels, spec.maximumBlockSize);

    prevDrive = juce::Decibels::decibelsToGain(treeState.getRawParameterValue(DRIVE_ID)->load());
    prevDriveInv = juce::Decibels::decibelsToGain(0.75 * treeState.getRawParameterValue(DRIVE_ID)->load());
    prevGain = juce::Decibels::decibelsToGain(treeState.getRawParameterValue(GAIN_ID)->load());
    prevMix = treeState.getRawParameterValue(MIX_ID)->load() / 100.0f;

}

void SaturatorTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SaturatorTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SaturatorTestAudioProcessor::updateParameters() {
    
    // update paramaters for your process here
    
    float freq = treeState.getRawParameterValue(FREQ_ID)->load();
    float gain = juce::Decibels::decibelsToGain(treeState.getRawParameterValue(DEPTH_ID)->load());
    float gainInv = juce::Decibels::decibelsToGain(-1 * treeState.getRawParameterValue(DEPTH_ID)->load());
    float width = treeState.getRawParameterValue(WIDTH_ID)->load();
    width = (1 - (width / 100.0)) + 0.01;
    float base = juce::Decibels::decibelsToGain(treeState.getRawParameterValue(BASE_ID)->load());
    float baseInv = juce::Decibels::decibelsToGain(-1.0 * treeState.getRawParameterValue(BASE_ID)->load());
    float soft = -1.f * treeState.getRawParameterValue(SOFT_ID)->load();
    float softAfter = juce::Decibels::decibelsToGain(soft * 0.2);
    soft = juce::Decibels::decibelsToGain(soft);

    // Since 'peakFilter' is a ProcessDuplicator (NOT an actual filter), we need to use peakFilter.state to access the filter
    *peakFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), freq, width, gain);
    *peakFilterAfter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), freq, width, gainInv);
    *lowShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), LOW_SHELF_FREQ, LOW_SHELF_Q, base);
    *lowShelfFilterAfter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), LOW_SHELF_FREQ, LOW_SHELF_Q, baseInv);
    *highShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), HIGH_SHELF_FREQ, LOW_SHELF_Q, soft);
    *highShelfFilterAfter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), HIGH_SHELF_FREQ, LOW_SHELF_Q, softAfter);
}

void SaturatorTestAudioProcessor::setDistType(float* selection) {

}

// Does all the DSP processes
void SaturatorTestAudioProcessor::process(juce::dsp::ProcessContextReplacing<float> context) {

    // do processing here
    peakFilter.process(context);
    //peakFilterAfter.process(context);
    lowShelfFilter.process(context);
    highShelfFilter.process(context);

}

// Does all the DSP processes
void SaturatorTestAudioProcessor::processAfter(juce::dsp::ProcessContextReplacing<float> context) {

    // do processing here
    peakFilterAfter.process(context);
    lowShelfFilterAfter.process(context);
    highShelfFilterAfter.process(context);

}

void SaturatorTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    dryBuffer.makeCopyOf(buffer, true);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    // ====================================================================================================
    // Load all the necessary values from the parameters
    const float drive = juce::Decibels::decibelsToGain(treeState.getRawParameterValue(DRIVE_ID)->load());
    const float driveInv = juce::Decibels::decibelsToGain(0.75 * treeState.getRawParameterValue(DRIVE_ID)->load());
    const float gain = juce::Decibels::decibelsToGain(treeState.getRawParameterValue(GAIN_ID)->load());
    const float mix = treeState.getRawParameterValue(MIX_ID)->load() / 100.0f;
    const float distType = treeState.getRawParameterValue(DISTTYPE_ID)->load();
    float newSample;
    const bool autoGain = treeState.getRawParameterValue(AUTOGAIN_ID)->load();

    // Apply input gain/drive
    buffer.applyGainRamp(0, buffer.getNumSamples(), prevDrive, drive);

    dryBuffer.applyGainRamp(0, buffer.getNumSamples(), 1 - prevMix, 1 - mix);

    // add samples to output buffer for the editor
    const int sampleRate = 5;
    int i = sampleRate;
    int index = 0;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (i == sampleRate) {

            newSample = (buffer.getSample(0, sample) + buffer.getSample(0, sample)) / 2;

            dryAudioArray[index] = newSample;

            i = 0;
            index++;
        }
        else
            i++;
    }

    // the dsp module uses AudioBlocks as its data type
    // no need to iterate through each sample
    juce::dsp::AudioBlock <float> block(buffer);
    updateParameters();
    process(juce::dsp::ProcessContextReplacing<float>(block));

    if (distType == ANALOG_CLIP) 
        customSoftclip(juce::dsp::ProcessContextReplacing<float>(block));
    else if (distType == DIGITAL_CLIP) 
        processDigitalClip(juce::dsp::ProcessContextReplacing<float>(block));
    else if (distType == SINOID_FOLD) 
        processSinoidFold(juce::dsp::ProcessContextReplacing<float>(block));
    else if (distType == SOFT_SINE)
        processSoftSine(juce::dsp::ProcessContextReplacing<float>(block));
    else if (distType == MEDIUM_CURVE)
        processMediumCurve(juce::dsp::ProcessContextReplacing<float>(block));
    else if (distType == HARD_CURVE)
        processHardCurve(juce::dsp::ProcessContextReplacing<float>(block));
    else if (distType == CUSTOM)
        processCustomCurve(juce::dsp::ProcessContextReplacing<float>(block));
    
    /*
    // add samples to circular buffer
    int i = 10;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (i == 10) {

            //for (int channel = 0; channel < totalNumInputChannels; ++channel) 
            //    newSample = buffer.getSample(channel, sample);

            newSample = (buffer.getSample(0, sample) + buffer.getSample(0, sample))/2;

            cbuf.put(newSample);

            i = 0;
        }
        else
            i++;
    } */


    processAfter(juce::dsp::ProcessContextReplacing<float>(block));

    // add samples to output buffer for the editor
    i = 10;
    index = 0;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (i == 10) {

            newSample = (buffer.getSample(0, sample) + buffer.getSample(0, sample))/2;

            wetAudioArray[index] = newSample;

            i = 0;
            index++;
        }
        else
            i++;
    }


    if (autoGain == true) {
        buffer.applyGainRamp(0, buffer.getNumSamples(), 1 / prevDriveInv, 1 / driveInv);
    }

    buffer.applyGainRamp(0, buffer.getNumSamples(), prevGain, gain);
    juce::dsp::AudioBlock <float> dryBlock(dryBuffer);
    juce::dsp::ProcessContextReplacing<float> dryContext(dryBlock);

    buffer.applyGainRamp(0, buffer.getNumSamples(), prevMix, mix);
    juce::dsp::AudioBlock <float> wetBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> wetContext(wetBlock);

    prevDrive = drive;
    prevDriveInv = driveInv;
    prevGain = gain;
    prevMix = mix;

    wetContext.getOutputBlock().add(dryBlock);

    //buffer.makeCopyOf();
    //buffer.add
}

//==============================================================================
bool SaturatorTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SaturatorTestAudioProcessor::createEditor()
{
    return new SaturatorTestAudioProcessorEditor (*this);
}

//==============================================================================
void SaturatorTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SaturatorTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    // 'If' statements are for error checking
    if (xmlState != nullptr) {
        if (xmlState->hasTagName(treeState.state.getType())) {
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SaturatorTestAudioProcessor();
}

std::array<float, BUFFER_SIZE> SaturatorTestAudioProcessor::getWetArray() {
    return wetAudioArray;
}
std::array<float, BUFFER_SIZE> SaturatorTestAudioProcessor::getDryArray() {
    return dryAudioArray;
}

// very gentle soft clipping with normalization
void SaturatorTestAudioProcessor::customSoftclip(juce::dsp::ProcessContextReplacing<float> context) {
    return (processWaveshaper(context, &analogClip));
}

// digital clipping
void SaturatorTestAudioProcessor::processDigitalClip(juce::dsp::ProcessContextReplacing<float> context) {
    return (processWaveshaper(context, &digitalClip));
}

// sinoid clipping
void SaturatorTestAudioProcessor::processSinoidFold(juce::dsp::ProcessContextReplacing<float> context) {
    return (processWaveshaper(context, &sinoidFold));
}

// soft sine clipping
void SaturatorTestAudioProcessor::processSoftSine(juce::dsp::ProcessContextReplacing<float> context) {
    return (processWaveshaper(context, &softSine));
}


void SaturatorTestAudioProcessor::processMediumCurve(juce::dsp::ProcessContextReplacing<float> context) {
    return (processWaveshaper(context, &mediumCurve));
}


void SaturatorTestAudioProcessor::processHardCurve(juce::dsp::ProcessContextReplacing<float> context) {
    return (processWaveshaper(context, &hardCurve));
}


void SaturatorTestAudioProcessor::processCustomCurve(juce::dsp::ProcessContextReplacing<float> context) {
    return (processWaveshaper(context, &customCurve));
}

// general waveshaper
void SaturatorTestAudioProcessor::processWaveshaper(juce::dsp::ProcessContextReplacing<float> context, float (*func)(float, float)) {
    // process audio...
    float newSample;
    const float param1 = treeState.getRawParameterValue(PARAM1_ID)->load();
    auto totalNumInputChannels = getTotalNumInputChannels();

    // iterate through each sample
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = context.getOutputBlock().getChannelPointer(channel);

        // process audio...
        for (int sample = 0; sample < context.getInputBlock().getNumSamples(); ++sample) {

            newSample = context.getInputBlock().getSample(channel, sample);

            channelData[sample] = func(newSample, param1);
        }
    }
}

// general waveshaper
void SaturatorTestAudioProcessor::processWaveshaper(juce::dsp::ProcessContextReplacing<float> context, float (*func)(float)) {
    // process audio...
    float newSample;
    auto totalNumInputChannels = getTotalNumInputChannels();

    // iterate through each sample
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = context.getOutputBlock().getChannelPointer(channel);

        // process audio...
        for (int sample = 0; sample < context.getInputBlock().getNumSamples(); ++sample) {

            newSample = context.getInputBlock().getSample(channel, sample);

            channelData[sample] = func(newSample);
        }
    }
}
