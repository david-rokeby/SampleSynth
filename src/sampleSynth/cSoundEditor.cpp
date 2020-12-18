//
//  cSoundEditor.cpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-25.
//

#include "cSoundEditor.hpp"

#include "ofMath.h"
#include "cPlayer.h"
#include "cSampleSynth.hpp"


void cSoundEditor::SetupGUI( void )
{
	int width = guiWidth;
    int left = 0;
    guiElementHeight = 28;
	int offset = 0;
	int heightStart = 256;
	int column = 0;
    sliderLabelWidth = 160;
    waveRange = 1.0f;
	
	offset ++;
    currentGUIXStart = left;
    currentGUIYStart = heightStart;
    CreateButton("LOAD SYNTH", guiWidth * 2 / 7, column*guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSynthEvent );
    CreateButton("SAVE SYNTH", guiWidth * 2 / 7, column*guiWidth + guiWidth * 2 / 7, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onSaveSynthEvent );
    offset = CreateButton("CREATE FROM FOLDER", guiWidth * 3 / 7, column*guiWidth + guiWidth * 4 / 7, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onCreateSynthFromFolderEvent );

    offset=CreateFloatSlider("MASTER VOLUME", 0.0f, 4.0f, &owner->masterVolume, guiWidth, column*guiWidth, offset);

    offset ++;
    CreateButton("LOAD SOUND OBJECT", guiWidth / 2, column*guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSoundObjectEvent );

    offset = CreateButton("SAVE SOUND OBJECT", guiWidth / 2, column*guiWidth + guiWidth / 2, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onSaveSoundObjectEvent );

    offset=CreateNumericInputInt("SOUND OBJECT", &soundObjectID, guiWidth *.6, column*guiWidth, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onSoundObjectSelectEvent );

    soundObjectGUIComponentsIndex = guiComponents_.size();
    vector<string> options = {"STEREO", "STEREO PLACED", "STEREO QUAD"};
    CreateDropDown("MIX MODE", options, GetMixModeFunction, guiWidth / 2, column*guiWidth, offset );
    guiComponents_.back()->onDropdownEvent( this, &cSoundEditor::onMixModeDropdownEvent );

    offset=CreateNumericInputInt("# CHANNELS", &owner -> outChannelCount, guiWidth *.5, column * guiWidth + guiWidth / 2.0, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onNumOutChannelsSelectEvent );

    offset = CreateFloatSlider( "PAN", 0.0f, 1.0f, GetPanFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onPanEvent );
	
    CreateFloatSlider( "FRONT BACK", 0.0f, 3.0f, GetPanFrontBackFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onPanFrontBackEvent );
    frontBackIndex = guiComponents_.size() - 1;

    CreateNumericInputInt("LEFT CHAN", GetLeftChannelFunction, guiWidth / 2, column * guiWidth, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onLeftChannelAssignEvent );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(0);
    leftChannelIndex = guiComponents_.size() - 1;

    offset = CreateNumericInputInt("RIGHT CHAN", GetRightChannelFunction, guiWidth / 2, column * guiWidth + guiWidth / 2, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onRightChannelAssignEvent );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(1);
    rightChannelIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "AMPLITUDE", 0.0f, 4.0f, GetAmplitudeFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onAmpEvent );

    offset = CreateFloatSlider( "PITCH", 0.0f, 10.0f, GetObjPitchFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onObjPitchEvent );
	
    offset = CreateFloatSlider( "SENSITIVITY", 0.0f, 256.0f, GetSensitivityFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSensitivityEvent );

    offset = CreateFloatSlider( "SMOOTHING", 0.0f, 1.0f, GetSmoothingFunction, guiWidth, column * guiWidth, offset);
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSmoothingEvent );

    offset = CreateFloatSlider( "RELEASE RATE", 0.0f, 1.0f, GetReleaseRateFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onReleaseRateEvent );

    offset = CreateFloatSlider( "RETRIG DELAY", 0.0f, 20.0f, GetRetriggerDelayFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onRetriggerDelayEvent );

    offset = CreateNumericInputInt( "OSC PORT", GetOSCPortFunction, guiWidth - 94, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onOSCPortEvent );
    rxIndicatorRect = ofRectangle(currentGUIXStart + column * guiWidth + guiWidth - 94, offset * guiElementHeight + currentGUIYStart - guiElementHeight + 2, 92, guiElementHeight - 6);


    offset = 0;
	column ++;

    soundDefGUIComponentsStartIndex = guiComponents_.size();
    offset = CreateNumericInputInt("SOUND DEF", GetSoundDefFunction, guiWidth *.5, column*guiWidth, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onSoundDefSelectEvent );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(-1);
	
    CreateButton("LOAD SOUND DEF", guiWidth / 2, column * guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSoundDefEvent );

    offset = CreateButton("SAVE SOUND DEF", guiWidth / 2, column * guiWidth + guiWidth / 2.0, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onSaveSoundDefEvent );

    offset ++;
    CreateButton("LOAD SAMPLE", sliderLabelWidth, column * guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSampleButtonEvent );
	
    offset = CreateLabel("", guiWidth - sliderLabelWidth, column * guiWidth + sliderLabelWidth, offset);
    guiComponents_.back()->setLabelUpperCase( false );
    sampleNameIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "AMPLITUDE", 0.0f, 10.0f, GetSampleAmpFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleAmpEvent );

    offset = CreateFloatSlider( "PITCH", 0.0f, 10.0f, GetSamplePitchFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSamplePitchEvent );

    offset = CreateFloatSlider( "START", 0.0f, 1.0f, GetSampleStartFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleStartEvent );
    sampleStartIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "END", 0.0f, 1.0f, GetSampleEndFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleEndEvent );
    sampleEndIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "ATTACK", 0.0f, 10.0f, GetSampleAttackFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleAttackEvent );
	attackIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "DECAY", 0.0f, 10.0f, GetSampleDecayFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleDecayEvent );
	decayIndex = guiComponents_.size() - 1;

	column ++;
	offset = 0;
    offset = CreateToggle("LOOP", GetSampleDoLoopFunction, guiWidth / 2, column * guiWidth, offset );
    guiComponents_.back()->onToggleEvent( this, &cSoundEditor::onSampleDoLoopEvent );

    offset = CreateFloatSlider( "LOOP START", 0.0f, 1.0f, GetSampleLoopStartFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleLoopStartEvent );
    loopStartIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "LOOP END", 0.0f, 1.0f, GetSampleLoopEndFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleLoopEndEvent );
    loopEndIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "CROSS FADE", 0.0f, 0.5f, GetSampleCrossFadeFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleCrossFadeEvent );
}

void cSoundEditor::ShowGUI( void )
{
    std::ostringstream scale;
    scale << std::setprecision(2);
	ofColor white( 255, 255, 255 );
	ofSetColor( white );
	
	float max = ofGetWidth();
	float height = 256;
    if( visible)
        ShowGUIElements();
	
	ofNoFill();
	cSoundDefinition *soundDef = SoundDef();
	
	if( soundDef )
		{
		if( soundDef -> SampleLoaded() )
			{
			ofPushMatrix();
			for( int c = 0; c < soundDef -> channelCount; ++ c )
				{
                ofSetColor(0, 192, 64);
				ofBeginShape();
                ofSetLineWidth(0.5);
				for( int x = 0; x < max; ++ x)
					{
					int n = ofMap( x, 0, max, 0, soundDef -> sampleCount, true );
					float val = soundDef -> buffer[ n * soundDef -> channelCount + c ];
                    float y = ofMap( val, -waveRange, waveRange, height * 0.5, 0.0f, true);
                    //ofClamp(y, -1, 1);
					ofVertex( x, y );
					}
				ofEndShape();
				ofTranslate( 0.0, height * 0.5 );
				}
			ofPopMatrix();
            ofSetLineWidth(1.0);
            scale << (1.0f/waveRange);

            ofSetColor(128, 128, 128);
            guiTheme.font.ptr->draw("SCALE: " + scale.str(), 20, height * 0.5 - 10);
            ofDrawLine(glm::vec2(0.0, height * 0.5), glm::vec2(max, height * 0.5));
            ofSetColor(ofColor::white);
        }

		if( visible )
			{
			//soundDef
			ofFill();
			if( soundDef -> SampleLoaded() )
				{

				bool looping = ( soundDef -> loop );
				if( looping )
					{
					ofColor red( looping ? 255 : 192, looping ? 192 : 128, 0 );
					ofSetColor( red );
					float n = ofMap( soundDef -> startLoopSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					n = ofMap( soundDef -> endLoopSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					
					ofColor dark_red( looping ? 192 : 128, looping ? 128 : 64, 0 );
					ofSetColor( dark_red );
					n = ofMap( soundDef -> enterLoopCrossFadeSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					n = ofMap( soundDef -> exitLoopCrossFadeSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					}
				
				ofColor dark_blue( 0, 128, 255 );
				ofSetColor( dark_blue );
				float sa = ofMap( soundDef -> startSample, 0, soundDef -> sampleCount, 0, max, true );
				float attackSamples = ofMap( soundDef -> attackSamples, 0, soundDef -> sampleCount, 0, max, true );
				ofDrawLine( sa + attackSamples, 0, sa, height);
				float ed = ofMap( soundDef -> endSample, 0, soundDef -> sampleCount, 0, max, true );
				float decaySamples = ofMap( soundDef -> releaseSamples, 0, soundDef -> sampleCount, 0, max, true );
				ofDrawLine( ed - decaySamples, 0, ed, height);
				}
			}
		}
	if( soundDef && soundDef -> SampleLoaded() )
		{
		ofNoFill();
		ofColor red( 255, 0, 0 );
		ofSetColor( red );
		
		if(	player && player -> active )
			{
			uint64_t pos = static_cast<uint64_t>(player -> position);
			if( pos > soundDef -> sampleCount )
				pos = pos - soundDef -> sampleCount + soundDef -> exitLoopCrossFadeSample;
			float phx = ofMap( pos, 0, soundDef -> sampleCount, 0, ofGetWidth() );
			ofDrawLine( phx, 0, phx, height);
			}
		}
	if( visible )
		{
		ofFill();
		for( int c = 0; c < owner -> players.size(); ++ c )
			{
			cPlayer *thisPlayer = owner -> players[ c ];
			if( thisPlayer -> claimed )
				{
				if( thisPlayer -> active )
					{
					switch(  thisPlayer -> envelopeMode )
						{
						case t_EnvelopeIdle:
							ofSetColor( 255, 0, 255 );
							break;
						
						case t_EnvelopeAttackMode:
							ofSetColor( 255, 255, 0 );
							break;
							
						case t_EnvelopeSustainMode:
							ofSetColor( 0, 255, 0 );
							break;
							
						case t_EnvelopeReleaseMode:
                            ofSetColor( 0, 192, 255 );
							break;
						}
					}
				else
					{
					switch( thisPlayer -> envelopeMode )
						{
						case t_EnvelopeIdle:
							ofSetColor( 64, 0, 64 );
							break;
							
						case t_EnvelopeAttackMode:
							ofSetColor( 64, 64, 0 );
							break;
							
						case t_EnvelopeSustainMode:
							ofSetColor( 0, 64, 0 );
							break;
							
						case t_EnvelopeReleaseMode:
                            ofSetColor( 0, 48, 64 );
							break;
						}
                    }
				}
			else
				{
				ofSetColor( 48, 48, 48 );
				}
			
			ofRectangle rect;
            rect.x = ( c / 4 ) * 15;
            rect.y = 258 + ( c & 3 ) * 6;
            rect.width = 14;
			rect.height = 5;
			ofDrawRectangle(rect);
			}
		}
}

void cSoundEditor::UpdateGUI( void )
{
	if( soundObject && visible )
		{
        UpdateGUIElements();
		}
}

void cSoundEditor::ExtractSoundObjectValuesForDisplay( void )
{
    if( soundObject == nullptr )
    {
        AttachToSoundObjectByID( soundObjectID );
        return;
    }

    if( soundObject )
        {
        cSoundDefinition *soundDef = SoundDef();
		if(soundDef)
			soundDef -> editor = this;
        for( int i = soundObjectGUIComponentsIndex; i < soundDefGUIComponentsStartIndex; i ++ )
        {
            ofxDatGuiComponent *comp = guiComponents_[i];
            switch(comp -> getType())
            {
            case ofxDatGuiType::BUTTON:
                ((ofxDatGuiButton *)comp) -> update();
                break;

            case ofxDatGuiType::TOGGLE:
                ((cToggle *)comp) -> UpdateValueFromSource();
 //               ((cToggle *)comp) -> update();
                break;

            case ofxDatGuiType::DROPDOWN:
                ((cDropDown *)comp) -> UpdateValueFromSource();
//                ((cDropDown *)comp) -> update();
                break;

            case ofxDatGuiType::SLIDER:
                ((cFocussedSlider *)comp) -> UpdateValueFromSource();
 //               ((cFocussedSlider *)comp) -> update();
                break;

            case ofxDatGuiType::VALUE_PLOTTER:
                ((ofxDatGuiValuePlotter *)comp) -> update(true);
                break;

            case ofxDatGuiType::TEXT_INPUT:
            {
                ofxDatGuiNumericInput *input = ((ofxDatGuiNumericInput *)comp);
                if( input -> isInt)
                {
                    ((ofxDatGuiNumericIntInput *)comp) -> UpdateValueFromSource();
                }
                else
                {
                    ((ofxDatGuiNumericInput *)comp) -> UpdateValueFromSource();
                }
//                ((ofxDatGuiNumericInput *)comp) -> update();
                break;
            }

            case ofxDatGuiType::LABEL:
                if(i==sampleNameIndex)
                {
                    if(SoundDef())
                        ((ofxDatGuiLabel*)comp) ->setLabel(SoundDef()->sampleName);
                }
                break;
            }
        }
    }
    ExtractSoundDefValuesForDisplay();
}


void cSoundEditor::ExtractSoundDefValuesForDisplay( void )
{
	if( soundObject == nullptr )
		AttachToSoundObjectByID( soundObjectID );
	
	if( soundObject )
		{
		cSoundDefinition *soundDef = SoundDef();

		if( soundDef == nullptr )
			return;

        for( int i = soundDefGUIComponentsStartIndex; i < guiComponents_.size(); i ++ )
        {
            ofxDatGuiComponent *comp = guiComponents_[i];
            switch(comp -> getType())
            {
            case ofxDatGuiType::BUTTON:
                ((ofxDatGuiButton *)comp) -> update();
                break;

            case ofxDatGuiType::TOGGLE:
                ((cToggle *)comp) -> UpdateValueFromSource();
 //               ((cToggle *)comp) -> update();
                break;

            case ofxDatGuiType::DROPDOWN:
                ((cDropDown *)comp) -> UpdateValueFromSource();
//                ((cDropDown *)comp) -> update();
                break;

            case ofxDatGuiType::SLIDER:
                ((cFocussedSlider *)comp) -> UpdateValueFromSource();
 //               ((cFocussedSlider *)comp) -> update();
                break;

            case ofxDatGuiType::VALUE_PLOTTER:
                ((ofxDatGuiValuePlotter *)comp) -> update(true);
                break;

            case ofxDatGuiType::TEXT_INPUT:
            {
                ofxDatGuiNumericInput *input = ((ofxDatGuiNumericInput *)comp);
                if( input -> isInt)
                {
                    ((ofxDatGuiNumericIntInput *)comp) -> UpdateValueFromSource();
                }
                else
                {
                    ((ofxDatGuiNumericInput *)comp) -> UpdateValueFromSource();
                }
//                ((ofxDatGuiNumericInput *)comp) -> update();
            }
                break;

            case ofxDatGuiType::LABEL:
                if(i==sampleNameIndex)
                {
                    if(SoundDef())
                        ((ofxDatGuiLabel*)comp) ->setLabel(SoundDef()->sampleName);
                }
                break;
            }
        }
    }
}


void GetMixModeFunction(cSoundEditor *editor, ofxDatGuiComponent *dropDown)
{
    ((cDropDown *)dropDown) -> select( editor -> soundObject -> mix.mode );
}



void cSoundEditor::onMasterVolumeEvent(ofxDatGuiSliderEvent e)
{
	float vol = e.target -> getValue();
	owner -> masterVolume = vol;
}


void GetPanFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> mix.pan );
}


void cSoundEditor::onPanEvent(ofxDatGuiSliderEvent e)
{
	float pan = e.target -> getValue();
	if( soundObject )
		soundObject -> mix.SetPan( pan );
}

void GetPanFrontBackFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> mix.frontBackPan );
}


void cSoundEditor::onPanFrontBackEvent(ofxDatGuiSliderEvent e)
{
	float panFrontBack = e.target -> getValue();
	if( soundObject )
		soundObject -> mix.SetFrontBackPan( panFrontBack );
}

void GetAmplitudeFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> mix.globalLevel );
}


void cSoundEditor::onAmpEvent(ofxDatGuiSliderEvent e)
{
	float amp = e.target -> getValue();
	if( soundObject )
		soundObject -> mix.globalLevel = amp;
	
}

void GetObjPitchFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> pitch );
}


void cSoundEditor::onObjPitchEvent(ofxDatGuiSliderEvent e)
{
	float pitch = e.target -> getValue();
	if( soundObject )
		soundObject -> pitch = pitch;
	
}


void GetSoundDefFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> soundDefID );
}


void GetRetriggerDelayFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> retriggerDelay );
}


void cSoundEditor::onRetriggerDelayEvent(ofxDatGuiSliderEvent e)
{
	float delay = e.target -> getValue();
    if(soundObject)
        soundObject -> retriggerDelay = delay;
}


void GetReleaseRateFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> fadeRate );
}


void cSoundEditor::onReleaseRateEvent(ofxDatGuiSliderEvent e)
{
    float fadeRate = e.target -> getValue();
    if(soundObject)
        soundObject -> fadeRate = fadeRate;
}


void GetSmoothingFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> smoothing );
}


void cSoundEditor::onSmoothingEvent(ofxDatGuiSliderEvent e)
{
    float smoothing = e.target -> getValue();
    if(soundObject)
        soundObject -> smoothing = smoothing;
}

void GetSensitivityFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> sensitivity );
}


void cSoundEditor::onSensitivityEvent(ofxDatGuiSliderEvent e)
{
    float sensitivity = e.target -> getValue();
    if(soundObject)
        soundObject -> sensitivity = sensitivity;
}


void cSoundEditor::onSoundObjectSelectEvent(ofxDatGuiTextInputEvent e)
{
	int selectedSoundObject = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	if( player && player -> active )
		player -> Release();
	AttachToSoundObjectByID( selectedSoundObject );
//    ExtractSoundObjectValuesForDisplay();
}

void GetOSCPortFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    ((ofxDatGuiNumericIntInput *)numberBox) -> setValue( editor->owner->oscPort );
}


void cSoundEditor::onOSCPortEvent(ofxDatGuiSliderEvent e)
{
    int32_t port = e.target -> getValue();
    if(owner)
        owner -> oscPort = port;
}

void cSoundEditor::onNumOutChannelsSelectEvent(ofxDatGuiTextInputEvent e)
{
	int numChannelsOut = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	if( soundObject )
		{
		soundObject -> mix.SetOutChannelCount( numChannelsOut );
		}
}


void cSoundEditor::onLeftChannelAssignEvent(ofxDatGuiTextInputEvent e)
{
	int leftChan = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	if( soundObject )
		{
		soundObject -> mix.leftFrontOutChannel = leftChan;
		}
}

void GetLeftChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    if(editor -> soundObject)
        ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> mix.leftFrontOutChannel );
}


void cSoundEditor::onRightChannelAssignEvent(ofxDatGuiTextInputEvent e)
{
	int rightChan = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	if( soundObject )
		{
		soundObject -> mix.rightFrontOutChannel = rightChan;
		}
}

void GetRightChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    if(editor -> soundObject)
        ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> mix.rightFrontOutChannel );
}



void GetSampleAmpFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> amplitude );
}


void cSoundEditor::onSampleAmpEvent(ofxDatGuiSliderEvent e)
{
    float amp = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> amplitude = amp;

}

void GetSamplePitchFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> pitch );
}


void cSoundEditor::onSamplePitchEvent(ofxDatGuiSliderEvent e)
{
    float pitch = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> pitch = pitch;

}


void GetSampleStartFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> startSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleStartEvent(ofxDatGuiSliderEvent e)
{
    float startSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetSampleStart( static_cast<uint64_t>(startSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleEndFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> endSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleEndEvent(ofxDatGuiSliderEvent e)
{
    float endSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetSampleEnd( static_cast<uint64_t>(endSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleAttackFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> attackTime );
}


void cSoundEditor::onSampleAttackEvent(ofxDatGuiSliderEvent e)
{
    float attack = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetAttackTime(attack);

}

void GetSampleDecayFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> releaseTime );
}


void cSoundEditor::onSampleDecayEvent(ofxDatGuiSliderEvent e)
{
    float release = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetReleaseTime(release);

}


void GetSampleDoLoopFunction(cSoundEditor *editor, ofxDatGuiComponent *toggle)
{
    if(editor -> SoundDef())
        ((cToggle *)toggle) -> setChecked( editor -> SoundDef() -> loop );
}


void cSoundEditor::onSampleDoLoopEvent(ofxDatGuiToggleEvent e)
{
    bool loop = e.target -> getChecked();
    if( SoundDef() )
        SoundDef() -> loop = loop;

}


void GetSampleLoopStartFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> startLoopSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleLoopStartEvent(ofxDatGuiSliderEvent e)
{
    float startLoopSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetLoopStart( static_cast<uint64_t>(startLoopSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleLoopEndFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> endLoopSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleLoopEndEvent(ofxDatGuiSliderEvent e)
{
    float endLoopSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetLoopEnd( static_cast<uint64_t>(endLoopSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleCrossFadeFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>( editor -> SoundDef() -> loopCrossFadeFraction ) );
}


void cSoundEditor::onSampleCrossFadeEvent(ofxDatGuiSliderEvent e)
{
    float loopCrossFadeFraction = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetLoopCrossFadeFraction( loopCrossFadeFraction );
}


void cSoundEditor::onMixModeDropdownEvent(ofxDatGuiDropdownEvent e)
{
	int modeChoice = e.child;
	if( soundObject )
		{
		switch( modeChoice )
			{
				case 0:
					soundObject -> mix.InitStereo();

					break;
				
				case 1:
					soundObject -> mix.InitStereoToMulti( 2 );
					break;
				
				case 2:
					soundObject -> mix.InitStereoToQuadInMulti( 2 );
					break;
			}
		}
}


void cSoundEditor::onSoundDefSelectEvent(ofxDatGuiTextInputEvent e)
{
	int selectedSoundDef = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	if( player && player -> active )
		player -> Stop();
    if(selectedSoundDef >= 0)
    {
        if( soundObject )
            soundObject -> SetSoundDefByID( selectedSoundDef );
        ExtractSoundDefValuesForDisplay();
    }
}

	
void cSoundEditor::onToggleLoopEvent(ofxDatGuiToggleEvent e)
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
	bool loop = e.target -> getChecked();
	soundDef -> loop = loop;
	
}


bool cSoundEditor::LoadSampleAction( void )
{
	ofFileDialogResult openFileResult= ofSystemLoadDialog("select an audio sample");
	//Check if the user opened a file
	if (openFileResult.bSuccess)
		{
		if( player && player -> active )
			player -> Stop();
		string filepath = openFileResult.getPath();
		cSoundDefinition *soundDef = SoundDef();
		if( soundDef )
			{
			if( soundDef -> LoadSample( filepath, true ) )
				{
                ExtractSoundDefValuesForDisplay();
                waveRange = 1.0;
				}
			}
		}
	return false;
}


void cSoundEditor::onLoadSampleButtonEvent(ofxDatGuiButtonEvent e)
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
	LoadSampleAction();
}


void cSoundEditor::onLoadSynthEvent(ofxDatGuiButtonEvent e)
{
    owner->GetLoadSynthFile("Load Synth File:");
    AttachToSoundObjectByID(0);
}


void cSoundEditor::onCreateSynthFromFolderEvent(ofxDatGuiButtonEvent e)
{
    owner->CreateFromFolder();
}

void cSoundEditor::onSaveSynthEvent(ofxDatGuiButtonEvent e)
{
    owner->GetSaveSynthFile("Save Synth File:");
}


void cSoundEditor::onLoadSoundObjectEvent(ofxDatGuiButtonEvent e)
{
    soundObject -> GetLoadSoundObjectFile("Load SoundObject");
    ExtractSoundDefValuesForDisplay();
}


void cSoundEditor::onSaveSoundObjectEvent(ofxDatGuiButtonEvent e)
{
    soundObject -> GetSaveSoundObjectFile("Save SoundObject As");

}

void cSoundEditor::onLoadSoundDefEvent(ofxDatGuiButtonEvent e)
{
//  load the file into the current soundDef
    ofxJSON json;
    Json::Value def;

    if(SoundDef())
    {
        SoundDef() -> GetLoadSoundDefFile("Load SoundDef");
        ExtractSoundDefValuesForDisplay();
    }
}


void cSoundEditor::onSaveSoundDefEvent(ofxDatGuiButtonEvent e)
{
   SoundDef() -> GetSaveSoundDefFile("Save SoundDef As");
}


void cSoundEditor::MouseReleased(int x, int y, int button)
{
	selectedParam = -1;
}


void cSoundEditor::MousePressed(int x, int y, int button)
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
	
	if( y > waveHeight )
		return;
	int64_t mouseSample = static_cast<uint64_t>( static_cast<float>(x) / static_cast<float>( ofGetWidth() ) * static_cast<float>(soundDef -> sampleCount ) );
    uint64_t diffs[ loopEndIndex + 5 ];
    for( int i = 0; i <= loopEndIndex + 4; i ++ )
		diffs[ i ] = 10000000000;
    diffs[ sampleStartIndex ] = abs( static_cast<int64_t>(soundDef -> startSample) - mouseSample );
    diffs[ sampleEndIndex ] = abs( static_cast<int64_t>(soundDef -> endSample) - mouseSample );
    diffs[ loopStartIndex ] = abs( static_cast<int64_t>(soundDef -> startLoopSample) - mouseSample );
    diffs[ loopEndIndex ] = abs( static_cast<int64_t>(soundDef -> endLoopSample) - mouseSample );
    diffs[ loopEndIndex + 1 ] = abs( static_cast<int64_t>(soundDef -> enterLoopCrossFadeSample) - mouseSample );
    diffs[ loopEndIndex + 2 ] = abs( static_cast<int64_t>(soundDef -> exitLoopCrossFadeSample) - mouseSample );
    diffs[ loopEndIndex + 3 ] = abs( static_cast<int64_t>(soundDef -> startSample + soundDef->attackSamples) - mouseSample );
    diffs[ loopEndIndex + 4 ] = abs( static_cast<int64_t>(soundDef -> endSample - soundDef->releaseSamples) - mouseSample );
    uint64_t leastDiff = 10000000000;
	int closestIndex = -1;
	
    for( int i = sampleStartIndex; i <= loopEndIndex + 4; i ++ )
		{
		if( diffs[ i ] <= leastDiff )
			{
			leastDiff = diffs[ i ];
			closestIndex = i;
			}
		}
	if( closestIndex != -1 )
		selectedParam = closestIndex;
    if(selectedParam == sampleStartIndex && y < waveHeight / 4)
        selectedParam = loopEndIndex + 3;
    else if(selectedParam == sampleEndIndex && y < waveHeight / 4)
        selectedParam = loopEndIndex + 4;
}


void cSoundEditor::MouseScrolled( int yScroll )
{
    if(yScroll < 0 )
        waveRange *= 1.1;
    else if(yScroll > 0)
        waveRange /= 1.1;

}


void cSoundEditor::MouseDragged( int x, int y, int button )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
    double mouseSample = ( static_cast<double>(x) / static_cast<double>( ofGetWidth() ) * static_cast<double>(soundDef -> sampleCount ) );
    double sliderVal = mouseSample / static_cast<double>(soundDef -> sampleCount );
	
    if( selectedParam == sampleStartIndex)
    {
        SoundDef() -> SetSampleStart( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleStartFunction(this, guiComponents_[sampleStartIndex]);
	}
    else if( selectedParam == sampleEndIndex)
    {
        SoundDef() -> SetSampleEnd( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleEndFunction(this, guiComponents_[sampleEndIndex]);
    }
    else if( selectedParam == loopStartIndex)
    {
        SoundDef() -> SetLoopStart( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleLoopStartFunction(this, guiComponents_[loopStartIndex]);
	}
    else if( selectedParam == loopEndIndex)
    {
        SoundDef() -> SetLoopEnd( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleLoopEndFunction(this, guiComponents_[loopEndIndex]);
    }
    else if( selectedParam == loopEndIndex + 1)
    {
        uint64_t loopSamples = soundDef -> endLoopSample - soundDef -> startLoopSample;
		uint64_t fadeSamples = mouseSample - soundDef -> startLoopSample;
		float crossfadeFraction = static_cast<float>(fadeSamples) / static_cast<float>(loopSamples);
        SoundDef() -> SetLoopCrossFadeFraction(crossfadeFraction);
		GetSampleCrossFadeFunction(this, guiComponents_[loopEndIndex + 1]);
    }
    else if( selectedParam == loopEndIndex + 2)
    {
        uint64_t loopSamples = soundDef -> endLoopSample - soundDef -> startLoopSample;
		uint64_t fadeSamples = soundDef -> endLoopSample - mouseSample;
		float crossfadeFraction = static_cast<float>(fadeSamples) / static_cast<float>(loopSamples);
        SoundDef() -> SetLoopCrossFadeFraction(crossfadeFraction);
		GetSampleCrossFadeFunction(this, guiComponents_[loopEndIndex + 1]);
	}
    else if( selectedParam == loopEndIndex + 3)
    {
        uint64_t attackSamples = mouseSample - soundDef -> startSample;
        float attackTime = static_cast<double>(attackSamples) / static_cast<double>(soundDef -> sampleRate);
        SoundDef() -> SetAttackTime(attackTime);
		GetSampleAttackFunction( this, guiComponents_[attackIndex]);
    }
    else if( selectedParam == loopEndIndex + 4)
    {
        uint64_t decaySamples = soundDef -> endSample - mouseSample;
        float decayTime = static_cast<double>(decaySamples) / static_cast<double>(soundDef -> sampleRate);
        SoundDef() -> SetReleaseTime(decayTime);
		GetSampleDecayFunction( this, guiComponents_[decayIndex]);
   }
}


bool cSoundEditor::TogglePlay( void )
{
	if( player && player -> active )
		{
		player -> Release();
		return false;
		}
	else
		{
		if( player )
			player -> Release();
        player = owner -> Trigger( soundObjectID );
		return true;
		}
	return false;
}


bool cSoundEditor::ToggleUI( void ){ visible = !visible; return visible; }


void cSoundEditor::AttachToSoundObjectByID( int inSoundObjectID )
{
	if( owner )
		{
		soundObjectID = inSoundObjectID;
		soundObject = owner -> GetSoundObjectByID( inSoundObjectID );
        ExtractSoundObjectValuesForDisplay();
		}
}

//bool cSoundEditor::FillBuffer( ofSoundBuffer& buffer ){ if( player.active ) player.active = player.FillBuffer( buffer ); }
