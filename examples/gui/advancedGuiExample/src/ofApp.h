#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(ofMouseEventArgs & args);
		void mouseDragged(ofMouseEventArgs & args);
		void mousePressed(ofMouseEventArgs & args);
		void mouseReleased(ofMouseEventArgs & args);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

	private:

		std::unique_ptr<ofx::DOM::Document> gui_doc;

		ofxPanel *panel1, *panel4, *panel5;
		ofxGuiGroup *rotary;
		ofxGuiGroup* matrix, *panel3, *panel2;
		ofParameter<string> matrix_active_name;
		ofParameter<int> matrix_active_index;
		ofParameter<float> floatfield_param;
		ofParameter<string> textfield_param;

		ofxButton *button;

		ofParameter <string> label_param;
		ofParameter <bool> toggle_param;
		ofParameter <float> slider_param;
		ofParameter <bool> toggle1_param, toggle2_param, toggle3_param, toggle4_param;
		ofParameter <float> slider1_param, slider2_param, slider3_param, slider4_param;
		vector <ofParameter <bool> > matrix_params;
		ofParameterGroup g;

		ofParameterGroup cameraMatrixParameters;
		ofParameter<bool> cam0;
		ofParameter<bool> cam2;
		ofParameter<bool> cam3;
		ofParameter<bool> cam1;
		ofxGuiGroup *matrixCam;

		void toggleGroupHeader(bool & val);
		void updateMatrixIndex(int& index);

};

