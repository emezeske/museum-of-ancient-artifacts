///////////////////////////////////////////////////////////////////////////////
// Filename: userinterfaceobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _USERINTERFACEOBJECT_H_
#define _USERINTERFACEOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <string>
#include "mouseobject.h"
#include "textureobject.h"
#include "openglobject.h"
#include "fontobject.h"


///////////////////////////////////////////////////////////////////////////////
// Class name: UserInterfaceObject
///////////////////////////////////////////////////////////////////////////////
class UserInterfaceObject
{
public:
	UserInterfaceObject();
	UserInterfaceObject(const UserInterfaceObject&);
	~UserInterfaceObject();

	bool Initialize();
	bool Shutdown();
	bool Render(OpenGLObject*, int,  int, int, float, float, float, 
			    float, int, int);

	void ChangeDebugRender();
	void KeyPressed(int);
	void AddMessageToTalkbar(char*);
	void AddNewMessageForServer(char*);
	bool NewMessage();
	void GetNewMessage(char*);
	void SetMousePos(int, int);
	void SetMouseButton(bool);
	string itoa(int);

private:
	FontObject* Font;
	MouseObject* Mouse;
	TextureObject* UITextures;
	bool renderDebug;
	char talkBar1[80], talkBar2[80], talkBar3[80], talkBar4[80], talkBar5[80];
	int talkBarPos1, talkBarPos2, talkBarPos3, talkBarPos4, talkBarPos5;
	char serverMessage[80];
	bool newMessageReady;
};

#endif
