/*
  ==============================================================================

    jucer_XDLookAndFeelTemplate.h
    Created: 11 Apr 2024 5:09:59pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



static constexpr float knobStartAngle = 230./180.*M_PI;
static constexpr float knobEndAngle = 490./180.*M_PI;










class XDLookAndFeel : public juce::LookAndFeel_V4
{/*
 virtual juce::Font getLabelFont(juce::Label &l) override
 {
  return juce::Font(juce::FontOptions("Arial", 10., juce::Font::plain));
 }*/
 
 virtual void drawLinearSlider (juce::Graphics& g,
                                int x,
                                int y,
                                int width,
                                int height,
                                float sliderPos,
                                float minSliderPos,
                                float maxSliderPos,
                                const juce::Slider::SliderStyle style,
                                juce::Slider& slider) override
 {
  auto thickness = 2.5;
  float fx = static_cast<float>(x);
  float fy = static_cast<float>(y);
  float fWidth = static_cast<float>(width);
  float fHeight = static_cast<float>(height);

  juce::Point<float> startPoint(slider.isHorizontal() ?
                                fx :
                                fx + 0.5*fWidth,
                                slider.isHorizontal() ?
                                fy + 0.5*fHeight :
                                fHeight + fy);
  juce::Point<float> endPoint(slider.isHorizontal() ?
                              fWidth + fx : startPoint.x,
                              slider.isHorizontal() ?
                              startPoint.y : fy);
  juce::Point<float> perp(slider.isHorizontal() ? 0. : 0.5*fWidth,
                          slider.isHorizontal() ? 0.5*fHeight : 0.);
  juce::Point<float> par(perp.y, perp.x);
  
  {
   juce::Path runner;
   juce::PathStrokeType s(thickness);
   runner.startNewSubPath(startPoint);
   runner.lineTo(endPoint);
   g.setColour(juce::Colours::white.withBrightness(0.4));
   g.strokePath(runner, s);
  }
  
  if (slider.isThreeValue() || !slider.isTwoValue())
  {
   juce::Point<float> nub (slider.isHorizontal() ? sliderPos : startPoint.x,
                           slider.isHorizontal() ? startPoint.y : sliderPos);
   
   juce::Path p;
   juce::PathStrokeType s(thickness);

   p.startNewSubPath(nub - perp);
   p.lineTo(nub + perp);
   g.setColour(juce::Colours::white);
   g.strokePath(p, s);
  }

  if (slider.isTwoValue() || slider.isThreeValue())
  {
   {
    juce::Point<float> nub (slider.isHorizontal() ? maxSliderPos : startPoint.x,
                            slider.isHorizontal() ? startPoint.y : maxSliderPos);
    
    juce::Path p;
    juce::PathStrokeType s(thickness);

    p.startNewSubPath(nub - perp + 0.5f*par);
    p.lineTo(nub - perp);
    p.lineTo(nub + perp);
    g.setColour(juce::Colours::white);
    g.strokePath(p, s);
   }

   {
    juce::Point<float> nub (slider.isHorizontal() ? minSliderPos : startPoint.x,
                            slider.isHorizontal() ? startPoint.y : minSliderPos);
    
    juce::Path p;
    juce::PathStrokeType s(thickness);

    p.startNewSubPath(nub - perp);
    p.lineTo(nub + perp);
    p.lineTo(nub + perp - 0.5f*par);
    g.setColour(juce::Colours::white);
    g.strokePath(p, s);
   }
  }
 }
 
 virtual void drawRotarySlider (juce::Graphics& g,
                                int x,
                                int y,
                                int width,
                                int height,
                                float sliderPos,
                                const float rotaryStartAngle,
                                const float rotaryEndAngle,
                                juce::Slider&)
 override
 {
  auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
  auto thickness = 2.5;
  auto centreX = (float) x + (float) width  * 0.5f;
  auto centreY = (float) y + (float) height * 0.5f;
//  auto rx = centreX - radius;
//  auto ry = centreY - radius;
//  auto rw = radius * 2.0f;
  auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  
  {
   juce::Path p;
   juce::PathStrokeType s(thickness);
   p.addCentredArc(centreX, centreY, radius - 0.5*thickness, radius - 0.5*thickness, 0., rotaryStartAngle, rotaryEndAngle, true);
   g.setColour(juce::Colours::white.withBrightness(0.4));
   g.strokePath(p, s);
  }
  
  {
   juce::Path p;
   auto pointerLength = 6.f;
   auto pointerThickness = 3.0f;
   p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
   p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
   g.setColour(juce::Colours::white);
   g.fillPath(p);
  }
 }
 
 virtual void drawComboBox (juce::Graphics& g,
                            int width,
                            int height,
                            const bool isButtonDown,
                            int buttonX, int buttonY,
                            int buttonW, int buttonH,
                            juce::ComboBox& box)
 override
 {
//  g.fillAll (juce::Colours::black);
  juce::Path p;
  p.addRectangle(1., 1., box.getWidth() - 2., box.getHeight() - 2.);
  g.setColour(juce::Colours::white.withBrightness(0.4));
  g.fillPath(p);
 }
 
 virtual void positionComboBoxText (juce::ComboBox &box,
                                    juce::Label &label)
 override
 {
  label.setBounds (1, 1, box.getWidth() - 2, box.getHeight() - 2);
  label.setFont (getComboBoxFont (box));
 }
 
 virtual void drawToggleButton (juce::Graphics &g,
                                juce::ToggleButton &b,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown)
 override
 {
  juce::Path p;
  juce::Colour activeColour = juce::Colours::yellow;
  if (b.getName().startsWith("solo"))
  {
   activeColour = juce::Colours::orange;
  }
  if (b.getName().startsWith("mute"))
  {
   activeColour = juce::Colours::lightblue;
  }
  
  p.addRectangle(1., 1., b.getWidth() - 2., b.getHeight() - 2.);
  g.setColour(b.getToggleState() ? activeColour : juce::Colours::white.withBrightness(0.4));
  g.fillPath(p);
//  g.fillAll();

  g.setColour (b.getToggleState() ? juce::Colours::black : juce::Colours::white);
  
  g.setFont (juce::jmin (15.0f, b.getHeight() * 0.6f));
  
  if (! b.isEnabled())
   g.setOpacity (0.5f);
  
  const int textX = 1;
  
  g.drawFittedText (b.getButtonText(),
                    textX, 4,
                    b.getWidth() - textX - 2, b.getHeight() - 8,
                    juce::Justification::centredLeft, 10); }
};



class BoxFillSlider : public juce::Slider, public juce::Label::Listener, public juce::Timer
{
 bool settingValue {false};
 juce::Label editor;
 
 void init()
 {
  addAndMakeVisible(editor);
  editor.addListener(this);
  editor.addMouseListener(this, true);
  editor.setLookAndFeel(&getLookAndFeel());
  editor.setFont(getLookAndFeel().getLabelFont(editor));
  valueChanged();
 }
 
public:
 BoxFillSlider()
 {
  init();
 }
 
 BoxFillSlider(const juce::String &name) :
 juce::Slider(name)
 {
  init();
 }
 
 void valueChanged() override
 {
  if (!settingValue)
  {
   juce::ScopedValueSetter<bool>(settingValue, true);
   
   juce::String replace(getTextFromValue(getValue()));
   editor.setText(replace, juce::NotificationType::dontSendNotification);
  }
 }
 
 void labelTextChanged(juce::Label *e) override
 {
  if (!settingValue)
  {
   juce::ScopedValueSetter<bool>(settingValue, true);
   double newValue = editor.getText().getDoubleValue();
   
   auto range = getRange();
   float clampedValue = std::max(range.getStart(),
                                 std::min(range.getEnd(),
                                          newValue));
   if (clampedValue != newValue)
   {
    newValue = clampedValue;
   }
   juce::String replace(newValue, getNumDecimalPlacesToDisplay());
   editor.setText(replace, juce::NotificationType::dontSendNotification);
   if (getValue() != newValue) setValue(newValue, juce::NotificationType::sendNotification);
  }
 }

 void paint(juce::Graphics &g) override
 {
  const float value = getValue();
  const auto range = getNormalisableRange();
  const float nValue = range.convertTo0to1(value);
  const float sizeOfFill = nValue * getWidth();
  
  g.setColour(juce::Colours::black);
  g.fillAll();
  
  g.setColour(juce::Colours::yellow.withBrightness(0.5));
  g.fillRect(0.f, 0.f, sizeOfFill, static_cast<float>(getHeight()));
  
  g.setColour(juce::Colours::white.withBrightness(0.5));
  g.drawRect(0, 0, getWidth(), getHeight());
 }
 
 void resized() override
 {
  editor.setBounds(getLocalBounds());
 }
 
 void mouseUp(const juce::MouseEvent &e) override
 {
  if (e.mouseWasClicked()) startTimer(juce::MouseEvent::getDoubleClickTimeout());
  juce::Slider::mouseUp(e);
 }
 
 void mouseDoubleClick(const juce::MouseEvent &e) override
 {
  stopTimer();
  juce::Slider::mouseDoubleClick(e);
 }
 
 void timerCallback() override
 {
  stopTimer();
  editor.showEditor();
 }
};
