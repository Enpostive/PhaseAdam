/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PhaseAdamAudioProcessorEditor::PhaseAdamAudioProcessorEditor (PhaseAdamAudioProcessor& p, juce::AudioProcessorValueTreeState &vts)
: AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
 std::cout << "Initialising plugin display" << std::endl;
 
 std::unique_lock lock(audioProcessor.mtx);
 juce::Slider::RotaryParameters rotaryParam;
 rotaryParam.startAngleRadians = knobStartAngle;
 rotaryParam.endAngleRadians = knobEndAngle;
 rotaryParam.stopAtEnd = true;
 
 lookAndFeel = std::make_unique<XDLookAndFeel>();
 setLookAndFeel(lookAndFeel.get());
 
 addAndMakeVisible(controls);
 controls.setTopLeftPosition(0, 0);
 
 waveformDataSource.connect(audioProcessor.sampleBuffer);
 controls.getWaveformDisplay().connectDataSource(waveformDataSource);
 controls.getWaveformDisplay().setEnableMouseZoomScroll(true);
 
 for (const ParameterSpec &pS : p.paramSpecs)
 {
  juce::Component *component = controls.findChildWithID(pS.paramID.getParamID());
  
  if (component)
  {
   switch (pS.type)
   {
    default:
     break;
     
    case ParameterSpec::Type::Slider:
    {
     juce::Slider *slider = dynamic_cast<juce::Slider*>(component);
     std::unique_ptr<SliderAttachment> attachment = std::make_unique<SliderAttachment>(valueTreeState, pS.paramID.getParamID(), *slider);
     
     sliderAttach.push_back(std::move(attachment));
    }
     break;
     
    case ParameterSpec::Type::Combo:
    {
     juce::ComboBox *combo = dynamic_cast<juce::ComboBox*>(component);
     std::unique_ptr<ComboBoxAttachment> attachment = std::make_unique<ComboBoxAttachment>(valueTreeState, pS.paramID.getParamID(), *combo);
     comboAttach.push_back(std::move(attachment));
    }
     break;
     
    case ParameterSpec::Type::Button:
    {
     juce::Button *button = dynamic_cast<juce::Button*>(component);
     std::unique_ptr<ButtonAttachment> attachment = std::make_unique<ButtonAttachment>(valueTreeState, pS.paramID.getParamID(), *button);
     buttonAttach.push_back(std::move(attachment));
    }
     break;
   }
  }
 }
 
 controls.getWaveformDisplay().onClick = [&](float s) { std::cout << "User clicked on " << s << std::endl; };
  
 setSize (controls.getWidth(), controls.getHeight());
 
 audioProcessor.addChangeListener(this);
 
 waveformDataSource.sendChangeMessage();
 
 // If you have animated components, uncomment this line
 // startTimerHz(30);
}

PhaseAdamAudioProcessorEditor::~PhaseAdamAudioProcessorEditor()
{
 setLookAndFeel(nullptr);
}

bool PhaseAdamAudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray &files)
{
 return true;
}

void PhaseAdamAudioProcessorEditor::filesDropped (const juce::StringArray &files, int x, int y)
{
 audioProcessor.loadSampleFile(files[0]);
}

void PhaseAdamAudioProcessorEditor::timerCallback()
{
 {
  std::unique_lock lock(audioProcessor.mtx);
  // Do all the interfacing with the processor here
 }
 
 // Here update the plugin UI
}

//==============================================================================
void PhaseAdamAudioProcessorEditor::paint (juce::Graphics& g)
{
 // (Our component is opaque, so we must completely fill the background with a solid colour)
 g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PhaseAdamAudioProcessorEditor::resized()
{
}

void PhaseAdamAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
 waveformDataSource.sendChangeMessage();
}










AudioBufferWaveformSource::AudioBufferWaveformSource() {}
AudioBufferWaveformSource::~AudioBufferWaveformSource() {}

void AudioBufferWaveformSource::connect(JUCEAudioBuffer &newBuffer)
{
 buffer = &newBuffer;
 sendChangeMessage();
}

void AudioBufferWaveformSource::disconnect()
{
 buffer = nullptr;
 sendChangeMessage();
}

size_t AudioBufferWaveformSource::getSampleCount() const
{
 return buffer ? buffer->getNumSamples() : 0;
}

void AudioBufferWaveformSource::readSamples(size_t startIndex,
                                            size_t count,
                                            std::vector<float> &samples) const
{
 if (buffer)
 {
  samples.assign(count, 0.f);
  size_t totalFetched = std::min(count, (static_cast<size_t>(buffer->getNumSamples()) - startIndex));
  
  for (int i = 0; i < buffer->getNumChannels(); ++i)
  {
   auto channel = buffer->getReadPointer(i, static_cast<int>(startIndex));
   for (size_t j = 0; j < totalFetched; ++j)
   {
    samples[j] += channel[j];
   }
  }
  
  float divisor = 1./static_cast<float>(buffer->getNumChannels());
  for (size_t i = 0; i < totalFetched; ++i)
  {
   samples[i] *= divisor;
  }
 }
}
