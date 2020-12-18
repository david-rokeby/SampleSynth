//
//  cImpulse.hpp
//  vns_03
//
//  Created by David Rokeby on 2018-12-01.
//

#ifndef cImpulse_hpp
#define cImpulse_hpp

#include <stdio.h>
#include "ofxVNS.h"

enum T_ImpulseState
{
	t_ImpulseIdle,
	t_ImpulseNewOnset,
	t_ImpulseOn,
	t_ImpulseNewRelease,
	t_ImpulseRelease
};

class cImpulse
{
public:
	static float prescaler;
	static float outputRange;
	static float limit;
	static float upGainRate;
	static float downGainRate;
	static float smoothing1;
	static float smoothing2;
	static float volumeSmoother;
	static float onThreshold;
	static float offThreshold;
	static float bulger;

	float ramp;
	float gain;
	float last;
	float acc1;
	float acc2;
	float outputVal;
	T_ImpulseState state;
	
	cImpulse( void ){ Reset(); ResetGlobals(); }
	void ResetGlobals( void ){ onThreshold = 0.01; offThreshold = 0.001; smoothing1 = 0.75; smoothing2 = 0.8; volumeSmoother = 0.9; prescaler = 8.; outputRange = 2.0; ; upGainRate = 0.3; downGainRate = -0.12; limit = 0; }
	void Reset( void ){ gain = ramp = acc1 = acc2 = outputVal = 0.0; last = 0; state = t_ImpulseIdle; }
	
	float OutputValue( void ){ return outputVal; }
	T_ImpulseState State( void ){ return state; } // 2 means new attack
	void AckOn( void ){ state = t_ImpulseOn; }
	void AckOff( void ){ state = t_ImpulseIdle; }
	void Process( float in );
	float UpdateGain( void );
	float SmallTrigger( int32_t input );
	float Bulger( float input );

};



#endif /* cImpulse_hpp */
