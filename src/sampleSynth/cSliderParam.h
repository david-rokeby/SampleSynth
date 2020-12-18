#pragma once

#ifndef CSLIDERPARAM_H
#define CSLIDERPARAM_H

#include "ofxDatGui.h"
class cSoundDefinition;
class cSoundEditor;



class cSliderParam
{
public:
    cSoundEditor *soundEditor;
    ofxDatGuiSlider *slider;
    ofParameter<float> parameter;

    void draw(){ slider -> draw(); }
    void update(){ slider -> update(); }
    void onParamChanged(float & f){ ParameterChanged( f );}
    virtual void ParameterChanged(float & f){}
    virtual ~cSliderParam( void ){}
    virtual void ExtractValue( void ){}
    cSoundDefinition *SoundDef( void );
	
	void UpdateSlider( float newValue ){ slider -> setValue( newValue ); }
	void UpdateSlider( uint64_t newSampleValue );

	void SetAndUpdate( float newValue ){ slider -> setValue( newValue ); ParameterChanged( newValue ); }

    void Init( cSoundEditor *inSoundEditor, const char *name, float defaultVal, float min, float max, float width, float left, float top )
    {
		std::string nameString(name);
        soundEditor = inSoundEditor;
        slider = new ofxDatGuiSlider(parameter.set(nameString, defaultVal, min, max));
        slider->setPrecision(4, false);
        slider->setWidth(width, .3); // make label area 20% of width //
        slider->setPosition(left, top);
        parameter.addListener(this, &cSliderParam::onParamChanged);
    }
};


class cToggleParam
{
public:
	cSoundEditor *soundEditor;
	ofxDatGuiToggle *toggle;
	ofParameter<bool> parameter;
	
	void draw(){ toggle -> draw(); }
	void update(){ toggle -> update(); }
	void onParamChanged(bool &f){ ParameterChanged( f );}
	virtual void ParameterChanged(bool & f){}
	virtual ~cToggleParam( void ){}
	virtual void ExtractValue( void ){}
	cSoundDefinition *SoundDef( void );
	
	void UpdateToggle( bool newValue ){ toggle -> setChecked( newValue ); }
	
	void SetAndUpdate( bool newValue ){ toggle -> setChecked( newValue ); ParameterChanged( newValue ); }
	
	void Init( cSoundEditor *inSoundEditor, const char *name, bool defaultVal, float width, float left, float top )
	{
		std::string nameString(name);
		soundEditor = inSoundEditor;
		toggle = new ofxDatGuiToggle( nameString, defaultVal );
		toggle->setWidth(width, .3); // make label area 20% of width //
		toggle->setPosition(left, top);
		parameter.addListener(this, &cToggleParam::onParamChanged);
	}
};



#endif // CSLIDERPARAM_H
