/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveshaperAudioProcessor::WaveshaperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        params(*this, nullptr, "PARAMETERS", {
            std::make_unique<juce::AudioParameterFloat>("IN_GAIN", "In Gain", 0.0f, 2.0f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("OUT_GAIN", "Out Gain", 0.0f, 2.0f, 1.0f),
            std::unique_ptr<juce::AudioParameterChoice>(new juce::AudioParameterChoice("SHAPE", "Shape",
            {"5th", "3rd", "Digital Clip Only"}, 0)) // CHANGEME
        })
#endif
{
    shape_fns[0] = [](float x) -> float { return 5*x - 20*x*x*x + 16*x*x*x*x*x; };
    shape_fns[1] = [](float x) -> float { return 3*x- 4*x*x*x; };
    shape_fns[2] = [](float x) -> float { return x; };
    // see all the CHANGEME s to find relevant arrays
}

WaveshaperAudioProcessor::~WaveshaperAudioProcessor()
{
}

//==============================================================================
const juce::String WaveshaperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WaveshaperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WaveshaperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WaveshaperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WaveshaperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WaveshaperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WaveshaperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WaveshaperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WaveshaperAudioProcessor::getProgramName (int index)
{
    return {};
}

void WaveshaperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WaveshaperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    prev_in_gain = *params.getRawParameterValue("IN_GAIN");
    prev_out_gain = *params.getRawParameterValue("OUT_GAIN");
}

void WaveshaperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveshaperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//float waveshape(float in) {
//    //return 3*in - 4*in*in*in;
//    return juce::jlimit(-1.0f, 1.0f, 5*in - 20*in*in*in + 16*in*in*in*in*in);
//}

void WaveshaperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    float in_gain = *params.getRawParameterValue("IN_GAIN");
    float out_gain = *params.getRawParameterValue("OUT_GAIN");
    float shape_index = *params.getRawParameterValue("SHAPE");
    waveshaper_ptr_t waveshape = shape_fns[juce::truncatePositiveToUnsignedInt(shape_index)];

    buffer.applyGainRamp(0, buffer.getNumSamples(), prev_in_gain, in_gain);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        // do waveshaping
        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
            channelData[sample] = juce::jlimit(-1.0f, 1.0f, waveshape(channelData[sample]));
        }
    }
    
    buffer.applyGainRamp(0, buffer.getNumSamples(), prev_out_gain, out_gain);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        // limit all samples, digitally clipping
        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
            channelData[sample] = juce::jlimit(-1.0f, 1.0f, channelData[sample]);
        }
    }
    
    
    prev_in_gain = in_gain;
    prev_out_gain = out_gain;
}

//==============================================================================
bool WaveshaperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WaveshaperAudioProcessor::createEditor()
{
    return new WaveshaperAudioProcessorEditor (*this);
}

//==============================================================================
void WaveshaperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::ValueTree state = params.copyState();
    std::unique_ptr<juce::XmlElement> xml = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void WaveshaperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    
    if (xml.get() != nullptr && xml->hasTagName(params.state.getType())) {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        params.replaceState(state);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveshaperAudioProcessor();
}
