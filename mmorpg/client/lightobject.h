///////////////////////////////////////////////////////////////////////////////
// Filename: lightobject.h"
///////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTOBJECT_H_
#define _LIGHTOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>


/////////////
// DEFINES //
/////////////
const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT       = 1;


///////////////////////////////////////////////////////////////////////////////
// Class name: LightObject
///////////////////////////////////////////////////////////////////////////////
class LightObject
{
public:
	LightObject();
	LightObject(const LightObject&);
	~LightObject();

	void Initialize(int);
	void Shutdown();

	void TurnOn();
	void TurnOff();

	void SetLightType(int);
	
	void SetPosition(float, float, float);
	
	void SetDiffuse(float, float, float);
	void SetAmbient(float, float, float);
	void SetSpecular(float, float, float);
	
	void SetSpotlightDirection(float, float, float);
	void SetSpotlightExpAndCutoff(float, float);
	void SetAttenuation(float, float, float);

	void SetGlobalAmbient(float, float, float);

private:
	void SetLightNumber(int);

private:
	int lightNumber;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float position[4];
	float spotlightDirection[3];
	float spotlightExponent;
	float spotlightCutoffAngle;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float globalAmbient[4];
};

#endif
