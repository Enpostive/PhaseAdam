/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"










//==============================================================================
const juce::Identifier PhaseAdamAudioProcessor::PresetDataIdentifier {"PresetData"};
const juce::Identifier PhaseAdamAudioProcessor::SampleFileNameIdentifier {"Filename"};

const std::array<char[3], 12> NoteNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};









//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout PhaseAdamAudioProcessor::createParameterLayout()
{
 juce::AudioProcessorValueTreeState::ParameterLayout layout;
 
 /*
  
   //  =================== PUSH BUTTON ======================
  
   {
    juce::ParameterID id("ID", 1);
    juce::StringArray items {"Off", "On"};
  
    juce::AudioParameterChoice *param = new juce::AudioParameterChoice(id, "LABEL", items, 0);
    layout.add(std::unique_ptr<juce::AudioParameterChoice>(param));
    
    PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
    {
     std::unique_lock lock(mtx);
     bool turnedOn = (newValue > 0.0001);
     // Update code
    });
    
    listeners.
    push_back(std::unique_ptr<PluginParameterListener>(listener));
    param->addListener(listener);
   
    ParameterSpec pSpec =
    {
     id,
     ParameterSpec::Type::Button
    };
  
    paramSpecs.push_back(pSpec);
   }

  
  
  
  
  
  

  
  // =============== COMBO BOX PARAM ==================
  
  {
   juce::ParameterID id("ID", 1);
   juce::StringArray items {"1x", "2x", "4x"};
   
   juce::AudioParameterChoice *param = new juce::AudioParameterChoice(id, "LABEL", items, 0);
   layout.add(std::unique_ptr<juce::AudioParameterChoice>(param));
   
   PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
   {
    std::unique_lock lock(mtx);
    // Update code
   });
   
   listeners.
   push_back(std::unique_ptr<PluginParameterListener>(listener));
   param->addListener(listener);
   
   ParameterSpec pSpec =
   {
    id,
    ParameterSpec::Type::Combo
   };

   paramSpecs.push_back(pSpec);
  }
 */




 /*
  // =============== SLIDER PARAM ==================
  
  {
   juce::ParameterID id("ID", 1);
   auto attributes = juce::AudioParameterFloatAttributes().withLabel("UNITS");
   juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "LABEL", juce::NormalisableRange<float>(MIN, MAX, STEP, EXP), DEFAULT, attributes);
   layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
  
   PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
   {
    std::unique_lock lock(mtx);
    // Update code
   });
    
   listeners.
   push_back(std::unique_ptr<PluginParameterListener>(listener));
   param->addListener(listener);

   paramSpecs.push_back(
   {
    id,
    ParameterSpec::Type::Slider
   });
  }

  */

 // =============== MASTER GAIN ==================
 
 {
  juce::ParameterID id("mastergain", 1);
  auto attributes = juce::AudioParameterFloatAttributes().withLabel("dB");
  juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "Master Gain", juce::NormalisableRange<float>(-30, 20, 0.1), 0, attributes);
  layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
 
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   std::unique_lock lock(mtx);
   dsp.masterGain.setControl(XDDSP::dB2Linear(newValue));
  });
   
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);

  paramSpecs.push_back(
  {
   id,
   ParameterSpec::Type::Slider
  });
 }










 // =============== ROOT NOTE ==================
 
 {
  auto str2val = [&](juce::String str) -> float
  {
   // If the string just contains a number, return that.
   if (str.containsOnly("0123456789"))
   {
    return str.getIntValue();
   }
   
   // If the first part of the string matches one of the NoteNames, remember that note
   int note = -1;
   for (int i = 0; i < NoteNames.size(); ++i)
   {
    if (str.startsWithIgnoreCase(NoteNames[i]))
    {
     note = i;
     // Don't break, otherwise "D#" will match "D" and incorrectly stop there
    }
   }
   
   // otherwise, return a default
   if (note == -1) return 60;
   
   // If the part after the note name is a number between -2 and 7, use that as the octave and return the note
   if (str.substring(int(strlen(NoteNames[note]))).containsOnly("-0123456789"))
   {
    int octave = str.getTrailingIntValue() + 2;
    return 12*octave + note;
   }
   
   // otherwise, return the note in octave 3
   return 48 + note;
  };
  
  auto val2str = [&](float val, int decimalPlaces) -> juce::String
  {
   int ival = int(val);
   int octave = (ival / 12) - 2;
   int note = ival % 12;
   return juce::String(NoteNames[note]) + juce::String(octave);
  };
  
  juce::ParameterID id("rootnote", 1);
  auto attributes = juce::AudioParameterFloatAttributes().withLabel("").withStringFromValueFunction(val2str).withValueFromStringFunction(str2val);
  juce::AudioParameterFloat *param = new juce::AudioParameterFloat(id, "Root Note", juce::NormalisableRange<float>(0, 127, 1), 60, attributes);
  layout.add(std::unique_ptr<juce::AudioParameterFloat>(param));
 
  PluginParameterListener *listener = new PluginParameterListener(param, [&](float newValue)
  {
   std::unique_lock lock(mtx);
   dspParam.sampleRootNote = static_cast<int>(newValue);
  });
   
  listeners.
  push_back(std::unique_ptr<PluginParameterListener>(listener));
  param->addListener(listener);

  paramSpecs.push_back(
  {
   id,
   ParameterSpec::Type::Slider
  });
 }










 return layout;
}










//==============================================================================
PhaseAdamAudioProcessor::PhaseAdamAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
),
#endif
dsp(dspParam),
parameters(*this, nullptr, juce::Identifier("PhaseAdamAudioProcessor"), createParameterLayout())
{
 audioFormatManager.registerBasicFormats();
 
 dspParam.sampleBuffer = &sampleBuffer;
}

PhaseAdamAudioProcessor::~PhaseAdamAudioProcessor()
{
}










//==============================================================================
const juce::String PhaseAdamAudioProcessor::getName() const
{
 return JucePlugin_Name;
}

bool PhaseAdamAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
 return true;
#else
 return false;
#endif
}

bool PhaseAdamAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
 return true;
#else
 return false;
#endif
}

bool PhaseAdamAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
 return true;
#else
 return false;
#endif
}

double PhaseAdamAudioProcessor::getTailLengthSeconds() const
{
 return 0.0;
}

int PhaseAdamAudioProcessor::getNumPrograms()
{
 return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
 // so this should be at least 1, even if you're not really implementing programs.
}

int PhaseAdamAudioProcessor::getCurrentProgram()
{
 return 0;
}

void PhaseAdamAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PhaseAdamAudioProcessor::getProgramName (int index)
{
 return {};
}

void PhaseAdamAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}










//==============================================================================
void PhaseAdamAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
 dspParam.setSampleRate(sampleRate);
 dspParam.setBufferSize(samplesPerBlock);
 
 setLatencySamples(XDDSP::PreAnalysisBufferSize.size());

 for (std::unique_ptr<PluginListener> &p: listeners)
 {
  p->sendInternalUpdate();
 }
}

void PhaseAdamAudioProcessor::releaseResources()
{
 // When playback stops, you can use this as an opportunity to free up any
 // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaseAdamAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PhaseAdamAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
 std::unique_lock lock(mtx);
 juce::ScopedNoDenormals noDenormals;
 processMIDI(midiMessages);
 dsp.signalIn.connectFloats({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
 dsp.process(0, buffer.getNumSamples());
 dsp.signalOut.fastTransfer<float>({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
}

void PhaseAdamAudioProcessor::processMIDI (juce::MidiBuffer &midiMessages)
{
 juce::AudioPlayHead* playhead = getPlayHead();
 if (playhead)
 {
  double tempo;
  double ppq;
  double seconds;
  /*
   juce::AudioPlayHead::CurrentPositionInfo currentPosition;
   playhead->getCurrentPosition(currentPosition);
   
   tempo = currentPosition.bpm;
   ppq = currentPosition.ppqPosition;
   seconds = currentPosition.timeInSeconds;
   //  debugFile << tempo << "BPM, " << ppq << "@" << seconds << "s\n";
   if (!currentPosition.isPlaying) dspParam.clearTransportInformation();
   //  debugFile << (currentPosition.isPlaying ? "Playing" : "Not Playing") << "\n";
   */
  auto optPositionInfo = playhead->getPosition();
  
  juce::AudioPlayHead::PositionInfo posInfo = optPositionInfo.orFallback(juce::AudioPlayHead::PositionInfo());
  
  dspParam.getTransportInformation(tempo, ppq, seconds);
  
  tempo = posInfo.getBpm().orFallback(tempo);
  ppq = posInfo.getPpqPosition().orFallback(ppq);
  seconds = posInfo.getTimeInSeconds().orFallback(seconds);
  
  dspParam.setTransportInformation(tempo, ppq, seconds);
 }
 else
 {
//  debugFile << "No tempo information\n";
  dspParam.clearTransportInformation();
 }
 
 juce::MidiBufferIterator it = midiMessages.begin();
 while (it != midiMessages.end())
 {
  juce::MidiMessage message = (*it).getMessage();
  message.addToTimeStamp(XDDSP::PreAnalysisBufferSize.size());
  
  if (message.isNoteOn())
  {
   dsp.poly.scheduleNoteEvent(message.getNoteNumber(),
                                  message.getVelocity(),
                                  message.getTimeStamp());
  }
  else if (message.isNoteOff())
  {
   dsp.poly.scheduleNoteEvent(message.getNoteNumber(),
                                  0,
                                  message.getTimeStamp());
  }
/*  else if (message.isControllerOfType(1))
  {
   double value = static_cast<XDDSP::sample_T>(message.getControllerValue())/127.;
   synth.modWheel().addEvent(0, value, message.getTimeStamp());
  }
  else if (message.isPitchWheel())
  {
   dsp.poly.schedulePitchBendEvent(message.getPitchWheelValue() - 8192, message.getTimeStamp());
  }*/
  else if (message.isAllNotesOff())
  {
   dsp.poly.scheduleAllNotesOff(message.getTimeStamp());
  }
  else if (message.isAllSoundOff())
  {
   dsp.poly.scheduleAllSoundOff(message.getTimeStamp());
  }
  
  ++it;
 }
 midiMessages.clear();
}










//==============================================================================
bool PhaseAdamAudioProcessor::hasEditor() const
{
 return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PhaseAdamAudioProcessor::createEditor()
{
 return new PhaseAdamAudioProcessorEditor (*this, parameters);
}










//==============================================================================
void PhaseAdamAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
 auto p = parameters.copyState();
 juce::ValueTree strings = p.getOrCreateChildWithName(PresetDataIdentifier, nullptr);
 strings.setProperty(SampleFileNameIdentifier, sampleFile.getFullPathName(), nullptr);
 strings.removeAllChildren(nullptr);
 p.appendChild(strings, nullptr);
 std::unique_ptr<juce::XmlElement> xml (p.createXml());
 copyXmlToBinary (*xml, destData);
}

void PhaseAdamAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
 std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
 if (xmlState.get() != nullptr)
 {
  if (xmlState->hasTagName (parameters.state.getType()))
  {
   juce::ValueTree p = juce::ValueTree::fromXml(*xmlState);
   juce::ValueTree strings = p.getChildWithName(PresetDataIdentifier);
   if (strings.isValid())
   {
    loadSampleFile(strings.getPropertyAsValue(SampleFileNameIdentifier, nullptr).toString());
   }
   parameters.replaceState (p);
  }
 }
}










//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
 return new PhaseAdamAudioProcessor();
}










//==============================================================================
void PhaseAdamAudioProcessor::loadSampleFile(const juce::String &path)
{
 // Create a pointer for the reader
 std::unique_ptr<juce::AudioFormatReader> reader;
 reader.reset(audioFormatManager.createReaderFor(juce::File(path)));
 
 if (!reader)
 {
  // Can't load it! Tell the user
  juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                          "Oops!",
                                         (juce::String("Can't load file ") +
                                          path));
 }
 else
 {
  std::unique_lock lock(mtx);
  
  sampleFile = juce::File(path);

  // Create the buffer and pad with 4 extra samples on each end so that interpolation works correctly
  sampleBuffer.setSize(2, static_cast<int>(reader->lengthInSamples + 8));
  sampleBuffer.clear();
  reader->read(&sampleBuffer, 4, static_cast<int>(reader->lengthInSamples), 0, true, true);
  loadedSampleRate = reader->sampleRate;
  dspParam.loadedSampleRate = loadedSampleRate;
  dspParam.sampleStartPoint = 4;
  dspParam.sampleEndPoint = sampleBuffer.getNumSamples() - 4;
  dsp.refreshSampleBuffer();
  sendChangeMessage();
 }
}
