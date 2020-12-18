#include "cPlayer.h"
#include "ofSoundBuffer.h"
#include <math.h>
// add another layer that allows channel assign and mix...

bool cPlayer::SetSoundObject( cSoundObject *inSoundObject )
{
	soundObject = inSoundObject;
    return true;
}


bool cPlayer::AddToBuffer( ofSoundBuffer& buffer, float masterVolume )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return false;
	
	if( localBuffer.getNumFrames() != buffer.getNumFrames() )
		localBuffer.allocate( buffer.getNumFrames(), 2 );
	if( localBuffer.getNumFrames() != buffer.getNumFrames() )
		return false;
	
    if( active && soundDef && soundDef -> SampleLoaded() )
    	{
        active = soundDef -> FillBuffer( localBuffer, &position, sustain, soundObject -> pitch );
		float levelChange = targetLevel - currentLevel;
		size_t destBufferSize = localBuffer.getNumFrames();
		float rate = 0.0f;
		switch( envelopeMode )
			{
			case t_EnvelopeInteractiveMode:
				if( levelChange < 0 )
					{
					rate = - currentDecayRate;
					//if( (float)destBufferSize * rate < levelChange )
					//	rate = levelChange / (float)destBufferSize;
					}
				else
					{
					rate = currentAttackRate;
					//if( (float)destBufferSize * rate > levelChange )
					//	rate = levelChange / (float)destBufferSize;
					}
				break;
				
			case t_EnvelopeAttackMode:
				rate = currentAttackRate;
				break;
				
			case t_EnvelopeReleaseMode:
				rate = - currentDecayRate;
				break;
				
			case t_EnvelopeSustainMode:
				rate = 0;
				break;
				
			case t_EnvelopeIdle:
				currentLevel = 0.0;
				break;
			}
		currentLevel = soundDef -> ApplyEnvelope(localBuffer, position, envelope, &envelopeMode, currentLevel * masterVolume, rate * masterVolume, targetLevel * masterVolume );
		if( masterVolume != 0 )
			currentLevel /= masterVolume;
		if( envelopeMode != t_EnvelopeInteractiveMode )
			envelope = currentLevel / targetLevel;
//			currentLevel += (rate * destBufferSize);
		if( envelopeMode == t_EnvelopeInteractiveMode && currentLevel < releaseThreshold )
			{
			currentLevel = 0;
			}
		if(currentLevel <= 0.0)
			{
			currentLevel = 0.0;
			envelopeMode = t_EnvelopeIdle;
			}
/*		float leveledEnvelope = currentLevel * envelope;
        if( leveledEnvelope <= 0.0 )
			{
			active = false;
			envelopeMode = t_EnvelopeIdle; // is this correct, or should we give it some time before releasing?
			}*/
		soundObject -> mix.AddMix( localBuffer, buffer );
    	}
	active = ( envelopeMode != t_EnvelopeIdle );
    return active;
}


void cPlayer::SetLoop( float in, float out, float fade )
{
	cSoundDefinition *soundDef = SoundDef();
    Stop();
	if( soundDef && soundDef -> SampleLoaded() )
		{
		float samples = static_cast<float>(soundDef -> sampleCount);
		soundDef -> SetLoop( in * samples, out * samples, fade );
		}
}


void cPlayer::SetSpeed( float inSpeed )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef )
		soundDef -> pitch = inSpeed;
}


void cPlayer::SetVolume( float inVolume )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef )
		soundDef -> amplitude = inVolume;
}


void cPlayer::ShowPlayPosition( bool editorVisible )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr || soundDef -> SampleLoaded() == false )
		return;
	ofNoFill();
	ofColor white( 255, 255, 255 );
	ofSetColor( white );
	float height = ofGetHeight();
	if( editorVisible )
		height -= 300;

	if(	active)
		{
		uint64_t pos = static_cast<uint64_t>(position);
		if( pos > soundDef -> sampleCount )
			pos = pos - soundDef -> sampleCount + soundDef -> exitLoopCrossFadeSample;
		float phx = ofMap( pos, 0, soundDef -> sampleCount, 0, ofGetWidth() );
		ofDrawLine( phx, 0, phx, ofGetHeight());
		}
}


void cPlayer::SmoothUpdateLevel(float inLevel)
{
	if( soundObject)
		targetLevel = targetLevel * soundObject -> smoothing + inLevel * (1.0 - soundObject -> smoothing);
	if( targetLevel > 10000)
		targetLevel = 256;
//    	currentLevel = currentLevel * soundObject -> smoothing + inLevel * (1.0 - soundObject -> smoothing);
}


// called only when player received impulse

void cPlayer::UpdateLevel( float newLevel, uint64_t inTime)
{
	if( soundObject)
		{
		if(newLevel > 0)
			lastTouch = inTime;
		SmoothUpdateLevel(newLevel);
		if(newLevel > maxTarget)
			{
			maxTarget = newLevel;
			currentDecayRate = decayRate * newLevel;
			currentAttackRate = attackRate * newLevel;
			}
		else
			{
//			currentDecayRate = currentDecayRate * fadeRateSmoothing + decayRate * newLevel * ( 1.0 - fadeRateSmoothing );
			}
		}
}


//called on all active players only on receipt of interactions

void cPlayer::UpdateInteraction(uint64_t currentTime)
{
    if(active)
    {
        if(lastTouch != currentTime)
        {
		// interactive level always rises at a max of attack slew rate and falls at max decay slew rate
		// we should not smooth here, but use decay rate
//			double elapsedSamples = (double)(currentTime - lastTouch) / 1000.0 / soundDef -> sampleRate;
//            float proposedLevel = targetLevel - ( currentDecayRate * elapsedSamples );
//            if(proposedLevel < 0)
//                proposedLevel = 0;
//			targetLevel = proposedLevel;
 //           SmoothUpdateLevel(propo  sedLevel);
			targetLevel = 0;
			if(currentLevel < releaseThreshold)
            {
                currentLevel = 0;
			// Release will be called by Release();
            }
        }
/*        else
        {
//			if(envelopeMode == t_EnvelopeReleaseMode)
//				envelopeMode = t_EnvelopeAttackMode;
			//else we do nothing, since the Smooth has already happened
        }*/
    }
}
