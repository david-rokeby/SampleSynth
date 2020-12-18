#include <string.h>
#include <math.h>

#include "cSoundDefinition.h"

//#include "cSoundEditor.hpp"
#include "cPlayer.h"
#include <cstdlib>
#include "cSampleSynth.hpp"

cSoundDefinition::cSoundDefinition()
{
	editor = nullptr;
    buffer = nullptr;
    sampleRate = 44100;
    channelCount = 0;
    sampleCount = 0;
    duration = 0.0; //seconds
    allocatedSize = 0;
    pitch = 1.0;
    amplitude = 1.0;
    SetAttackTime( 0.1 );
    startSample = 0.0;
    loop = false;
    startLoopSample = 0.0;
    endLoopSample = 0.0;
    loopCrossFadeFraction = .1; //of loop
    endSample = 0.0;
    SetReleaseTime( 0.1 );
    endSample = 0.0;

    enterLoopCrossFadeSample = 0.0;
    exitLoopCrossFadeSample = 0.0;

    crossFadeStartSample = 0;
    crossFadeEndSample = 0;
    crossFadeSamples = 0;
    envelopeMode = t_EnvelopeIdle;
}


void cSoundDefinition::Init( ofxAudioFile& file, cSoundSettings* settings )
{
    sampleRate = file.samplerate();
    channelCount = file.channels();
    sampleCount = file.length();
    duration = ( static_cast<double>(sampleCount) / static_cast<double>(sampleRate) );

    if( buffer )
    {
        free( static_cast<void *>(buffer) );
        buffer = nullptr;
    }
	
	double sampleCountDouble = static_cast<double>(sampleCount);
	if( settings )
		{
		startSample = settings -> startFraction * sampleCountDouble;
		endSample = settings -> endFraction * sampleCountDouble;
		startLoopSample = settings -> loopStartFraction * sampleCountDouble;
		endLoopSample = settings -> loopEndFraction * sampleCountDouble;
		uint64_t crossFadeSamples = LoopCrossFadeSamples( settings -> loopCrossFadeFraction );
		loopCrossFadeFraction = settings -> loopCrossFadeFraction;
		loop = settings -> loop;
		pitch = settings -> pitch;
		amplitude = settings -> amplitude;
		SetAttackTime( settings -> attack );
		SetReleaseTime( settings -> decay );
		}

    buffer = static_cast<float *>(malloc( SamplesToBytes( sampleCount + 1 + crossFadeSamples ) ));

    if( buffer )
    {
        allocatedSize = SamplesToBytes( sampleCount + crossFadeSamples );
        memcpy( static_cast<void *>(buffer), static_cast<void *>(file.data()),  SamplesToBytes( sampleCount ) );
        endSample = sampleCount;
		if( settings )
        	CalculateLoopCrossFade();
    }
}



bool cSoundDefinition::LoadSample( string samplePath, bool init )
{
	ofxAudioFile audiofile;
//	ofFileDialogResult openFileResult= ofSystemLoadDialog("select an audio sample");
	//Check if the user opened a file
	audiofile.load( samplePath );
    if(audiofile.loaded())
    {
        cSoundSettings settings;
        if( init )
            Init( audiofile, &settings );
        else
            Init( audiofile, nullptr );
        const char *localPathName = samplePath.c_str();
        size_t len = strlen( localPathName );
        size_t stringOffset = 0;
        for( size_t i = len - 1; i > 0; i -- )
        {
            if( localPathName[i] == '/' )
            {
                stringOffset = i + 1;
                break;
            }
        }
        sampleName.assign( localPathName + stringOffset );
        pathName = samplePath;
        return true;
    }
    return false;
}

uint64_t cSoundDefinition::LoopCrossFadeSamples( float fraction )
{
	uint64_t samples = ( endLoopSample - startLoopSample ) * fraction;
	return samples;
}

void cSoundDefinition::CalculateLoopCrossFade( void )
{
    enterLoopCrossFadeSample = startLoopSample + LoopCrossFadeSamples(loopCrossFadeFraction);
    exitLoopCrossFadeSample = endLoopSample - LoopCrossFadeSamples(loopCrossFadeFraction);
    if( exitLoopCrossFadeSample < enterLoopCrossFadeSample )
    {
        exitLoopCrossFadeSample = enterLoopCrossFadeSample = static_cast<uint64_t>( ( exitLoopCrossFadeSample + enterLoopCrossFadeSample ) / 2.0 );
    }

    crossFadeSamples = endLoopSample - exitLoopCrossFadeSample;

    uint64_t bufferSize = SamplesToBytes( sampleCount + crossFadeSamples + 1 ); //+ 1 to provide gap?

    if( bufferSize > allocatedSize )
    {
        void *tb = malloc( static_cast<size_t>(bufferSize) );
        float *newBuffer = static_cast<float *>(tb);
        if( newBuffer )
        {
            if( buffer )
                memcpy( static_cast<void *>(newBuffer), static_cast<void *>(buffer),  allocatedSize );
            allocatedSize = bufferSize;
            buffer = newBuffer;
        }
    }
    crossFadeStartSample = sampleCount + 1;
    crossFadeEndSample = crossFadeStartSample + crossFadeSamples;

    for( uint64_t i = 0; i < crossFadeSamples; i ++ )
    {
        float xFader = static_cast<float>(i) / static_cast<float>(crossFadeSamples);
        for( uint64_t c = 0; c < channelCount; c ++ )
        {
            buffer[ SamplesToFloats( crossFadeStartSample + i ) + c ] = buffer[ SamplesToFloats(startLoopSample + i) + c ] * xFader + buffer[ SamplesToFloats(exitLoopCrossFadeSample + i) + c ] * (1.0 - xFader);
        }
    }
}


void cSoundDefinition::SetExtent( uint64_t start, uint64_t end )
{
    if(sampleCount==0)
        return;
	bool extentAdjusted = false;
//	if( end < start )
//		end = start + 1;
    if( start < sampleCount )
        startSample = start;
	else
		{
		extentAdjusted = true;
		startSample = sampleCount - 1;
		}
    if( end <= sampleCount )
        endSample = end;
	else
		{
		extentAdjusted = true;
		endSample = sampleCount;
		}
	if( endSample <= startSample )
		{
		endSample = startSample + 1;
		extentAdjusted = true;
		}
	if( extentAdjusted && editor )
		{
        ((cFocussedSlider *)(editor -> guiComponents_[ editor -> sampleStartIndex ])) -> setValue( static_cast<float>(startSample) / static_cast<float>(sampleCount) );
        ((cFocussedSlider *)(editor -> guiComponents_[ editor -> sampleEndIndex ])) -> setValue( static_cast<float>(endSample) / static_cast<float>(sampleCount) );
//		editor -> guiComponents_[ editor -> sampleEndIndex ] -> UpdateSlider( endSample );
		}
	bool loopChanged = false;
	if( startLoopSample < startSample )
		{
		startLoopSample = startSample;
		if( editor )
            ((cFocussedSlider *)(editor -> guiComponents_[ editor -> loopStartIndex ])) -> setValue( static_cast<float>(startLoopSample) / static_cast<float>(sampleCount) );
//			editor -> guiComponents_[ editor -> loopStartIndex ] -> UpdateSlider( startLoopSample );
		loopChanged = true;
		}
	if( endLoopSample > endSample )
		{
		endLoopSample = endSample;
		if( editor )
            ((cFocussedSlider *)(editor -> guiComponents_[ editor -> loopEndIndex ])) -> setValue( static_cast<float>(endLoopSample) / static_cast<float>(sampleCount) );
//			editor -> guiComponents[ editor -> loopEndIndex ] -> UpdateSlider( endLoopSample );
		loopChanged = true;
		}
	if( loopChanged )
		SetLoop( startLoopSample, endLoopSample, loopCrossFadeFraction, false );
}


void cSoundDefinition::SetLoop( uint64_t loopStart, uint64_t loopEnd, float crossFadeFraction, bool startChanged )
{
    if( sampleCount == 0 )
        return;

//    loop = true;
	bool loopChanged = false;
	
    startLoopSample = loopStart;
    endLoopSample = loopEnd;
    if( endLoopSample > endSample )
		{
		endLoopSample = endSample;
		loopChanged = true;
		}
	else if( endLoopSample < startSample )
		{
		endLoopSample = startSample + 1; //???
		loopChanged = true;
		}
    if( startLoopSample < startSample )
		{
		startLoopSample = startSample;
		loopChanged = true;
		}
	if( startLoopSample > endSample )
		{
		startLoopSample = endSample - 1;
		loopChanged = true;
		}
	if( startChanged == false )
		{
		if( endLoopSample <= startLoopSample )
			{
			endLoopSample = startLoopSample + 1;
			loopChanged = true;
			}
		}
	else
		{
		if( startLoopSample >= endLoopSample )
			{
			startLoopSample = endLoopSample - 1;
			loopChanged = true;
			}
		}
	if( loopChanged )
		{
		if( editor )
			{
            ((cFocussedSlider *)(editor -> guiComponents_[ editor -> loopStartIndex ])) -> setValue( static_cast<float>(startLoopSample) / static_cast<float>(sampleCount) );
            ((cFocussedSlider *)(editor -> guiComponents_[ editor -> loopEndIndex ])) -> setValue( static_cast<float>(endLoopSample) / static_cast<float>(sampleCount) );
//			editor -> guiComponents[ t_ParamLoopStartIndex ] -> UpdateSlider( startLoopSample );
    //		editor -> guiComponents[ t_ParamLoopEndIndex ] -> UpdateSlider( endLoopSample );
			}
		}
    if( crossFadeFraction > 0.5 )
		crossFadeFraction = 0.5;
    loopCrossFadeFraction = crossFadeFraction;
    CalculateLoopCrossFade();
}


bool cSoundDefinition::FillBuffer( ofSoundBuffer& destBuffer, double *sourceSampleIndex, bool sustain, float inPitch )
{
    size_t destBufferSize = destBuffer.getNumFrames();

    bool alive = true;
    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destPos = 0;
    uint64_t intSourceSample;

    while( destPos < destBufferSize )
    {
        sourcePos = *sourceSampleIndex;
        if( sustain && loop )
        {
            // IF IN SECTION BEFORE LOOP CROSS FADE
            if( sourcePos < exitLoopCrossFadeSample - 1 )
            {
                FillWithSamples( destBuffer, &destPos, sourceSampleIndex, exitLoopCrossFadeSample - 1, inPitch );
                sourcePos = *sourceSampleIndex;
                if( sourcePos >= exitLoopCrossFadeSample )
                {
                    sourcePos = SwitchToCrossFade( sourceSampleIndex );
                }
            }

            // IF NEXT SAMPLE INTERPOLATION IS ACROSS CROSS FADE START SAMPLE
            intSourceSample = static_cast<uint64_t>(sourcePos);
            if( intSourceSample == exitLoopCrossFadeSample - 1 )
            {
                InsertLoopCrossFadeStartTransition( destBuffer, &destPos, sourceSampleIndex, inPitch );
                sourcePos = *sourceSampleIndex;
            }
        }
        else  // NOT SUSTAINING
        {
            if( sourcePos < endSample )
            {
                // IF NOT ALREADY IN CROSS FADE
                FillWithSamplesPadSilence( destBuffer, &destPos, sourceSampleIndex, endSample, inPitch );
                sourcePos = *sourceSampleIndex;
                if( sourcePos >= endSample )
                    alive = false;
                break;
            }
        }

        // IF IN CROSSFADE
        if( sourcePos >= crossFadeStartSample ) // fill in cross fade
        {
            FillWithSamples( destBuffer, &destPos, sourceSampleIndex, crossFadeEndSample - 1, inPitch );
            sourcePos = *sourceSampleIndex;
            if( sourcePos >= crossFadeEndSample )
            {
                sourcePos = SwitchFromCrossFade( sourceSampleIndex );
            }
        }

        // IF NEXT SAMPLE INTERPOLATION IS ACROSS CROSS FADE START SAMPLE
        intSourceSample = static_cast<uint64_t>(sourcePos);
        if( intSourceSample == crossFadeEndSample - 1 )
        {
            InsertLoopCrossFadeEndTransition( destBuffer, &destPos, sourceSampleIndex, inPitch );
            sourcePos = *sourceSampleIndex;
        }

    }
    *sourceSampleIndex = sourcePos;
    return alive;
}


void cSoundDefinition::FillWithSamples( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, uint64_t sourceSampleStop, float inPitch )
{
    size_t destBufferSize = destBuffer.getNumFrames();

    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destSample = *destSampleIndex;
    uint64_t localSampleCount = destBufferSize - destSample;
	float localPitch = inPitch * pitch;
	
    for( uint64_t outSample = destSample; outSample < destSample + localSampleCount; outSample ++ )
    {
        double fraction = modf( sourcePos, &sourceSample );
        uint64_t intSourceSample = static_cast<uint64_t>(sourceSample);
        uint64_t offset = outSample * channelCount;
        uint64_t inOffset = intSourceSample * channelCount;

        if( fraction == 0.0 )
        {
            for( uint64_t c = 0; c < channelCount; c ++ )
                destBuffer[ offset ++ ] = buffer[ inOffset ++ ];
        }
        else
        {
            for( uint64_t c = 0; c < channelCount; c ++ )
			{
				destBuffer[ offset ++ ] = ( buffer[ inOffset + channelCount ] * fraction + buffer[ inOffset ] * ( 1.0 - fraction ) );
				inOffset ++;
			}
        }
        sourcePos += localPitch;

        if( sourcePos > sourceSampleStop )
        {
            localSampleCount = outSample;
            break;
        }

    }
    *sourceSampleIndex = sourcePos;
    *destSampleIndex = destSample + localSampleCount;
}


void cSoundDefinition::FillWithSamplesPadSilence( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, uint64_t sourceSampleStop, float inPitch )
{
    size_t destBufferSize = destBuffer.getNumFrames();
    FillWithSamples( destBuffer, destSampleIndex, sourceSampleIndex, sourceSampleStop, inPitch );
    if( *destSampleIndex < destBufferSize )
    {
        FillWithSilence( destBuffer, destSampleIndex );
    }
}


void cSoundDefinition::FillWithSilence( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex )
{
    size_t destBufferSize = destBuffer.getNumFrames();
    uint64_t destSample = *destSampleIndex;
    uint64_t localSampleCount = destBufferSize - destSample;

    for( int outSample = destSample; outSample < destSample + localSampleCount; outSample ++ )
    {
        destBuffer[ outSample ] = 0.0;
    }
    *destSampleIndex = destSample + localSampleCount;
}


void cSoundDefinition::InsertLoopCrossFadeStartTransition( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, float inPitch )
{
    size_t destBufferSize = destBuffer.getNumFrames();
    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destSample = *destSampleIndex;
    float startSourceSample = buffer[ exitLoopCrossFadeSample - 1 ];
    float endSourceSample = buffer[ endLoopSample ];
    float fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
	float localPitch = pitch * inPitch;
	
    while( static_cast<uint64_t>(sourceSample) == exitLoopCrossFadeSample - 1 )
    {
        destBuffer[ destSample ] = ( startSourceSample * ( 1.0f - fraction ) + endSourceSample * fraction );
        sourcePos += localPitch;
        fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
        destSample ++;
        if( destSample == destBufferSize )
        { // still in xFade at end of destBuffer
            *sourceSampleIndex = sourcePos;
			if( sourceSample > exitLoopCrossFadeSample - 1 )
				SwitchToCrossFade( sourceSampleIndex );
            *destSampleIndex = destSample;
            return;
        }
    }
    *sourceSampleIndex = sourcePos;
    SwitchToCrossFade( sourceSampleIndex );
    *destSampleIndex = destSample;
}


void cSoundDefinition::InsertLoopCrossFadeEndTransition( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, float inPitch )
{
    size_t destBufferSize = destBuffer.getNumFrames();
    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destSample = *destSampleIndex;
    float startSourceSample = buffer[ endLoopSample - 1 ];
    float endSourceSample = buffer[ enterLoopCrossFadeSample ];
    float fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
	float localPitch = pitch * inPitch;

    while( static_cast<uint64_t>(sourceSample) == endLoopSample - 1 )
    {
        destBuffer[ destSample ] = ( startSourceSample * ( 1.0f - fraction ) + endSourceSample * fraction );
        sourcePos += localPitch;
        fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
        destSample ++;
        if( destSample == destBufferSize )
        { // still in xFade at end of destBuffer
            *sourceSampleIndex = sourcePos;
            *destSampleIndex = destSample;
            return;
        }
    }
    *sourceSampleIndex = sourcePos;
    SwitchFromCrossFade( sourceSampleIndex );
    *destSampleIndex = destSample;
}




bool cSoundDefinition::FillBuffer( float *destBuffer, uint64_t destBufferSize, double *sourceSampleIndex, bool sustain, float inPitch )
{
    bool alive = true;
    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destPos = 0;
    uint64_t intSourceSample;

    while( destPos < destBufferSize )
    {
        sourcePos = *sourceSampleIndex;
        if( sustain )
        {
            // IF IN SECTION BEFORE LOOP CROSS FADE
            if( sourcePos < exitLoopCrossFadeSample - 1 )
            {
                FillWithSamples( destBuffer, &destPos, destBufferSize, sourceSampleIndex, exitLoopCrossFadeSample - 1, inPitch );
                sourcePos = *sourceSampleIndex;
                if( sourcePos >= exitLoopCrossFadeSample )
                {
                    sourcePos = SwitchToCrossFade( sourceSampleIndex );
                }
            }

            // IF NEXT SAMPLE INTERPOLATION IS ACROSS CROSS FADE START SAMPLE
            intSourceSample = static_cast<uint64_t>(sourcePos);
            if( intSourceSample == exitLoopCrossFadeSample - 1 )
            {
                InsertLoopCrossFadeStartTransition( destBuffer, &destPos, destBufferSize, sourceSampleIndex, inPitch );
                sourcePos = *sourceSampleIndex;
            }
        }
        else  // NOT SUSTAINING
        {
            if( sourcePos < endSample )
            {
                // IF NOT ALREADY IN CROSS FADE
                FillWithSamplesPadSilence( destBuffer, &destPos, destBufferSize, sourceSampleIndex, endSample, inPitch );
                sourcePos = *sourceSampleIndex;
                if( sourcePos >= endSample )
                    alive = false;
                break;
            }
        }

        // IF IN CROSSFADE
        if( sourcePos >= crossFadeStartSample ) // fill in cross fade
        {
            FillWithSamples( destBuffer, &destPos, destBufferSize, sourceSampleIndex, crossFadeEndSample - 1, inPitch );
            sourcePos = *sourceSampleIndex;
            if( sourcePos >= crossFadeEndSample )
            {
                sourcePos = SwitchFromCrossFade( sourceSampleIndex );
            }
        }

        // IF NEXT SAMPLE INTERPOLATION IS ACROSS CROSS FADE START SAMPLE
        intSourceSample = static_cast<uint64_t>(sourcePos);
        if( intSourceSample == crossFadeEndSample - 1 )
        {
            InsertLoopCrossFadeEndTransition( destBuffer, &destPos, destBufferSize, sourceSampleIndex, inPitch );
            sourcePos = *sourceSampleIndex;
        }

    }
    *sourceSampleIndex = sourcePos;
    return alive;
}


void cSoundDefinition::FillWithSamples( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize, double *sourceSampleIndex, uint64_t sourceSampleStop, float inPitch )
{
    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destSample = *destSampleIndex;
    uint64_t localSampleCount = destBufferSize - destSample;
	float localPitch = pitch * inPitch;

    for( uint64_t outSample = destSample; outSample < destSample + localSampleCount; outSample ++ )
    {
        double fraction = modf( sourcePos, &sourceSample );
        uint64_t intSourceSample = static_cast<uint64_t>(sourceSample);
        uint64_t offset = outSample * channelCount;
        uint64_t inOffset = intSourceSample * channelCount;

        if( fraction == 0.0 )
        {
            for( uint64_t c = 0; c < channelCount; c ++ )
                destBuffer[ offset ++ ] = buffer[ inOffset ++ ];
        }
        else
        {
            for( uint64_t c = 0; c < channelCount; c ++ )
			{
                destBuffer[ offset ++ ] = buffer[ inOffset + channelCount ] * fraction + buffer[ inOffset ] * ( 1.0 - fraction );
				inOffset ++;
			}
		}
        sourcePos += localPitch;

        if( sourcePos > sourceSampleStop )
        {
            localSampleCount = outSample - destSample;
            break;
        }

    }
    *sourceSampleIndex = sourcePos;
    *destSampleIndex = destSample + localSampleCount;
}


void cSoundDefinition::FillWithSamplesPadSilence( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize, double *sourceSampleIndex, uint64_t sourceSampleStop, float inPitch )
{
    FillWithSamples( destBuffer, destSampleIndex, destBufferSize, sourceSampleIndex, sourceSampleStop, inPitch );
    if( *destSampleIndex < destBufferSize )
    {
        FillWithSilence( destBuffer, destSampleIndex, destBufferSize );
    }
}


void cSoundDefinition::FillWithSilence( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize )
{
    uint64_t destSample = *destSampleIndex;
    uint64_t localSampleCount = destBufferSize - destSample;

    for( int outSample = destSample; outSample < destSample + localSampleCount; outSample ++ )
    {
        destBuffer[ outSample ] = 0.0;
    }
    *destSampleIndex = destSample + localSampleCount;
}


void cSoundDefinition::InsertLoopCrossFadeStartTransition( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize, double *sourceSampleIndex, float inPitch )
{
    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destSample = *destSampleIndex;
    float startSourceSample = buffer[ exitLoopCrossFadeSample - 1 ];
    float endSourceSample = buffer[ endLoopSample ];
    float fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
	float localPitch = pitch * inPitch;

    while( static_cast<uint64_t>(sourceSample) == exitLoopCrossFadeSample - 1 )
    {
        destBuffer[ destSample ] = startSourceSample * ( 1.0f - fraction ) + endSourceSample * fraction;
        sourcePos += localPitch;
        fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
        destSample ++;
        if( destSample == destBufferSize )
        { // still in xFade at end of destBuffer
            *sourceSampleIndex = sourcePos;
            *destSampleIndex = destSample;
            return;
        }
    }
    *sourceSampleIndex = sourcePos;
    SwitchToCrossFade( sourceSampleIndex );
    *destSampleIndex = destSample;
}


void cSoundDefinition::InsertLoopCrossFadeEndTransition( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize, double *sourceSampleIndex, float inPitch )
{
    double sourcePos = *sourceSampleIndex;
    double sourceSample = 0;
    uint64_t destSample = *destSampleIndex;
    float startSourceSample = buffer[ endLoopSample - 1 ];
    float endSourceSample = buffer[ enterLoopCrossFadeSample ];
    float fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
	float localPitch = pitch * inPitch;

    while( static_cast<uint64_t>(sourceSample) == endLoopSample - 1 )
    {
        destBuffer[ destSample ] = startSourceSample * ( 1.0f - fraction ) + endSourceSample * fraction;
        sourcePos += localPitch;
        fraction = static_cast<float>(modf( sourcePos, &sourceSample ));
        destSample ++;
        if( destSample == destBufferSize )
        { // still in xFade at end of destBuffer
            *sourceSampleIndex = sourcePos;
            *destSampleIndex = destSample;
            return;
        }
    }
    *sourceSampleIndex = sourcePos;
    SwitchFromCrossFade( sourceSampleIndex );
    *destSampleIndex = destSample;
}


double cSoundDefinition::SwitchToCrossFade( double *sourceSampleIndex )
{
    double sourcePos = *sourceSampleIndex;
    double offset = sourcePos - exitLoopCrossFadeSample;
    sourcePos = crossFadeStartSample + offset;
    *sourceSampleIndex = sourcePos;
    return sourcePos;
}


double cSoundDefinition::SwitchFromCrossFade( double *sourceSampleIndex )
{
    double sourcePos = *sourceSampleIndex;
    double offset = sourcePos - crossFadeEndSample;
    sourcePos = enterLoopCrossFadeSample + offset;
    *sourceSampleIndex = sourcePos;
    return sourcePos;
}


cSoundDefinition::~cSoundDefinition()
{
    if( buffer )
    {
        free( static_cast<void *>(buffer) );
        buffer = nullptr;
    }
}


float cSoundDefinition::ApplyEnvelope( ofSoundBuffer& buffer, double position, float envelope, T_EnvelopeMode *envelopeMode, float currentLevel, float slewRate, float targetLevel )
{
    size_t destBufferSize = buffer.getNumFrames();
    size_t index = 0;
	bool fillOut = false;

	currentLevel *= amplitude;
	targetLevel *= amplitude;
	slewRate *= amplitude;
	
	float outputLevel = currentLevel;
	
	if( position <= endSample && position > endSample - releaseSamples )
        *envelopeMode = t_EnvelopeReleaseMode;

    switch( *envelopeMode )
		{
		case t_EnvelopeAttackMode:
			for( size_t i = index; i < destBufferSize; i ++ )
				{
				envelope += slewRate;
				if( envelope >= 1.0 )
					{
					envelope = 1.0;
					index = i;
					fillOut = true;
					*envelopeMode = t_EnvelopeSustainMode;
					break;
					}

				float a = envelope * currentLevel;
				for( size_t c = 0; c < channelCount; c ++ )
					buffer[ i * 2 + c ] *= a;
				}
			if( fillOut )
				{
				ScaleBufferRange( buffer, index, destBufferSize, currentLevel );
				}
			outputLevel = targetLevel * envelope;
			break;

		case t_EnvelopeReleaseMode:
			for( size_t i = index; i < destBufferSize; i ++ )
				{
				envelope += slewRate;
				if( envelope <= 0.0 )
					{
					envelope = 0.0;
					index = i;
					fillOut = true;
					*envelopeMode = t_EnvelopeIdle;
					break;
					}
				float a = envelope * currentLevel;
				for( size_t c = 0; c < channelCount; c ++ )
					{
					buffer[ i * 2 + c ] *= a;
					}
				}
			if( fillOut )
				{
				envelope = 0.0;
				ZeroBufferRange( buffer, index, destBufferSize );
				}
			outputLevel = targetLevel * envelope;
			break;
		
		case t_EnvelopeSustainMode:
		default:
			envelope = 1.0;
			ScaleBufferRange( buffer, index, destBufferSize, currentLevel );
			outputLevel = targetLevel * envelope;
			break;
		
		case t_EnvelopeInteractiveMode:
			if( slewRate > 0 )
				{
				if( currentLevel >= targetLevel )
					{
					currentLevel = targetLevel;
					ScaleBufferRange( buffer, index, destBufferSize, currentLevel );
					}
				else
					{
					for( size_t i = index; i < destBufferSize; i ++ )
						{
						currentLevel += slewRate;
						if( currentLevel >= targetLevel )
							{
							currentLevel = targetLevel;
							fillOut = true;
							index = i;
							break;
							}
						for( size_t c = 0; c < channelCount; c ++ )
							buffer[ i * 2 + c ] *= currentLevel;
						}
					if( fillOut )
						{
						ScaleBufferRange( buffer, index, destBufferSize, currentLevel );
						}
					}
				}
			else if( slewRate < 0 )
				{
				if( currentLevel <= targetLevel )
					{
					currentLevel = targetLevel;
					ScaleBufferRange( buffer, 0, destBufferSize, currentLevel );
					}
				else
					{
					for( size_t i = index; i < destBufferSize; i ++ )
						{
						currentLevel += slewRate;
						if( currentLevel <= targetLevel )
							{
							currentLevel = targetLevel;
							index = i;
							fillOut = true;
							if( currentLevel == 0 )
								*envelopeMode = t_EnvelopeIdle;
							break;
							}
						for( size_t c = 0; c < channelCount; c ++ )
							buffer[ i * 2 + c ] *= currentLevel;
						}
					if( fillOut )
						{
						ScaleBufferRange( buffer, index, destBufferSize, currentLevel );
						}
					}
				}
			else
				{
				if( envelope <= 0.0 )
					{
					envelope = 0.0;
					*envelopeMode = t_EnvelopeIdle;
					}
				else
					ScaleBufferRange( buffer, index, destBufferSize, currentLevel );
				}
			outputLevel = currentLevel;
			break;
    	}
    return outputLevel / amplitude;
}


void cSoundDefinition::ScaleBufferRange( ofSoundBuffer& buffer, size_t start, size_t end, float scaler )
{
    float * bufferPtr = &(buffer.getBuffer()[ start * channelCount ] );
    for( size_t i = start * channelCount; i < end * channelCount; i ++ )
    {
        *bufferPtr++ *= scaler;
    }
}

void cSoundDefinition::ZeroBufferRange( ofSoundBuffer& buffer, size_t start, size_t end )
{
    float * bufferPtr = &(buffer.getBuffer()[ start * channelCount ] );
    for( size_t i = start * channelCount; i < end * channelCount; i ++ )
    {
        *bufferPtr++ = 0;
    }
}

void cSoundDefinition::GetSaveSoundDefFile(string prompt)
{
    ofFileDialogResult result = ofSystemSaveDialog(string("soundDef_settings.json"), prompt);
    if(result.bSuccess)
        SaveSelfToJSONFile(result.getPath());
}


void cSoundDefinition::SaveSelfToJSONFile(string path)
{
   Json::Value soundDef;

   if(path.length() == 0)
       currentSoundDefFilePath = "soundDef_settings.json";
   else
       currentSoundDefFilePath = path;
   UpdateJSONSettings(soundDef, 0, true, string(""));
   ofxJSONElement def(soundDef);
   def.save(currentSoundDefFilePath, true);
}

void cSoundDefinition::GetLoadSoundDefFile(string prompt)
{
    ofFileDialogResult result = ofSystemLoadDialog(prompt);
    if(result.bSuccess)
        LoadSelfFromJSONFile(result.getPath());
}

void cSoundDefinition::LoadSelfFromJSONFile(string path)
{
   ofxJSON root;
   bool success = false;

   if( path.length() == 0 )
       success = root.open(string("soundDef_settings.json"));
   else
       success = root.open(path);

   const Json::Value soundDef = root[ "soundDef"]["0"];
   LoadFromJSONSettings(soundDef, "");
   if(path.length() != 0)
       currentSoundDefFilePath = path;
}


void cSoundDefinition::UpdateJSONSettings( Json::Value& container, int soundDefIndex, bool useFullPath, string synthBase )
{
    if( sampleName.size() == 0 )
        return;

    string soundDefIndexString = to_string(soundDefIndex);
    if( useFullPath )
        container["soundDef"][soundDefIndexString]["pathName"] = pathName;
    else
    {
       size_t found = pathName.find_last_of("/");
       string folder = pathName.substr(0, found);
       if(synthBase.compare(folder)==0)
            container["soundDef"][soundDefIndexString]["pathName"] = sampleName;
       else
           container["soundDef"][soundDefIndexString]["pathName"] = pathName;
    }

    container["soundDef"][soundDefIndexString]["pitch"] = pitch;
    container["soundDef"][soundDefIndexString]["amplitude"] = amplitude;
    container["soundDef"][soundDefIndexString]["startSample"] = (Json::Value::UInt64)startSample;
    container["soundDef"][soundDefIndexString]["endSample"] = (Json::Value::UInt64)endSample;
    container["soundDef"][soundDefIndexString]["attackTime"] = attackTime;
    container["soundDef"][soundDefIndexString]["releaseTime"] = releaseTime;
    container["soundDef"][soundDefIndexString]["startLoopSample"] = (Json::Value::UInt64)startLoopSample;
    container["soundDef"][soundDefIndexString]["endLoopSample"] = (Json::Value::UInt64)endLoopSample;
    container["soundDef"][soundDefIndexString]["loop"] = loop;
    container["soundDef"][soundDefIndexString]["loopCrossFadeFraction"] = loopCrossFadeFraction;
}


void cSoundDefinition::LoadFromJSONSettings( const Json::Value &soundDefJSON, string samplePathBase )
{
	uint64_t start = 0;
	uint64_t end = 0;
	uint64_t startLoop = 0;
	uint64_t endLoop = 0;
	float crossFadeFraction = 0.1;
	
	const Json::Value pathNameValue = soundDefJSON[ "pathName" ];
	if( pathNameValue.type() == Json::nullValue )
		return;
//	ofToDataPath(<#const std::filesystem::path &path#>)

    string inPath = pathNameValue.asString();
    if(samplePathBase.length() > 0 && inPath.c_str()[0] != '/')
        inPath = samplePathBase + "/" + pathNameValue.asString();
	
	const Json::Value pitchValue = soundDefJSON[ "pitch" ];
	if( pitchValue.type() != Json::nullValue )
		pitch = pitchValue.asFloat();
	
	const Json::Value ampValue = soundDefJSON[ "amplitude" ];
	if( ampValue.type() != Json::nullValue )
		amplitude = ampValue.asFloat();
	
	const Json::Value startSampleValue = soundDefJSON[ "startSample" ];
	if( startSampleValue.type() != Json::nullValue )
		start = startSampleValue.asUInt64();

	const Json::Value endSampleValue = soundDefJSON[ "endSample" ];
	if( endSampleValue.type() != Json::nullValue )
		end = endSampleValue.asUInt64();

	const Json::Value attackTimeValue = soundDefJSON[ "attackTime" ];
	if( attackTimeValue.type() != Json::nullValue )
		attackTime = attackTimeValue.asFloat();
	
	const Json::Value releaseTimeValue = soundDefJSON[ "releaseTime" ];
	if( releaseTimeValue.type() != Json::nullValue )
		releaseTime = releaseTimeValue.asFloat();
	
	const Json::Value startLoopSampleValue = soundDefJSON[ "startLoopSample" ];
	if( startLoopSampleValue.type() != Json::nullValue )
		startLoop = startLoopSampleValue.asUInt64();
	
	const Json::Value endLoopSampleValue = soundDefJSON[ "endLoopSample" ];
	if( endLoopSampleValue.type() != Json::nullValue )
		endLoop = endLoopSampleValue.asUInt64();
	
	const Json::Value loopValue = soundDefJSON[ "loop" ];
	if( loopValue.type() != Json::nullValue )
		loop = loopValue.asBool();

	const Json::Value crossFadeFractionValue = soundDefJSON[ "loopCrossFadeFraction" ];
	if( crossFadeFractionValue.type() != Json::nullValue )
		crossFadeFraction = crossFadeFractionValue.asFloat();
	
	if( inPath.size() > 0 )
		{
//		ofToDataPath(<#const std::filesystem::path &path#>)

            if(LoadSample( inPath, false ))
            {
                SetExtent( start, end );
                SetLoop( startLoop, endLoop, crossFadeFraction );
                SetAttackTime( attackTime );
                SetReleaseTime( releaseTime );
            }
		}
}
