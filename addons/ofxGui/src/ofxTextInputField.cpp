//
//  textInput.cpp
//
//	MIT license
//	http://www.opensource.org/licenses/mit-license.php
//

#include "ofxTextInputField.h"

//----------
ofxTextInputField::ofxTextInputField() {
    shiftMap[44] = '<';
    shiftMap[45] = '_';
    shiftMap[46] = '>';
    shiftMap[48] = ')';
    shiftMap[49] = '!';
    shiftMap[50] = '@';
    shiftMap[51] = '#';
    shiftMap[52] = '$';
    shiftMap[53] = '%';
    shiftMap[54] = '^';
    shiftMap[55] = '&';
    shiftMap[56] = '*';
    shiftMap[57] = '(';
    shiftMap[61] = '+';
    shiftMap[63] = '/';
    shiftMap[91] = '{';
    shiftMap[92] = '|';
    shiftMap[93] = '}';
    shiftMap[96] = '~';
    
	text = "";
	multiline = false;
	autoTab = true;
	cursorPosition = 0;
	selectionBegin = 0;
	selectionEnd = 0;
	selecting = false;
	
	fontRef = NULL;
    this->enabled = false;
	this->editing = false;
	this->useListeners = false;
	this->hasListeners = false;
    bounds = ofRectangle(0,0,100,22);
	
    drawCursor = false;
	autoClear = false;
	mouseDownInRect = false;

	offset = 0;

	fontRef = new ofxTextInput::BitmapFontRenderer();
	
	verticalPadding = 2;
	horizontalPadding = 3;
	lastTimeCursorMoved = ofGetElapsedTimef();
}

//----------
ofxTextInputField::~ofxTextInputField(){
	this->removeListeners();
}

//----------
void ofxTextInputField::setup(bool enableListeners){
	enable();
	this->setUseListeners(enableListeners);
}

//----------
void ofxTextInputField::setFont(OFX_TEXTFIELD_FONT_RENDERER & font){
	if (fontRef->isBitmapFont()) {
		delete fontRef;
	}
	fontRef = new ofxTextInput::TypedFontRenderer(&font);
}

//----------
ofxTextInput::FontRenderer * ofxTextInputField::getFontRenderer() {
	return this->fontRef;
}

//----------
void ofxTextInputField::enable(){
	if(!this->enabled){
		this->enabled = true;
	}
}

//----------
void ofxTextInputField::disable(){
	if(this->editing){
		endEditing();
	}
	if(this->enabled){
		this->enabled = false;
    }
	
}

//----------
bool ofxTextInputField::isEnabled() const {
	return this->enabled;
}

//----------
void ofxTextInputField::beginEditing() {
    if(!this->editing){
        ofSendMessage(TEXTFIELD_IS_ACTIVE);
        this->editing = true;
        drawCursor = true;
		if(autoClear) {
			clear();
		}
    }
}

//----------
void ofxTextInputField::endEditing() {
    if(this->editing){
        ofSendMessage(TEXTFIELD_IS_INACTIVE);
        this->editing = false;
        this->drawCursor = false;
		this->shiftHeld = false;
		this->commandHeld = false;
    }
}

//----------
bool ofxTextInputField::isEditing() const {
    return this->editing;
}

//----------
void ofxTextInputField::setUseListeners(bool useListeners) {
	if (useListeners) {
		this->addListeners();
	} else {
		this->removeListeners();
	}
}

//----------
bool ofxTextInputField::getUseListeners() const {
	return this->useListeners;
}

//----------
void ofxTextInputField::draw() {

	// argh, splitting all the time.
	lines = ofSplitString(text, "\n");
	getCursorCoords(cursorPosition, cursorX, cursorY);
	cursorPos = horizontalPadding + fontRef->stringWidth(lines[cursorY].substr(0, cursorX))+offset;

	float textWidth = fontRef->stringWidth(text);
	float maxWidth = bounds.width-horizontalPadding*2;
	if(textWidth + offset > maxWidth && cursorPos > maxWidth){
		offset -= cursorPos-maxWidth;
	}

	ofPushMatrix();
	ofTranslate(bounds.x, bounds.y);

	if(selecting) {
		ofPushStyle();
		// argh, splitting all the time.
		int beginCursorX, beginCursorY;
		int endCursorX, endCursorY;
		getCursorCoords(selectionBegin, beginCursorX, beginCursorY);
		getCursorCoords(selectionEnd, endCursorX, endCursorY);

		float startX = fontRef->stringWidth(lines[beginCursorY].substr(0,beginCursorX));
		float endX = fontRef->stringWidth(lines[endCursorY].substr(0, endCursorX));

		ofSetHexColor(0x6988db);
		ofFill();
		
		if(beginCursorY==endCursorY) {
			// single line selection
			ofDrawRectangle(horizontalPadding + startX, verticalPadding + fontRef->getLineHeight()*beginCursorY,
				   endX - startX, fontRef->getLineHeight());
		} else {
			
			// multiline selection.
			// do first line to the end
			ofDrawRectangle(horizontalPadding + startX, verticalPadding + fontRef->getLineHeight()*beginCursorY,
				   fontRef->stringWidth(lines[beginCursorY]) - startX,
				   fontRef->getLineHeight()
			);
			
			// loop through entirely selected lines
			for(int i = beginCursorY + 1; i < endCursorY; i++) {
				ofDrawRectangle(horizontalPadding, verticalPadding + fontRef->getLineHeight()*i,
					   fontRef->stringWidth(lines[i]),
					   fontRef->getLineHeight()
				);
			}
			// do last line up to endX
			ofDrawRectangle(horizontalPadding, verticalPadding + fontRef->getLineHeight()*endCursorY,
					endX, fontRef->getLineHeight()
			);
		}
		ofPopStyle();
		
		
		//draw cursor line
    } else if(drawCursor) {
        ofPushStyle();
		// cursor should only blink when its been idle, and animation
		// should be a clipped sine wave
        float timeFrac = 0.5 * ofClamp(cos(6.0f * (ofGetElapsedTimef()-lastTimeCursorMoved))*4, -1, 1) + 0.5;
        
        ofColor col = ofGetStyle().color;
		ofSetColor(col.r * timeFrac, col.g * timeFrac, col.b * timeFrac);
		// calculate this every loop.
        getCursorCoords(cursorPosition, cursorX, cursorY);
	//	printf("Pos: %d    X: %d   Y: %d\n", cursorPosition, cursorX, cursorY);
		cursorPos = horizontalPadding*2 + fontRef->stringWidth(lines[cursorY].substr(0, cursorX))+offset;
        
		int cursorTop = verticalPadding + fontRef->getLineHeight()*cursorY;
		int cursorBottom = cursorTop + fontRef->getLineHeight();
		
		ofSetLineWidth(1.0f);
		//TODO: multiline with fontRef
		ofDrawLine(cursorPos, cursorTop,
			   cursorPos, cursorBottom);
        ofPopStyle();
    }
	
	fontRef->drawString(
				getCroppedText(text,-offset-horizontalPadding,bounds.width-2*horizontalPadding),
				horizontalPadding,
				fontRef->getLineHeight() + verticalPadding);
	ofPopMatrix();
}

//----------
void ofxTextInputField::clear(){
	text.clear();
	cursorPosition = 0;
	this->notifyTextChange();
}

//----------
bool ofxTextInputField::keyPressed(ofKeyEventArgs& args) {
	//ew: add charachter (non unicode sorry!)
	//jg: made a step closer to this with swappable renderers and ofxFTGL -- but need unicode text input...

	//if we're not focused, then ignore the keypress
	if (!this->editing) {
		return false;
	}

	lastTimeCursorMoved = ofGetElapsedTimef();
	int key = args.key;
	
    if(key == OF_KEY_SHIFT) {
		this->shiftHeld = true;
    }
    
    if(key == 4352) {
		this->commandHeld = true;
    }
	
#if defined(USE_GLFW_CLIPBOARD) && defined(TARGET_OSX)
    if(key == 'c' && this->commandHeld) {
        setClipboard(text.substr(selectionBegin, selectionEnd - selectionBegin));
		return true;
    }
	
	if(key == 'v' && this->commandHeld) {
        text.insert(cursorPosition, getClipboard());
		return true;
    }
	#endif
    
	if ((key >=32 && key <=126) || key=='\t' || key==OF_KEY_RETURN) {
		if(selecting) {
			text.erase(text.begin() + selectionBegin,
					   text.begin() + selectionEnd
					   );
			cursorPosition = selectionBegin;
			selecting = false;
			this->notifyTextChange();
		}
	}
			
	if (key == OF_KEY_RETURN) {
		if(!multiline) {
			endEditing();
			this->notifyHitReturn();
			return true;
		}
		text.insert(text.begin()+cursorPosition, '\n');
		cursorPosition++;

		if(autoTab) {
			// how much whitespace is there on the previous line?
			int xx, yy;
			getCursorCoords(cursorPosition, xx, yy);
			vector<string> lines = ofSplitString(text, "\n");
			if(yy>0) {
				// collect all the whitespace on the previous line.
				string previousWhitespace = "";
				string previousLine = lines[yy-1];
				for(unsigned int i = 0; i < previousLine.size(); i++) {
					if(previousLine[i]==' ' || previousLine[i]=='\t') {
						previousWhitespace += previousLine[i];
					} else {
						break;
					}
				}
				// if we have a curly brace as the last char on the previous line
				// increase the indentation
				if(previousLine[previousLine.size()-1]=='{') {
					if(previousWhitespace=="") {
						previousWhitespace = "\t";
					} else {
						previousWhitespace += previousWhitespace[previousWhitespace.size()-1];
					}
				}
				text = text.insert(cursorPosition, previousWhitespace);
				cursorPosition += previousWhitespace.size();
			}
		}
		this->notifyTextChange();
		return true;
	}
	
	if ((key >=32 && key <=126) || key=='\t') {
        if(this->shiftHeld) {
            
            char toInsert;
            if( !(key > 96 && key < 123) && !(key > 65 && key < 90) && shiftMap.find(key) != shiftMap.end() ) {
                toInsert = shiftMap[key];//toInsert = key - 32;
            } else {
                toInsert = key;
            }
            
            text.insert(text.begin()+cursorPosition, toInsert);
        } else {
            text.insert(text.begin()+cursorPosition, key);
        }
		cursorPosition++;
		this->notifyTextChange();
	}
	
	if (key==OF_KEY_BACKSPACE) {
		if(selecting) {
			text.erase(text.begin() + selectionBegin,
					   text.begin() + selectionEnd
			);
			cursorPosition = selectionBegin;
			selecting = false;
			this->notifyTextChange();
		}
		else {
			if (cursorPosition>0) {
				text.erase(text.begin()+cursorPosition-1);
				--cursorPosition;
				this->notifyTextChange();

				getCursorCoords(cursorPosition, cursorX, cursorY);
				cursorPos = horizontalPadding + fontRef->stringWidth(lines[cursorY].substr(0, cursorX))+offset;
				if(cursorPos < 0){
					offset -= cursorPos;
				}
			}
		}
	}
	
	if (key==OF_KEY_DEL) {
		if(selecting) {
			text.erase(text.begin() + selectionBegin,
					   text.begin() + selectionEnd
					   );
			cursorPosition = selectionBegin;
			selecting = false;
			this->notifyTextChange();
		}
		else {
			if ((int)text.size() > cursorPosition) {
				text.erase(text.begin()+cursorPosition);
				this->notifyTextChange();
			}
		}
	}
	
	if (key==OF_KEY_LEFT){
		if(selecting) {
			cursorPosition = selectionBegin;
			selecting = false;
			
		} else {
			if (cursorPosition>0){
				--cursorPosition;

				getCursorCoords(cursorPosition, cursorX, cursorY);
				cursorPos = horizontalPadding + fontRef->stringWidth(lines[cursorY].substr(0, cursorX))+offset;
				if(cursorPos < 0){
					offset -= cursorPos;
				}

			}
		}
	}
	
	if (key==OF_KEY_RIGHT){
		if(selecting) {
			cursorPosition = selectionEnd;
			selecting = false;
		} else {
			if (cursorPosition<(int)text.size()){
				++cursorPosition;
			}
		}
	}
	
	if (key==OF_KEY_UP){
		if(selecting) {
			cursorPosition = selectionBegin;
			selecting = false;
			
		} else if (this->multiline) {
			if (cursorPosition>0) {
				int xx, yy;
				getCursorCoords(cursorPosition, xx, yy);
				if(yy>0) {
					yy--;
					vector<string> lines = ofSplitString(text, "\n");
					xx = MIN(lines[yy].size()-1, xx);
					cursorPosition = xx;
					for(int i = 0; i < yy; i++) cursorPosition += lines[i].size()+1;
					printf("Cursor position: %d\n", cursorPosition);
				} else {
					cursorPosition = 0;
				}
			}
		}
	}
	
	if (key==OF_KEY_DOWN){
		if(selecting) {
			cursorPosition = selectionEnd;
			selecting = false;
		} else if (this->multiline) {
			int xx, yy;
			getCursorCoords(cursorPosition, xx, yy);
			vector<string> lines = ofSplitString(text, "\n");
			yy++;
			if(yy<(int)lines.size()-1) {
				
				xx = MIN(lines[yy].size()-1, xx);
				cursorPosition = xx;
				for(int i = 0; i < yy; i++) cursorPosition += lines[i].size()+1;
				printf("Cursor position: %d\n", cursorPosition);
			} else {
				cursorPosition = text.size()-1;
			}
		}
	}
	return true;
}

//----------
bool ofxTextInputField::keyReleased(ofKeyEventArgs &a){
	//if we're not focused, then ignore the keypress
	if (!this->editing) {
		return false;
	}

    if(a.key == 4352) {
        this->commandHeld = false;
    }

    if(a.key == OF_KEY_SHIFT) {
        this->shiftHeld = false;
    }
	return true;
}

//----------
bool ofxTextInputField::mousePressed(ofMouseEventArgs& args) {
	if (!this->enabled) {
		return false;
	}

	mouseDownInRect = bounds.inside(args.x, args.y);
	if (mouseDownInRect) {
		cursorPosition = getCursorPositionFromMouse(args.x, args.y);
		lastTimeCursorMoved = ofGetElapsedTimef();
		selecting = false;
		return true;
	}
	return false;
}

//----------
bool ofxTextInputField::mouseDragged(ofMouseEventArgs& args) {
	if (!this->enabled) {
		return false;
	}

	if (bounds.inside(args.x, args.y)) {
		int pos = getCursorPositionFromMouse(args.x, args.y);
		if (pos != cursorPosition) {
			selecting = true;
			selectionBegin = MIN(pos, cursorPosition);
			selectionEnd = MAX(pos, cursorPosition);
			return true;
		}
		else {
			selecting = false;
		}
	}
	return false;
}

//----------
bool ofxTextInputField::mouseReleased(ofMouseEventArgs& args) {
	if (!this->enabled) {
		return false;
	}

	if (bounds.inside(args.x, args.y)) {
		if (!this->editing && mouseDownInRect) {
			beginEditing();
			return true;
		}
	}
	else {
		endEditing();
	}
	return false;
}

//----------
float ofxTextInputField::getVerticalPadding() const {
	return this->verticalPadding;
}

//----------
float ofxTextInputField::getHorizontalPadding() const {
	return this->horizontalPadding;
}

//----------
void ofxTextInputField::setVerticalPadding(float vp){
    verticalPadding = vp;
}
//----------
void ofxTextInputField::setHorizontalPadding(float hp){
    horizontalPadding = hp;
}




#ifdef USE_GLFW_CLIPBOARD

#if (_MSC_VER)
#include <GLFW/glfw3.h>
#else
#include "GLFW/glfw3.h"
#endif

//----------
void ofxTextInputField::setClipboard(string clippy){
	glfwSetClipboardString((GLFWwindow*)ofGetWindowPtr()->getCocoaWindow(), clippy.c_str());
}

//----------
string ofxTextInputField::getClipboard(){
	const char *clip = glfwGetClipboardString((GLFWwindow*)ofGetWindowPtr()->getCocoaWindow());
	if (clip != NULL) {
		return string(clip);
	}
	else {
		return "";
	}

}

#endif


//----------
void ofxTextInputField::notifyTextChange() {
	ofNotifyEvent(this->onTextChange, this->text, this);
}

//----------
void ofxTextInputField::notifyHitReturn() {
	ofNotifyEvent(this->onHitReturn, this->text, this);
}

//----------
void ofxTextInputField::getCursorCoords(int pos, int &cursorX, int &cursorY) {
	vector<string> lines = ofSplitString(text, "\n");

	int c = 0;
	for (unsigned int i = 0; i < lines.size(); i++) {
		if (pos <= c + (int)lines[i].size()) {
			cursorY = i;
			cursorX = pos - c;
			return;
		}
		c += lines[i].size() + 1;
	}
}

//----------
string ofxTextInputField::getCroppedText(std::string text, float offset_beginning, float width) {
	unsigned int index1, index2;
	for(index1 = 0; index1 < text.size(); index1++){
		float w = fontRef->stringWidth(text.substr(0,index1));
		if(w >= offset_beginning){
			break;
		}
	}
	for(index2 = text.size()-1; index2 >= index1; index2--){
		float w = fontRef->stringWidth(text.substr(index1,index2+1));
		if(w <= width){
			break;
		}
	}

	return text.substr(index1, index2+1);
}

//----------
int ofxTextInputField::getCursorPositionFromMouse(int x, int y) {
	int cursorX = 0;
	int cursorY = 0;
	float pos = y - bounds.y - verticalPadding;
	pos /= fontRef->getLineHeight();
	int line = pos;
	cursorY = line;

	vector<string> lines = ofSplitString(text, "\n");
	if (cursorY >= (int)lines.size() - 1) cursorY = (int)lines.size() - 1;
	if (lines.size()>0) {
		cursorX = fontRef->getPosition(lines[cursorY], x - horizontalPadding - bounds.x - offset);
	}
	int c = 0;
	for (int i = 0; i < cursorY; i++) {
		c += lines[i].size() + 1;
	}
	c += cursorX;
	return c;
}

//----------
void ofxTextInputField::addListeners() {
	if (this->hasListeners) {
		return;
	}
	ofRegisterMouseEvents(this, -1);
	ofRegisterKeyEvents(this, -1);
}

//----------
void ofxTextInputField::removeListeners() {
	if (!this->hasListeners) {
		return;
	}
	ofUnregisterMouseEvents(this, -1);
	ofUnregisterKeyEvents(this, -1);
}
