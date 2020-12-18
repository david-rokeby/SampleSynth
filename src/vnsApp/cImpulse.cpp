//
//  cImpulse.cpp
//  vns_03
//
//  Created by David Rokeby on 2018-12-01.
//

#include "cImpulse.hpp"

float cImpulse::prescaler = 8000.0;
float cImpulse::outputRange = 2.0;
float cImpulse::bulger = 0.3;
float cImpulse::limit = 0.0;
float cImpulse::upGainRate = 0.3;
float cImpulse::downGainRate = -0.12;
float cImpulse::smoothing1 = 0.75;
float cImpulse::smoothing2 = 0.8;
float cImpulse::volumeSmoother = 0.95;
float cImpulse::onThreshold = 0.01;
float cImpulse::offThreshold = 0.001;

void cImpulse::Process( float in )
{
	in -= limit;
	if( in < 0 )
		in = 0;
	acc1 = acc1 * smoothing1 + in * ( 1.0 - smoothing1 );
	acc2 = acc2 * smoothing2 + in * ( 1.0 - smoothing2 );
	in = acc1 - acc2;
	if( in < 0 )
		in = 0;
//	float out = in;
//	float out = SmallTrigger( in );
	float out = Bulger( in );
	outputVal = outputVal * volumeSmoother + out * ( 1.0 - volumeSmoother );
	
	switch( state )
		{
		case t_ImpulseIdle:
			if( outputVal > onThreshold )
				state = t_ImpulseNewOnset;
			break;
			
		case t_ImpulseOn:
			if( outputVal < offThreshold )
				state = t_ImpulseNewRelease;
			break;
		}
}
	
float cImpulse::UpdateGain( void )
{
	gain += ramp;
	if( gain < 0 && ramp < 0 )
		gain = ramp = 0.0;
	if( gain > 1.0 && ramp > 1.0 )
		{
		gain = 1.0;
		ramp = 0.0;
		}
    return gain;
}
	
float cImpulse::SmallTrigger( int32_t input )
{
	float in = static_cast<float>( input );
	float preIn = in / prescaler;
	
	float out = preIn / ( ( preIn - 1.0 ) * ( preIn - 1.0 ) + 1.0 ) * outputRange;
	
	int32_t change = preIn - last;
	
	if( change > 0 && preIn <= 2.0 )
		{
		ramp = upGainRate;
		}
	else if( preIn > 1.0 )
		{
		ramp = downGainRate;
		}
	
	UpdateGain();
	last = preIn;
	out *= gain;
	return out;
}


float cImpulse::Bulger( float input )
{
	if( input > prescaler || input < 0.0 )
		return 0;
	if( input > 0 )
		{
		float scaledBulger = bulger * prescaler;
		float x = (input / (input + scaledBulger) - (input / ( scaledBulger + prescaler )));
		float subScaler = prescaler / 10.0;
		float scaler = subScaler / ( scaledBulger + subScaler );
		x = x / scaler * outputRange;
		return x;
		}
	return 0.0;
}
