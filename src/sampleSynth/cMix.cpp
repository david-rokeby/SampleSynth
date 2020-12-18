//
//  cMix.cpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//

#include "cMix.hpp"

/*void cMix::MixTo( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer, int channelOffset )
{
	int inChanCount = inBuffer.getNumChannels();
	int outChanCount = outBuffer.getNumChannels();
	if( inChanCount != inputChannelCount )
		return;
	if( outChanCount == outputChannelCount )
		{
		
		}
}*/

void cMix::AddMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer )
{
	switch( mode )
	{
		case t_MixSimpleStereo: AddSimpleStereoMix( inBuffer, outBuffer ); break;
		case t_MixStereoToMulti: AddPlacedStereoToMultiMix( inBuffer, outBuffer ); break;
		case t_MixStereoToQuadInMulti: AddPlacedStereoQuadToMultiMix( inBuffer, outBuffer ); break;
		default: AddSimpleStereoMix( inBuffer, outBuffer ); break;
	}
}

void cMix::AddSimpleStereoMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer )
{
	float leftGain = StereoLeftGain();
	float rightGain = StereoRightGain();
	
	if( inBuffer.getNumChannels() == 2 && outBuffer.getNumChannels() == 2 )
		{
		int sampleCount = inBuffer.getNumFrames();
		float *in = &inBuffer.getBuffer()[0];
		float *out = &outBuffer.getBuffer()[0];
		while( sampleCount -- > 0 )
			{
			*out++ += *in++ * leftGain;
			*out++ += *in++ * rightGain;
			}
		}
	else if( inBuffer.getNumChannels() == 1 && outBuffer.getNumChannels() == 2 )
		{
		int sampleCount = inBuffer.getNumFrames();
		float *in = &inBuffer.getBuffer()[0];
		float *out = &outBuffer.getBuffer()[0];
		while( sampleCount -- > 0 )
			{
			*out++ += *in * leftGain;
			*out++ += *in++ * rightGain;
			}
		}
	else if( inBuffer.getNumChannels() == 2 && outBuffer.getNumChannels() == 1 )
		{
		int sampleCount = inBuffer.getNumFrames();
		float *in = &inBuffer.getBuffer()[0];
		float *out = &outBuffer.getBuffer()[0];
		while( sampleCount -- > 0 )
			{
			*out += *in++ * leftGain;
			*out++ += *in++ * rightGain;
			}
		}
	else if( inBuffer.getNumChannels() == 2 && outBuffer.getNumChannels() > 2 )
		{
		int sampleCount = inBuffer.getNumFrames();
		int left = leftFrontOutChannel + channelOffset;
		int right = rightFrontOutChannel + channelOffset;
		float *in = &inBuffer.getBuffer()[0];
		float *out = &outBuffer.getBuffer()[0];
		int outChans = outBuffer.getNumChannels();
		
		while( sampleCount -- > 0 )
			{
			out[ left ] += *in++ * leftGain;
			out[ right ] += *in++ * rightGain;
			out += outChans;
			}
		}

}

void cMix::AddFullStereoMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer )
{
	float leftToLeftGain = levels[ 0 ] * globalLevel;
	float leftToRightGain = levels[ 1 ] * globalLevel;
	float rightToLeftGain = levels[ 2 ] * globalLevel;
	float rightToRightGain = levels[ 3 ] * globalLevel;
	
	if( inBuffer.getNumChannels() == 2 && outBuffer.getNumChannels() == 2 )
		{
		int sampleCount = inBuffer.getNumFrames();
		float *in = &inBuffer.getBuffer()[0];
		float *out = &outBuffer.getBuffer()[0];
		while( sampleCount -- > 0 )
			{
			float inLeft = *in ++;
			float inRight = *in ++;
			*out ++ += inLeft * leftToLeftGain + inRight * rightToLeftGain;
			*out ++ += inLeft * leftToRightGain + inRight * rightToRightGain;
			}
		}
}

void cMix::UpdateJSONSettings( Json::Value& soundObject )
{
    soundObject["mix"]["inputChannelCount"] = inputChannelCount;
    soundObject["mix"]["outputChannelCount"] = outputChannelCount;
    soundObject["mix"]["globalLevel"] = globalLevel;
    soundObject["mix"]["pan"] = pan;
    soundObject["mix"]["frontBackPan"] = frontBackPan;
    soundObject["mix"]["mode"] = mode;
    if( mode == t_MixStereoToMulti )
        {
        soundObject["mix"]["leftFrontOutChannel"] = leftFrontOutChannel;
        soundObject["mix"]["rightFrontOutChannel"] = rightFrontOutChannel;
        }
}


void cMix::UpdateJSONSettings( string heirarchy )
{
    Settings::getInt( heirarchy + "/inputChannelCount" ) = inputChannelCount;
    Settings::getInt( heirarchy + "/outputChannelCount" ) = outputChannelCount;
    Settings::getDouble( heirarchy + "/globalLevel" ) = globalLevel;
    Settings::getDouble( heirarchy + "/pan" ) = pan;
    Settings::getDouble( heirarchy + "/frontBackPan" ) = frontBackPan;
    Settings::getInt( heirarchy + "/mode" ) = mode;
	if( mode == t_MixStereoToMulti )
		{
        Settings::getInt( heirarchy + "/leftFrontOutChannel" ) = leftFrontOutChannel;
        Settings::getInt( heirarchy + "/rightFrontOutChannel" ) = rightFrontOutChannel;
		}
}

void cMix::LoadFromJSONSettings( const Json::Value &mixJSON )
{
	float holdGlobalLevel = 1.0;
	float holdPan = 0.5;
	float holdFrontBackPan = 0.0;
	int holdLeftFrontOutChannel = 0;
	int holdRightFrontOutChannel = 1;
	
	const Json::Value inputChannelCountValue = mixJSON[ "inputChannelCount" ];
	if( inputChannelCountValue.type() == Json::intValue || inputChannelCountValue.type() == Json::realValue )
		inputChannelCount = inputChannelCountValue.asUInt64();
	
	const Json::Value outputChannelCountValue = mixJSON[ "outputChannelCount" ];
	if( outputChannelCountValue.type() == Json::intValue || outputChannelCountValue.type() == Json::realValue )
		outputChannelCount = outputChannelCountValue.asUInt64();

	const Json::Value globalLevelValue = mixJSON[ "globalLevel" ];
	if( globalLevelValue.type() == Json::intValue || globalLevelValue.type() == Json::realValue )
		holdGlobalLevel = globalLevelValue.asFloat();

	const Json::Value panValue = mixJSON[ "pan" ];
	if( panValue.type() == Json::intValue || panValue.type() == Json::realValue )
		holdPan = panValue.asFloat();

	const Json::Value frontBackPanValue = mixJSON[ "frontBackPan" ];
	if( frontBackPanValue.type() == Json::intValue || frontBackPanValue.type() == Json::realValue )
		holdFrontBackPan = frontBackPanValue.asFloat();
	
	const Json::Value modeValue = mixJSON[ "mode" ];
	if( modeValue.type() == Json::intValue || modeValue.type() == Json::realValue )
		mode = static_cast<T_MixMode>( modeValue.asUInt64() );

	const Json::Value leftFrontOutChannelValue = mixJSON[ "leftFrontOutChannel" ];
	if( leftFrontOutChannelValue.type() == Json::intValue || leftFrontOutChannelValue.type() == Json::realValue )
		holdLeftFrontOutChannel = leftFrontOutChannelValue.asUInt64();
	
	const Json::Value rightFrontOutChannelValue = mixJSON[ "rightFrontOutChannel" ];
	if( rightFrontOutChannelValue.type() == Json::intValue || rightFrontOutChannelValue.type() == Json::realValue )
		holdRightFrontOutChannel = rightFrontOutChannelValue.asUInt64();

	switch( mode )
	{
		case t_MixSimpleStereo:
			InitStereo();
			SetStereoPan( holdPan );
			globalLevel = holdGlobalLevel;
			break;
		
		case t_MixStereoToMulti:
			InitStereoToMulti( outputChannelCount );
			SetStereoPan( holdPan );
			PlaceStereoInMulti( holdLeftFrontOutChannel, holdRightFrontOutChannel );
			globalLevel = holdGlobalLevel;
			break;

		case t_MixStereoToQuadInMulti:
			InitStereoToQuadInMulti( outputChannelCount );
			SetStereoPlacedInQuad( holdPan, holdFrontBackPan );
			globalLevel = holdGlobalLevel;
			break;
	}
}

	
	/*	__m128 mix_A, mix_B;
	 // in0->out0, in1->out0, in0->out1, in1->out1
	 int i = 0;
	 
	 mix_A = _mm_set_ps( levels[0], levels[2], levels[1], levels[3] );	// mix inLOutL inROutL inLOutR inROutR
	 mix_B = _mm_set_ps( levels[1], levels[3], levels[1], levels[3] );	// mix inLOutR inROutR inLOutR inROutR
	 
	 __m128 audio = _mm_load_ps( inBuffer + i );			// inL0, inR0, inL1, inR1
	 __m128 out_L = _mm_mul_ps( audio, mix_A ); 			// inL0outL0 inR0outL0 inL1outR1 inR1outR1
	 __m128 out_R = _mm_mul_ps( audio, mix_B ); 			// inL0outR0 inR0outR0 inL1outL1 inR1outL1
	 __m128 out = _mm_hadd_ps(out_L, out_R); 			//outL0 outR1 outR0 outL1
	 out = _mm_shuffle_ps( out, out, 0x78 );*/
	
	
	
	
	//_mm_haddps
void cMix::AddPlacedStereoToMultiMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer )
{
	float leftGain = StereoLeftGain();
	float rightGain = StereoRightGain();
	int outChannelCount = outBuffer.getNumChannels();
	
	int sampleCount = inBuffer.getNumFrames();
	float *in = &inBuffer.getBuffer()[0];
	float *out = &outBuffer.getBuffer()[0];
	int left = leftFrontOutChannel + channelOffset;
	int right = rightFrontOutChannel + channelOffset;

	while( sampleCount -- > 0 )
		{
		out[ left ] += *in++ * leftGain;
		out[ right ] += *in++ * rightGain;
		out += outChannelCount;
		}
}


void cMix::AddPlacedStereoQuadToMultiMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer )
{
	float leftFrontGain = QuadLeftFrontGain();
	float rightFrontGain = QuadRightFrontGain();
	float leftBackGain = QuadLeftBackGain();
	float rightBackGain = QuadRightBackGain();
	
	int outChannelCount = outBuffer.getNumChannels();
	
	int sampleCount = inBuffer.getNumFrames();
	float *in = &inBuffer.getBuffer()[0];
	float *out = &outBuffer.getBuffer()[0];
	
	int LF = leftFrontOutChannel + channelOffset;
	int RF = rightFrontOutChannel + channelOffset;
	int LB = leftBackOutChannel + channelOffset;
	int RB = rightBackOutChannel + channelOffset;
	
	while( sampleCount -- > 0 )
		{
		out[ LF ] += *in * leftFrontGain;
		out[ LB ] += *in++ * leftBackGain;
		out[ RF ] += *in * rightFrontGain;
		out[ RB ] += *in++ * rightBackGain;
		out += outChannelCount;
		}
}

void cMix::SetStereoRotateMulti( float rotatePanPos )
{
	// Left and Right separated by 1 channel
	// A B C
	// L -> A X B    R -> B X C
	// if rotatePanPos = 4.0 then A = 3, B = 4, C = 5  (left is .5, right is .5)
	// if rotatePanPos = 4.5- then A = 3, B = 4, C = 5 (left is all B, right is all C
	// if rotatePanPos = 4.5+ then A = 4, B = 5, C = 6 (left is all A, right is all B
	// if rotatePanPos = 5.0 then A = 4, B = 5, C = 6  (left is .5, right is .5)

	int bChannel = static_cast<int>( rotatePanPos + 0.5 );
	int aChannel = bChannel - 1;
	int cChannel = cChannel + 1;
	if( bChannel > outputChannelCount )
		bChannel -= outputChannelCount;
	if( aChannel < 0 )
		aChannel += outputChannelCount;
	if( cChannel > outputChannelCount )
		cChannel -= outputChannelCount;
	
	leftFrontOutChannel = RotationMappedChannel( aChannel );
	leftBackOutChannel = RotationMappedChannel( bChannel );
	rightFrontOutChannel = RotationMappedChannel( bChannel );
	rightBackOutChannel = RotationMappedChannel( cChannel );
	float localPan = rotatePanPos - static_cast<float>( static_cast<int>( rotatePanPos ) );

	levels[ 0 ] = 1.0 - localPan;
	levels[ 2 ] = 1.0 - localPan;
	levels[ 1 ] = localPan;
	levels[ 3 ] = localPan;
}


int cMix::RotationMappedChannel( int inChan )
{
	if( inChan < outputChannelCount )
		return inChan * 2;
	return ( outputChannelCount - 1 - inChan ) * 2 + 1;
}


void cMix::SetStereoPlacedInQuad( float leftRight, float frontBack )
{
	// assuming front and rear pan
	// frontBack reaches across multiple quad zones
	pan = leftRight;
	frontBackPan = frontBack;
	
	int quadZone = static_cast<int>( frontBack );
	float localFrontBackPan = frontBack - static_cast<float>( quadZone );
	
	leftFrontOutChannel = quadZone * 2;
	rightFrontOutChannel = leftFrontOutChannel + 1;
	leftBackOutChannel = rightFrontOutChannel + 1;
	rightBackOutChannel = leftBackOutChannel + 1;
	
	levels[ 0 ] = ( 1 - leftRight ) * ( 1 - localFrontBackPan );
	levels[ 1 ] = ( leftRight ) * ( 1 - localFrontBackPan );
	levels[ 2 ] = ( 1 - leftRight ) * ( localFrontBackPan );
	levels[ 3 ] = ( leftRight ) * ( localFrontBackPan );
}


void cMix::SetPan( float p )
{
	switch( mode )
	{
		case t_MixSimpleStereo: SetStereoPan( p ); break;
		case t_MixStereoToMulti: SetStereoPan( p ); break;
		case t_MixStereoToQuadInMulti: SetStereoPlacedInQuad( p, frontBackPan ); break;
	}
}

void cMix::SetFrontBackPan( float p )
{
	switch( mode )
	{
		case t_MixStereoToQuadInMulti: SetStereoPlacedInQuad( pan, p ); break;
	}
}


void cMix::SetOutChannelCount( int count )
{
	switch( mode )
	{
		case t_MixSimpleStereo:
			{
			float holdPan = pan;
			InitStereo();
			SetPan( holdPan );
			}
			break;
		
		case t_MixStereoToMulti:
			{
			float holdPan = pan;
			int holdLeftFrontOutChannel = leftFrontOutChannel;
			int holdRightFrontOutChannel = rightFrontOutChannel;
			InitStereoToMulti( count );
			PlaceStereoInMulti( holdLeftFrontOutChannel, holdRightFrontOutChannel );
			SetPan( holdPan );
			}
			break;
		
		case t_MixStereoToQuadInMulti:
			{
			float holdPan = pan;
			float holdFrontBackPan = frontBackPan;
			InitStereoToQuadInMulti( count );
			SetStereoPlacedInQuad( holdPan, holdFrontBackPan );
			SetPan( holdPan );
			SetFrontBackPan( holdFrontBackPan );
			}
			break;
	}
}
