import java.io.BufferedInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class SocketObject extends Thread {

	public static final byte MSG_ID          = 22; // The server is giving you your ID number
	public static final byte MSG_PLACE_PIECE = 23; // Send a message to indicate you have placed your piece
	public static final byte MSG_MVMT_UPDATE = 25; // Send update about position info 
	public static final byte MSG_GAME_CHAT   = 26; // Send an in game chat msg
	public static final byte MSG_LOGON       = 27; // Sent server username/pass
	public static final byte MSG_LOBBY_CHAT  = 28; // Send a chat message to the lobby
	public static final byte MSG_ENTER_LOBBY = 29; // Notify others that we are in lobby
	public static final byte MSG_EXIT_LOBBY  = 30; // Notify others that we are in lobby
	public static final byte MSG_NEW_GAME    = 31; // Request a new game be started
	public static final byte MSG_CANCEL_GAME = 32; // Cancel a game you started earlier
	public static final byte MSG_JOIN_GAME   = 33; // Join someone else's game 
	public static final byte MSG_START_GAME  = 34; // Request a list of all games 
	public static final byte MSG_DISCONNECT  = 24; // Duh

	long timeSinceLastUpdate;
	boolean active;

        Socket socket          = null;
        DataOutputStream out   = null;
        BufferedInputStream in = null;

	GameObject go;

	// Set up a new socket and get everything ready to go
	public SocketObject(GameObject tgo) {
		go = tgo;

		try 
		{
			socket = new Socket(go.getDocumentBase().getHost(), 4444);
			out    = new DataOutputStream(socket.getOutputStream());
			in     = new BufferedInputStream(socket.getInputStream());
		}
		catch (IOException e) 
		{
			System.err.println("Could not open connection to: "+go.getCodeBase().getHost());
			System.exit(-1);
		}
		active = true;
		timeSinceLastUpdate = 0;
	} // end SocketObject()

	private void sendBytes(byte[] bites) {
		try
		{
			out.write(bites, 0, bites.length);
		}
		catch(IOException ioe) 
		{
			System.out.println(ioe);
		}
	}

	public void sendExitLobbyMessage() {
		if(!active)
			return;
		byte[] bites = { MSG_EXIT_LOBBY, (byte)3, (byte)GameObject.playerID }; 
		sendBytes(bites);
	} // end sendExitLobbyMessage()

	public boolean logonToServer(String userName, String password) {
		if(!active)
			return false;

		byte bites[] = new byte[userName.length() + 3];
		byte stringBites[] = userName.getBytes();
		bites[0] = MSG_LOGON;
		bites[1] = (byte)(userName.length() + 3);
		bites[2] = (byte)(GameObject.playerID);
		for(int i = 3; i < userName.length() + 3; ++i)
			bites[i] = stringBites[i - 3];

		sendBytes(bites);
		return true;
	} // end logonToServer()

	public void requestNewGame(String gameName, int numPlayers) {
		if(!active)
			return;

		byte bites[] = new byte[gameName.length() + 4];
		byte stringBites[] = gameName.getBytes();
		bites[0] = MSG_NEW_GAME;
		bites[1] = (byte)(gameName.length() + 4);
		bites[2] = (byte)(GameObject.playerID);
		bites[3] = (byte)(numPlayers);

		for(int i = 4; i < gameName.length() + 4; ++i)
			bites[i] = stringBites[i - 4];

		sendBytes(bites);
	} // end requestNewGame()

	public void cancelNewGame() {
		if(!active)
			return;
		byte bites[] = { MSG_CANCEL_GAME, (byte)GameObject.playerID }; 
		sendBytes(bites);
	} // end cancelNewGame()

	public void joinGame(int gameID) {
		if(!active)
			return;

		byte bites[] = { MSG_JOIN_GAME, (byte)3, (byte)gameID };
		sendBytes(bites);
	} // end joinGame()

	public void sendTextMessage(String message, byte messageType) {
		if(!active)
			return;

		byte bites[] = new byte[message.length() + 3];
		byte stringBites[] = message.getBytes();

		bites[0] = messageType;
		bites[1] = (byte)(message.length() + 3);
		bites[2] = (byte)(GameObject.playerID);
		for(int i = 3; i < message.length() + 3; ++i)
			bites[i] = stringBites[i - 3];

		sendBytes(bites);
	} // end sendTextMessage()

	public void sendPositionMessage(byte msgType) {
		if(!active)
			return;

		byte bites[] = { msgType, 
				 (byte)7,
				 (byte)GameObject.playerID, 
	 			 (byte)(GameObject.players[GameObject.playerID].x), 
		 		 (byte)(GameObject.players[GameObject.playerID].y), 
			 	 (byte)GameObject.players[GameObject.playerID].curWallPiece.rotation, 
				 (byte)GameObject.players[GameObject.playerID].curWallPiece.whichPiece };
		sendBytes(bites);
	} // end sendPositionMessage()

	public void shutDown() {
		if(!active)
			return;
		try
		{
			if(socket.isConnected())
			{
				if(Lobby.inLobby)
					sendExitLobbyMessage();
				byte bites[] = { MSG_DISCONNECT, (byte)2 };
				sendBytes(bites);
				socket.close();
			}
			active = false;

			out.close();
			in.close();
		}
		catch(IOException ioe)
		{
			System.out.println("Error While Closing Connection");
			System.out.println(ioe);
			active = false;
			return;
		}
		if(!Lobby.inLoginScreen && !Lobby.inLobby)
			GameObject.chatDisplay.append("Connection to server broken...\n");
		if(Lobby.inLobby)
			Lobby.lobbyChatDisplay.append("Connection to server broken...\n");
	} // end shutDown()

	public void run() {
		byte[] byteBuffer = new byte[1024];

		try 
		{
       		 	while (active) 
			{
				int bytesRead = in.read(byteBuffer, 0, 1024);
				int bytesToRead = 0;
				byte messageType = byteBuffer[0];

				if(bytesRead < 1)
				{
					System.out.println("Connection broken hard");
					shutDown();
					break;
				}
				else if(messageType == MSG_DISCONNECT)
				{
					shutDown();
					break;
				}
				else if(bytesRead > 1)
					bytesToRead = (int)byteBuffer[1];
				else
					bytesToRead = 2;

				while(bytesRead < bytesToRead)
				{
					int moreBytesRead = in.read(byteBuffer, bytesRead, 1024 - bytesRead);
					if(moreBytesRead == 0)
					{
						shutDown();
						return;
					}
					bytesRead += moreBytesRead;
					bytesToRead = (int)byteBuffer[1];
				}
				handleMessage(byteBuffer);
			}
		} 
		catch(IOException ioe) 
		{
			if(active)
			{
				System.out.println("Error stopping listening thread");
				System.out.println(ioe);
			}
		}
	} // end run()

	private void handleMessage(byte[] msgBuf) {
		byte messageType = msgBuf[0];

		if(messageType == MSG_ID)
			GameObject.playerID = (int)msgBuf[2];
		else if( (messageType == MSG_PLACE_PIECE) || (messageType == MSG_MVMT_UPDATE) )
		{
			int curPlayer = Lobby.playersInLobby[(int)msgBuf[2]].inGameID;

			// Aquire a lock on the player we are working on before doing anything
			synchronized(GameObject.players[curPlayer])
			{
				GameObject.players[curPlayer].x = (int)msgBuf[3];
				GameObject.players[curPlayer].y = (int)msgBuf[4];
				GameObject.players[curPlayer].curWallPiece.whichPiece = (int)msgBuf[6];
				GameObject.players[curPlayer].curWallPiece.setPiece();
	
				for(int i = 0; i < (int)msgBuf[5]; ++i)
					GameObject.players[curPlayer].curWallPiece.rotateCW();
	
				if(messageType == MSG_PLACE_PIECE)
					GameObject.players[curPlayer].placeCurPiece();
			} // end lock
		}
		else if(messageType == MSG_GAME_CHAT)
		{
			int msgLen = (int)msgBuf[1];
			int id     = (int)msgBuf[2];
			String message = new String(msgBuf, 3, msgLen - 3);
			GameObject.chatDisplay.append(Lobby.playersInLobby[id].name+": \""+message+"\"\n");
		}
		else if(messageType == MSG_LOBBY_CHAT)
		{
			int msgLen = (int)msgBuf[1];
			int id     = (int)msgBuf[2];
			String message = new String(msgBuf, 3, msgLen - 3);
			Lobby.lobbyChatDisplay.append(Lobby.playersInLobby[id].name+": \""+message+"\"\n");
		}
		else if(messageType == MSG_ENTER_LOBBY)
		{
			int id = (int)msgBuf[2];
			Lobby.playersInLobby[id].active = true;
			Lobby.playersInLobby[id].name = new String(msgBuf, 3, (int)msgBuf[1] - 3);
			go.lobby.updateListOfPlayers();
			Lobby.lobbyChatDisplay.append(Lobby.playersInLobby[id].name+" has entered the lobby.\n");
		}
		else if(messageType == MSG_EXIT_LOBBY)
		{
			int id = (int)msgBuf[2];
			Lobby.playersInLobby[id].active = false;
			Lobby.lobbyChatDisplay.append(Lobby.playersInLobby[id].name+" has left the lobby.\n");
			go.lobby.updateListOfPlayers();
		}
		else if(messageType == MSG_NEW_GAME)
		{
			int id = (int)msgBuf[2];
			Lobby.playersInLobby[id].gameIsActive = true;
			Lobby.playersInLobby[id].gameName = new String(msgBuf, 4, (int)msgBuf[1] - 4);
			Lobby.playersInLobby[id].gameCapacity = (int)msgBuf[3];
			Lobby.lobbyChatDisplay.append(Lobby.playersInLobby[id].name+" has created a game called "+Lobby.playersInLobby[id].gameName+", with room for "+Lobby.playersInLobby[id].gameCapacity+".\n");
		}
		else if(messageType == MSG_START_GAME)
		{
			Lobby.lobbyChatDisplay.append("Starting game!\n");
			if((int)msgBuf[1] == 4)
			{
				Lobby.playersInLobby[(int)msgBuf[2]].inGameID = 0;
				Lobby.playersInLobby[(int)msgBuf[3]].inGameID = 1;
				go.startGame(2);
			}
			if((int)msgBuf[1] == 5)
			{
				Lobby.playersInLobby[(int)msgBuf[2]].inGameID = 0;
				Lobby.playersInLobby[(int)msgBuf[3]].inGameID = 1;
				Lobby.playersInLobby[(int)msgBuf[4]].inGameID = 2;
				go.startGame(3);
			}
		}
	} // end handleMessage()

	public void updateYourself(long delta) {
		if(!active)
			return;

		timeSinceLastUpdate += delta;
		if(timeSinceLastUpdate < 300)
			return;
		else
			timeSinceLastUpdate = 0;
		sendPositionMessage(MSG_MVMT_UPDATE);
	} // end updateYourself()

} // end class SocketObject
