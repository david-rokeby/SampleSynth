#include "cSliderParam.h"

#include "cSoundDefinition.h"
#include "cSoundEditor.hpp"

void cSliderParam::UpdateSlider( uint64_t newSampleValue )
{
	float newValue = static_cast<float>(newSampleValue) / static_cast<float>(SoundDef() -> sampleCount);
	slider -> setValue( newValue );	
}

cSoundDefinition *cSliderParam::SoundDef( void ){ return soundEditor -> SoundDef(); }

cSoundDefinition *cToggleParam::SoundDef( void ){ return soundEditor -> SoundDef(); }

