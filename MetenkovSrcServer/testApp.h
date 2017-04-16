#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

struct PhotoEvent {
	float timeBegin, timeEnd;
	float bright;
	float fadeIn, fadeOut;
};

struct myRect {
	vector<ofPoint> point;
	int numEvents;
	vector<PhotoEvent> photoEvent;
	myRect();
	myRect(ofPoint A, ofPoint B, ofPoint C, ofPoint D);
	void draw();
	void move(int id, float x, float y);
};

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);

	ofxXmlSettings XmlPhoto, XmlSettings;
	ofxOscSender sender;
	ofSoundPlayer sample;
	vector<myRect> photo;
	int num, whichPhoto, whichPoint, whichSpeed, lastSendTime;
	float timeMusic;
	bool isSetup, showMusic, isMuted;
};