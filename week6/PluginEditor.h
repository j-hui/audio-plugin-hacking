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
class WaveshaperAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WaveshaperAudioProcessorEditor (WaveshaperAudioProcessor&);
    ~WaveshaperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider in_gain_slider;
    juce::Slider out_gain_slider;
    juce::ComboBox shape_combobox;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> in_gain_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> out_gain_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> shape_combobox_attachment;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WaveshaperAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperAudioProcessorEditor)
};
