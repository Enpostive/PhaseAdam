/*
 ==============================================================================
 
 WaveformVisualiser.cpp
 Created: 22 Oct 2025 9:56:02pm
 Author:  Adam Jackson
 
 ==============================================================================
 */

#include <JuceHeader.h>
#include "WaveformVisualiser.h"










//==============================================================================
WaveformVisualiser::WaveformVisualiser()
{
 // In your constructor, you should add any child components, and
 // initialise any special settings that your component needs.
 
}

WaveformVisualiser::~WaveformVisualiser()
{
}










//==============================================================================
void WaveformVisualiser::connectDataSource(WaveformDataSourceBase &newDataSource)
{
 dataSource = &newDataSource;
 newDataSource.addChangeListener(this);
 triggerAsyncUpdate();
}

void WaveformVisualiser::disconnectDataSource()
{
 if (dataSource)
 {
  dataSource->removeChangeListener(this);
 }
 dataSource = nullptr;
}










//==============================================================================
void WaveformVisualiser::setBackgroundColour(juce::Colour bg)
{
 backgroundColour = bg;
 repaint();
}

void WaveformVisualiser::setWaveformColour(juce::Colour wg)
{
 waveformColour = wg;
 repaint();
}

void WaveformVisualiser::setSelectionColour(juce::Colour sg)
{
 selectionColour = sg;
 repaint();
}

void WaveformVisualiser::setCursorColour(juce::Colour cg)
{
 cursorColour = cg;
 repaint();
}

void WaveformVisualiser::setGuideColour(juce::Colour gg)
{
 guideColour = gg;
 repaint();
}










//==============================================================================
void WaveformVisualiser::setCursorPosition(float newCursorPosition)
{
 cursorEnabled = true;
 cursorPosition = newCursorPosition;
 repaint();
}

void WaveformVisualiser::clearCursorPosition()
{
 cursorEnabled = false;
 repaint();
}

void WaveformVisualiser::setSelectionPosition(float firstSample, float lastSample)
{
 selectionEnabled = true;
 selectionStart = firstSample;
 selectionEnd = lastSample;
 repaint();
}

void WaveformVisualiser::clearSelection()
{
 selectionEnabled = false;
}










//==============================================================================
void WaveformVisualiser::setWaveGain(float linearGain)
{
 waveGain = linearGain;
 repaint();
}

void WaveformVisualiser::setGuideLevel(float level)
{
 guideLevel = level;
 repaint();
}










//==============================================================================
void WaveformVisualiser::setEnableMouseZoomScroll(bool canZoom)
{
 mouseZoomScrollEnabled = canZoom;
}

void WaveformVisualiser::resetView()
{
 if (dataSource)
 {
  auto length = dataSource->getSampleCount();
  setExactView(0, length);
 }
 else
 {
  viewMiddleSample = 0.f;
  viewZoomFactor = 1.f;
  triggerAsyncUpdate();
 }
}

void WaveformVisualiser::setExactView(size_t firstSample, size_t lastSample)
{
 float length = static_cast<float>(lastSample - firstSample);
 float middle = 0.5f*(static_cast<float>(lastSample) + static_cast<float>(firstSample));
 setView(middle, length*inverseWidth);
}

void WaveformVisualiser::setView(float middleSample, float zoomFactor)
{
 viewMiddleSample = middleSample;
 viewZoomFactor = zoomFactor;
 inverseZoomFactor = 1.f/zoomFactor;
 triggerAsyncUpdate();
}

float WaveformVisualiser::getFirstVisibleSample()
{
 return mapV2S(0.f);
}

float WaveformVisualiser::getLastVisibleSample()
{
 return mapV2S(static_cast<float>(getWidth()));
}

float WaveformVisualiser::mapS2V(float s)
{
 return (s - viewMiddleSample)*inverseZoomFactor + halfWidth;
}

float WaveformVisualiser::mapV2S(float v)
{
 return (v - halfWidth)*viewZoomFactor + viewMiddleSample;
}










//==============================================================================
void WaveformVisualiser::paint (juce::Graphics& g)
{
 // Fill backdrop
 g.fillAll(backgroundColour);
 
 // Draw horizontal guide lines
 float upperGuideHeight = halfHeight - guideLevel*halfHeight;
 float lowerGuideHeight = halfHeight + guideLevel*halfHeight;
 g.setColour(guideColour);
 g.fillRect(0.f,
            upperGuideHeight - 0.5f,
            static_cast<float>(getWidth()),
            1.f);
 g.fillRect(0.f,
            lowerGuideHeight - 0.5f,
            static_cast<float>(getWidth()),
            1.f);
 
 // Draw waveform
 g.setColour(waveformColour);
 g.setFillType(juce::FillType(waveformColour));
 g.strokePath(waveformGraphic, juce::PathStrokeType(1.f));
 if (fillWaveform) g.fillPath(waveformGraphic);
 
 if (selectionEnabled)
 {
  // Draw selection, boxes are automatically clipped to the view portal
  g.setColour(selectionColour.withAlpha(0.5f));
  float firstPixel = mapS2V(selectionStart);
  float lastPixel = mapS2V(selectionEnd);
  g.fillRect(firstPixel,
             upperGuideHeight,
             lastPixel - firstPixel,
             lowerGuideHeight - upperGuideHeight);
 }
 
 // Draw cursor
 if (cursorEnabled)
 {
  g.setColour(cursorColour);
  g.fillRect(mapS2V(cursorPosition) - 0.5f,
             0.f,
             1.f,
             static_cast<float>(getHeight()));
 }
}

void WaveformVisualiser::resized()
{
 float w = static_cast<float>(getWidth());
 float h = static_cast<float>(getHeight());
 inverseWidth = 1./w;
 inverseHeight = 1./h;
 halfWidth = 0.5*w;
 halfHeight = 0.5*h;
 triggerAsyncUpdate();
}










//==============================================================================
void WaveformVisualiser::mouseDown(const juce::MouseEvent &event)
{
 dragStarted = false;
 dragSampleStart = mapV2S(event.position.getX());
 dragZoomStart = viewZoomFactor;
}

void WaveformVisualiser::mouseDoubleClick(const juce::MouseEvent &event)
{
 if (onDoubleClick) onDoubleClick(mapV2S(event.position.getX()));
}

void WaveformVisualiser::mouseDrag(const juce::MouseEvent &event)
{
 if (mouseZoomScrollEnabled)
 {
  // Calculate the new zoom based on the mouse delta-y and dragZoomStart
  float zoomFactor = powf(2, -0.01*static_cast<float>(event.getDistanceFromDragStartY()));
  viewZoomFactor = std::min(dragZoomStart*zoomFactor, maximumZoom);
  inverseZoomFactor = 1./viewZoomFactor;
  
  // Calculate the minimum and maximum sample which can appear in the middle based on the zoom factor
  float minimumMiddle = halfWidth*viewZoomFactor;
  float maximumMiddle = static_cast<float>(dataSource->getSampleCount()) - minimumMiddle;
  if (minimumMiddle > maximumMiddle)
  {
   minimumMiddle = maximumMiddle = 0.5*static_cast<float>(dataSource->getSampleCount());
  }
  
  // Calculate the new viewMiddleSample based on the dragSampleStart sample appearing at the mouse x coord
  // Start with the mouse position relative to the middle
  float mousePixelsFromMiddle = event.position.getX() - halfWidth;
  // Calculate the distance from the middle of the screen, but measured in samples on the screen
  float samplesFromViewMiddle = mousePixelsFromMiddle*viewZoomFactor;
  // viewMiddleSample is that far from dragSampleStart
  viewMiddleSample = dragSampleStart - samplesFromViewMiddle;
  viewMiddleSample = std::min(std::max(minimumMiddle, viewMiddleSample), maximumMiddle);
  
  triggerAsyncUpdate();
 }
 else
 {
  if (!dragStarted)
  {
   dragStarted = true;
   if (onDragStart) onDragStart(dragSampleStart);
  }
  if (onDrag) onDrag(mapV2S(event.position.getX()));
 }
}

void WaveformVisualiser::mouseUp(const juce::MouseEvent &event)
{
 if (dragStarted)
 {
  dragStarted = false;
  if (onDragEnd) onDragEnd(mapV2S(event.position.getX()));
 }
 //else if (onClick) onClick(dragSampleStart);
}

void WaveformVisualiser::changeListenerCallback(juce::ChangeBroadcaster *source)
{
 resetView();
 updatePath();
}

void WaveformVisualiser::handleAsyncUpdate()
{
 updatePath();
}










//==============================================================================
float WaveformVisualiser::mapSampleToY(size_t i)
{
 return halfHeight - fetchBuffer[i]*halfHeight*waveGain;
}

void WaveformVisualiser::updatePath()
{
 // TODO: Reduce the memory footprint for large samples by using a smaller fetch buffer and adding code to draw a shape enveloping the waveform at long zoom distances. Currently, this code duplicates the entire sample in memory if the zoom level is at maximum, which is not ideal!
 
 waveformGraphic.clear();

 if (dataSource)
 {
  float count = static_cast<float>(dataSource->getSampleCount());
  
  maximumZoom = count*inverseWidth;
  
  float start = floorf(getFirstVisibleSample());
  float end = ceilf(getLastVisibleSample() + 1.f);
  size_t firstSample = std::max(0, static_cast<int>(start));
  size_t samplesToGet = std::min(static_cast<int>(dataSource->getSampleCount()),
                              static_cast<int>(end - start));

  fillWaveform = false;

  fetchBuffer.clear();
  dataSource->readSamples(firstSample, samplesToGet, fetchBuffer);
  if (!fetchBuffer.empty())
  {
   waveformGraphic.startNewSubPath(mapS2V(start), mapSampleToY(0));
   for (size_t i = 1; i < fetchBuffer.size(); ++i)
   {
    float x = mapS2V(start + static_cast<float>(i));
    float y = mapSampleToY(i);
    waveformGraphic.lineTo(x, y);
   }
  }
 }
 
 repaint();
}
