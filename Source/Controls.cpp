/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 1.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "Controls.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Controls::Controls ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    mastergain.reset (new juce::Slider ("mastergain"));
    addAndMakeVisible (mastergain.get());
    mastergain->setRange (-30, 20, 0.1);
    mastergain->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    mastergain->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    mastergain->addListener (this);

    mastergain->setBounds (704, 0, 88, 112);

    waveformDisplay.reset (new WaveformVisualiser());
    addAndMakeVisible (waveformDisplay.get());
    waveformDisplay->setName ("waveformDisplay");

    waveformDisplay->setBounds (8, 112, 784, 280);

    rootnote.reset (new juce::Slider ("rootnote"));
    addAndMakeVisible (rootnote.get());
    rootnote->setRange (0, 127, 1);
    rootnote->setSliderStyle (juce::Slider::IncDecButtons);
    rootnote->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 32, 20);
    rootnote->addListener (this);

    rootnote->setBounds (64, 88, 80, 24);

    juce__label.reset (new juce::Label ("new label",
                                        TRANS ("Master")));
    addAndMakeVisible (juce__label.get());
    juce__label->setFont (juce::Font (juce::FontOptions("Arial", 19.00f, juce::Font::plain)).withTypefaceStyle ("Regular"));
    juce__label->setJustificationType (juce::Justification::centredRight);
    juce__label->setEditable (false, false, false);
    juce__label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label->setBounds (648, 0, 64, 88);

    juce__label2.reset (new juce::Label ("new label",
                                         TRANS ("Root note")));
    addAndMakeVisible (juce__label2.get());
    juce__label2->setFont (juce::Font (juce::FontOptions("Arial", 10.80f, juce::Font::plain)).withTypefaceStyle ("Regular"));
    juce__label2->setJustificationType (juce::Justification::centredRight);
    juce__label2->setEditable (false, false, false);
    juce__label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label2->setBounds (8, 88, 56, 24);


    //[UserPreSize]
    for (auto &child: getChildren())
 {
  child->setComponentID(child->getName());
 }
    //[/UserPreSize]

    setSize (800, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Controls::~Controls()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    mastergain = nullptr;
    waveformDisplay = nullptr;
    rootnote = nullptr;
    juce__label = nullptr;
    juce__label2 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Controls::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Controls::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Controls::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == mastergain.get())
    {
        //[UserSliderCode_mastergain] -- add your slider handling code here..
        //[/UserSliderCode_mastergain]
    }
    else if (sliderThatWasMoved == rootnote.get())
    {
        //[UserSliderCode_rootnote] -- add your slider handling code here..
        //[/UserSliderCode_rootnote]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Controls" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="800" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <SLIDER name="mastergain" id="8a0c6abf801c3352" memberName="mastergain"
          virtualName="" explicitFocusOrder="0" pos="704 0 88 112" min="-30.0"
          max="20.0" int="0.1" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <GENERICCOMPONENT name="waveformDisplay" id="a77109b32542ed12" memberName="waveformDisplay"
                    virtualName="WaveformVisualiser" explicitFocusOrder="0" pos="8 112 784 280"
                    class="juce::Component" params=""/>
  <SLIDER name="rootnote" id="9970af6b120133ca" memberName="rootnote" virtualName=""
          explicitFocusOrder="0" pos="64 88 80 24" min="0.0" max="127.0"
          int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="32" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="new label" id="363293444783fb67" memberName="juce__label"
         virtualName="" explicitFocusOrder="0" pos="648 0 64 88" edTextCol="ff000000"
         edBkgCol="0" labelText="Master" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Arial" fontsize="19.0" kerning="0.0"
         bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="541dba946357531c" memberName="juce__label2"
         virtualName="" explicitFocusOrder="0" pos="8 88 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Root note" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Arial" fontsize="10.8" kerning="0.0"
         bold="0" italic="0" justification="34"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

