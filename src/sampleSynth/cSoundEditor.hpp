//
//  cSoundEditor.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-25.
//
#pragma once
#ifndef cSoundEditor_hpp
#define cSoundEditor_hpp

#include "cSoundDefinition.h"
#include "cSoundObject.hpp"
#include "cPlayer.h"
#include "ofxDatGuiIntObject.h"
#include "cGUIElements.h"
#include "ofxDatGuiEvents.h"

//class cPlayer;
class cSampleSynth;
//class cSoundDefinition;

class cSoundEditor
{
public:
	cSampleSynth *owner;
	int soundObjectID;
	cSoundObject *soundObject;
	cPlayer *player;
	bool visible;
	int guiWidth;
	int guiHeight;
	int waveHeight;
	int selectedParam;

	float panHolder; //placeholder for panValue for ui

	azureKinectTheme guiTheme;
	vector<ofxDatGuiComponent *>guiComponents_;
	vector<ofxDatGuiComponent *>soundDefGuiComponents;
	int32_t currentGUIXStart;
	int32_t currentGUIYStart;
	int32_t sliderLabelWidth;
	int32_t guiElementHeight;
	int32_t guiElementWidth;
	int32_t soundDefGUIComponentsStartIndex;
	int32_t soundObjectGUIComponentsIndex;

	int32_t frontBackIndex;
	int32_t leftChannelIndex;
	int32_t rightChannelIndex;
	int32_t sampleStartIndex;
	int32_t sampleEndIndex;
	int32_t loopStartIndex;
	int32_t loopEndIndex;
	int32_t attackIndex;
	int32_t decayIndex;
	int32_t sampleNameIndex;

	float waveRange;
	ofRectangle rxIndicatorRect;

//	vector<cSliderParam *> guiComponents;
//	ofxDatGuiSlider *masterVolumeSlider;
//	ofxDatGuiSlider *panSlider;
//        ofxDatGuiSlider *panFrontBackSlider;
//	ofxDatGuiSlider *ampSlider;
//	ofxDatGuiSlider *objPitchSlider;
//	ofxDatGuiSlider *retriggerSlider;
//	ofxDatGuiNumericInput *whichSoundDef;
//	ofxDatGuiNumericInput *whichSoundObject;
//	cToggleLoop *loopToggle;
//        ofxDatGuiButton *loadButton;
//	ofxDatGuiLabel *sampleName;
//	ofxDatGuiDropdown* mixModeDropdown;
//	ofxDatGuiNumericInput *numOutChannels;
//	ofxDatGuiNumericInput *leftChannel;
//	ofxDatGuiNumericInput *rightChannel;

	cSoundEditor( void ){ soundObjectID = 0; owner = nullptr; soundObject = nullptr; player = nullptr; visible = true; guiWidth = 480; guiHeight = 256; selectedParam = -1; waveHeight = 256; }
	void SetOwner( cSampleSynth *inSynth ){ owner = inSynth; }
	void onCreateSynthFromFolderEvent(ofxDatGuiButtonEvent e);
	void AttachToSoundObjectByID( int soundObjectID );
	void DetachFromSoundObject( void );
	cSoundDefinition *SoundDef( void ){ if( soundObject == nullptr ) return nullptr; return soundObject -> soundDef; }
	void ShowGUI( void );
	void UpdateGUI( void );
	void UpdateGUIElements( void );
	void SetupGUI( void );
	void ShowGUIElements( void );

	void ExtractSoundDefValuesForDisplay(void );
	void ExtractSoundObjectValuesForDisplay(void );
	void Show( void ){visible = true;}
	void Hide( void){ visible = false; }
	bool IsVisible( void ){return visible; }
	void MousePressed(int x, int y, int button );
	void MouseDragged(int x, int y, int button );
	void MouseReleased(int x, int y, int button );
	void MouseScrolled( int yScroll );
	void onToggleLoopEvent(ofxDatGuiToggleEvent e);
	void onLoadSampleButtonEvent(ofxDatGuiButtonEvent e);
	bool LoadSampleAction( void );
	bool TogglePlay( void );
	bool ToggleUI( void );
	bool FillBuffer( ofSoundBuffer& buffer );
	void onSoundDefSelectEvent(ofxDatGuiTextInputEvent e);
	void onPanEvent(ofxDatGuiSliderEvent e);
//        void GetPanFunction(ofxDatGuiComponent *slider);
	void onSoundObjectSelectEvent(ofxDatGuiTextInputEvent e);
	void onAmpEvent(ofxDatGuiSliderEvent e);
	void onRetriggerDelayEvent(ofxDatGuiSliderEvent e);
	void onObjPitchEvent(ofxDatGuiSliderEvent e);
	void onMasterVolumeEvent(ofxDatGuiSliderEvent e);
	void onPanFrontBackEvent(ofxDatGuiSliderEvent e);
	void onMixModeDropdownEvent(ofxDatGuiDropdownEvent e);
	void onNumOutChannelsSelectEvent(ofxDatGuiTextInputEvent e);
	void onLeftChannelAssignEvent(ofxDatGuiTextInputEvent e);
	void onRightChannelAssignEvent(ofxDatGuiTextInputEvent e);
	void onReleaseRateEvent(ofxDatGuiSliderEvent e);
	void onSmoothingEvent(ofxDatGuiSliderEvent e);
	void onSensitivityEvent(ofxDatGuiSliderEvent e);
	void onOSCPortEvent(ofxDatGuiSliderEvent e);

	void onSampleAmpEvent(ofxDatGuiSliderEvent e);
	void onSamplePitchEvent(ofxDatGuiSliderEvent e);
	void onSampleStartEvent(ofxDatGuiSliderEvent e);
	void onSampleEndEvent(ofxDatGuiSliderEvent e);
	void onSampleAttackEvent(ofxDatGuiSliderEvent e);
	void onSampleDecayEvent(ofxDatGuiSliderEvent e);
	void onSampleDoLoopEvent(ofxDatGuiToggleEvent e);
	void onSampleLoopStartEvent(ofxDatGuiSliderEvent e);
	void onSampleLoopEndEvent(ofxDatGuiSliderEvent e);
	void onSampleCrossFadeEvent(ofxDatGuiSliderEvent e);

	void onLoadSynthEvent(ofxDatGuiButtonEvent e);
	void onSaveSynthEvent(ofxDatGuiButtonEvent e);
	void onLoadSoundObjectEvent(ofxDatGuiButtonEvent e);
	void onSaveSoundObjectEvent(ofxDatGuiButtonEvent e);
	void onLoadSoundDefEvent(ofxDatGuiButtonEvent e);
	void onSaveSoundDefEvent(ofxDatGuiButtonEvent e);

	int32_t CreateLabel(string label, int width, int32_t x_off, int32_t offset);
	int32_t CreatePlotter(string label, int32_t min, int32_t max, int width, int32_t x_off, int32_t offset);
	int32_t CreateDropDown(string label, vector<string> options, int32_t *intVariablePtr, int width, int32_t x_off, int32_t offset );
	int32_t CreateDropDown(string label, vector<string> options, _get_value_function getValueFunction, int width, int32_t x_off, int32_t offset );
	int32_t CreateButton(string label, int width, int32_t x_off, int32_t offset );
	int32_t CreateToggle(string label, bool *boolVariablePtr, int width, int32_t x_off, int32_t offset );
	int32_t CreateToggle(string label, _get_value_function getValueFunction, int width, int32_t x_off, int32_t offset );
	int32_t CreateNumericInput( string label, float *ptr, int width, int32_t x_off, int offset);
	int32_t CreateNumericInput( string label, _get_value_function getValueFunction, int width, int32_t x_off, int offset );
	int32_t CreateNumericInputInt( string label, int32_t *ptr, int width, int32_t x_off, int offset);
	int32_t CreateNumericInputInt( string label, _get_value_function getValueFunction, int width, int32_t x_off, int offset );
	int32_t CreateInt32Slider( string label, int32_t min, int32_t max, int32_t *ptr, int width, int32_t x_off, int offset);
	int32_t CreateInt32Slider( string label, int32_t min, int32_t max, _get_value_function getValueFunction, int width, int32_t x_off, int offset);
	int32_t CreateFloatSlider( string label, float min, float max, float *ptr, int width, int32_t x_off, int offset);
	int32_t CreateFloatSlider( string label, float min, float max, _get_value_function getValueFunction, int width, int32_t x_off, int offset);
//        void ExtractValuesForDisplay( void );
	void onSliderChangedEvent(ofxDatGuiSliderEvent e);
	void onDropdownEvent(ofxDatGuiDropdownEvent e);
	void onToggleChangedEvent(ofxDatGuiToggleEvent e);
	void onButtonClickedEvent(ofxDatGuiButtonEvent e);
	void onTextInputChangedEvent(ofxDatGuiTextInputEvent e);
};

//void onSoundObjectSelectEvent(cSoundEditor *owner, ofxDatGuiTextInputEvent e);
void GetSoundDefFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox);
void GetPanFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetAmplitudeFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetObjPitchFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetRetriggerDelayFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleAmpFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSamplePitchFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleStartFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleEndFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleAttackFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleDecayFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleDoLoopFunction(cSoundEditor *editor, ofxDatGuiComponent *toggle);
void GetSampleLoopStartFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleLoopEndFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSampleCrossFadeFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetMixModeFunction(cSoundEditor *editor, ofxDatGuiComponent *dropDown);
void GetPanFrontBackFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetLeftChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetRightChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetReleaseRateFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSmoothingFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetSensitivityFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);
void GetOSCPortFunction(cSoundEditor *editor, ofxDatGuiComponent *slider);



#endif /* cSoundEditor_hpp */
