#ifndef CPLAYER_H
#define CPLAYER_H
#pragma once
#include "cSoundDefinition.h"
#include "cSoundObject.hpp"
#include "cMix.hpp"

enum T_PlayMode
{
	t_triggeredPlayMode,
	t_collisionListPlayMode
};

class cPlayer
{
public:
    double position;
    int soundObjectID;
    cSoundObject *soundObject;
    cSoundDefinition *soundDef;
    bool claimed;
    bool active;
    bool looping;
    bool sustain;
    float envelope;
    float currentLevel;
	float targetLevel;
	float attackRate;
	float decayRate;
    float currentDecayRate;
	float currentAttackRate;
	float fadeRateSmoothing;
    float age;
	float maxTarget;
    uint64_t lastTouch;
	float frameTimeSpan;
    float releaseThreshold;
    T_EnvelopeMode envelopeMode;
	T_PlayMode playMode;
    ofSoundBuffer localBuffer;

    cSoundDefinition *SoundDef( void ){ if( soundObject == nullptr )return nullptr; return soundObject -> soundDef; }

	cPlayer(){ maxTarget = 0.0; frameTimeSpan = 1.0 / 44100.0; fadeRateSmoothing = 0.995; attackRate = 0.1; decayRate = currentDecayRate = 0.1; targetLevel = 0.0; soundObjectID = 0; currentLevel = 1.0; releaseThreshold = 0.001; position = 0.0; age = 0.0; claimed = false; active = false; looping = true; sustain = false; envelopeMode = t_EnvelopeIdle; envelope = 0.0; soundObject = nullptr; }
    bool SetSoundObject( cSoundObject *inSoundObject );
    bool AddToBuffer( ofSoundBuffer& buffer, float masterVolume );
    void SetSustain( bool inSustain ){ sustain = inSustain;}
    void SetRelease( void ){ envelopeMode = t_EnvelopeReleaseMode;}
    void Stop( void ){ active = false; }
    void Release( void ){ active = false; claimed = false; soundObject = nullptr; }

	void PlayFromCollisionList( cSoundObject *inSoundObject, int inSoundObjectID, float inTargetLevel = 1.0 )
	{
		if( inSoundObject == nullptr )
			return;
		SetSoundObject( inSoundObject, inSoundObjectID );
		cSoundDefinition *soundDef = SoundDef();
		if( soundDef == nullptr || soundDef -> SampleLoaded() == false )
			return;
		playMode = t_collisionListPlayMode;
		envelopeMode = t_EnvelopeInteractiveMode;
		attackRate = soundDef -> attackRate;
		float sampleDecayTime = (float)soundDef -> sampleRate * soundObject -> fadeRate;
		decayRate = 1.0 / sampleDecayTime;
		Play( inSoundObject, inSoundObjectID, inTargetLevel, soundDef );
	}
	
	void SetSoundObject( cSoundObject *inSoundObject, int inSoundObjectID )
	{
		soundObject = inSoundObject;
		soundObjectID = inSoundObjectID;
	}
	
	void Trigger( cSoundObject *inSoundObject, int inSoundObjectID, float inTargetLevel = 1.0 )
	{
		if( inSoundObject == nullptr )
			return;
		SetSoundObject( inSoundObject, inSoundObjectID );
		cSoundDefinition *soundDef = SoundDef();
		if( soundDef == nullptr || soundDef -> SampleLoaded() == false )
			return;
		playMode = t_triggeredPlayMode;
		envelopeMode = t_EnvelopeAttackMode;
		attackRate = soundDef -> attackRate;
		decayRate = soundDef -> releaseRate;
		Play( inSoundObject, inSoundObjectID, inTargetLevel, soundDef );
	}

    void Play( cSoundObject *inSoundObject, int inSoundObjectID, float inTargetLevel, cSoundDefinition *soundDef )
    {
        Stop();
		maxTarget = 0.0;
		position = soundDef -> startSample;
		currentAttackRate = attackRate * inTargetLevel;
		currentDecayRate = decayRate * inTargetLevel;
        envelope = 0.0;
        currentLevel = 0;
		targetLevel = inTargetLevel;
        sustain = true;
        active = true;
    }
	
    void SetLoop( float in, float out, float fade );
    void SetSpeed( float inSpeed );
    void SetVolume( float inVolume );
    void ScaleBufferRange( ofSoundBuffer& buffer, size_t start, size_t end, float scaler );
    void ZeroBufferRange( ofSoundBuffer& buffer, size_t start, size_t end );
    void ShowPlayPosition( bool editorVisible );

    void SmoothUpdateLevel(float inLevel);
    void UpdateLevel( float newLevel, uint64_t inTime);;
    void UpdateInteraction(uint64_t currentTime);

};

#endif // CPLAYER_H
