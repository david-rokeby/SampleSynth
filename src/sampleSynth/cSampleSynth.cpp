//
//  cSampleSynth.cpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-25.
//

#include "cSampleSynth.hpp"
//#include "vns.hpp"
#include "ofSoundStream.h"
#include "ofSystemUtils.h"


cSampleSynth::cSampleSynth( int numSoundDefs, int numPlayers, int numObjects )
{
	samplePathBase = "";
	for( int i = 0; i < numPlayers; i ++ )
		{
		cPlayer *tempPlayer = new cPlayer;
		players.push_back(tempPlayer);
		}
	for( int i = 0; i < numSoundDefs; i ++ )
		{
		cSoundDefinition *tempSoundDef = new cSoundDefinition;
		soundDefs.push_back(tempSoundDef);
		}
	for( int i = 0; i < numObjects; i ++ )
		{
		cSoundObject *tempSoundObject = new cSoundObject( this );
		soundObjects.push_back(tempSoundObject);
		}
	for( int i = 0; i < numObjects; i ++ )
		{
		assignedPlayers.push_back(static_cast<cPlayer *>( nullptr ) );
		}
	editor.SetOwner( this );
	masterVolume = 1.0;
	channelOffset = 0;
    LoadFromJSONSettings(string(""));
    oscPort = 2500;
    receiver.setup(oscPort);
    receivedOSC=false;
//    ofAddListener(receiver.onMessageReceived, this, &cSampleSynth::onMessageReceived);
 //   editor.AttachToSoundObjectByID(0);
}


void cSampleSynth::Clear(void)
{
    int numSoundDefs;
    int numObjects;

    numSoundDefs = soundDefs.size();
    numObjects = soundObjects.size();
    soundDefs.clear();
    soundObjects.clear();

    for( int i = 0; i < numSoundDefs; i ++ )
        {
        cSoundDefinition *tempSoundDef = new cSoundDefinition;
        soundDefs.push_back(tempSoundDef);
        }
    for( int i = 0; i < numObjects; i ++ )
        {
        cSoundObject *tempSoundObject = new cSoundObject( this );
        soundObjects.push_back(tempSoundObject);
        }
}


cSampleSynth::~cSampleSynth( void )
{
	ofSoundStreamClose();

	for( vector<cPlayer *>::iterator it = players.begin() ; it != players.end(); ++ it)
		{
		cPlayer *tempPlayer = *it;
		if( tempPlayer )
			{
			tempPlayer -> Stop();
			delete tempPlayer;
			}
		}
	for( vector<cSoundObject *>::iterator it = soundObjects.begin() ; it != soundObjects.end(); ++ it)
		{
		cSoundObject *tempSoundObject = *it;
		if( tempSoundObject )
			delete tempSoundObject;
		}
	for( vector<cSoundDefinition *>::iterator it = soundDefs.begin() ; it != soundDefs.end(); ++ it)
		{
		cSoundDefinition *tempSoundDef = *it;
		if( tempSoundDef )
			delete tempSoundDef;
		}
}

void cSampleSynth::Setup( void )
{
	audiofile.setVerbose(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	// we can get devices
	std::vector<ofSoundDevice> devices = ofSoundStreamListDevices();
	
	bool found = false;
	int chosen = 0;
	int foundChannels = 0;
    for( int i = 0; i < devices.size(); i ++ )
		{
		if( devices[ i ].outputChannels > foundChannels )
			{
			found = true;
			foundChannels = devices[ i ].outputChannels;
			deviceName = devices[ i ].name;
			chosen = i;
			}
        }
//    chosen = 0;
    found = true;

	// audio setup for testing audio file stream
	ofSoundStreamSettings settings;
	
	if( found == true )
		{
        settings.setOutDevice( devices[ chosen ] );
		settings.numOutputChannels = devices[ chosen ].outputChannels;
		if( devices[ chosen ].name == "MOTU: MOTU UltraLite mk3 Hybrid"	)
			{
			if( devices[ chosen ].outputChannels >= 10 ) // HACK!!!!
				{
				settings.numOutputChannels = 10;
				channelOffset = 2;
				}
			}
		else if( devices[ chosen ].name == "MOTU: UltraLite-mk4"	)
			{
			if( devices[ chosen ].outputChannels >= 10 ) // HACK!!!!
				{
				settings.numOutputChannels = 10;
				channelOffset = 2;
				}
			}
		else
			{
			if( devices[ chosen ].outputChannels >= 8 ) // HACK!!!!
				settings.numOutputChannels = 8;
			}
		}
	else
		settings.numOutputChannels = 2;
	
	for( int i = 0; i < soundObjects.size(); i ++ )
		soundObjects[ i ] -> mix.channelOffset = channelOffset;
	
	outChannelCount = settings.numOutputChannels;
	
	float sampleRate = 44100.0;
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numInputChannels = 0;
	settings.bufferSize = 512;
	
	ofSoundStreamSetup(settings);
	
	editor.SetupGUI();
	editor.AttachToSoundObjectByID( 0 );
    ofSoundStreamStart();
}


void cSampleSynth::CreateFromFolder(void)
{
    ofFileDialogResult result = ofSystemLoadDialog("Select a folder of samples", true);
    if(result.bSuccess)
    {
        Clear();
        ofDirectory dir(result.getPath());
        samplePathBase = result.getPath();
        dir.allowExt("aif");
        dir.allowExt("wav");
//        dir.allowExt("aiff");
        dir.listDir();
        dir.sort();
        for(int32_t i = 0; i < dir.size(); i ++)
        {
            string path = samplePathBase + "/" + dir[i].getFileName();
            soundDefs[i]->LoadSample(path, true);
            soundDefs[i]->loop = true;
            //create new sound object
            soundObjects[i]->SetSoundDefByID(i);
        }
        editor.AttachToSoundObjectByID(0);
    }
}


void cSampleSynth::Update( void )
{
    if( receiver.hasWaitingMessages())
    {
        ofxOscMessage message;
        while(receiver.getNextMessage(message))
        {
            onMessageReceived(message);
        }
    }
	editor.UpdateGUI();
}


void cSampleSynth::Draw( void )
{
	editor.ShowGUI();
}


void cSampleSynth::ReleasePlayer(cPlayer *inPlayer)
{
    int32_t soundObjID = inPlayer->soundObjectID;
    if(soundObjID >= 0 && soundObjID < assignedPlayers.size())
        assignedPlayers[soundObjID] = nullptr;
    inPlayer->Release();
}

void cSampleSynth::FillBuffer( ofSoundBuffer& buffer )
{
	for( vector<cPlayer *>::iterator it = players.begin() ; it != players.end(); ++ it)
		{
		cPlayer *tempPlayer = *it;
		if( tempPlayer )
			{
			if( tempPlayer -> active )
				{
				tempPlayer -> AddToBuffer( buffer, masterVolume );
				if( tempPlayer -> active == false )
                    ReleasePlayer(tempPlayer);
				}
			}
		}
//	cSoundDefinition *soundDef = editor.SoundDef();
//	if( soundDef && soundDef -> SampleLoaded() )
//		editor.FillBuffer( buffer );
}


cPlayer *cSampleSynth::ChooseIdlePlayer( void )
{
	for( vector<cPlayer *>::iterator it = players.begin() ; it != players.end(); ++ it)
		{
		cPlayer *tempPlayer = *it;
		if( tempPlayer )
			{
			if( tempPlayer -> claimed == false )
				{
				tempPlayer -> claimed = true;
				return tempPlayer;
				}
			}
		}
	return nullptr;
}

cPlayer *cSampleSynth::Trigger( int inSoundObjectID, float level )
{
	if( inSoundObjectID < soundObjects.size() )
		{
		cSoundObject *thisSoundObj = soundObjects[ inSoundObjectID ];
		if( thisSoundObj -> SoundDef() != nullptr )
			{ 
			cPlayer *chosenPlayer = ChooseIdlePlayer();
			if( chosenPlayer )
				{
                assignedPlayers[inSoundObjectID]=chosenPlayer;
				chosenPlayer -> Trigger( thisSoundObj, inSoundObjectID, level );
				return chosenPlayer;
				}
			}
		}
	return nullptr;
}


cPlayer *cSampleSynth::PlayInteractive( int inSoundObjectID, float level )
{
	if( inSoundObjectID < soundObjects.size() )
		{
		cSoundObject *thisSoundObj = soundObjects[ inSoundObjectID ];
		if( thisSoundObj -> SoundDef() != nullptr )
			{
				cPlayer *chosenPlayer = ChooseIdlePlayer();
				if( chosenPlayer )
					{
					assignedPlayers[inSoundObjectID]=chosenPlayer;
					chosenPlayer -> PlayFromCollisionList( thisSoundObj, inSoundObjectID, level );
					return chosenPlayer;
					}
			}
		}
	return nullptr;
}


void cSampleSynth::UpdateJSONSettings(Json::Value& synthDef)
{
    bool useFullPath;

    // do sound device settings here.
    //	getOutDevice
    //	const ofSoundDevice *outDevice
    synthDef["synth"]["soundDefCount"] = (Json::Value::UInt64)soundDefs.size();
    synthDef["synth"]["samplePathBase"] = samplePathBase;
    if( samplePathBase.size() == 0 )
        useFullPath = true;

    for( size_t i = 0; i < soundDefs.size(); i ++ )
        {
        soundDefs[ i ] -> UpdateJSONSettings( synthDef["synth"], i, useFullPath, samplePathBase );
        }

    synthDef["synth"]["soundObjectCount"] = (Json::Value::UInt64)soundObjects.size();
    for( size_t i = 0; i < soundObjects.size(); i ++ )
        {
        soundObjects[ i ] -> UpdateJSONSettings( synthDef["synth"], i, false );
        }

}


void cSampleSynth::SaveToJSONSettings( string path )
{
	bool useFullPath = false;
    Json::Value synthDef;
    UpdateJSONSettings(synthDef);

    if(path.length() == 0)
        currentSynthFilePath = "synth_settings.json";
    else
        currentSynthFilePath = path;
 //   Settings::get().save(currentSynthFilePath);
    ofxJSONElement def(synthDef);
    def.save(currentSynthFilePath, true);
 }


void cSampleSynth::GetLoadSynthFile(string prompt)
{
    ofFileDialogResult result = ofSystemLoadDialog(prompt);
    if(result.bSuccess)
        LoadFromJSONSettings(result.getPath());
}


void cSampleSynth::GetSaveSynthFile(string prompt)
{
    ofFileDialogResult result = ofSystemSaveDialog(string("synth_settings.json"), prompt);
    if(result.bSuccess)
        SaveToJSONSettings(result.getPath());
}


void cSampleSynth::LoadFromJSONSettings( string path )
{
	ofxJSON root;
	samplePathBase = "";
    bool success = false;

    if( path.length() == 0 )
        success = root.open(string("synth_settings.json"));
    else
        success = root.open(path);

	if( success )
		{
        if( path.length() == 0 )
            currentSynthFilePath = "synth_settings.json";
        else
            currentSynthFilePath = path;

		const Json::Value synth = root[ "synth" ];
		if( synth.type() != Json::nullValue )
			{
			const Json::Value samplePathBaseVal = synth[ "samplePathBase" ];
			if( samplePathBaseVal.type() == Json::stringValue )
				samplePathBase = samplePathBaseVal.asString();
			
			const Json::Value soundDefCountVal = synth[ "soundDefCount" ];
			if( soundDefCountVal.type() != Json::nullValue )
				{
				uint64_t soundDefCount = soundDefCountVal.asUInt64();
				const Json::Value soundDefList = synth[ "soundDef" ];
				if( soundDefList.type() != Json::nullValue )
					{
					for( size_t i = 0; i < soundDefCount; i ++ )
						{
						const Json::Value thisSoundDef = soundDefList[ to_string( i ) ];
						if( thisSoundDef.type() != Json::nullValue )
							soundDefs[ i ] -> LoadFromJSONSettings( thisSoundDef, samplePathBase );
						}
					}
				}
			
			const Json::Value soundObjectCountVal = synth[ "soundObjectCount" ];
			if( soundObjectCountVal.type() != Json::nullValue )
				{
				uint64_t soundObjectCount = soundObjectCountVal.asUInt64();
				const Json::Value soundObjectList = synth[ "soundObject" ];
				if( soundObjectList.type() != Json::nullValue )
					{
					for( size_t i = 0; i < soundObjectCount; i ++ )
						{
						const Json::Value thisSoundObject = soundObjectList[ to_string( i ) ];
						if( thisSoundObject.type() != Json::nullValue )
							soundObjects[ i ] -> LoadFromJSONSettings( thisSoundObject );
						}
					}
				}
			}
		}
}


int32_t cSampleSynth::FindEmptySoundDef(void)
{
    for(int i = 0; i < soundDefs.size(); i ++)
    {
        if(soundDefs[i]->buffer==nullptr)
            return i;
    }
    return -1;

}

void cSampleSynth::HandleKeys( int key )
{
	if( key == ' ')
		{
		editor.TogglePlay();
		}
	if( key == '/')
		{
		Trigger( 0 );
		}
	if( key == 'd')
		{
//		if( editor.ToggleUI())
//			ofSetWindowShape(1024, windowHeight + editor.guiHeight);
//		else
//			ofSetWindowShape(1024, windowHeight);
		}
	if( key == 's')
		{
        Settings settings;
        SaveToJSONSettings(string(""));
		}
	if( key == 'r')
		{
        LoadFromJSONSettings(string(""));
		}
}

void cSampleSynth::MouseDragged( int x, int y, int button )
{
	if( editor.IsVisible() == false || x < ofGetWidth() )
		editor.MouseDragged( x, y, button );
}

void cSampleSynth::MouseScrolled( int scrollY )
{
    if( editor.IsVisible() == true )
        editor.MouseScrolled( scrollY );
}

void cSampleSynth::MousePressed( int x, int y, int button )
{
	if( editor.IsVisible() == false || x < ofGetWidth() )
		editor.MousePressed( x, y, button );
}

void cSampleSynth::MouseReleased( int x, int y, int button )
{
	if( editor.IsVisible() == false || x < ofGetWidth() )
		editor.MouseReleased( x, y, button );
}

void cSampleSynth::audioOut(ofSoundBuffer & buffer)
{
	FillBuffer( buffer );
	//	if( soundEditor )
	//	soundEditor -> FillBuffer( buffer );
}


void cSampleSynth::UpdatePlayers(uint64_t currentTime)
{
	for(cPlayer *p:players)
		{
		if(p->active)
			{
			if( p->playMode == t_collisionListPlayMode)
				p -> UpdateInteraction(currentTime);
			if(p->active==false)
				{
				p->claimed=false;
				assignedPlayers[p->soundObjectID] = nullptr;
				}
			}
		else if(p->claimed && p->soundObject)
			{
			assignedPlayers[p->soundObjectID] = nullptr;
			}
		}
}


void cSampleSynth::onMessageReceived(ofxOscMessage &msg)
{
    string addr = msg.getAddress();
    size_t numArgs = msg.getNumArgs();
    uint64_t currentTime = ofGetSystemTimeMillis();

	if(addr.compare("/heart_beat") == 0)
	{
		UpdatePlayers(currentTime);
	}
    else if(addr.compare("/collision_list") == 0)
    {
        receivedOSC = true;
        for(int i = 0; i < numArgs; i +=2)
        {
            int32_t soundObjectID = msg.getArgAsInt32(i);
            if(soundObjectID < soundObjects.size())
            {
                float sense = 1.0 / soundObjects[soundObjectID]-> sensitivity;
                sense = sense * sense;
				float impulse = 0.0;
				if(msg.getArgType(i + 1) == OFXOSC_TYPE_FLOAT)
                	impulse = msg.getArgAsFloat( i + 1 ) * sense;
				else if(msg.getArgType(i + 1) == OFXOSC_TYPE_INT32)
					impulse = ((float)msg.getArgAsInt32( i + 1 )) * sense;
				if(impulse > 1000.f)
					impulse = 1000.0f;
                if(impulse > 1.0f)
                    impulse = 1.0f;
                //locate player for this object
         		if(assignedPlayers[soundObjectID] && assignedPlayers[soundObjectID] -> active )
					assignedPlayers[soundObjectID] -> UpdateLevel(impulse, currentTime);
				else if( soundObjects[soundObjectID] -> SoundDef() != nullptr && impulse > 0 )
				{
					cPlayer *p = PlayInteractive( soundObjectID, impulse );
					p -> UpdateLevel(impulse, currentTime);
				}

            }
        }
		UpdatePlayers(currentTime);
    }
}


// on collisions_list
//  for each collision
//      is object active?
//          if so, set new interactiveLevel (with smoothing), adjust faderate to new level
//      if not
//          trigger object and set initial interactionLevel and fadeRate
//
//  for each active player
//      if touched by collisionList... OK
//      else
//          fade interactiveLevel by fadeRate
//          if at zero for a certain period of time, stop play
//          increase age

/*void cSampleSynth::ReceiveImpulses( vector<cImpulse *>impulses )
{
	int count = impulses.size();
	if( count > soundObjects.size() )
		count = soundObjects.size();
	
	//update active levels
	for( int32_t i = 0; i < players.size(); i ++ )
		{
		if( players[ i ] -> active )
			{
			cPlayer *player = players[ i ];
			cImpulse *impulse = impulses[ player -> soundObjectID ];
			if( impulse -> state == t_ImpulseNewRelease )
				{
				impulse -> AckOff();
				player -> SetRelease();
				player -> SetSustain( false );
				}
			player -> currentLevel = impulse -> outputVal;
			}
		}
	
	// look for ending samples
	for( int i = 0; i < count; i ++ )
		{
		cImpulse *impulse = impulses[ i ];
		switch( impulse -> state )
			{
			case t_ImpulseNewRelease:
				//missed (not active?)
				impulse -> AckOff();
				break;
				
			case t_ImpulseNewOnset:
				impulse -> AckOn();
				cSoundObject *sound = soundObjects[ i ];
				if( sound -> SoundDef() != nullptr )
					Play( i, impulse -> outputVal );
				break;
			}
		}
}*/
