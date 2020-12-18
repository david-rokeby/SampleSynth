//
//  vns.hpp
//  vns_03
//
//  Created by David Rokeby on 2018-11-30.
//

#ifndef vns_hpp
#define vns_hpp

#include <stdio.h>
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxVNS.h"
#include "cVideoFrame.h"
#include "cImpulse.hpp"
#include "ofxDatGui.h"
#include "ofxDatGuiNumericInput.hpp"
#include "ofxJsonSettings.h"

#define kMostRegions 4096
#define kMaxSort 256

class cSortRecord
{
public:
	int16_t bigger;
	int16_t smaller;
	int16_t index;
	int32_t value;
};


class vns
{
public:
	ofxVNS ofxvns;
	ofxKinect kinect;
	cVideoFrame *rgbFrame;
	cVideoFrame *grayFrame;
	cVideoFrame *outFrame;
	cVideoFrame *lastFrame;
	cVideoFrame *slurFrame;
	cVideoFrame *aliasFrame;
	cVideoFrame *mapFrame;
	cVideoFrame *imageFrame;
	
	uint32_t numRegions;
	float averageRegionSize;
	int32_t columns;
	int32_t rows;
	int32_t sensitivity;
	bool averageByArea;
	bool squareRoots;
	float displayGain;
	int32_t sortDepth;
	bool showShape;
	
	vector<cImpulse *> impulses;
	
	ofxDatGuiNumericInput *rowsGUI;
	ofxDatGuiNumericInput *columnsGUI;
	ofxDatGuiSlider *sensitivityGUI;
	ofxDatGuiSlider *thresholdGUI;
	ofxDatGuiSlider *displaySensitivityGUI;
	ofxDatGuiToggle *sqrtGUI;
	ofxDatGuiSlider *sortDepthGUI;
	ofxDatGuiToggle *onOffGUI;
	ofxDatGuiComponent *scopeGUI;
	ofxDatGuiValuePlotter *plotter;
	ofxDatGuiNumericInput *soloGUI;
	ofxDatGuiSlider *regionThresholdGUI;
	ofxDatGuiToggle *showShapeGUI;
	ofxDatGuiSlider *outputRangeGUI;
	ofxDatGuiSlider *inputRangeGUI;
	ofxDatGuiSlider *bulgerGUI;

	uint32_t motionAccumulators[ kMostRegions ];//must be aligned!!!!!
	//	uint32_t xAccs[ kMostRegions ];
	//	uint32_t yAccs[ kMostRegions ];
	uint32_t motionAccs[ kMostRegions ];
	//	uint32_t xCenters[ kMostRegions ];
	//	uint32_t yCenters[ kMostRegions ];
	uint32_t regionSize[ kMostRegions ];
	uint32_t scaledMotionAccs[ kMostRegions ];
	uint32_t motionPixelValues[ kMostRegions ];
	int32_t sortedVals[ 32 ];
	int16_t sortedIndices[ 32 ];
	float outputValues[ kMostRegions ];
	int totalMotion;
	int regionThreshold;

	int solo;
	bool kinectOpen;
	bool gridChanged;
	ofxCvColorImage colorImg;
	ofPixels outPixels;
	ofPixels mapOutPixels;
	cOperand threshold;
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayMappedImage; // the near thresholded image
	
//	vns( void );
	~vns( void );
	void setup( void );
	void update( void );
	void draw( void );
	
	void RenderGridMap( cVideoFrame *mapFrame, int32_t rows, int32_t columns );
	void CalculateRegionSizes( cVideoFrame *mapFrame );
	void MotionScaling( void );
	void CalculateMotionPixels( void );
	uint16_t TranslateMotionValues( int16_t whichRegion, float sensitivity );
	void FastSort( void );
	float SmallTrigger( int32_t input );
	void UpdateMapAndImpulses( void );
	void Shape( void );
	
	void SetupGUI( void );
	void DrawGUI( void );
	void UpdateGUI( void );
	void onRowsChangedEvent(ofxDatGuiTextInputEvent e);
	void onColumnsChangedEvent(ofxDatGuiTextInputEvent e);
	void onThresholdChangedEvent(ofxDatGuiSliderEvent e);
	void onSensitivityChangedEvent(ofxDatGuiSliderEvent e);
	void onDisplaySensitivityChangedEvent(ofxDatGuiSliderEvent e);
	void onOnOffChangedEvent(ofxDatGuiToggleEvent e);
	void onSqrtChangedEvent(ofxDatGuiToggleEvent e);
	void onSortDepthChangedEvent(ofxDatGuiSliderEvent e);
	void onSoloChangedEvent(ofxDatGuiTextInputEvent e);
	void onRegionThresholdChangedEvent(ofxDatGuiSliderEvent e);
	void onShowShapeChangedEvent(ofxDatGuiToggleEvent e);
	void onInputRangeChangedEvent(ofxDatGuiSliderEvent e);
	void onOutputRangeChangedEvent(ofxDatGuiSliderEvent e);
	void onBulgerChangedEvent(ofxDatGuiSliderEvent e);
	
	void SaveToJSONSettings( void );
	void LoadFromJSONSettings( void );
	void HandleKeys( int key );
	void ExtractValuesForDisplay( void );

};

#endif /* vns_hpp */
