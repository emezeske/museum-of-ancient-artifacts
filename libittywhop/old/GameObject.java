import java.applet.Applet;
import java.util.Random;
import java.awt.*;
import java.awt.event.*;

public class GameObject extends Applet {

	public static final int SCREEN_WIDTH  = 800;
	public static final int SCREEN_HEIGHT = 600;
	public static final int UI_HEIGHT     = SCREEN_HEIGHT - WallPiece.MATRIX_WIDTH * WallPiece.TILE_WIDTH - 30;
	public static final int WORLD_WIDTH   = SCREEN_WIDTH  / WallPiece.TILE_WIDTH;
	public static final int WORLD_HEIGHT  = SCREEN_HEIGHT / WallPiece.TILE_WIDTH - (SCREEN_HEIGHT - UI_HEIGHT) / WallPiece.TILE_WIDTH;

	public  TimerObject    timer;
	public  SocketObject   socketObject;
	public  Lobby          lobby;
	public  Image          backBufferImage;
	public  Image          drawListImage;
	private RenderingHints rh;
	public  static Random  randGen;

	public static PlayerObject[]  players;
	public static int             NUM_PLAYERS;
	public static int             playerID;
	public static byte       drawList[][];
	public static boolean    drawListChanged;
	public static boolean    inGameChatMode;
	public static TextArea   chatDisplay, chatInput;

	public static final byte PLAYER1_WALL = 1;
	public static final byte PLAYER2_WALL = 2;
	public static final byte PLAYER3_WALL = 3;
	public static final byte PLAYER1_TERR = 4;
	public static final byte PLAYER2_TERR = 5;
	public static final byte PLAYER3_TERR = 6;
	public static final Color PLAYER1_COL = Color.red;
	public static final Color PLAYER2_COL = Color.blue;
	public static final Color PLAYER3_COL = Color.orange;

	public void init() {

		inGameChatMode = false;

		// Create the global random number generator
		randGen = new Random();

		// Make a temp WallPiece to call the init func
		WallPiece wallPiece = new WallPiece();
		wallPiece.init(this);
		wallPiece = null;

		drawList = new byte[WORLD_WIDTH][WORLD_HEIGHT];
		drawListChanged = true;


		// Prepare the images
		backBufferImage = createImage(SCREEN_WIDTH, SCREEN_HEIGHT);
		drawListImage   = createImage(SCREEN_WIDTH, UI_HEIGHT);

		// Set up the rendering hints to get the best quality
		rh = new RenderingHints(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		rh.put(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
		rh.put(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);

		// Start the network 
		socketObject = new SocketObject(this);
		socketObject.start();

		lobby = new Lobby(this);
	} // end init()

	public void startGame(int numPlayers) {
		// Set up all the in game chatting stuff
		lobby.inLobby = false;
		inGameChatMode = false;
		removeAll();
		setLayout(null);

		chatInput = new TextArea("", 1, 40, TextArea.SCROLLBARS_NONE);
		chatInput.setFont(new Font("Monospaced", Font.PLAIN, 12));
		chatInput.setBounds(0, SCREEN_HEIGHT - 25, 400, 25);
		chatInput.setBackground(Color.black);
		chatInput.setForeground(Color.white);
		add(chatInput);
		chatInput.requestFocus();

		chatDisplay = new TextArea("", 6, 43, TextArea.SCROLLBARS_VERTICAL_ONLY);
		chatDisplay.enableInputMethods(false);
		chatDisplay.setEditable(false);
		chatDisplay.setFocusable(false);
		chatDisplay.setFont(new Font("Monospaced", Font.PLAIN, 12));
		chatDisplay.setBackground(Color.black);
		chatDisplay.setForeground(Color.white);
		chatDisplay.setBounds(SCREEN_WIDTH / 2, UI_HEIGHT, SCREEN_WIDTH / 2, SCREEN_HEIGHT - UI_HEIGHT);
		add(chatDisplay);

		NUM_PLAYERS = numPlayers;
		players    = new PlayerObject[NUM_PLAYERS];
		for(int i = 0; i < NUM_PLAYERS; ++i)
			players[i] = new PlayerObject(i);

		// Start the timer with specified interval
		// -- begin animation
		timer = new TimerObject(this, 35L);
	} // end startGame()

	public boolean keyDown(Event evt, int key) {
		if(Lobby.inLoginScreen)
			return false;
		else if(Lobby.inLobby)
			if(key == Event.ENTER)
			{
				lobby.handleLobbyTextEntered();
				return true;
			}
			else
				return false;
		// ESC toggles chat mode, if in chat mode enter sends message.
		if(key == 27)
		{
			inGameChatMode = !inGameChatMode;
				return false;
		}
		if(inGameChatMode)
			if(key == Event.ENTER)
			{
				sendChatMessage();
				return true;
			}
			else
				return false;

		int whoAmI = Lobby.playersInLobby[playerID].inGameID;
		switch(key) {
			case Event.UP:
				players[whoAmI].moveUp();
				break;
			case Event.DOWN:
				players[whoAmI].moveDown();
				break;
			case Event.LEFT:
				players[whoAmI].moveLeft();
				break;
			case Event.RIGHT:
				players[whoAmI].moveRight();
				break;
			case Event.ENTER:
				if(players[whoAmI].isPiecePlaceable())
				{
					socketObject.sendPositionMessage(SocketObject.MSG_PLACE_PIECE);
					players[whoAmI].placeCurPiece();
				}
				break;
			case 'a':
				players[whoAmI].rotateCW();
				break;
			case 's':
				players[whoAmI].rotateCCW();
				break;
		}
		return true;
	} // end keyDown()

	public void sendChatMessage() {
		// Get the string in the input bar and add to the display
		String message = chatInput.getText();
		chatDisplay.append("You say, \""+message+"\"\n");
		// Clear the input bar
		chatInput.setText("");
		// Send the message to everyone else
		socketObject.sendTextMessage(message, SocketObject.MSG_GAME_CHAT);
	} // end sendChatMessage()

	public void updateEverything(long delta) {
		for(int i = 0; i < NUM_PLAYERS; ++i)
			players[i].updateYourself(delta);

		socketObject.updateYourself(delta);

		repaint();
	} // end updateEverything()

	public void update(Graphics g) {
		// get a Graphics2D for the backbuffer, and clear it
		Graphics2D backBufferGraphics = (Graphics2D)backBufferImage.getGraphics();
		backBufferGraphics.setRenderingHints(rh);
		// draw everything to the backbuffer
		paint(backBufferGraphics);
		// Get the real Graphics2D and draw the backbuffer onto it
		Graphics2D g2d = (Graphics2D)g;
		g2d.setRenderingHints(rh);
		g2d.drawImage(backBufferImage, 0, 0, null);
	} // end update()

	public void paint(Graphics2D g2d) {
		drawWallsAndTerritory(g2d);

		for(int i = 0; i < NUM_PLAYERS; ++i)
			players[i].drawYourself(g2d);

		int whoAmI = Lobby.playersInLobby[playerID].inGameID;

		// Draw the UI
		int yOffset = 30;
		g2d.setColor(Color.black);
		g2d.fillRect(0, UI_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT); 

		g2d.setColor(players[whoAmI].playerColor);
		g2d.fillRect(0, UI_HEIGHT, WallPiece.MATRIX_WIDTH * WallPiece.TILE_WIDTH, WallPiece.MATRIX_WIDTH * WallPiece.TILE_WIDTH); 

		players[whoAmI].nextWallPiece.drawYourself(g2d, players[whoAmI].playerColor, 0, UI_HEIGHT);

		g2d.setColor(players[whoAmI].playerColor);
		g2d.drawString("Territory : "+players[whoAmI].howMuchTerritory, WallPiece.TILE_WIDTH * WallPiece.MATRIX_WIDTH + 5, UI_HEIGHT + 12);

		int spacing = 12;
		for(int i = 0; i < NUM_PLAYERS; ++i)
			if(i != whoAmI)
			{
				spacing += 15;
				g2d.setColor(players[i].playerColor);
				g2d.drawString("Territory : "+players[i].howMuchTerritory, 0 + WallPiece.TILE_WIDTH * WallPiece.MATRIX_WIDTH + 5, spacing + UI_HEIGHT);
			}
	} // end pain()

	private void drawWallsAndTerritory(Graphics2D g2d) {

		if(drawListChanged)
		{
			drawListChanged = false;
			Graphics2D drawListGraphics = (Graphics2D)drawListImage.getGraphics();
			drawListGraphics.setRenderingHints(rh);
			drawListGraphics.setColor(new Color(0, 225, 0));
			drawListGraphics.fillRect(0, 0, SCREEN_WIDTH, UI_HEIGHT);
			for(int x = 0; x < WORLD_WIDTH; ++x)
				for(int y = 0; y < WORLD_HEIGHT; ++y)
				{
					if(drawList[x][y] == 0)
						continue;
					else if(drawList[x][y] == PLAYER1_WALL)
						drawListGraphics.drawImage(WallPiece.redWallImage, x * WallPiece.TILE_WIDTH, y * WallPiece.TILE_WIDTH, null);
					else if(drawList[x][y] == PLAYER2_WALL)
						drawListGraphics.drawImage(WallPiece.blueWallImage, x * WallPiece.TILE_WIDTH, y * WallPiece.TILE_WIDTH, null);
					else if(drawList[x][y] == PLAYER3_WALL)
						drawListGraphics.drawImage(WallPiece.orangeWallImage, x * WallPiece.TILE_WIDTH, y * WallPiece.TILE_WIDTH, null);
					else if(drawList[x][y] == PLAYER1_TERR)
					{
						drawListGraphics.setColor(PLAYER1_COL);
						drawListGraphics.fillRect(x * WallPiece.TILE_WIDTH, y * WallPiece.TILE_WIDTH, WallPiece.TILE_WIDTH, WallPiece.TILE_WIDTH);
					}
					else if(drawList[x][y] == PLAYER2_TERR)
					{
						drawListGraphics.setColor(PLAYER2_COL);
						drawListGraphics.fillRect(x * WallPiece.TILE_WIDTH, y * WallPiece.TILE_WIDTH, WallPiece.TILE_WIDTH, WallPiece.TILE_WIDTH);
					}
					else if(drawList[x][y] == PLAYER3_TERR)
					{
						drawListGraphics.setColor(PLAYER3_COL);
						drawListGraphics.fillRect(x * WallPiece.TILE_WIDTH, y * WallPiece.TILE_WIDTH, WallPiece.TILE_WIDTH, WallPiece.TILE_WIDTH);
					}
				}
			}

		g2d.drawImage(drawListImage, 0, 0, null);
	} // end drawWallsAndTerritory()

	public Image getTrackedImage(String filename) {
		// Start getting the image 
		Image im = getImage(getCodeBase(), filename); 
		// MediaTracker will wait for the download of the
		// image file to be complete before returning --
		MediaTracker mt = new MediaTracker(this);
		mt.addImage(im, 0); 
		// -- or it might throw an exception
		try 
		{ 
			mt.waitForID(0);
		}
		catch (InterruptedException ie) 
		{
			System.out.println(ie+" occurred while trying to get image: "+filename);
			System.exit(1);
		}

		return im;
	} // end getTrackedImage()

	public void destroy() {
		socketObject.shutDown();
	} // end destroy()

} // end class GameObject
