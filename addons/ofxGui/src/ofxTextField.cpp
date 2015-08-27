#include "ofxTextField.h"
#include "ofGraphics.h"
using namespace std;

ofxTextField::ofxTextField():ofxBaseGui(){}

ofxTextField::ofxTextField(ofParameter<std::string> _text, const Config & config)
:ofxBaseGui(config){
    setup(_text, config);
}

ofxTextField::~ofxTextField(){
    value.removeListener(this,&ofxTextField::valueChanged);
}

ofxTextField & ofxTextField::setup(ofParameter<std::string> _text, const Config & config){
    ofxBaseGui::setup(config);
    bGuiActive = false;
    value.makeReferenceTo(_text);
    value.addListener(this,&ofxTextField::valueChanged);
    registerMouseEvents();
    setNeedsRedraw();
    monoLineTextInput.setup();
    monoLineTextInput.text = value;

    return *this;
}

ofxTextField & ofxTextField::setup(ofParameter<std::string> _text, float width, float height){
	b.x = 0;
	b.y = 0;
	b.width = width;
	b.height = height;
	bGuiActive = false;
    value.makeReferenceTo(_text);

    value.addListener(this,&ofxTextField::valueChanged);
	registerMouseEvents();
	setNeedsRedraw();
    monoLineTextInput.setup();

	return *this;
}

bool ofxTextField::mouseMoved(ofMouseEventArgs & args){
	if(isGuiDrawing() && b.inside(ofPoint(args.x,args.y))){
		return true;
	}else{
		return false;
	}
}

bool ofxTextField::mousePressed(ofMouseEventArgs & args){
	if(setValue(args.x, args.y, true)){
		return true;
	}else{
		return false;
	}
}

bool ofxTextField::mouseDragged(ofMouseEventArgs & args){
	if(bGuiActive && b.inside(ofPoint(args.x,args.y))){
		return true;
	}else{
		return false;
	}
}

bool ofxTextField::mouseReleased(ofMouseEventArgs & args){
	bool wasGuiActive = bGuiActive;
	bGuiActive = false;
	if(wasGuiActive && b.inside(ofPoint(args.x,args.y))){
		return true;
	}else{
		return false;
	}
}

void ofxTextField::generateDraw(){

    textMesh.clear();
    if(bShowName){
        string name_str = getName()+":";
        float name_width = getTextWidth(name_str, b.height);
        textMesh = getTextMesh(name_str, b.x+textPadding, b.y+b.height / 2 + 4);
        monoLineTextInput.bounds.setPosition(b.x+name_width, b.getCenter().y - monoLineTextInput.bounds.getHeight()/2);
        monoLineTextInput.bounds.setWidth(b.getWidth() - name_width);
    }else {
        monoLineTextInput.bounds.setPosition(b.x, b.getCenter().y - monoLineTextInput.bounds.getHeight()/2);
        monoLineTextInput.bounds.setWidth(b.getWidth());
    }

    border.rectangle(monoLineTextInput.bounds.x, b.y, monoLineTextInput.bounds.width, b.height);
    bg.rectangle(b);
    bg.setFillColor(thisBackgroundColor);
    bg.setFilled(false);
    border.setStrokeColor(thisBorderColor);
    border.setStrokeWidth(1);
    border.setFilled(false);

}

void ofxTextField::render(){
    bg.draw();
    border.draw();
    if(bShowName){
        ofColor c = ofGetStyle().color;
        ofBlendMode blendMode = ofGetStyle().blendingMode;
        if(blendMode!=OF_BLENDMODE_ALPHA){
            ofEnableAlphaBlending();
        }
        ofSetColor(thisTextColor);

        bindFontTexture();
        textMesh.draw();
        unbindFontTexture();

        ofSetColor(c);
        if(blendMode!=OF_BLENDMODE_ALPHA){
            ofEnableBlendMode(blendMode);
        }
    }
    monoLineTextInput.draw();

}

std::string ofxTextField::operator=(std::string v){
	value = v;
	return v;
}

ofxTextField::operator const std::string & (){
	return value;
}

bool ofxTextField::setValue(float mx, float my, bool bCheck){

	if( !isGuiDrawing() ){
		bGuiActive = false;
		return false;
    }
	return false;
}

ofAbstractParameter & ofxTextField::getParameter(){
	return value;
}

void ofxTextField::valueChanged(std::string & value){
    monoLineTextInput.text = value;
    setNeedsRedraw();
}
