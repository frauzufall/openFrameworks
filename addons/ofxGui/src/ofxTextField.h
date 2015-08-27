#pragma once

#include "ofParameter.h"
#include "ofxBaseGui.h"
#include "ofxTextInputField.h"

class ofxTextField : public ofxBaseGui{
public:
	struct Config: public ofxBaseGui::Config{
		Config(){}
		Config(const ofxBaseGui::Config & c)
		:ofxBaseGui::Config(c){}
	};

    ofxTextField();
    ofxTextField(ofParameter<std::string> _text, const Config & config = Config());
    ~ofxTextField();
    ofxTextField & setup(ofParameter<std::string> _text, const Config & config);
    ofxTextField & setup(ofParameter<std::string> _text, float width = defaultWidth, float height = defaultHeight);
	

	virtual bool mouseMoved(ofMouseEventArgs & args);
	virtual bool mousePressed(ofMouseEventArgs & args);
	virtual bool mouseDragged(ofMouseEventArgs & args);
	virtual bool mouseReleased(ofMouseEventArgs & args);
	virtual bool mouseScrolled(ofMouseEventArgs & args){return false;}
	

	template<class ListenerClass, typename ListenerMethod>
	void addListener(ListenerClass * listener, ListenerMethod method){
		value.addListener(listener,method);
	}

	template<class ListenerClass, typename ListenerMethod>
	void removeListener(ListenerClass * listener, ListenerMethod method){
		value.removeListener(listener,method);
	}



    std::string operator=(std::string v);
    operator const std::string & ();

	virtual ofAbstractParameter & getParameter();

protected:
    virtual void render();
    ofParameter<string> value;
    ofParameter<string> labelName;
	bool bGuiActive;

	bool setValue(float mx, float my, bool bCheck);
	void generateDraw();
    void valueChanged(string & value);
    ofxTextInputField monoLineTextInput;
    ofVboMesh textMesh;
    ofPath border, bg;
};
