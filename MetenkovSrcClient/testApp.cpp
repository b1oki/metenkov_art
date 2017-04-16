#include "testApp.h"

void testApp::setup()
{
	if(!XmlPhoto.loadFile("photo.xml") || !XmlSettings.loadFile("settings.xml"))
	{
		exit();
	}
	isSetup = false;
	showMusic = false;
	receiver.setup(XmlSettings.getValue("port", 10000));
	ofSetFullscreen(XmlSettings.getValue("fullscreen", 1));
	ofSetWindowShape(XmlSettings.getValue("width", 800), XmlSettings.getValue("height", 600));
	num = XmlPhoto.getNumTags("photo");
	if(num == 0)
	{
		exit();
	}
	photo.resize(num);
	whichPhoto = 0;
	whichPoint = 0;
	whichSpeed = 1;
	for(int j = 0; j < num; j++)
	{
		XmlPhoto.pushTag("photo", j);
		photo[j] = myRect();
		photo[j].numEvents = XmlPhoto.getNumTags("event");
		photo[j].photoEvent.resize(photo[j].numEvents);
		for(int i = 0; i < photo[j].numEvents; i++)
		{
			XmlPhoto.pushTag("event", i);
			photo[j].photoEvent[i].fadeIn = XmlPhoto.getValue("in", 0.0);
			photo[j].photoEvent[i].timeBegin = XmlPhoto.getValue("begin", 0.0);
			photo[j].photoEvent[i].timeEnd = XmlPhoto.getValue("end", 0.0);
			photo[j].photoEvent[i].fadeOut = XmlPhoto.getValue("out", 0.0);
			photo[j].photoEvent[i].bright = XmlPhoto.getValue("bright", 1.0);
			XmlPhoto.popTag();
		}
		for(int i = 0; i < 4; i++)
		{
			XmlPhoto.pushTag("point", i);
			photo[j].point[i].x = XmlPhoto.getValue("x", 0.0);
			photo[j].point[i].y = XmlPhoto.getValue("y", 0.0);
			XmlPhoto.popTag();
		}
		XmlPhoto.popTag();
	}
	sample.loadSound("music.mp3");
	sample.setLoop(true);
	isMuted = true;
	//sample.setVolume(XmlSettings.getValue("volume", 0.5));
	sample.setVolume(0.0f);
	sample.setPosition(0.0f);
	ofSetFrameRate(30);
	ofBackground(0,0,0);
	sample.play();
}

void testApp::update()
{
	if(!isSetup)
	{
		while( receiver.hasWaitingMessages() )
		{
			ofxOscMessage m;
			receiver.getNextMessage( &m );

			if ( m.getAddress() == "/time" )
			{
				timeMusic = m.getArgAsInt32( 0 );
				sample.setPositionMS(timeMusic);
			}
		}
		ofSoundUpdate();
		timeMusic = sample.getPositionMS();
	}
}

void testApp::draw()
{
	ofSetColor(255,255,255);
	ofFill();
	for(int i = 0; i < num; i++)
	{
		if(isSetup)
		{
			ofSetColor(255,255,255);
			photo[i].draw();
			ofSetColor(0,255,0);
			ofDrawBitmapString(
				ofToString(i+1),
				photo[i].point[0].x + (photo[i].point[1].x - photo[i].point[0].x) * 0.5f,
				photo[i].point[0].y + (photo[i].point[3].y - photo[i].point[0].y) * 0.5f);
		}
		else
		{
			for(int j = 0; j < photo[i].numEvents; j++)
			{
				if(ofInRange(
					timeMusic,
					photo[i].photoEvent[j].timeBegin - photo[i].photoEvent[j].fadeIn,
					photo[i].photoEvent[j].timeEnd + photo[i].photoEvent[j].fadeOut))
				{
					float alpha1 = ofMap(
						timeMusic,
						photo[i].photoEvent[j].timeBegin - photo[i].photoEvent[j].fadeIn,
						photo[i].photoEvent[j].timeBegin,
						0,
						photo[i].photoEvent[j].bright,
						true);
					float alpha2 = ofMap(
						timeMusic,
						photo[i].photoEvent[j].timeEnd + photo[i].photoEvent[j].fadeOut,
						photo[i].photoEvent[j].timeEnd,
						0,
						photo[i].photoEvent[j].bright,
						true);
					float alpha = min(alpha1, alpha2);
					ofSetColor(alpha * 255);
					photo[i].draw();
				}
			}
		}
	}
	ofSetColor(255,0,0);
	if(isSetup)
	{
		if(whichPoint == 5)
		{
			ofNoFill();
			ofCircle(
				photo[whichPhoto].point[0].x + (photo[whichPhoto].point[1].x - photo[whichPhoto].point[0].x) * 0.5f,
				photo[whichPhoto].point[0].y + (photo[whichPhoto].point[3].y - photo[whichPhoto].point[0].y) * 0.5f,
				10.0f);
		}
		else
		{
			ofCircle(
				photo[whichPhoto].point[whichPoint].x,
				photo[whichPhoto].point[whichPoint].y,
				5.0f);
		}
	}
	else
	{
		if(showMusic)
		{
			ofDrawBitmapString(
				ofToString(timeMusic),
				10.0f,
				ofGetHeight()-10);
		}
	}
}

void testApp::exit() {
	XmlPhoto.saveFile();
}

void testApp::keyPressed  (int key)
{
	switch (key)
	{
	case OF_KEY_F2:
		isSetup = false;
		sample.play();
		break;
	case OF_KEY_F3:
		isSetup = true;
		sample.stop();
		break;
	case OF_KEY_F7:
		showMusic = !showMusic;
		break;
	case 's':
		if(isSetup)
		{
			XmlPhoto.saveFile();
		}
		break;
	case 'm':
		if(!isSetup)
		{
			isMuted = !isMuted;
			if(isMuted)
			{
				sample.setVolume(0.0f);
			}
			else
			{
				sample.setVolume(XmlSettings.getValue("volume", 0.5));
			}
		}
		break;
	case 'q':
		if(isSetup)
		{
			whichSpeed = 1;
		}
		break;
	case 'w':
		whichSpeed = 2;
		break;
	case '1':
		if(isSetup)
		{
			whichPoint = 0;
		}
		break;
	case '2':
		if(isSetup)
		{
			whichPoint = 1;
		}
		break;
	case '3':
		if(isSetup)
		{
			whichPoint = 2;
		}
		break;
	case '4':
		if(isSetup)
		{
			whichPoint = 3;
		}
		break;
	case '5':
		if(isSetup)
		{
			whichPoint = 5;
		}
		break;
	case ',':
	case '<':
		if(isSetup)
		{
			if(whichPhoto > 0)
			{
				whichPhoto--;
			}
		}
		else
		{
			sample.setPositionMS(sample.getPositionMS() - 5000);
		}
		break;
	case '.':
	case '>':
		if(isSetup)
		{
			if(whichPhoto < num-1)
			{
				whichPhoto++;
			}
		}
		else
		{
			sample.setPositionMS(sample.getPositionMS() + 5000);
		}
		break;
	case OF_KEY_LEFT:
		if(isSetup)
		{
			if(whichPoint == 5)
			{
				switch (whichSpeed)
				{
				case 1:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, -0.1f, 0.0f);
					}
					break;
				case 2:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, -10.0f, 0.0f);
					}
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				for(int i = 0; i < 4; i++)
				{
					XmlPhoto.pushTag("point", i);
					XmlPhoto.setValue("x", photo[whichPhoto].point[i].x);
					XmlPhoto.popTag();
				}
				XmlPhoto.popTag();
			} else
			{
				switch (whichSpeed)
				{
				case 1:
					photo[whichPhoto].move(whichPoint, -0.1f, 0.0f);
					break;
				case 2:
					photo[whichPhoto].move(whichPoint, -10.0f, 0.0f);
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				XmlPhoto.pushTag("point", whichPoint);
				XmlPhoto.setValue("x", photo[whichPhoto].point[whichPoint].x);
				XmlPhoto.popTag();
				XmlPhoto.popTag();
			}
		}
		break;
	case OF_KEY_RIGHT:
		if(isSetup) {
			if(whichPoint == 5)
			{
				switch (whichSpeed)
				{
				case 1:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, 0.1f, 0.0f);
					}
					break;
				case 2:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, 10.0f, 0.0f);
					}
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				for(int i = 0; i < 4; i++) {
					XmlPhoto.pushTag("point", i);
					XmlPhoto.setValue("x", photo[whichPhoto].point[i].x);
					XmlPhoto.popTag();
				}
				XmlPhoto.popTag();
			} else
			{
				switch (whichSpeed)
				{
				case 1:
					photo[whichPhoto].move(whichPoint, 0.1f, 0.0f);
					break;
				case 2:
					photo[whichPhoto].move(whichPoint, 10.0f, 0.0f);
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				XmlPhoto.pushTag("point", whichPoint);
				XmlPhoto.setValue("x", photo[whichPhoto].point[whichPoint].x);
				XmlPhoto.popTag();
				XmlPhoto.popTag();
			}
		}
		break;
	case OF_KEY_UP:
		if(isSetup)
		{
			if(whichPoint == 5)
			{
				switch (whichSpeed)
				{
				case 1:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, 0.0f, -0.1f);
					}
					break;
				case 2:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, 0.0f, -10.0f);
					}
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				for(int i = 0; i < 4; i++)
				{
					XmlPhoto.pushTag("point", i);
					XmlPhoto.setValue("x", photo[whichPhoto].point[i].x);
					XmlPhoto.popTag();
				}
				XmlPhoto.popTag();
			} else
			{
				switch (whichSpeed)
				{
				case 1:
					photo[whichPhoto].move(whichPoint,0.0f, -0.1f);
					break;
				case 2:
					photo[whichPhoto].move(whichPoint, 0.0f, -10.0f);
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				XmlPhoto.pushTag("point", whichPoint);
				XmlPhoto.setValue("y", photo[whichPhoto].point[whichPoint].y);
				XmlPhoto.popTag();
				XmlPhoto.popTag();
			}
		}
		break;
	case OF_KEY_DOWN:
		if(isSetup) {
			if(whichPoint == 5)
			{
				switch (whichSpeed)
				{
				case 1:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, 0.0f, 0.1f);
					}
					break;
				case 2:
					for(int i = 0; i < 4; i++)
					{
						photo[whichPhoto].move(i, 0.0f, 10.0f);
					}
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				for(int i = 0; i < 4; i++)
				{
					XmlPhoto.pushTag("point", i);
					XmlPhoto.setValue("x", photo[whichPhoto].point[i].x);
					XmlPhoto.popTag();
				}
				XmlPhoto.popTag();
			} else
			{
				switch (whichSpeed)
				{
				case 1:
					photo[whichPhoto].move(whichPoint,0.0f, 0.1f);
					break;
				case 2:
					photo[whichPhoto].move(whichPoint, 0.0f, 10.0f);
					break;
				}
				XmlPhoto.pushTag("photo", whichPhoto);
				XmlPhoto.pushTag("point", whichPoint);
				XmlPhoto.setValue("y", photo[whichPhoto].point[whichPoint].y);
				XmlPhoto.popTag();
				XmlPhoto.popTag();
			}
		}
		break;
	}
}

void testApp::keyReleased(int key){}
void testApp::mouseMoved(int x, int y ){}
void testApp::mouseDragged(int x, int y, int button){}
void testApp::mousePressed(int x, int y, int button){}
void testApp::mouseReleased(int x, int y, int button){}
void testApp::windowResized(int w, int h){}
void testApp::dragEvent(ofDragInfo dragInfo){}

myRect::myRect()
{
	point.resize(4);
	for(int i = 0; i < 4; i++)
	{
		point[i] = ofPoint();
	}
}

myRect::myRect(ofPoint A, ofPoint B, ofPoint C, ofPoint D)
{
	point.resize(4);
	point[0] = A;
	point[1] = B;
	point[2] = C;
	point[3] = D;
}

void myRect::draw()
{
	ofBeginShape();
	ofVertex(point[0].x, point[0].y);
	ofVertex(point[1].x, point[1].y);
	ofVertex(point[2].x, point[2].y);
	ofVertex(point[3].x, point[3].y);
	ofEndShape();
}
void myRect::move(int id, float dx, float dy)
{
	point[id].x += dx;
	point[id].y += dy;
}