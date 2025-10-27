/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Controls.h"










//==============================================================================
class AudioBufferWaveformSource : public WaveformDataSourceBase
{
 typedef juce::AudioBuffer<XDDSP::SampleType> JUCEAudioBuffer;
 JUCEAudioBuffer *buffer {nullptr};
 
 public:
 AudioBufferWaveformSource();
 virtual ~AudioBufferWaveformSource();
 
 void connect(JUCEAudioBuffer &newBuffer);
 void disconnect();
 virtual size_t getSampleCount() const override;
 virtual void readSamples(size_t startIndex, size_t count, std::vector<float> &samples) const override;
};










//==============================================================================
/**
 */
class PhaseAdamAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer, public juce::FileDragAndDropTarget, public juce::ChangeListener
{
public:
 PhaseAdamAudioProcessorEditor (PhaseAdamAudioProcessor&, juce::AudioProcessorValueTreeState &);
 ~PhaseAdamAudioProcessorEditor() override;
 
 void paint (juce::Graphics&) override;
 void resized() override;
 
 void timerCallback() override;
 
 bool isInterestedInFileDrag (const juce::StringArray &files) override;
 void filesDropped (const juce::StringArray &files, int x, int y) override;
 
 virtual void changeListenerCallback(juce::ChangeBroadcaster *source) override;

 private:
 typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
 typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
 typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

 PhaseAdamAudioProcessor& audioProcessor;
 
 juce::AudioProcessorValueTreeState& valueTreeState;
 std::unique_ptr<XDLookAndFeel> lookAndFeel;
 Controls controls;
 AudioBufferWaveformSource waveformDataSource;

 std::vector<std::unique_ptr<SliderAttachment>> sliderAttach;
 std::vector<std::unique_ptr<ButtonAttachment>> buttonAttach;
 std::vector<std::unique_ptr<ComboBoxAttachment>> comboAttach;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseAdamAudioProcessorEditor)
};
