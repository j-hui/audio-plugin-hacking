/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveshaperAudioProcessorEditor::WaveshaperAudioProcessorEditor (WaveshaperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 150);
    in_gain_slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    in_gain_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    in_gain_slider.setBounds(0, 5, 140, 140);
    addAndMakeVisible(in_gain_slider);
    
    out_gain_slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    out_gain_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    out_gain_slider.setBounds(260, 5, 140, 140);
    addAndMakeVisible(out_gain_slider);
    
    shape_combobox.setEditableText(false);
    shape_combobox.setBounds(140, 40, 120, 30);
    const juce::StringArray* choices = &((juce::AudioParameterChoice*)audioProcessor.params.getParameter("SHAPE"))->choices;
    for (int i = 0; i < choices->size(); i++) {
        shape_combobox.addItem(choices->getReference(i), i + 1);
    }
    addAndMakeVisible(shape_combobox);
    
    in_gain_slider_attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.params, "IN_GAIN", in_gain_slider));
    out_gain_slider_attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.params, "OUT_GAIN", out_gain_slider));
    shape_combobox_attachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.params,
                                                                                               "SHAPE", shape_combobox));
    
}

WaveshaperAudioProcessorEditor::~WaveshaperAudioProcessorEditor()
{
}

//==============================================================================
void WaveshaperAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
}

void WaveshaperAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
