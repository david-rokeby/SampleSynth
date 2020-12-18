//
//  vnsGUI.cpp
//  vns_03
//
//  Created by David Rokeby on 2018-11-30.
//

#include "vns.hpp"
#include "ofxVNS.h"
#include "cVideoFrame.h"
#include "ConvertPlanar.h"
#include "ofxVNSConvert.hpp"
#include "FrameMotion.h"
#include "FrameSlur.h"
#include "FrameBins.h"

void vns::SetupGUI( void )
{
	int offset = 1;
	int left = 0;
	int height = 26;
	int width = 300;
	int heightStart = 256;
	
	onOffGUI = new ofxDatGuiToggle( "kinect on/off", kinectOpen );
	onOffGUI -> setWidth( width, .3);
	onOffGUI -> setPosition( left, height * offset + heightStart );
	onOffGUI -> onToggleEvent( this, &vns::onOnOffChangedEvent );
	offset ++;
	
	rowsGUI = new ofxDatGuiNumericInput( "rows", rows );
	rowsGUI -> setPosition(left, offset * height + heightStart);
	rowsGUI -> setWidth( width / 2.0, 0.6 );
	rowsGUI -> setPrecision( 0 );
	//	rowsGUI -> setValue( rows );
	rowsGUI -> onTextInputEvent( this, &vns::onRowsChangedEvent );
	
	columnsGUI = new ofxDatGuiNumericInput( "columns", columns );
	columnsGUI -> setPosition(left + width / 2.0, offset * height + heightStart);
	columnsGUI -> setWidth( width / 2.0, 0.6 );
	columnsGUI -> setPrecision( 0 );
	//	columnsGUI -> setValue( columns );
	columnsGUI -> onTextInputEvent( this, &vns::onColumnsChangedEvent );
	offset ++;
	
	thresholdGUI = new ofxDatGuiSlider( "threshold", 0, 64, threshold.op[ 0 ] );
	thresholdGUI -> setPosition(left, offset * height + heightStart);
	thresholdGUI -> setWidth( width, 0.3 );
	thresholdGUI -> setPrecision( 0 );
	thresholdGUI -> onSliderEvent( this, &vns::onThresholdChangedEvent );
	offset ++;
	
	regionThresholdGUI = new ofxDatGuiSlider( "region thresh", 0, 2000, regionThreshold );
	regionThresholdGUI -> setPosition(left, offset * height + heightStart);
	regionThresholdGUI -> setWidth( width, 0.3 );
	regionThresholdGUI -> setPrecision( 0 );
	regionThresholdGUI -> onSliderEvent( this, &vns::onRegionThresholdChangedEvent );
	offset ++;
	
	sensitivityGUI = new ofxDatGuiSlider( "sensitivity", 0, 256, sensitivity );
	sensitivityGUI -> setPosition(left, offset * height + heightStart);
	sensitivityGUI -> setWidth( width, 0.3 );
	sensitivityGUI -> setPrecision( 0 );
	sensitivityGUI -> onSliderEvent( this, &vns::onSensitivityChangedEvent );
	offset ++;
	
	displaySensitivityGUI = new ofxDatGuiSlider( "display gain", 0, 16, displayGain );
	displaySensitivityGUI -> setPosition(left, offset * height + heightStart);
	displaySensitivityGUI -> setWidth( width, 0.3 );
	displaySensitivityGUI -> setPrecision( 2 );
	displaySensitivityGUI -> onSliderEvent( this, &vns::onDisplaySensitivityChangedEvent );
	offset ++;
	
	sqrtGUI = new ofxDatGuiToggle( "square root", false );
	sqrtGUI -> setWidth( width, .3); // make label area 20% of width //
	sqrtGUI -> setPosition( left, height * offset + heightStart );
	sqrtGUI -> onToggleEvent( this, &vns::onSqrtChangedEvent );
	offset ++;
	
	sortDepthGUI = new ofxDatGuiSlider( "sort depth", 0, 64, sortDepth );
	sortDepthGUI -> setPosition(left, offset * height + heightStart);
	sortDepthGUI -> setWidth( width, 0.3 );
	sortDepthGUI -> setPrecision( 0 );
	sortDepthGUI -> onSliderEvent( this, &vns::onSortDepthChangedEvent );
	offset ++;
	
	soloGUI = new ofxDatGuiNumericInput( "solo", 12 );
	soloGUI -> setPosition(left, offset * height + heightStart);
	soloGUI -> setWidth( width / 2.0, 0.6 );
	soloGUI -> setPrecision( 0 );
	//	rowsGUI -> setValue( rows );
	soloGUI -> onTextInputEvent( this, &vns::onSoloChangedEvent );
	offset ++;
	
	showShapeGUI = new ofxDatGuiToggle( "show shape", showShape );
	showShapeGUI -> setWidth( width, .3);
	showShapeGUI -> setPosition( left, height * offset + heightStart );
	showShapeGUI -> onToggleEvent( this, &vns::onShowShapeChangedEvent );
	offset ++;

	plotter = new ofxDatGuiValuePlotter( "shape", 0, 1000000);
	plotter -> setSpeed(1.0f);
	plotter -> setDrawMode(ofxDatGuiGraph::LINES);
	scopeGUI = static_cast<ofxDatGuiComponent *>( plotter );
	scopeGUI -> setPosition(left, offset * height + heightStart);

	inputRangeGUI = new ofxDatGuiSlider( "input range", 0, 16000, 8000 );
	inputRangeGUI -> setPosition(left + width, offset * height + heightStart);
	inputRangeGUI -> setWidth( width, 0.3 );
	inputRangeGUI -> setPrecision( 0 );
	inputRangeGUI -> onSliderEvent( this, &vns::onInputRangeChangedEvent );
	offset ++;

	outputRangeGUI = new ofxDatGuiSlider( "output range", 0, 8.0, 2.0 );
	outputRangeGUI -> setPosition(left + width, offset * height + heightStart);
	outputRangeGUI -> setWidth( width, 0.3 );
	outputRangeGUI -> setPrecision( 3 );
	outputRangeGUI -> onSliderEvent( this, &vns::onOutputRangeChangedEvent );
	offset ++;

	bulgerGUI = new ofxDatGuiSlider( "bulger", 0, 1.0, .2 );
	bulgerGUI -> setPosition(left + width, offset * height + heightStart);
	bulgerGUI -> setWidth( width, 0.3 );
	bulgerGUI -> setPrecision( 3 );
	bulgerGUI -> onSliderEvent( this, &vns::onBulgerChangedEvent );
	offset ++;
	

}

void vns::DrawGUI( void )
{
	rowsGUI -> draw();
	columnsGUI -> draw();
	sensitivityGUI -> draw();
	thresholdGUI -> draw();
	regionThresholdGUI -> draw();
	displaySensitivityGUI -> draw();
	sqrtGUI -> draw();
	sortDepthGUI -> draw();
	onOffGUI -> draw();
	scopeGUI -> draw();
	soloGUI -> draw();
	showShapeGUI -> draw();
	inputRangeGUI -> draw();
	outputRangeGUI -> draw();
	bulgerGUI -> draw();
}

void vns::UpdateGUI( void )
{
	rowsGUI -> update();
	columnsGUI -> update();
	sensitivityGUI -> update();
	thresholdGUI -> update();
	regionThresholdGUI -> update();
	displaySensitivityGUI -> update();
	sqrtGUI -> update();
	sortDepthGUI -> update();
	onOffGUI -> update();
	if( solo != -1 && impulses.size() > solo )
		{
		plotter -> setRange( 0, 16 );
		plotter -> setValue( impulses[ solo ] -> OutputValue() );
		}
	else
		{
		plotter -> setRange( 0, 1000000 );
		plotter -> setValue( totalMotion );
		}
	
	scopeGUI -> update();
	soloGUI -> update();
	showShapeGUI -> update();
	inputRangeGUI -> update();
	outputRangeGUI -> update();
	bulgerGUI -> update();
}

void vns::onRowsChangedEvent(ofxDatGuiTextInputEvent e)
{
	rows = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	gridChanged = true;
}

void vns::onColumnsChangedEvent(ofxDatGuiTextInputEvent e)
{
	columns = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	gridChanged = true;
}

void vns::onThresholdChangedEvent(ofxDatGuiSliderEvent e)
{
	float thresh = e.target -> getValue();
	threshold.Fill( thresh );
}

void vns::onRegionThresholdChangedEvent(ofxDatGuiSliderEvent e)
{
	float thresh = e.target -> getValue();
	regionThreshold = thresh;
}

void vns::onSensitivityChangedEvent(ofxDatGuiSliderEvent e)
{
	float sense = e.target -> getValue();
	sensitivity = sense;
}

void vns::onDisplaySensitivityChangedEvent(ofxDatGuiSliderEvent e)
{
	float gain = e.target -> getValue();
	displayGain = gain;
}


void vns::onOnOffChangedEvent(ofxDatGuiToggleEvent e)
{
	bool onOff = e.target -> getChecked();
	if( onOff )
		{
		if( kinectOpen == false )
			kinect.open();
		kinectOpen = true;
		}
	else
		{
		if( kinectOpen == true )
			kinect.close();
		kinectOpen = false;
		}
}


void vns::onSqrtChangedEvent(ofxDatGuiToggleEvent e)
{
	squareRoots = e.target -> getChecked();
}

void vns::onSortDepthChangedEvent(ofxDatGuiSliderEvent e)
{
	sortDepth = e.target -> getValue();
}

void vns::onSoloChangedEvent(ofxDatGuiTextInputEvent e)
{
	solo = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
}

void vns::onShowShapeChangedEvent(ofxDatGuiToggleEvent e)
{
	showShape = e.target -> getChecked();
}

void vns::onInputRangeChangedEvent(ofxDatGuiSliderEvent e)
{
	cImpulse::prescaler = e.target -> getValue();
}

void vns::onOutputRangeChangedEvent(ofxDatGuiSliderEvent e)
{
	cImpulse::outputRange = e.target -> getValue();
}

void vns::onBulgerChangedEvent(ofxDatGuiSliderEvent e)
{
	cImpulse::bulger = e.target -> getValue();
}

