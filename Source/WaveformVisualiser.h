/*
 ==============================================================================
 
 WaveformVisualiser.h
 Created: 22 Oct 2025 9:56:02pm
 Author:  Adam Jackson
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>










/*
 Extend this class to provide the audio data to the visualiser
 */
class WaveformDataSourceBase : public juce::ChangeBroadcaster
{
 public:
 WaveformDataSourceBase(){}
 virtual ~WaveformDataSourceBase(){}
 
 // This method must return the total number of samples available
 virtual size_t getSampleCount() const = 0;
 
 // This method must fill the provided vector with the requested samples.
 // The vector is provided empty, use whatever methods are appropriate to fill the vector
 // The visualiser will handle incomplete data appropriately, so if some or no data is available,
 // return what you can or leave the vector empty.
 // The visualiser will never ask for data outside of the value reported by getSampleCount()
 virtual void readSamples(size_t startIndex, size_t count, std::vector<float> &samples) const = 0;
};










//==============================================================================
/*
 */
class WaveformVisualiser  : public juce::Component, juce::ChangeListener, juce::AsyncUpdater
{
 const float SOFT_BUFFER_LIMIT = 16384.f;
 
 public:
 
 // Object stuff
 WaveformVisualiser();
 ~WaveformVisualiser() override;
 
 // Data connection management
 void connectDataSource(WaveformDataSourceBase &newDataSource);
 void disconnectDataSource();
 
 // Colour management
 void setBackgroundColour(juce::Colour bg);
 void setWaveformColour(juce::Colour wg);
 void setSelectionColour(juce::Colour sg);
 void setCursorColour(juce::Colour cg);
 void setGuideColour(juce::Colour gg);

 // Cursor and selection properties
 void setCursorPosition(float cursorPosition);
 void clearCursorPosition();
 void setSelectionPosition(float firstSample, float lastSample);
 void clearSelection();
 
 // Set height of waveform (1.0 takes up the full height of the window)
 void setWaveGain(float linearGain);
 
 // Set the level of guide bars between 0 and 1
 void setGuideLevel(float level);

 // Set the view
 void setEnableMouseZoomScroll(bool canZoom);
 void resetView();
 void setExactView(size_t firstSample, size_t lastSample);
 void setView(float middleSample, float zoomFactor);
 void setViewMiddleSample(float middleSample) { setView(middleSample, viewZoomFactor); }
 void setViewZoomFactor(float zoomFactor) { setView(viewMiddleSample, zoomFactor); }
 
 // Get the current view
 float getViewMiddleSample() { return viewMiddleSample; }
 float getViewZoomFactor() { return viewZoomFactor; }
 float getFirstVisibleSample();
 float getLastVisibleSample();
 
 // Overridden Component, AsyncUpdater and ChangeListener methods
 void paint (juce::Graphics&) override;
 void resized() override;
 virtual void mouseDown(const juce::MouseEvent &event) override;
 virtual void mouseDoubleClick(const juce::MouseEvent &event) override;
 virtual void mouseDrag(const juce::MouseEvent &event) override;
 virtual void mouseUp(const juce::MouseEvent &event) override;
 virtual void changeListenerCallback(juce::ChangeBroadcaster *source) override;
 virtual void handleAsyncUpdate() override;
 

 // Where applicable, these functions are called with the sample that was clicked on as the parameter value
 // If set, this function is called when the user clicks on the waveform
 std::function<void (float)> onClick;
 // If set, this function is called when the user double clicks on the waveform
 std::function<void (float)> onDoubleClick;
 // If set, this function is called when the user clicks and starts dragging and zoom is turned off
 std::function<void (float)> onDragStart;
 // If set, this function is called when the user drags the mouse and zoom is turned off
 std::function<void (float)> onDrag;
 // If set, this function is called when the user releases the mouse button after dragging
 std::function<void (float)> onDragEnd;
 
 private:
 
 WaveformDataSourceBase *dataSource {nullptr};
 
 juce::Colour backgroundColour {juce::Colours::black};
 juce::Colour waveformColour {juce::Colours::lightsteelblue};
 juce::Colour selectionColour {juce::Colours::white.withBrightness(0.5f).withAlpha(0.5f)};
 juce::Colour cursorColour {juce::Colours::white.withBrightness(0.8f)};
 juce::Colour guideColour {juce::Colours::white.withBrightness(0.8)};
 
 // The position of the sample which appears at the middle of the view
 float viewMiddleSample {0.f};
 
 // The number of samples which are fit to a pixel
 float viewZoomFactor {1.f};
 // The number of pixels taken up by a sample
 float inverseZoomFactor {1.f};
 
 float waveGain {0.9};
 
 float guideLevel {0.9};
 
 bool cursorEnabled {false};
 float cursorPosition {0.f};
 
 bool selectionEnabled {false};
 float selectionStart {0.f};
 float selectionEnd {0.f};
 
 float inverseHeight {1.f};
 float inverseWidth {1.f};
 float halfWidth {1.f};
 float halfHeight {1.f};
 float maximumZoom {1.f};
 
 // Short named convenience functions to map sample time to horizontal view co-ordinate
 float mapS2V(float s);
 float mapV2S(float v);
 
 bool mouseZoomScrollEnabled {false};
 float dragSampleStart {0.f};
 float dragZoomStart {0.f};
 bool dragStarted {false};
 
 juce::Path waveformGraphic;
 bool fillWaveform {false};
 void updatePath();
 
 std::vector<float> fetchBuffer;
 std::vector<float> maxShapeBuffer;
 std::vector<float> minShapeBuffer;
 float mapSampleToY(size_t i);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformVisualiser)
};
