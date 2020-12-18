//
//  cSoundObject.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//
#pragma once
#ifndef cSoundObject_hpp
#define cSoundObject_hpp

#include <stdio.h>
#include "cSoundDefinition.h"
#include "cMix.hpp"
class cSampleSynth;
class cSoundDefinition;

class cSoundObject
{
public:
	cSampleSynth *synth;
	int soundDefID;
	cSoundDefinition *soundDef;
	cMix mix;
	float pitch;
	float retriggerDelay;
	float fadeRate;
	float sampleDecayRate;
	float smoothing;
	float sensitivity;
	string currentSoundObjectFilePath;

	cSoundObject( cSampleSynth *inSynth )
	{
		synth = inSynth;
		soundDefID = -1;
		soundDef = nullptr;
		mix.InitStereo();
		retriggerDelay = 0.0;
		pitch = 1.0;
		smoothing = 0.925;
		fadeRate = .25;
		sampleDecayRate = fadeRate / 44100.0f;
		sensitivity = 16.0f;
	}

	void SetSoundDefByID( int id );
	cSoundDefinition *SoundDef( void );
	void SetPan( float inPan ){ mix.SetStereoPan( inPan ); }
	void SetVolume( float inVolume ){ mix.globalLevel = inVolume; }
	void SetRetriggerDelay( float inDelay ){ retriggerDelay = inDelay; }
	void LoadFromJSONSettings( const Json::Value &soundObjectJSON );
	void SaveSelfToJSONFile(string path);
	void GetSaveSoundObjectFile(string prompt);
	void LoadSelfFromJSONFile(string path);
	void GetLoadSoundObjectFile(string prompt);
	void UpdateJSONSettings( Json::Value& synth, int32_t index, bool includeDef );
};
#endif /* cSoundObject_hpp */
