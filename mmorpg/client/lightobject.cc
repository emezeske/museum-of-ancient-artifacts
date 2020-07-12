///////////////////////////////////////////////////////////////////////////////
// Filename: lightobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "lightobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: LightObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the light object.
///////////////////////////////////////////////////////////////////////////////
LightObject::LightObject()
{
	position[0] = 0.0f;
	position[1] = 0.0f;
	position[2] = 0.0f;
	position[3] = 0.0f;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: LightObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
LightObject::LightObject(const LightObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~LightObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
LightObject::~LightObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initializes the light object.
// Inputs: lightId - The ID number of this light.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::Initialize(int lightId)
{
	// Initialize the position of this light.
	position[0] = 0.0f;
	position[1] = 0.0f;
	position[2] = 0.0f;
	position[3] = 0.0f;

	// Set the ID number of this light.
	SetLightNumber(lightId);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Destroys the light object.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::Shutdown()
{
	// Turn off the light object since it will not longer be used.
	TurnOff();

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: TurnOn
// Purpose: Turns on the light.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::TurnOn()
{
	glEnable(lightNumber);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: TurnOff
// Purpose: Turns off the light.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::TurnOff()
{
	glDisable(lightNumber);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetLightType
// Purpose: Sets the type of light for this light.
// Inputs: type - The type of light.
// Outputs: None.
// Details: There are two types of lights in opengl; directional and point.  To
//          set a light in opengl to one of the choices the w coordinate in the
//          lights position must be set to 0 for directional and 1 for point.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetLightType(int type)
{
	switch(type)
	{
		case LIGHT_TYPE_DIRECTIONAL:
		{

			position[3] = 0.0f;
			break;
		}
		case LIGHT_TYPE_POINT:
		{
			position[3] = 1.0f;
			break;
		}
		default:
		{
			break;
		}
	}
	
	// Set the light type (and position since it is related).
	glLightfv(lightNumber, GL_POSITION, position);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetPosition
// Purpose: Sets the location of the light.
// Inputs: x - The x coordinate position.
//         y - The y coordinate position.
//         z - The z coordinate position.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetPosition(float x, float y, float z)
{
	// Save the position from the input.
	position[0] = x;
	position[1] = y;
	position[2] = z;
	
	// Set the position of the light.
	glLightfv(lightNumber, GL_POSITION, position);
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetDiffuse
// Purpose: Sets the diffuse color and level of light.
// Inputs: r - The red component.
//         g - The green component.
//         b - The blue component.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetDiffuse(float r, float g, float b)
{
	// Store the diffuse color of light.
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = 1.0f;
	
	// Set the diffuse color of light.
	glLightfv(lightNumber, GL_DIFFUSE, diffuse);
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetAmbient
// Purpose: Sets the ambient color and level of light.
// Inputs: r - The red component.
//         g - The green component.
//         b - The blue component.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetAmbient(float r, float g, float b)
{
	// Store the ambient color.
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
	ambient[3] = 1.0f;
	
	// Set the ambient color.
	glLightfv(lightNumber, GL_AMBIENT, ambient);
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetSpecular
// Purpose: Sets the specular color of light.
// Inputs: r - The red component.
//         g - The green component.
//         b - The blue component.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetSpecular(float r, float g, float b)
{
	// Store the specular color.
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
	specular[3] = 1.0f;
	
	// Set the specular color.
	glLightfv(lightNumber, GL_SPECULAR, specular);
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetSpotlightDirection
// Purpose: Sets the direction of the spotlight.
// Inputs: directionX - The x direction of the spotlight.
//         directionY - The y direction of the spotlight.
//         directionZ - The z direction of the spotlight.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetSpotlightDirection(float directionX, float directionY, float directionZ)
{
	// Store the direction of the spotlight.
	spotlightDirection[0] = directionX;
	spotlightDirection[1] = directionY;
	spotlightDirection[2] = directionZ;

	// Set the spotlight.
	glLightfv(lightNumber, GL_SPOT_DIRECTION, spotlightDirection);	

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetSpotlightExpAndCutoff
// Purpose: Sets the spotlight exponent and cut off.
// Inputs: sExp - The exponent value.
//         cutoff - The cut off value.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetSpotlightExpAndCutoff(float sExp, float cutoff)
{
	// Store the exponent and cutoff.
	spotlightExponent    = sExp;
	spotlightCutoffAngle = cutoff;

	// Set the exponent and cutoff.
	glLightf(lightNumber, GL_SPOT_EXPONENT, spotlightExponent);
	glLightf(lightNumber, GL_SPOT_CUTOFF,   spotlightCutoffAngle);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetAttenuation
// Purpose: Sets the attenuation of the light.
// Inputs: constantA - The constant attenuation.
//         linearA - The linear attenuation.
//         quadraticA - The quadratic attentuation.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetAttenuation(float constantA, float linearA, float quadraticA)
{
	// Store the attenuation factors.
	constantAttenuation  = constantA;
	linearAttenuation    = linearA;
	quadraticAttenuation = quadraticA;

	// Set the attenuation.
	glLightf(lightNumber, GL_CONSTANT_ATTENUATION,  constantAttenuation);
	glLightf(lightNumber, GL_LINEAR_ATTENUATION,    linearAttenuation);
	glLightf(lightNumber, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetGlobalAmbient
// Purpose: Sets the global ambient for the entire scene.
// Inputs: r - The red component.
//         g - The green component.
//         b - The blue component.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetGlobalAmbient(float r, float g, float b)
{
	// Store the global ambient light.
	globalAmbient[0] = r;
	globalAmbient[1] = g;
	globalAmbient[2] = b;
	globalAmbient[3] = 1.0f;
	
	// Set the the global ambient light.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetLightNumber
// Purpose: Sets the id number for this light so opengl knows which one it is.
// Inputs: id - The id number for this light.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void LightObject::SetLightNumber(int id)
{
	lightNumber = GL_LIGHT0 + id;
	return;
}
