/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "fxobjects.h"




//==============================================================================
/**
*/
class PitchShiftAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PitchShiftAudioProcessor();
    ~PitchShiftAudioProcessor() override;

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
    
    //==============================================================================
    void setPitchShift (double semitones);
    void setParams (double semitones, bool enablePeakPhaseLocking, bool enablePeakTracking);
    
    
    
    void setSemitones (double semitones);
    void setPeakPhaseLocking (bool enablePeakPhaseLocking);
    void setPeakTracking (bool enablePeakTracking);
    
    
    double getSemitones();
    bool getPeakPhaseLocking();
    bool getPeakTracking();
    
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchShiftAudioProcessor)
    
    double semitones;
    bool enablePeakPhaseLocking;
    bool enablePeakTracking;
    
    PSMVocoder theVocoder = PSMVocoder();
    PSMVocoderParameters params;
    
    
    
};
