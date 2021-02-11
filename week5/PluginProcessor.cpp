/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModDelayAudioProcessor::ModDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(mDryWetParameter = new juce::AudioParameterFloat("drywet",
                                                                  "Dry Wet",
                                                                  0.0,
                                                                  1.0,
                                                                  0.5));
    
    addParameter(mDepthParameter = new juce::AudioParameterFloat("depth",
                                                                 "Depth",
                                                                 0.0,
                                                                 1.0,
                                                                 0.5));
    
    addParameter(mRateParameter = new juce::AudioParameterFloat("rate",
                                                                "Rate",
                                                                0.1f,
                                                                20.f,
                                                                10.f));
    
    addParameter(mPhaseOffsetParameter = new juce::AudioParameterFloat("phaseoffset",
                                                                       "Phase Offset",
                                                                       0.0f,
                                                                       1.f,
                                                                       0.f));
    
    addParameter(mFeedbackParameter = new juce::AudioParameterFloat("feedback",
                                                                    "Feedback",
                                                                    0,
                                                                    0.98,
                                                                    0.5));
    
    addParameter(mTypeParameter = new juce::AudioParameterInt("type",
                                                              "Type",
                                                              0,
                                                              1,
                                                              0));

    
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    mLFOPhase = 0;
}

ModDelayAudioProcessor::~ModDelayAudioProcessor()
{
    if (mCircularBufferLeft != nullptr) {
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
    
    if (mCircularBufferRight != nullptr) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
}

//==============================================================================
const juce::String ModDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ModDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ModDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ModDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ModDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ModDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ModDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ModDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ModDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void ModDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ModDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
    
    if (mCircularBufferLeft == nullptr) {
        mCircularBufferLeft = new float[mCircularBufferLength];
    }
    juce::zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float));
    
    if (mCircularBufferRight == nullptr) {
        mCircularBufferRight = new float[mCircularBufferLength];
    }
    juce::zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float));
    
    mCircularBufferWriteHead = 0;
    mLFOPhase = 0;
}

void ModDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ModDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ModDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    
    
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        
        // LFO is a sine wave
        float lfoOutLeft = sin(2*M_PI * mLFOPhase);
        
        // offset the right phase from the left by parameter
        float lfoPhaseRight = mLFOPhase + *mPhaseOffsetParameter;
        if (lfoPhaseRight > 1) {
            lfoPhaseRight -= 1;
        }
        float lfoOutRight = sin(2*M_PI * lfoPhaseRight);
        
        // Intensity of the LFO effect
        lfoOutLeft *= *mDepthParameter;
        lfoOutRight *= *mDepthParameter;
        
        float lfoOutMappedLeft;
        float lfoOutMappedRight;
        
        // Change depth range based on effect type
        // Chorus
        if (*mTypeParameter == 0) {
            lfoOutMappedLeft = juce::jmap(lfoOutLeft, -1.f, 1.f, 0.001f, 0.03f);
            lfoOutMappedRight = juce::jmap(lfoOutRight, -1.f, 1.f, 0.001f, 0.03f);
        } else { // Flanger
            lfoOutMappedLeft = juce::jmap(lfoOutLeft, -1.f, 1.f, 0.f, 0.007f);
            lfoOutMappedRight = juce::jmap(lfoOutRight, -1.f, 1.f, 0.f, 0.007f);
        }
        
        // Calculate Delay time
        float delayTimeSamplesLeft = getSampleRate() * lfoOutMappedLeft;
        float delayTimeSamplesRight = getSampleRate() * lfoOutMappedRight;
        
        // update LFO phase
        mLFOPhase += *mRateParameter / getSampleRate();
        if (mLFOPhase > 1) {
            mLFOPhase -= 1;
        }
        
        // Write
        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;
        
        // Figure out where we're reading from
        float delayReadHeadLeft = mCircularBufferWriteHead - delayTimeSamplesLeft;
        if (delayReadHeadLeft < 0) {
            delayReadHeadLeft += mCircularBufferLength;
        }
        
        float delayReadHeadRight = mCircularBufferWriteHead - delayTimeSamplesRight;
        if (delayReadHeadRight < 0) {
            delayReadHeadRight += mCircularBufferLength;
        }
        
        // Get linear interpolation parameters
        int readHeadLeft_x = (int)delayReadHeadLeft;
        int readHeadLeft_x1 = readHeadLeft_x + 1;
        float readHeadFloatLeft = delayReadHeadLeft - readHeadLeft_x;
        if (readHeadLeft_x1 >= mCircularBufferLength) {
            readHeadLeft_x1 -= mCircularBufferLength;
        }
        
        int readHeadRight_x = (int)delayReadHeadRight;
        int readHeadRight_x1 = readHeadRight_x + 1;
        float readHeadFloatRight = delayReadHeadRight - readHeadRight_x;
        if (readHeadRight_x1 >= mCircularBufferLength) {
            readHeadRight_x1 -= mCircularBufferLength;
        }
        
        // Read delay & Interpolate
        float delay_sample_left = lin_interp(mCircularBufferLeft[readHeadLeft_x],  mCircularBufferLeft[readHeadLeft_x1], readHeadFloatLeft);
        float delay_sample_right = lin_interp(mCircularBufferRight[readHeadRight_x],  mCircularBufferRight[readHeadRight_x1], readHeadFloatRight);
        
        // Multiply by how much feedback we want
        mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
        mFeedbackRight = delay_sample_right * *mFeedbackParameter;
        
        // Play current sample + delay sample based on w/d parameter
        buffer.setSample(0, i, buffer.getSample(0,i) * (1-*mDryWetParameter) + delay_sample_left * *mDryWetParameter);
        buffer.setSample(1, i, buffer.getSample(1,i) * (1-*mDryWetParameter) + delay_sample_right * *mDryWetParameter);

        // Increment write head
        mCircularBufferWriteHead++;
        if (mCircularBufferWriteHead > mCircularBufferLength) {
            mCircularBufferWriteHead = 0;
        }
    }
}

//==============================================================================
bool ModDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ModDelayAudioProcessor::createEditor()
{
    return new ModDelayAudioProcessorEditor (*this);
}

//==============================================================================
void ModDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ModDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModDelayAudioProcessor();
}

float ModDelayAudioProcessor::lin_interp(float sample_x, float sample_x1, float inPhase) {
    return (1 -  inPhase) * sample_x + inPhase * sample_x1;
}
