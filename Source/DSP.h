/*
  ==============================================================================

    jucer_AudioPluginDSPTemplate.h
    Created: 11 Apr 2024 5:14:40pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "XDDSP/XDDSP.h"

namespace XDDSP
{










static constexpr unsigned int PreAnalysisBufferSizeBits = 9;
static constexpr PowerSize PreAnalysisBufferSize {PreAnalysisBufferSizeBits};
static constexpr int ChannelCount = 2;
static constexpr int MaxVoices = 16;
typedef std::array<CircularBuffer<PreAnalysisBufferSizeBits>, ChannelCount> InputBuffer;










/*
 template <typename SignalIn>
 class PhaseAdamVoice : public Component<PhaseAdamVoice<SignalIn>>
 {
  // Private data members here
 public:
  static constexpr int Count = SignalIn::Count;
  
  // Specify your inputs as public members here
  SignalIn signalIn;
  
  // Specify your outputs like this
  Output<Count> signalOut;
  
  // Include a definition for each input in the constructor
  PhaseAdamVoice(Parameters &p, SignalIn _signalIn) :
  signalIn(_signalIn),
  signalOut(p)
  {}
  
  // This function is responsible for clearing the output buffers to a default state when
  // the component is disabled.
  void reset()
  {
   signalOut.reset();
  }
  
  // startProcess prepares the component for processing one block and returns the step
  // size. By default, it returns the entire sampleCount as one big step.
 // int startProcess(int startPoint, int sampleCount)
 // { return std::min(sampleCount, StepSize); }

  // stepProcess is called repeatedly with the start point incremented by step size
  void stepProcess(int startPoint, int sampleCount)
  {
   for (int c = 0; c < Count; ++c)
   {
    for (int i = startPoint, s = sampleCount; s--; ++i)
    {
     // DSP work done here
    }
   }
  }
  
  // finishProcess is called after the block has been processed
 // void finishProcess()
 // {}
 };

  
  
  
  
  
  
  
  
  

 */









class PhaseAdamParameters : public PolySynthParameters
{
public:
 int sampleRootNote {60};
 float sampleStartPoint {0.f};
 float sampleEndPoint {0.f};
 
 juce::AudioBuffer<SampleType> *sampleBuffer {nullptr};
 float loadedSampleRate {44100};
 
 float samplePlayLength() const { return sampleEndPoint - sampleStartPoint; }
};










template <int ChannelCount>
class PhaseAdamVoice : public Component<PhaseAdamVoice<ChannelCount>>
{
private:
 PhaseAdamParameters &dspParam;
 std::array<SampleType, ChannelCount*PreAnalysisBufferSize.size()> preRenderBuffer;
 float outputPhase {1};
 
public:
 static constexpr int Count = ChannelCount;
 
 InputBuffer *inputBuffers {nullptr};
 
 RampTo<> noteIn;
 
 ControlConstant<> velocityIn;
 
 ADSRGenerator<PConnector<>, ControlConstant<>, ControlConstant<>, PConnector<>> adsr;
 
 Counter<ControlConstant<>, ControlConstant<>, ControlConstant<>> playPositionCntr;
 
#define PLAYPOSITIONFANOUT_IN playPositionCntr.counterOut
 ChannelPicker<decltype(PLAYPOSITIONFANOUT_IN), 0, Count> playPositionFanOut;
 
#define PLAYHEAD_IN playPositionFanOut
 SamplePlaybackHead<decltype(playPositionFanOut), SampleType, Count> playhead;
 
#define AMP_IN playhead
 SimpleGain<Connector<decltype(AMP_IN)>, Product<2>> amp;
 
#define PHASEFLIP_IN amp.signalOut
 SignalModifier<decltype(PHASEFLIP_IN), Count> phaseFlip;

#define SIGNALOUT_IN phaseFlip
 Connector<decltype(SIGNALOUT_IN)> signalOut;
 
 PhaseAdamVoice(Parameters &p) :
 dspParam(dynamic_cast<PhaseAdamParameters&>(p)),
 noteIn(p),
 adsr(p, {}, {0}, {1}, {}),
 playPositionCntr(p, {0}, {0}, {1}),
 playPositionFanOut(PLAYPOSITIONFANOUT_IN),
 playhead(PLAYHEAD_IN),
 amp(p, {AMP_IN}, Product<2>(adsr.envOut, velocityIn)),
 phaseFlip(PHASEFLIP_IN),
 signalOut({SIGNALOUT_IN})
 {
  phaseFlip.func = [&](SampleType x) { return x*outputPhase; };
 }
 
 void reset()
 {
  noteIn.reset();
  adsr.reset();
  playPositionCntr.reset();
  amp.reset();
 }
 
 void stepProcess(int startPoint, int sampleCount)
 {
  noteIn.process(startPoint, sampleCount);
  adsr.process(startPoint, sampleCount);
  playPositionCntr.process(startPoint, sampleCount);
  amp.process(startPoint, sampleCount);
 }
 
 void triggerProcess(int triggerPoint)
 {
  adsr.releaseEnvelope();
 }
 
 void resetForNotePlayback(float playLength)
 {
  noteIn.setRamp(0, 0, noteIn.getControl());
  adsr.reset();
  adsr.triggerEnvelope();
  playPositionCntr.setCounter(dspParam.sampleStartPoint);

  this->setNextTrigger(playLength - adsr.releaseTimeSamples(0, 0));
 }
 
 void fillPreRenderBuffer()
 {
  size_t i = 0;
  this->process(0, dspParam.bufferSize());
  while (i < PreAnalysisBufferSize.size())
  {
   int transfer = XDDSP::fastMin(dspParam.bufferSize(), i - PreAnalysisBufferSize.size());
   std::array<float*, 2> p = {preRenderBuffer.data() + i,
    preRenderBuffer.data() + i + PreAnalysisBufferSize.size()};
   signalOut.fastTransfer(p, transfer);
   i += transfer;
   this->process(0, dspParam.bufferSize());
  }
 }
 
 SampleType calculateCovariance()
 {
  if (inputBuffers)
  {
   decltype(*inputBuffers) &b = *inputBuffers;
   
   SampleType accum = 0;
   for (int i = 0; i < PreAnalysisBufferSize.size(); ++i)
   {
    for (int c = 0; c < Count; ++c)
    {
     SampleType c1 = b[c].tapOut(PreAnalysisBufferSize.size() - 1 - i);
     SampleType c2 = preRenderBuffer[i + c*PreAnalysisBufferSize.size()];
     accum += c1*c2;
    }
   }
   return accum;
  }
  else return 1;
 }
 
 void noteOn()
 {
  float noteDelta = (dspParam.sampleRootNote != -1) ?
                    (noteIn.getControl() - dspParam.sampleRootNote) :
                    0;
  float resampleRate = dspParam.loadedSampleRate*dspParam.sampleInterval()*semitoneRatio(noteDelta);
  
  playPositionCntr.speedIn.setControl(resampleRate);
  playPositionCntr.startIn.setControl(dspParam.sampleStartPoint);
  playPositionCntr.endIn.setControl(dspParam.sampleEndPoint);
  
  float playLength = dspParam.samplePlayLength() / resampleRate;

  resetForNotePlayback(playLength);
  fillPreRenderBuffer();
  outputPhase = signum(calculateCovariance());
  if (outputPhase == 0) outputPhase = 1;
  resetForNotePlayback(playLength);
 }
 
 void noteOff()
 {
  adsr.releaseEnvelope();
 }
 
 void noteStop()
 {
  reset();
 }
 
 bool isActive()
 { return adsr.envelopeActive(); }
};










typedef PhaseAdamVoice<ChannelCount> Voice;
 
 
 
 
 
 
 
 
 

class PluginDSP : public Component<PluginDSP>
{
 PhaseAdamParameters &dspParam;
public:
 static constexpr int Count = ChannelCount;
 
 AudioPropertiesInput<AudioPropertiesInputMode::SamplesPerSecond> envelopeTime;
 ControlConstant<> masterGain {dB2Linear(0)};
 
 PluginInput<Count> signalIn;
 InputBuffer inputBuffers;

 SummingArray<Voice, MaxVoices> voices;
 MIDIPoly<Voice, MaxVoices, 1> poly;
 
#define MASTERAMP_IN voices.sumOut
#define MASTERAMP_GAIN masterGain
 SimpleGain<Connector<decltype(MASTERAMP_IN)>, Connector<decltype(MASTERAMP_GAIN)>> masterAmp;

#undef SIGNALOUT_IN
#define SIGNALOUT_IN masterAmp.signalOut
 Connector<decltype(SIGNALOUT_IN)> signalOut;
 
 PluginDSP(Parameters &p) :
 dspParam(dynamic_cast<PhaseAdamParameters&>(p)),
 envelopeTime(p, 0.005),
 voices(p),
 poly(p, voices),
 masterAmp(p, {MASTERAMP_IN}, {MASTERAMP_GAIN}),
 signalOut({SIGNALOUT_IN})
 {
  for (auto& voice: voices)
  {
   voice.adsr.attackTimeSamples.connect(envelopeTime);
   voice.adsr.releaseTimeSamples.connect(envelopeTime);
   voice.inputBuffers = &inputBuffers;
  }
 }
 
 void refreshSampleBuffer()
 {
  for (auto& voice: voices)
  {
   for (int c = 0; c < ChannelCount; ++c)
   {
    voice.playhead.connectChannel(c, dspParam.sampleBuffer->getWritePointer(c));
   }
   voice.playhead.setLength(dspParam.sampleBuffer->getNumSamples());
  }
 }
 
 // This function is responsible for clearing the output buffers to a default state when
 // the component is disabled.
 void reset()
 {
  for (auto &buffer: inputBuffers) buffer.reset(0);
  voices.reset();
  poly.reset();
  masterAmp.reset();
 }
 
 // stepProcess is called repeatedly with the start point incremented by step size
 void stepProcess(int startPoint, int sampleCount)
 {
  for (int c = 0; c < ChannelCount; ++c)
  {
   int e = startPoint + sampleCount;
   for (int i = startPoint; i < e; ++i)
   {
    inputBuffers[c].tapIn(signalIn(c, i));
   }
  }
  poly.process(startPoint, sampleCount);
  masterAmp.process(startPoint, sampleCount);
 }
};

}
