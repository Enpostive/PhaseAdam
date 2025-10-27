/*
  ==============================================================================

    jucer_AudioPluginParametersTemplate.h
    Created: 11 Apr 2024 5:01:01pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>
#include "LookAndFeel.h"

struct ParameterSpec
{
 enum class Type
 {
  Slider,
  Combo,
  Button
 };
 
 juce::ParameterID paramID {};
 Type type {Type::Slider};
};

class PluginListener : public juce::AudioProcessorParameter::Listener
{
protected:
 typedef std::function<void (float)> ListenerFunction;
 ListenerFunction lambda;

public:
 PluginListener(ListenerFunction l):
 lambda(l)
 {}
 
 virtual void sendInternalUpdate()
 {}
 
 virtual ~PluginListener() {}
};

class PluginParameterListener : public PluginListener
{
 juce::AudioProcessorParameter *param;
 juce::NormalisableRange<float> const *paramRange;

public:
 template <typename P>
 PluginParameterListener(P *p, ListenerFunction l):
 PluginListener(l)
 {
  param = p;
  paramRange = &(p->getNormalisableRange());
  p->addListener(this);
 }
 
 virtual ~PluginParameterListener()
 {}
 
 void sendInternalUpdate() override
 {
  lambda(paramRange->convertFrom0to1(param->getValue()));
 }
 
 virtual void parameterValueChanged (int parameterIndex, float newValue) override
 {
  lambda(paramRange->convertFrom0to1(newValue));
 }
  
 virtual void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
 {}
};

class PluginGestureListener : public PluginListener
{
 typedef std::function<void (float)> ListenerFunction;
 juce::AudioProcessorParameter *param;
 juce::NormalisableRange<float> const *paramRange;
 float lastUpdate {0};

public:
 template <typename P>
 PluginGestureListener(P *p, ListenerFunction l):
 PluginListener(l)
 {
  param = p;
  paramRange = &(p->getNormalisableRange());
  p->addListener(this);
  lastUpdate = param->getValue();
 }
 
 virtual ~PluginGestureListener()
 {}
 
 void sendInternalUpdate() override
 {
  lastUpdate = param->getValue();
  lambda(paramRange->convertFrom0to1(param->getValue()));
 }
 
 virtual void parameterValueChanged (int parameterIndex, float newValue) override
 {
  lastUpdate = newValue;
 }
  
 virtual void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
 {
  if (!gestureIsStarting) lambda(paramRange->convertFrom0to1(lastUpdate));
 }
};
