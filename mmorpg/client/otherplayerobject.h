///////////////////////////////////////////////////////////////////////////////
// Filename: otherplayerobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _OTHERPLAYEROBJECT_H_
#define _OTHERPLAYEROBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>
#include <math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureobject.h"


///////////////////////////////////////////////////////////////////////////////
// Class name: OtherPlayerObject
///////////////////////////////////////////////////////////////////////////////
class OtherPlayerObject
{
public:
	OtherPlayerObject();
	OtherPlayerObject(const OtherPlayerObject&);
	~OtherPlayerObject();

	void SetActive(bool);
	bool IsActive();
	void Render();

	void UpdateMovement(short, float, float, float, float);
	void GetPosition(float&, float&, float&, float&);
	void SetPosition(float, float, float, float);

private:
	void Update();

private:
	bool active;                   // If the player is active or not.
	float posX, posY, posZ, posR;  // Position of the player.
	short movement;                // Movement state of the player.
	TextureObject* Texture;        // Texture for that player.
};

#endif
