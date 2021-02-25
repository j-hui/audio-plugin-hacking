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
class PitchShiftAudioProcessorEditor  : public juce::AudioProcessorEditor,
public juce::Slider::Listener,
public juce::Button::Listener
{
public:
    PitchShiftAudioProcessorEditor (PitchShiftAudioProcessor&);
    ~PitchShiftAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider *slider) override;
    void buttonClicked (juce::Button *button) override;

private:
    
    short componentWidth = 150;
    short componentHeight = 100;
    
    juce::Component sliderComponent;
    juce::Slider mPitchShiftSlider;
    juce::Label mPitchSliderLabel;
    
    juce::Component peakPhaseComponent;
    juce::ToggleButton mPeakPhaseButton;
    juce::Label mPeakPhaseLabel;
    
    juce::Component peakTrackingComponent;
    juce::ToggleButton mPeakTrackingButton;
    juce::Label mPeakTrackingLabel;
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PitchShiftAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchShiftAudioProcessorEditor)
};
