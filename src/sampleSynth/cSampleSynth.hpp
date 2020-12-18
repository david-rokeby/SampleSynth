//
//  cSampleSynth.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-25.
//

#ifndef cSampleSynth_hpp
#define cSampleSynth_hpp
#pragma once
#include <stdio.h>
#include "cSoundDefinition.h"
#include "cPlayer.h"
#include "cSoundEditor.hpp"
#include "ofxJsonSettings.h"
#include "ofxOsc.h"
//#include "vns.hpp"

class cSoundObject;

class cSampleSynth
{
public:
	string deviceName;
	int outChannelCount;
	int channelOffset;

        string currentSynthFilePath;
	
	vector<cPlayer *> players;
	vector<cPlayer *> assignedPlayers;
	vector<cSoundDefinition *> soundDefs;
	vector<cSoundObject *> soundObjects;
	cSoundEditor editor;
	string samplePathBase;
	ofxAudioFile audiofile;
	float masterVolume;

        ofxOscReceiver receiver;
        int32_t oscPort;
        bool receivedOSC;

	cSampleSynth( int numSoundDefs, int numSoundObjects, int numPlayers );
	~cSampleSynth( void );
	void CreateFromFolder(void);
	void Clear(void);
	void Setup( void );
	void Draw( void );
	void Update( void );
	void FillBuffer( ofSoundBuffer& buffer );
	bool LoadSoundDefinition( cSoundSettings *soundSpec );
	cSoundDefinition *GetSoundDefByID( int id ){ if( id < soundDefs.size() ) return soundDefs[ id ]; return nullptr; }
	cSoundObject *GetSoundObjectByID( int id ){ if( id < soundObjects.size() ) return soundObjects[ id ]; return nullptr; }
	cPlayer *ChooseIdlePlayer( void );
	cPlayer *Trigger( int soundObjectID ){ return Trigger( soundObjectID, 1.0 ); }
	cPlayer *Trigger( int soundObjectID, float level );
	cPlayer *PlayInteractive( int soundObjectID ){ return PlayInteractive( soundObjectID, 1.0 ); }
	cPlayer *PlayInteractive( int soundObjectID, float level );

	void GetLoadSynthFile(string prompt);
	void GetSaveSynthFile(string prompt);
	void SaveToJSONSettings( string path );
	void UpdateJSONSettings(Json::Value& settings);

	void LoadFromJSONSettings( string path );
	void HandleKeys( int key );
	void MouseDragged( int x, int y, int button );
	void MousePressed( int x, int y, int button );
	void MouseReleased( int x, int y, int button );
	void MouseScrolled( int scrollY );
	void audioOut(ofSoundBuffer & buffer);
//	void ReceiveImpulses( vector<cImpulse *>impulses );
	void onMessageReceived(ofxOscMessage &msg);

	int32_t FindEmptySoundDef(void);
	void ReleasePlayer(cPlayer *inPlayer);
	void UpdatePlayers(uint64_t currentTime);
};
	
	

#endif /* cSampleSynth_hpp */
