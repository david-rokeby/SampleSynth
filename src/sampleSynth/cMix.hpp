//
//  cMix.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//

#ifndef cMix_hpp
#define cMix_hpp

#include <stdio.h>
#include "ofSoundBuffer.h"
#include "ofxJsonSettings.h"


enum T_MixMode
{
	t_MixSimpleStereo,
	t_MixStereoToMulti,
	t_MixStereoToQuadInMulti,
	t_MixFullStereo
};

class cMix
{
public:
	// map source channels to output channels with weights
	int inputChannelCount;
	int outputChannelCount;
	float globalLevel;
	float pan;
	float frontBackPan;
	int leftFrontOutChannel;
	int rightFrontOutChannel;
	int leftBackOutChannel;
	int rightBackOutChannel;
	int channelOffset; // offset to real channels (MOTU ultralite Analog out 1-8 are channels 3-10
	T_MixMode mode;
	std::vector<int> channelMatrix; //input channel to output channel(s)
	std::vector<float> levels; // level to output channels
	
	cMix( void ){ channelOffset = 0; globalLevel = 1.0; inputChannelCount = 0; outputChannelCount = 0; pan = 0.5; frontBackPan = 0.5; leftFrontOutChannel = 0; rightFrontOutChannel = 1; leftBackOutChannel = 2; rightBackOutChannel = 3; InitStereo(); }

	void InitStereo( void )
	{
		mode = t_MixSimpleStereo;
		leftFrontOutChannel = 0;
		rightFrontOutChannel = 1;
		pan = 0.5;
		inputChannelCount = outputChannelCount = 2;
		levels.clear();
		levels.push_back( 0.5 );
		levels.push_back( 0.0 );
		levels.push_back( 0.0 );
		levels.push_back( 0.5 );
	}
	
	void InitStereoToMulti( int outChannels )
	{
		mode = t_MixStereoToMulti;
		pan = 0.5;
		leftFrontOutChannel = 0;
		rightFrontOutChannel = 1;
		inputChannelCount = 2;
		outputChannelCount = outChannels;
		levels.clear();
		levels.push_back( 0.5 );
		levels.push_back( 0.0 );
		levels.push_back( 0.0 );
		levels.push_back( 0.5 );
	}
	
	void InitStereoToQuadInMulti( int outChannels )
	{
		mode = t_MixStereoToQuadInMulti;
		leftFrontOutChannel = 0;
		rightFrontOutChannel = 1;
		leftBackOutChannel = 2;
		rightBackOutChannel = 3;
		pan = 0.5;
		frontBackPan = 0.0;
		inputChannelCount = 2;
		outputChannelCount = outChannels;
		levels.clear();
		levels.push_back( 0.5 );
		levels.push_back( 0.5 );
		levels.push_back( 0.0 );
		levels.push_back( 0.0 );
	}

	void SetStereoPan( float panPos ){ pan = panPos; levels[ 0 ] = 1.0 - panPos; levels[ 1 ] = 0.0; levels[ 2 ] = 0.0; levels[ 3 ] = panPos; }
	void SetStereoRotateMulti( float rotatePanPos );
	void PlaceStereoInMulti( int left, int right ){ leftFrontOutChannel = left + channelOffset; rightFrontOutChannel = right + channelOffset; }
	void SetStereoPlacedInQuad( float leftRight, float frontBack );
	int RotationMappedChannel( int inChan );
	void SetPan( float p );
	void SetFrontBackPan( float p );
	void SetOutChannelCount( int count );
	
	float StereoLeftGain( void ){ return levels[ 0 ] * globalLevel; }
	float StereoRightGain( void ){ return levels[ 3 ] * globalLevel; }
	float QuadLeftFrontGain( void ){ return levels[ 0 ] * globalLevel; }
	float QuadLeftBackGain( void ){ return levels[ 2 ] * globalLevel; }
	float QuadRightFrontGain( void ){ return levels[ 1 ] * globalLevel; }
	float QuadRightBackGain( void ){ return levels[ 3 ] * globalLevel; }
	
	void AddMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddSimpleStereoMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddFullStereoMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddPlacedStereoQuadToMultiMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddPlacedStereoToMultiMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );

        void UpdateJSONSettings( string heirarchy );
        void UpdateJSONSettings( Json::Value& heirarchy );

	void LoadFromJSONSettings( const Json::Value &mixJSON );

};

#endif /* cMix_hpp */
