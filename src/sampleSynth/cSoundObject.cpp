//
//  cSoundObject.cpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//

#include "cSoundObject.hpp"
#include "cSampleSynth.hpp"
#include "ofSystemUtils.h"

void cSoundObject::SetSoundDefByID( int id ){ if( synth == nullptr ) return; soundDefID = id; soundDef = synth -> GetSoundDefByID( soundDefID ); }

cSoundDefinition *cSoundObject::SoundDef( void ){ return soundDef; }
// cSoundObject describes how a cSoundDefinition should be played
//

void cSoundObject::GetSaveSoundObjectFile(string prompt)
{
    ofFileDialogResult result = ofSystemSaveDialog(string("soundObject_settings.json"), prompt);
    if(result.bSuccess)
        SaveSelfToJSONFile(result.getPath());
}


void cSoundObject::SaveSelfToJSONFile(string path)
{
   Json::Value soundObject;

   if(path.length() == 0)
       currentSoundObjectFilePath = "soundObject_settings.json";
   else
       currentSoundObjectFilePath = path;
   UpdateJSONSettings(soundObject, 0, true);

   ofxJSONElement def(soundObject);
   def.save(currentSoundObjectFilePath, true);
}


void cSoundObject::UpdateJSONSettings( Json::Value& container, int32_t index, bool includeDef )
{
    if( soundDefID == -1 )
        return;
    string indexStr = to_string(index);
    container["soundObject"][indexStr]["soundDefID"] = soundDefID;
    container["soundObject"][indexStr]["retriggerDelay"] = retriggerDelay;
    container["soundObject"][indexStr]["pitch"] = pitch;
    container["soundObject"][indexStr]["smoothing"] = smoothing;
    container["soundObject"][indexStr]["fadeRate"] = fadeRate;
    container["soundObject"][indexStr]["sensitivity"] = sensitivity;
    mix.UpdateJSONSettings( container["soundObject"][indexStr] );
    if(includeDef)
        synth->GetSoundDefByID(soundDefID)->UpdateJSONSettings(container, soundDefID, true, synth->samplePathBase);
}


void cSoundObject::GetLoadSoundObjectFile(string prompt)
{
    ofFileDialogResult result = ofSystemLoadDialog(prompt);
    if(result.bSuccess)
        LoadSelfFromJSONFile(result.getPath());
}


void cSoundObject::LoadSelfFromJSONFile(string path)
{
   ofxJSON root;
   bool success = false;

   if( path.length() == 0 )
       success = root.open(string("soundObject_settings.json"));
   else
       success = root.open(path);
   if(path.length() != 0)
       currentSoundObjectFilePath = path;

   const Json::Value soundObject = root[ "soundObject" ]["0"];
   LoadFromJSONSettings(soundObject);

   //NOTE we also have a soundDef in this file
   //we should load it as a new soundDef
   //we need to reconnect the soundDefID
   int32_t fileSoundDefID = soundDefID;
   int32_t newSoundDefID = synth->FindEmptySoundDef();
   if( newSoundDefID != -1 && soundDefID >= 0)
   {
       const Json::Value soundDef = root[ "soundDef" ][to_string(fileSoundDefID)];
       synth->soundDefs[newSoundDefID]->LoadFromJSONSettings(soundDef, "");
       SetSoundDefByID(newSoundDefID);
   }
}


void cSoundObject::LoadFromJSONSettings( const Json::Value &soundObjectJSON )
{
	const Json::Value soundDefIDValue = soundObjectJSON[ "soundDefID" ];
	if( soundDefIDValue.type() != Json::nullValue )
		{
		soundDefID = soundDefIDValue.asInt64();
		SetSoundDefByID( soundDefID );
		}
	
	const Json::Value retriggerDelayValue = soundObjectJSON[ "retriggerDelay" ];
    if( retriggerDelayValue.type() != Json::nullValue )
		retriggerDelay = retriggerDelayValue.asFloat();

    const Json::Value smoothingValue = soundObjectJSON[ "smoothing" ];
    if( smoothingValue.type() != Json::nullValue )
        smoothing = smoothingValue.asFloat();

    const Json::Value fadeRateValue = soundObjectJSON[ "fadeRate" ];
    if( fadeRateValue.type() != Json::nullValue )
		fadeRate = fadeRateValue.asFloat();

    const Json::Value sensitivityValue = soundObjectJSON[ "sensitivity" ];
    if( sensitivityValue.type() != Json::nullValue )
        sensitivity = sensitivityValue.asFloat();

    const Json::Value pitchValue = soundObjectJSON[ "pitch" ];
	if( pitchValue.type() != Json::nullValue )
		pitch = pitchValue.asFloat();
	
	const Json::Value mixJSON = soundObjectJSON[ "mix" ];
	mix.LoadFromJSONSettings( mixJSON );
}
