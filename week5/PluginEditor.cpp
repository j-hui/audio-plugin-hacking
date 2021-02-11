/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModDelayAudioProcessorEditor::ModDelayAudioProcessorEditor (ModDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    auto& params = processor.getParameters();
    
    // This is the wrong way to do this :)
    
    juce::AudioParameterFloat* dryWetParameter = (juce::AudioParameterFloat*) params.getUnchecked(0);
    mDryWetSlider.setBounds(50, 30, 100, 100);
    mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
    mDryWetSlider.setValue(*dryWetParameter);
    addAndMakeVisible(mDryWetSlider);
    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue(); };
    mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture();};
    mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture();};
    addAndMakeVisible(dryWetLabel);
    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.attachToComponent(&mDryWetSlider, false);
    dryWetLabel.setJustificationType(juce::Justification::centredBottom);
    
    juce::AudioParameterFloat* depthParameter = (juce::AudioParameterFloat*) params.getUnchecked(1);
    mDepthSlider.setBounds(100, 160, 100, 100);
    mDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDepthSlider.setRange(depthParameter->range.start, depthParameter->range.end);
    mDepthSlider.setValue(*depthParameter);
    addAndMakeVisible(mDepthSlider);
    mDepthSlider.onValueChange = [this, depthParameter] { *depthParameter = mDepthSlider.getValue(); };
    mDepthSlider.onDragStart = [depthParameter] { depthParameter->beginChangeGesture();};
    mDepthSlider.onDragEnd = [depthParameter] { depthParameter->endChangeGesture();};
    addAndMakeVisible(depthSliderLabel);
    depthSliderLabel.setText("LFO Depth", juce::dontSendNotification);
    depthSliderLabel.attachToComponent(&mDepthSlider, false);
    depthSliderLabel.setJustificationType(juce::Justification::centredBottom);
    
    juce::AudioParameterFloat* rateParameter = (juce::AudioParameterFloat*) params.getUnchecked(2);
    mRateSlider.setBounds(200, 160, 100, 100);
    mRateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mRateSlider.setRange(rateParameter->range.start, rateParameter->range.end);
    mRateSlider.setValue(*rateParameter);
    addAndMakeVisible(mRateSlider);
    mRateSlider.onValueChange = [this, rateParameter] { *rateParameter = mRateSlider.getValue(); };
    mRateSlider.onDragStart = [rateParameter] { rateParameter->beginChangeGesture();};
    mRateSlider.onDragEnd = [rateParameter] { rateParameter->endChangeGesture();};
    addAndMakeVisible(rateSliderLabel);
    rateSliderLabel.setText("LFO Rate", juce::dontSendNotification);
    rateSliderLabel.attachToComponent(&mRateSlider, false);
    rateSliderLabel.setJustificationType(juce::Justification::centredBottom);
    
    juce::AudioParameterFloat* phaseParameter = (juce::AudioParameterFloat*) params.getUnchecked(3);
    mPhaseOffsetSlider.setBounds(250, 30, 100, 100);
    mPhaseOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mPhaseOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mPhaseOffsetSlider.setRange(phaseParameter->range.start, phaseParameter->range.end);
    mPhaseOffsetSlider.setValue(*phaseParameter);
    addAndMakeVisible(mPhaseOffsetSlider);
    mPhaseOffsetSlider.onValueChange = [this, phaseParameter] { *phaseParameter = mPhaseOffsetSlider.getValue(); };
    mPhaseOffsetSlider.onDragStart = [phaseParameter] { phaseParameter->beginChangeGesture();};
    mPhaseOffsetSlider.onDragEnd = [phaseParameter] { phaseParameter->endChangeGesture();};
    addAndMakeVisible(phaseOffsetSliderLabel);
    phaseOffsetSliderLabel.setText("Phase Offset", juce::dontSendNotification);
    phaseOffsetSliderLabel.attachToComponent(&mPhaseOffsetSlider, false);
    phaseOffsetSliderLabel.setJustificationType(juce::Justification::centredBottom);
    
    juce::AudioParameterFloat* feedbackParamater = (juce::AudioParameterFloat*) params.getUnchecked(4);
    mFeedbackSlider.setBounds(150, 30, 100, 100);
    mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mFeedbackSlider.setRange(feedbackParamater->range.start, feedbackParamater->range.end);
    mFeedbackSlider.setValue(*feedbackParamater);
    addAndMakeVisible(mFeedbackSlider);
    mFeedbackSlider.onValueChange = [this, feedbackParamater] { *feedbackParamater = mFeedbackSlider.getValue(); };
    mFeedbackSlider.onDragStart = [feedbackParamater] { feedbackParamater->beginChangeGesture();};
    mFeedbackSlider.onDragEnd = [feedbackParamater] { feedbackParamater->endChangeGesture();};
    addAndMakeVisible(feedbackSliderLabel);
    feedbackSliderLabel.setText("Feedback", juce::dontSendNotification);
    feedbackSliderLabel.attachToComponent(&mFeedbackSlider, false);
    feedbackSliderLabel.setJustificationType(juce::Justification::centredBottom);
    
    juce::AudioParameterInt* typeParameter = (juce::AudioParameterInt*) params.getUnchecked(5);
    mType.setBounds(150, 260, 100, 30);
    mType.addItem("Chorus", 1);
    mType.addItem("Flanger", 2);
    addAndMakeVisible(mType);
    mType.onChange = [this, typeParameter] {
        typeParameter->beginChangeGesture();
        *typeParameter = mType.getSelectedItemIndex();
        typeParameter->endChangeGesture();
    };
    mType.setSelectedItemIndex(*typeParameter);
}

ModDelayAudioProcessorEditor::~ModDelayAudioProcessorEditor()
{
}

//==============================================================================
void ModDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void ModDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
