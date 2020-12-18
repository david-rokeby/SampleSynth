//
//  vns.cpp
//  vns_03
//
//  Created by David Rokeby on 2018-11-30.
//

//#include "vns.hpp"
//#include "ofxVNS.h"
#include "cVideoFrame.h"
#include "ConvertPlanar.h"
#include "ofxVNSConvert.hpp"
#include "FrameMotion.h"
#include "FrameSlur.h"
#include "FrameBins.h"


vns::~vns( void )
{
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	delete rgbFrame;
	delete grayFrame;
	delete lastFrame;
	delete slurFrame;
	delete aliasFrame;
	delete mapFrame;
	delete imageFrame;
}

void vns::setup( void )
{
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
//	kinect.open();		// opens first available kinect
//	kinectOpen = true;
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayMappedImage.allocate(kinect.width, kinect.height);
	
	mapOutPixels.allocate( kinect.width, kinect.height, 1 );
	outPixels.allocate( kinect.width, kinect.height, 1 );
	ofxvns.InitVNS();
	aliasFrame = new cVideoFrame();
	cAliasMap aliasMap;
	cBufferSpec rgbSpec, graySpec;
	rgbSpec.Init( kinect.width, kinect.height, t_RGBPlanar, t_UInt8 );
	rgbFrame = new cVideoFrame( &rgbSpec, true, &aliasMap );
	graySpec.Init( kinect.width, kinect.height, t_Y, t_UInt8 );
	grayFrame = new cVideoFrame( &graySpec, true, &aliasMap );
	lastFrame = new cVideoFrame( &graySpec, true, &aliasMap );
	outFrame = new cVideoFrame( &graySpec, true, &aliasMap );
	slurFrame = new cVideoFrame( &graySpec, true, &aliasMap );
	imageFrame = new cVideoFrame( &graySpec, true, &aliasMap );
	graySpec.SetComponent( t_SInt16 );
	mapFrame = new cVideoFrame( &graySpec, true, &aliasMap );
	numRegions = 0;
	gridChanged = true;
	sortDepth = 8;
	solo = -1;
	regionThreshold = 1000;
	
	rows = 8;
	columns = 10;
	sensitivity = 256;
	displayGain = 2;
	averageByArea = false;
	squareRoots = false;
	threshold.Fill( 12 );
	
	SetupGUI();
}


void vns::update( void )
{
	ofBackground(100, 100, 100);
	
	if( kinectOpen )
		kinect.update();
	ofxvns.MaintainAllocations();
	// there is a new frame and we are connected
	
	if( gridChanged )
		{
		UpdateMapAndImpulses();
		gridChanged = false;
		}
	
	if( kinect.isFrameNewVideo() ) {
		ofPixels pixels = kinect.getPixels();
		cBufferSpec rawSpec;
		rawSpec.format = t_RGB;
		rawSpec.bufferWidth = pixels.getWidth();
		rawSpec.height = pixels.getHeight();
		rawSpec.bufferHeight = pixels.getHeight();
		rawSpec.width = pixels.getWidth();
		rawSpec.component = t_UInt8;
		
        aliasFrame -> SetAsNonPlanarRawAlias_BR( &rawSpec, (uint8_t *)pixels.getData() );
		
		cBufferSpec destSpec;
		destSpec.Init( aliasFrame -> Spec() -> Width(), aliasFrame -> Spec() -> Height(), t_RGBPlanar, t_UInt8 );
		cAliasMap aliasMap;
		T_BufferResult result = rgbFrame -> UpdatePlaneBuffers_BR( &destSpec, &aliasMap, true, true, false );
		if( result == t_BufferIsValid )
			{
			CopyRGBToRGPlanar_8( aliasFrame, rgbFrame );
			destSpec.SetFormat( t_Y );
			T_BufferResult result = grayFrame -> UpdatePlaneBuffers_BR( &destSpec, &aliasMap, true, true, false );
			if( result == t_BufferIsValid )
				{
				ConvertPlanarRGBToY( rgbFrame, grayFrame, NULL, true );
//				Slur( grayFrame, lastFrame, slurFrame, NULL );
				AbsoluteDifferenceDouble_V( grayFrame, lastFrame, outFrame, &threshold, false, NULL );
				if( numRegions > 0 )
					{
					for( int32_t i = 0; i < kMostRegions; i ++ )
						motionAccs[ i ] = 0;
					MapDifferenceToRegionsExpectZeroVelocityStreams( outFrame, mapFrame, motionAccs, NULL );
					MotionScaling();
					FastSort();
					Shape(); // ready to send to Synth...
					// handle onset and release and adjust volume
					
					CalculateMotionPixels();
					// try mapping impulses to image for visual verification!!!!!!
					MapAccumulatorsToRegionsPictVelocityStreams( imageFrame, mapFrame, motionPixelValues, NULL );
					FillOFPixelsFromFrame( imageFrame, &mapOutPixels );
					}
				
				FillOFPixelsFromFrame( outFrame, &outPixels );
				grayImage.setFromPixels( outPixels.getData(), outPixels.getWidth(), outPixels.getHeight() );
				grayMappedImage.setFromPixels( mapOutPixels.getData(), mapOutPixels.getWidth(), mapOutPixels.getHeight() );
				lastFrame -> Copy( grayFrame );
				// now convert y to ofFrame
				}
			}
		}
	UpdateGUI();
}

void vns::draw( void )
{
	ofSetColor(255, 255, 255);
	
	//	kinect.drawDepth(10, 10, 400, 300);
	kinect.draw(0, 640, 425, 320);
	grayImage.draw(426, 640, 425, 320);
	grayMappedImage.draw(853, 640, 425, 320);
	
	DrawGUI();
	
	ofFill();
	for( int c = 0; c < rows * columns; ++ c )
		{
		int val = static_cast<int>( impulses[ c ] -> OutputValue() * 128 );
		if( val > 255 )
			val = 255;
		if( impulses[ c ] -> state == t_ImpulseNewOnset )
			{
			ofSetColor( 255, val, val );
			}
		else if( impulses[ c ] -> state == t_ImpulseNewRelease )
			{
			ofSetColor( val, 255, val );
			}
		else
			ofSetColor( val, val, val );
		
		ofRectangle rect;
		rect.x = ( c / 4 ) * 9 + 8;
		rect.y = 258 + ( c & 3 ) * 6;
		rect.width = 8;
		rect.height = 5;
		ofDrawRectangle(rect);
		}
}


void vns::CalculateMotionPixels( void )
{
	int32_t i;
	uint16_t value;
	
	if( showShape == false )
		{
		for( i = 0; i < numRegions; i ++ )
			{
			value = TranslateMotionValues( i, pow( ( displayGain ) / 4.0, 2.0 ) );
			if( averageByArea == false )
				value >>= 8;
			motionPixelValues[ i ] = ( value > 255 ) ? 255 : (uint8_t)value;
			}
		}
	else
		{
		for( i = 0; i < numRegions; i ++ )
			{
			value = impulses[ i ] -> OutputValue() * 64;
			motionPixelValues[ i ] = ( value > 255 ) ? 255 : (uint8_t)value;
			}
		}

}

uint16_t vns::TranslateMotionValues( int16_t whichRegion, float sensitivity )
{
	uint16_t cellsize;
	int32_t value;
	
	value = scaledMotionAccs[ whichRegion ];
	if( averageByArea == false )
		{
		cellsize = ( grayFrame -> Width() * grayFrame -> Height() ) / ( columns * rows );
		value = (int32_t)( (float)value * sensitivity );
		//value = (int32_t)( ( (float)value * (float)kDefaultWidth ) / (float)inputFrame -> Width() * sensitivity );
		}
	else
		value = (int32_t)( (float)value * sensitivity );
	
	if( value > 0xFFFF )
		value = 0xFFFF;
	
	return( (uint16_t)value );
}


void vns::RenderGridMap( cVideoFrame *mapFrame, int32_t rows, int32_t columns )
{
	//	renderingMap = true;
	
	int32_t i, j, rowbase;
	int32_t mapper, mapperbase, rowsize;
	int32_t x, y;
	int16_t *rMap = (int16_t *)mapFrame -> Buffer();
	
	if( rMap == NULL )
		return;
	
	for( i = 0; i < mapFrame -> BufferWidth() * mapFrame -> Height(); i ++ )
		rMap[ i ] = ( kMostRegions - 1 );
	
	for( y = 0; y < mapFrame -> Height(); y ++ )
		{
		mapperbase = y * rows / mapFrame -> Height() * columns;
		rowbase = y * mapFrame -> BufferWidth();
		for( x = 0; x < mapFrame -> Width(); x ++ )
			{
			mapper = mapperbase + ( ( x * columns ) / mapFrame -> Width() );
			rMap[ rowbase + x ] = mapper;
			}
		}
	j = 0;
	rowsize = columns;
	numRegions = rows * columns;
	CalculateRegionSizes( mapFrame );
	
	//	renderingMap = false;
}


void vns::CalculateRegionSizes( cVideoFrame *mapFrame )
{
	int32_t x;
	int32_t regionTotal = 0;
	//	cVideoFrame *mapFrame = BufferSet( mapBufferIndex ) -> Frame();
	int16_t *rMap = (int16_t *)mapFrame -> Buffer();
	
	if( rMap == NULL )
		{
		for( x = 0; x < numRegions; x ++ )
			regionSize[ x ] = 1;
		return;
		}
	
	for( x = 0; x < numRegions; x ++ )
		regionSize[ x ] = 0;
	
	//THIS IS PROBLEM!!!!!	????
	for( x = 0; x < mapFrame -> BufferWidth() * mapFrame -> Height(); x ++ )
		{
		if( rMap[ x ] < kMostRegions )
			regionSize[ rMap[ x ] ] ++;
		}
	
	for( int32_t i = 1; i < numRegions; i ++ )
		{
		regionTotal += regionSize[ i ];
		}
	if( regionTotal > 0 )
		averageRegionSize = regionTotal / ( numRegions - 1 );
	else
		averageRegionSize = mapFrame -> BufferWidth() * mapFrame -> Height();
	
}


void vns::MotionScaling( void )
{
	int16_t i;
	
	uint32_t *motionBuffer = motionAccs;
	uint32_t value;
	totalMotion = 0;
	for( i = 0; i < numRegions; i ++ )
		{
//		if( motionBuffer[ i ] < regionThreshold )
//			motionBuffer[ i ] = 0;
//		else
//			motionBuffer[ i ] -= regionThreshold;
		
		if( averageByArea )
			{
			if( squareRoots )
				{
				value = sqrt( ( ( (float)motionBuffer[ i ] ) * sensitivity / (float)regionSize[ i ] ) );
				}
			else
				value = ( ( (float)motionBuffer[ i ] ) * sensitivity / 256.0 ) / (float)regionSize[ i ];
			}
		else
			{
			if( squareRoots )
				{
				value = sqrt( ( (float)motionBuffer[ i ] ) * sensitivity );
				}
			else
				value = ( (float)motionBuffer[ i ] ) * sensitivity / 256.0;
			}
		
		scaledMotionAccs[ i ] = value;
		totalMotion += value;
		}
}

void vns::FastSort( void )
{
	cSortRecord most[ kMaxSort ];
	
	int16_t i, j, bufSize;
	int32_t testValue;
	int16_t testLink, firstLink, lastLink, numLinks, newLink, nextSmaller, numVals;
	//	int32_t sortedVals[ 32 ];
	//	int16_t sortedIndices[ 32 ];
	
	bufSize = numRegions;
	numVals = sortDepth; //or max sort depth
	if( numVals > kMaxSort )
		numVals = kMaxSort;
	if( numVals == 0 )
		return;
	if( numVals == 1 ) //FastSort fails with sortDepth == 1
		numVals = 2;
	
	for( i = 0; i < numVals; i ++ )
		{
		most[ i ].value = 0;
		most[ i ].bigger = -1;
		most[ i ].smaller = -1;
		most[ i ].index = 0;
		}
	
	firstLink = -1;
	lastLink = -1;
	numLinks = 0;
	
	int32_t firstRegion = 0;
	
	for( j = firstRegion; j < bufSize; j ++ )
		{
		testValue = scaledMotionAccs[ j ];
		if( testValue > 0 )				//if this entry has anything
			{
			if( lastLink == -1 ) 		//if this is the first value entered
				{
				most[ 0 ].value = testValue;
				most[ 0 ].index = j;
				firstLink = 0;
				lastLink = 0;
				numLinks ++;
				}
			else						//other entries exist in sorted list
				{
				if( testValue > most[ lastLink ].value )	//if the value is greater than the smallest rated value
					{
					testLink = most[ lastLink ].bigger;	//compare to next highest, through list until we come to one that is higher or the top
					if( testLink >= 0 )
						{
						while( testValue > most[ testLink ].value )
							{
							testLink = most[ testLink ].bigger;
							if( testLink < 0 )
								break;
							}
						}
					if( numLinks < numVals )
						newLink = numLinks ++;
					else
						{//all locations full
							newLink = lastLink;
							lastLink = most[ lastLink ].bigger;//lastLink is now one up the current list
							if( lastLink >= 0 )
								most[ lastLink ].smaller = -1;
						}
					most[ newLink ].index = j;				//fill in new entry
					most[ newLink ].bigger = testLink;
					most[ newLink ].value = testValue;
					
					if( testLink == -1 ) 					//new one is also largest
						{
						most[ newLink ].smaller = firstLink;
						most[ firstLink ].bigger = newLink;
						firstLink = newLink;
						}
					else									//insert in order
						{
						nextSmaller = most[ testLink ].smaller;
						most[ testLink ].smaller = newLink;
						most[ newLink ].smaller = nextSmaller;
						if( nextSmaller == -1 )
							lastLink = newLink; //make last link
						else
							most[ nextSmaller ].bigger = newLink; //or link smaller
						}
					}
				else if( numLinks < numVals )
					{
					newLink = numLinks ++;
					most[ newLink ].value = testValue;
					most[ newLink ].bigger = lastLink;
					most[ newLink ].index = j;
					most[ lastLink ].smaller = newLink;
					lastLink = newLink;
					}
				}
			}
		}
	for( i = 0; i < numVals; i ++ )
		{
		if( firstLink == -1 )
			break;
		sortedVals[ i ] = most[ firstLink ].value;
		sortedIndices[ i ] = most[ firstLink ].index;
		firstLink = most[ firstLink ].smaller;
		}
	if( i < numVals )
		{
		for( j = i; j < numVals; j ++ )
			{
			sortedVals[ j ] = 0;
			sortedIndices[ j ] = 0;
			}
		}
}

void vns::UpdateMapAndImpulses( void )
{
	RenderGridMap( mapFrame, rows, columns );
	int32_t outCount = rows * columns;
	if( outCount != impulses.size() )
		{
		int32_t oldSize = impulses.size();
		if( oldSize > outCount )
			{
			for( int i = outCount; i < oldSize; i ++ )
				{
				delete impulses[ i ];
				}
			}
		impulses.resize( (size_t)outCount );
		if( oldSize < outCount )
			{
			for( int i = oldSize; i < outCount; i ++ )
				impulses[ i ] = new cImpulse;
			}
		}
	for( int i = 0; i < outCount; i ++ )
		impulses[ i ] -> Reset();
}

void vns::Shape( void )
{
	cImpulse::limit = sortedVals[ sortDepth - 1 ];
	if( cImpulse::limit < regionThreshold )
		cImpulse::limit = regionThreshold;
	int size = rows * columns;
	for( int32_t i = 0; i < size; i ++ )
		{
		impulses[ i ] -> Process( scaledMotionAccs[ i ] );
		}
}


void vns::SaveToJSONSettings( void )
{
	bool useFullPath = false;
	string heirarchy( "vns" );
	
	// clear settings?
	Settings::getInt( heirarchy + "/rows" ) = rows;
	Settings::getInt( heirarchy + "/columns" ) = columns;
	Settings::getInt( heirarchy + "/sensitivity" ) = sensitivity;
	Settings::getInt( heirarchy + "/threshold" ) = static_cast<int>(threshold.GetPlaneValue( 0 ) );
	Settings::getInt( heirarchy + "/regionThreshold" ) = regionThreshold;
	Settings::getInt( heirarchy + "/sortDepth" ) = sortDepth;
	Settings::getFloat( heirarchy + "/displayGain" ) = displayGain;
	Settings::getBool( heirarchy + "/averageByArea" ) = averageByArea;
	Settings::getBool( heirarchy + "/squareRoots" ) = squareRoots;
	
	Settings::getFloat( heirarchy + "/prescaler" ) = cImpulse::prescaler;
	Settings::getFloat( heirarchy + "/outputRange" ) = cImpulse::outputRange;
	Settings::getFloat( heirarchy + "/bulger" ) = cImpulse::bulger;
	Settings::getFloat( heirarchy + "/smoothing1" ) = cImpulse::smoothing1;
	Settings::getFloat( heirarchy + "/smoothing2" ) = cImpulse::smoothing2;
	Settings::getFloat( heirarchy + "/volumeSmoother" ) = cImpulse::volumeSmoother;

	Settings::get().save("vns_settings.json");
}

void vns::LoadFromJSONSettings( void )
{
	ofxJSON root;
	bool success = root.open(string("vns_settings.json"));
	if( success )
		{
		const Json::Value vns_ = root[ "vns" ];
		if( vns_.type() != Json::nullValue )
			{
			const Json::Value rowsValue = vns_[ "rows" ];
			if( rowsValue.type() == Json::intValue )
				rows = rowsValue.asInt();

			const Json::Value columnsValue = vns_[ "columns" ];
			if( columnsValue.type() == Json::intValue )
				columns = columnsValue.asInt();

			const Json::Value sensitivityVal = vns_[ "sensitivity" ];
			if( sensitivityVal.type() == Json::intValue )
				sensitivity = sensitivityVal.asInt();
			
			const Json::Value thresholdVal = vns_[ "threshold" ];
			if( thresholdVal.type() == Json::intValue )
				threshold.Fill( thresholdVal.asInt() );
			
			const Json::Value regionThresholdVal = vns_[ "regionThreshold" ];
			if( regionThresholdVal.type() == Json::intValue )
				regionThreshold = regionThresholdVal.asInt();
			
			const Json::Value sortDepthVal = vns_[ "sortDepth" ];
			if( sortDepthVal.type() == Json::intValue )
				sortDepth = sortDepthVal.asInt();
			
			const Json::Value displayGainVal = vns_[ "displayGain" ];
			if( displayGainVal.type() == Json::realValue || displayGainVal.type() == Json::intValue )
				displayGain = displayGainVal.asFloat();
			
			const Json::Value averageByAreaVal = vns_[ "averageByArea" ];
			if( averageByAreaVal.type() == Json::booleanValue )
				averageByArea = averageByAreaVal.asBool();
			
			const Json::Value squareRootsVal = vns_[ "squareRoots" ];
			if( squareRootsVal.type() == Json::booleanValue )
				squareRoots = squareRootsVal.asBool();

			const Json::Value prescalerVal = vns_[ "prescaler" ];
			if( prescalerVal.type() == Json::realValue || prescalerVal.type() == Json::intValue )
				cImpulse::prescaler = prescalerVal.asFloat();
			
			const Json::Value outputRangeVal = vns_[ "outputRange" ];
			if( outputRangeVal.type() == Json::realValue || outputRangeVal.type() == Json::intValue )
				cImpulse::outputRange = outputRangeVal.asFloat();
			
			const Json::Value smoothing1Val = vns_[ "smoothing1" ];
			if( smoothing1Val.type() == Json::realValue || smoothing1Val.type() == Json::intValue )
				cImpulse::smoothing1 = smoothing1Val.asFloat();
			
			const Json::Value smoothing2Val = vns_[ "smoothing2" ];
			if( smoothing2Val.type() == Json::realValue || smoothing2Val.type() == Json::intValue )
				cImpulse::smoothing2 = smoothing2Val.asFloat();
			
			const Json::Value volumeSmootherVal = vns_[ "volumeSmoother" ];
			if( volumeSmootherVal.type() == Json::realValue || volumeSmootherVal.type() == Json::intValue )
				cImpulse::volumeSmoother = volumeSmootherVal.asFloat();
			
			const Json::Value bulgerVal = vns_[ "bulger" ];
			if( bulgerVal.type() == Json::realValue || bulgerVal.type() == Json::intValue )
				cImpulse::bulger = bulgerVal.asFloat();
			
	
			}
		}
	ExtractValuesForDisplay();
}

void vns::HandleKeys( int key )
{
	if( key == 's')
		{
		SaveToJSONSettings();
		}
	if( key == 'r')
		{
		LoadFromJSONSettings();
		}
}

void vns::ExtractValuesForDisplay( void )
{
	rowsGUI -> setValue( rows );
	columnsGUI -> setValue( columns );
	sensitivityGUI -> setValue( sensitivity );
	thresholdGUI -> setValue( static_cast<int>(threshold.GetPlaneValue( 0 ) ) );
	regionThresholdGUI -> setValue( regionThreshold );
	sortDepthGUI -> setValue( sortDepth );
	displaySensitivityGUI -> setValue( displayGain );
	sqrtGUI -> setChecked( squareRoots );
	outputRangeGUI -> setValue( cImpulse::outputRange );
	inputRangeGUI -> setValue( cImpulse::prescaler );
	bulgerGUI -> setValue( cImpulse::bulger );
}

