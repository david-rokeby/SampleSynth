//
//  ofxDatGuiNumericInput.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//
#pragma once

#ifndef ofxDatGuiNumericInput_hpp
#define ofxDatGuiNumericInput_hpp

#include "ofxDatGuiComponent.h"
#include "ofxDatGuiTextInputField.h"
#include "ofxDatGuiTextInput.h"

class ofxDatGuiNumericInput : public ofxDatGuiTextInput
{
public:
	static const int MAX_PRECISION = 4;
	
	ofxDatGuiNumericInput(string label, float inValue = 0.0f ) : ofxDatGuiTextInput(label)
	{
	setValue(inValue);
	mInput.setTextInputFieldType(ofxDatGuiInputType::NUMERIC);
	setPrecision(2);
	}

	void setPrecision(int precision, bool truncateValue = true)
	{
	mPrecision = precision;
	mTruncateValue = truncateValue;
	if (mPrecision > MAX_PRECISION) mPrecision = MAX_PRECISION;
	}
	
	void setValue( float value )
	{
	ostringstream convert;   // stream used for the conversion
	convert << value;      // insert the textual representation of 'Number' in the characters in the stream
	mInput.setText(convert.str());
	}
	
	float getValue()
	{
	float result = 0;
	istringstream convert(mInput.getText());
	if ( !(convert >> result) ) //give the value to 'Result' using the characters in the stream
		result = 0;             //if that fails set 'Result' to 0
	return result;
	}
	
	int     mPrecision;
	bool    mTruncateValue;

};



#endif /* ofxDatGuiNumericInput_hpp */
