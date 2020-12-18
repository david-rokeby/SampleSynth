#include "ofVNSApp.h"
#include "ofSampleSynthApp.h"
#include "ofWindowSettings.h"
//#include "ofVNSApp.h"

int main() {
    ofSetupOpenGL(1440, 720, OF_WINDOW);
    ofRunApp(new ofVNSApp());
}

/*int main( void ){
    ofGLWindowSettings settings;
    settings.setSize( 1024, 512 );
    settings.setPosition(ofVec2f(300,0));
    auto synthWindow = ofCreateWindow(settings);

    settings.setSize( 1280, 960 );
    settings.setPosition(ofVec2f(0,0));
    auto vnsWindow = ofCreateWindow(settings);

    auto VNSApp = make_shared<ofVNSApp>();
    auto SampleSynthApp = make_shared<ofSampleSynthApp>();

    ofRunApp(synthWindow, SampleSynthApp);
    ofRunApp(vnsWindow, VNSApp);
    ofRunMainLoop();
}*/
