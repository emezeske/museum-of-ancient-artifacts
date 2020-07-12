import java.awt.*;
import java.awt.event.*;

public class Lobby {
	public static int       LOBBY_CAPACITY = 20;
	public static boolean   inLoginScreen, inLobby, waitingForGameStart;
	public static TextArea  lobbyChatDisplay, lobbyChatInput, listOfPlayers;
	public static String    userName, password; 

	public static PlayerInLobby[] playersInLobby;

	public GameObject go;

	public Lobby(GameObject go) {
		this.go = go;

		inLoginScreen 	    = false;
		inLobby       	    = false;
		waitingForGameStart = false;

		playersInLobby = new PlayerInLobby[LOBBY_CAPACITY];
		for(int i = 0; i < LOBBY_CAPACITY; ++i)
			playersInLobby[i] = new PlayerInLobby();

		login();
	}

	public void login() {
		go.setBackground(Color.black);
		// Let the user log in and find a game to play in
		final TextField userNameField = new TextField(20);
		final TextField passwordField = new TextField(20);
		final Button loginButton = new Button("Login");
		final Label lb1 = new Label("User Name:");
		final Label lb2 = new Label("Password:");
		final Label lb3 = new Label("User Named and Password MUST be less than 20 characters but not empty.");
		final Label lb4 = new Label("To create a new User Name and Password, just type them in and remember them.");
		final Label lb5 = new Label("This connection is NOT encrypted so don't use the password to your bank account.");

		userNameField.setBackground(Color.black);
		userNameField.setForeground(Color.white);
		passwordField.setBackground(Color.black);
		passwordField.setForeground(Color.white);
		passwordField.setEchoChar('*');
		loginButton.setBackground(Color.black);
		loginButton.setForeground(Color.white);
		lb1.setForeground(Color.white);
		lb2.setForeground(Color.white);
		lb3.setForeground(Color.white);
		lb4.setForeground(Color.white);
		lb5.setForeground(Color.white);

		go.add(lb1);
		go.add(userNameField);
		go.add(lb2);
		go.add(passwordField);
		go.add(loginButton);
		go.add(lb3);
		go.add(lb4);
		go.add(lb5);

		inLoginScreen = true;

		loginButton.addActionListener(new ActionListener()
			{
				public void actionPerformed(ActionEvent ae)
				{
					userName = userNameField.getText();
					password = passwordField.getText();
					if(userName.length() < 1 || password.length() < 1 || userName.length() >= 20 || password.length() >= 20)
						return;
					go.removeAll();
					enterLobby();
				}
			});
	} // end login()

	public void enterLobby() {
		inLoginScreen = false;
		inLobby       = true;

		playersInLobby[GameObject.playerID].active = true;
		playersInLobby[GameObject.playerID].name   = userName;

		Label lb1 = new Label("who:");
		lb1.setFont(new Font("Monospaced", Font.PLAIN, 24));
		lb1.setBackground(Color.black);
		lb1.setForeground(Color.white);
		lb1.setBounds(GameObject.SCREEN_WIDTH / 2 - 330, GameObject.SCREEN_HEIGHT - 320, 55, 20);
		go.add(lb1);

		Label lb2 = new Label("what:");
		lb2.setFont(new Font("Monospaced", Font.PLAIN, 24));
		lb2.setBackground(Color.black);
		lb2.setForeground(Color.white);
		lb2.setBounds(GameObject.SCREEN_WIDTH / 2 - 30, GameObject.SCREEN_HEIGHT - 320, 75, 20);
		go.add(lb2);

		Label lb3 = new Label("how:");
		lb3.setFont(new Font("Monospaced", Font.PLAIN, 24));
		lb3.setBackground(Color.black);
		lb3.setForeground(Color.white);
		lb3.setBounds(GameObject.SCREEN_WIDTH / 2 + 275, GameObject.SCREEN_HEIGHT - 320, 55, 20);
		go.add(lb3);

		TextArea commandList = new TextArea("", 20, 20, TextArea.SCROLLBARS_NONE);
		commandList.enableInputMethods(false);
		commandList.setEditable(false);
		commandList.setFocusable(false);
		commandList.setFont(new Font("Monospaced", Font.PLAIN, 12));
		commandList.setBackground(Color.black);
		commandList.setForeground(Color.white);
		commandList.setBounds(GameObject.SCREEN_WIDTH / 2 + 200, GameObject.SCREEN_HEIGHT - 300, 200, 300);
		go.add(commandList);
		commandList.append("To create a game type:\n");
		commandList.append("/create [name] [num]\n");
		commandList.append("[name] = Game Name\n");
		commandList.append("[num]  = Num Players\n");
		commandList.append("Example:\n");
		commandList.append("/create MyGame 3\n\n");
		commandList.append("To join a game type:\n");
		commandList.append("/join [name]\n");
		commandList.append("[name] = Game Name\n");
		commandList.append("Example:\n");
		commandList.append("/join HerGame\n\n");
		commandList.append("For a list of all games waiting for players type:\n");
		commandList.append("/list\n");

		lobbyChatInput = new TextArea("", 1, 40, TextArea.SCROLLBARS_NONE);
		lobbyChatInput.setBackground(Color.black);
		lobbyChatInput.setForeground(Color.white);
		lobbyChatInput.setBounds(GameObject.SCREEN_WIDTH / 2 - 200, GameObject.SCREEN_HEIGHT - 25, 400, 25);
		lobbyChatInput.setFont(new Font("Monospaced", Font.PLAIN, 12));
		go.add(lobbyChatInput);
		lobbyChatInput.requestFocus();

		lobbyChatDisplay = new TextArea("", 6, 40, TextArea.SCROLLBARS_VERTICAL_ONLY);
		lobbyChatDisplay.enableInputMethods(false);
		lobbyChatDisplay.setEditable(false);
		lobbyChatDisplay.setFocusable(false);
		lobbyChatDisplay.setFont(new Font("Monospaced", Font.PLAIN, 12));
		lobbyChatDisplay.setBackground(Color.black);
		lobbyChatDisplay.setForeground(Color.white);
		lobbyChatDisplay.setBounds(GameObject.SCREEN_WIDTH / 2 - 200, GameObject.SCREEN_HEIGHT - 300, 400, 275);
		go.add(lobbyChatDisplay);

		listOfPlayers = new TextArea("", 20, 20, TextArea.SCROLLBARS_NONE);
		listOfPlayers.enableInputMethods(false);
		listOfPlayers.setEditable(false);
		listOfPlayers.setFocusable(false);
		listOfPlayers.setFont(new Font("Monospaced", Font.PLAIN, 12));
		listOfPlayers.setBackground(Color.black);
		listOfPlayers.setForeground(Color.white);
		listOfPlayers.setBounds(0, GameObject.SCREEN_HEIGHT - 300, GameObject.SCREEN_WIDTH / 2 - 200, 300);
		go.add(listOfPlayers);

		go.socketObject.logonToServer(userName, password);
		updateListOfPlayers();
	} // end enterLobby()

	public void handleLobbyTextEntered() {
		// Grab the text that is in the input bar
		String text = lobbyChatInput.getText();
		// Clear the input bar
		lobbyChatInput.setText("");
		if(text.startsWith("/create"))
		{
			if(waitingForGameStart)
			{
				lobbyChatDisplay.append("You already have a game waiting. You may cancel it with the /cancel command.\n");
				return;
			}
			String tempString = text.substring(7);
			tempString = tempString.trim();
			int aSpace = tempString.indexOf(" ");
			if(aSpace == -1)
			{
				lobbyChatDisplay.append("I'm sorry, but that command made no sense to me.\n");
				return;
			}
			String gameName = tempString.substring(0, aSpace);
			if(aSpace == -1)
			{
				lobbyChatDisplay.append("I'm sorry, but that command made no sense to me.\n");
				return;
			}
			tempString = tempString.substring(aSpace);
			tempString = tempString.trim();
			int numPlayers;
			try
			{
				numPlayers = Integer.parseInt(tempString);
			}
			catch(NumberFormatException nfe)
			{
				lobbyChatDisplay.append("I'm sorry, but that command made no sense to me.\n");
				return;
			}
			if((numPlayers != 2) && (numPlayers != 3) )
			{
				lobbyChatDisplay.append("Each game must have 2 or 3 players.\n");
				return;
			}
			lobbyChatDisplay.append("Creating game called "+gameName+" with "+numPlayers+" players.\n");
			waitingForGameStart = true;
			playersInLobby[GameObject.playerID].gameIsActive = true;
			playersInLobby[GameObject.playerID].gameName = gameName;
			playersInLobby[GameObject.playerID].gameCapacity = numPlayers;
			go.socketObject.requestNewGame(gameName, numPlayers);
		}
		else if(text.startsWith("/join"))
		{
			if(waitingForGameStart)
			{
				lobbyChatDisplay.append("You already are waiting on a game. You may cancel it with the /cancel command.\n");
				return;
			}
			String gameName = text.substring(5);
			gameName = gameName.trim();

			for(int i = 0; i < LOBBY_CAPACITY; ++i)
				if(playersInLobby[i].gameIsActive && playersInLobby[i].gameName.equals(gameName))
				{
					if(i == GameObject.playerID)
					{
						lobbyChatDisplay.append("You can't join your own game...\n");
						return;
					}

					waitingForGameStart = true;
					go.socketObject.joinGame(i);
					return;
				}
				lobbyChatDisplay.append("There are no games called "+gameName+"\n");
		}
		else if(text.startsWith("/list"))
		{
			for(int i = 0; i < LOBBY_CAPACITY; ++i)
				if(playersInLobby[i].gameIsActive)
					lobbyChatDisplay.append(playersInLobby[i].name+" has created a game called "+playersInLobby[i].gameName+", with room for "+playersInLobby[i].gameCapacity+".\n");
		}
		else
		{
			lobbyChatDisplay.append("You say, \""+text+"\"\n");
			go.socketObject.sendTextMessage(text, SocketObject.MSG_LOBBY_CHAT);
		}
	} // end handleLobbyTextEntered()

	public void updateListOfPlayers() {
		listOfPlayers.setText("");

		for(int i = 0; i < LOBBY_CAPACITY; ++i)
			if(playersInLobby[i].active)
				listOfPlayers.append(playersInLobby[i].name+"\n");
	} // end updateListOfPlayers()

	public class PlayerInLobby {
		String name;
		boolean active;
		String gameName;
		int    gameCapacity;
		boolean gameIsActive;
		int    inGameID;

		public PlayerInLobby() {
			name = null;
			active = false;
			gameName = null;
			gameCapacity = 0;
			gameIsActive = false;
			inGameID = 0;
		}
	} // end class PlayerInLobby()
} // end class Lobby
