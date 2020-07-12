////////////////////////////////////////////////////////////////////////////////
// Filename: zoneobject.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ZONEOBJECT_H_
#define _ZONEOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <iostream.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "playerobject.h"
#include "socketobject.h"


///////////////////////
// CLASS PRE-DEFINES //
///////////////////////
class SocketObject;


////////////////////////////////////////////////////////////////////////////////
// Class name: ZoneObject
////////////////////////////////////////////////////////////////////////////////
class ZoneObject
{
public:
  ZoneObject();                       // Constructor.
  ZoneObject(const ZoneObject&);      // Copy constructor.
  ~ZoneObject();                      // Destructor.

  void GetSocketPtr(SocketObject*);   // Copies pointer to the socket object.

  void SetMaxPlayers(int);            // Sets maximum players in a zone.
  void UpdatePlayerMovement(short, short, float, float, float, float);  // Updates player movement.
  void SetPlayerActive(short);        // Sets player to active.
  void SetPlayerInactive(short);      // Sets player to inactive.
  void NewTextMessage(short, char*);  // Handles new text message from player.

private:
  SocketObject* SockPtr;  // The socket pointer.
  PlayerObject* Players;  // The pointer to the players.
  int maxPlayers;         // The maximum players in the zone.
};

#endif
