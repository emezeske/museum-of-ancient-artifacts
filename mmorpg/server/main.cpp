////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "socketobject.h"
#include "zoneobject.h"


////////////////////////////////////////////////////////////////////////////////
// Function name: main
// Purpose: Program entry point.
////////////////////////////////////////////////////////////////////////////////
int main()
{
  SocketObject* ServerSocket;
  ZoneObject* Zone;
  bool result;
  char input;

  cout << "==================================================" << endl;
  Zone = new ZoneObject;
  ServerSocket = new SocketObject;
  Zone->GetSocketPtr(ServerSocket);

  // Start the server.
  result = ServerSocket->Initialize(Zone);
  if(!result)
  {
    cout << "> Error: Server could not be initialized." << endl;
  }
  else
  {
    // Loop while the server is online.
    while(ServerSocket->Online())
    {
      cin.get(input);
      if((input == 'q') || (input == 'Q'))
      {
	// If the user pressed 'Q' then shutdown the server.
	ServerSocket->SetOffline();
      }
    }
  }

  result = ServerSocket->Shutdown();
  if(!result)
  {
    cout << "> Error: Could not shutdown server properly." << endl;
  }
  delete ServerSocket;
  ServerSocket = NULL;
  delete Zone;
  Zone = NULL;
  cout << "==================================================" << endl;

  return 0;
}
