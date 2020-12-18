#include "ofVNSApp.h"
//#include "vns.hpp"
#include "cSampleSynth.hpp"

//--------------------------------------------------------------
void ofVNSApp::setup() {
	ofBackground( 0 );
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate( 60 );
//	v.setup();
	synth = new cSampleSynth( 512, 128, 512 );

	synth -> Setup();
    synth -> LoadFromJSONSettings(string(""));
}

//--------------------------------------------------------------
void ofVNSApp::update() {
//	v.update();
//	if( v.kinectOpen )
//		synth -> ReceiveImpulses( v.impulses );
	synth -> Update();
}

//--------------------------------------------------------------
void ofVNSApp::draw() {
	ofBackground( 0 );
//	v.draw();
	synth -> Draw();
}

//--------------------------------------------------------------
void ofVNSApp::exit() {
}

//--------------------------------------------------------------
void ofVNSApp::keyPressed (int key) {
//	v.HandleKeys( key );
	synth -> HandleKeys( key );
}

//--------------------------------------------------------------
void ofVNSApp::mouseDragged(int x, int y, int button)
{
	synth -> MouseDragged( x, y, button );
}

//--------------------------------------------------------------
void ofVNSApp::mousePressed(int x, int y, int button)
{
    synth -> MousePressed( x, y, button );
}

//--------------------------------------------------------------
void ofVNSApp::mouseReleased(int x, int y, int button)
{
	synth -> MouseReleased( x, y, button );
}

//--------------------------------------------------------------
void ofVNSApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofVNSApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofVNSApp::windowResized(int w, int h)
{

}



void ofVNSApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
    synth->MouseScrolled(scrollY);
}
