/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PitchShiftAudioProcessorEditor::PitchShiftAudioProcessorEditor (PitchShiftAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    mPitchShiftSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mPitchShiftSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, 0, 50, 20);
    mPitchShiftSlider.setRange(-12.0, 12.0, 0.01);
    mPitchShiftSlider.setValue(0.0);
    mPitchShiftSlider.addListener(this);
    sliderComponent.addAndMakeVisible(mPitchShiftSlider);
    
    mPitchSliderLabel.setText("shift (tones)", juce::dontSendNotification);
    mPitchSliderLabel.attachToComponent(&sliderComponent, false);
    
    addAndMakeVisible(sliderComponent);
    
    
    mPeakPhaseButton.setButtonText("enablePeakPhaseLocking");
    mPeakPhaseButton.addListener(this);
    peakPhaseComponent.addAndMakeVisible(mPeakPhaseButton);
    
    mPeakPhaseLabel.setText("enablePeakPhaseLocking", juce::dontSendNotification);
    mPeakPhaseLabel.attachToComponent(&peakPhaseComponent, false);
    
    addAndMakeVisible(peakPhaseComponent);
    
    
    mPeakTrackingButton.setButtonText("enablePeakTracking");
    mPeakTrackingButton.addListener(this);
    peakTrackingComponent.addAndMakeVisible(mPeakTrackingButton);
    
    mPeakTrackingLabel.setText("enablePeakTracking", juce::dontSendNotification);
    mPeakTrackingLabel.attachToComponent(&peakTrackingComponent, false);
    
    addAndMakeVisible(peakTrackingComponent);
    
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (5*componentWidth, 3*componentHeight);
}

PitchShiftAudioProcessorEditor::~PitchShiftAudioProcessorEditor()
{
}

//==============================================================================
void PitchShiftAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::blueviolet);
    
    
//    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("reset?", getLocalBounds(), juce::Justification::centred, 1);
}

void PitchShiftAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    short sliderWidth = 150, sliderHeight = 100;
    short buttonWidth = 150, buttonHeight = 100;
    
    
    sliderComponent.setBounds(/*(getWidth() - componentWidth) / 5,*/ 0,
                              (getHeight() - componentHeight) / 2,
                              componentWidth*1.1,
                              componentHeight*1.1);
    mPitchShiftSlider.setBounds(0, 0, sliderWidth, sliderHeight);
    
    
    peakPhaseComponent.setBounds(2*((getWidth() - componentWidth) / 5),
                                 (getHeight() - componentHeight) / 2,
                                 componentWidth*1.1,
                                 componentHeight*1.1);
    mPeakPhaseButton.setBounds(0, 0, buttonWidth, buttonHeight);
    
    
    peakTrackingComponent.setBounds(4*((getWidth() - componentWidth) / 5),
                                 (getHeight() - componentHeight) / 2,
                                 componentWidth*1.1,
                                 componentHeight*1.1);
    mPeakTrackingButton.setBounds(0, 0, buttonWidth, buttonHeight);
}


void PitchShiftAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &mPitchShiftSlider) {
        audioProcessor.setSemitones(slider->getValue());
        audioProcessor.setPitchShift(slider->getValue());
    }
}


void PitchShiftAudioProcessorEditor::buttonClicked(juce::Button *button)
{
    if (button == &mPeakPhaseButton) {
        audioProcessor.setPeakPhaseLocking(!audioProcessor.getPeakPhaseLocking());
    } else if (button == &mPeakTrackingButton) {
        audioProcessor.setPeakTracking(!audioProcessor.getPeakTracking());
    }
    audioProcessor.setParams(audioProcessor.getSemitones(),
                             audioProcessor.getPeakPhaseLocking(),
                             audioProcessor.getPeakTracking());
}
