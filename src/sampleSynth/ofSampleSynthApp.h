#pragma once

#define windowHeight 256

#include "ofMain.h"
#include "ofxAudioFile.h"
#include "cPlayer.h"
//#include "cSliderParam.h"
#include "ofxDatGui.h"
#include "cSoundEditor.hpp"
class cSampleSynth;

class ofSampleSynthApp: public ofBaseApp{
    public:
        void setup();
        void update();
        void draw();
        void exit();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseScrolled(int x, int y, float scrollX, float scrollY);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

        cSampleSynth *synth;

        double playhead;
        std::atomic<double> playheadControl;
        double step;
        double sampleRate;
        float speed;
        float volume;
};

