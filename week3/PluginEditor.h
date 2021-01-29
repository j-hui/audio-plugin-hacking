/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class MyGainAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MyGainAudioProcessorEditor (MyGainAudioProcessor&);
    ~MyGainAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MyGainAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyGainAudioProcessorEditor)
};
