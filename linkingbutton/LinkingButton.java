import java.applet.Applet;
import java.net.URL;
import java.net.MalformedURLException;
import java.util.Random;
import java.lang.Math;
import java.awt.*;
import java.awt.image.BufferedImage;

public class LinkingButton extends Applet {

	ButtonClass[]  buttons;
	TimerClass     timer;
	String         openInFrame;
	Image          backBufferImage;
	RenderingHints rh;
	Random         randGen;
	MovingBarClass movingBar;
	StaticClass    staticBG;

	public void init() {

		//Get the frame that all the URLs should be opened in
		openInFrame = getParameter("frame");

		// Get the number of buttons we will have
		ButtonClass.numButtons = Integer.parseInt(getParameter("numbuttons"));
		buttons = new ButtonClass[ButtonClass.numButtons];
		ButtonClass.curButton = 0;

		for(int i = 0; i < ButtonClass.numButtons; ++i)
		{
			buttons[i] = new ButtonClass();

			// Get the URL that we should link to
			String url = getParameter("url"+(i+1));
			try
			{
				buttons[i].linkToURL = new URL(getCodeBase()+url);
			}
			catch(MalformedURLException badURL)
			{
				System.out.println("BAD URL ERROR");
				buttons[i].linkToURL = null;
			}

			// Get the text this button should display
			buttons[i].buttonText = getParameter("text"+(i+1));

		} // end for

		// Set up the rendering hints to get the best quality
		rh = new RenderingHints(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		rh.put(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
		rh.put(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);

		// Prepare the backBufferImage
		backBufferImage = createImage(getSize().width, getSize().height);

		// Random Number Generator
		randGen = new Random();

		// Create the static background
		staticBG = new StaticClass();

		// Create the moving bar
		movingBar = new MovingBarClass();

		// Start the timer with specified interval
		// -- begin animation
		timer = new TimerClass(this, 30);

	}

	public void start() {
		repaint();
	}

	short timeSinceLastRedraw = 0;
	final short maxTimeout    = 500;

	public void updateEverything(long delta) {
		timeSinceLastRedraw += delta;

//		staticBG.updateYourself(delta);
		boolean active = movingBar.updateYourself(delta);

		if(active || timeSinceLastRedraw > maxTimeout)
		{
			repaint();
			timeSinceLastRedraw = 0;
		}
	}

	public void update(Graphics g) {

		// get a Graphics2D for the backbuffer, and clear it
		Graphics2D backBufferGraphics = (Graphics2D)backBufferImage.getGraphics();
		backBufferGraphics.setRenderingHints(rh);
		backBufferGraphics.setColor(Color.blue);
		backBufferGraphics.fillRect(0, 0, getSize().width, getSize().height);
		// draw everything to the backbuffer
		paint(backBufferGraphics);
		// Get the real Graphics2D and draw the backbuffer onto it
		Graphics2D g2d = (Graphics2D)g;
		g2d.setRenderingHints(rh);
		g2d.drawImage(backBufferImage, 0, 0, null);

	}

	public void paint(Graphics g) {

		// draw static
//		for(int i = 0; i < getHeight() + 100; i += 100)
//			g.drawImage(staticBG.staticImage, 0, i, null);

		// draw movingBar
		g.drawImage(movingBar.barImage, 0, (int)movingBar.y, null);
		for(int i = (int)movingBar.y - ButtonClass.buttonHeight; i > -ButtonClass.buttonHeight; i -= ButtonClass.buttonHeight)
		{
			g.drawImage(movingBar.chainImage, 10, i, null);
			g.drawImage(movingBar.chainImage, 70, i, null);
		}
		for(int i = (int)movingBar.y + ButtonClass.buttonHeight; i < getHeight(); i += ButtonClass.buttonHeight)
		{
			g.drawImage(movingBar.chainImage, 10, i, null);
			g.drawImage(movingBar.chainImage, 70, i, null);
		}

		for(int i = 0; i < buttons.length; ++i)
		{
			g.setColor(Color.white);
			if(i == ButtonClass.curButton)
				g.setColor(Color.red);
			g.drawString(buttons[i].buttonText, 0, i * ButtonClass.buttonHeight + (ButtonClass.buttonHeight / 2) + 4);
		}
	}

	public boolean mouseDown(Event evt, int x, int y) {

		if(!whichButton(y))
			return true;

		if(buttons[ButtonClass.curButton].linkToURL != null)
			getAppletContext().showDocument(buttons[ButtonClass.curButton].linkToURL, openInFrame);
		return true;
	}

	public boolean mouseEnter(Event evt, int x, int y) {
		whichButton(y);
		return true;
	}

	public boolean mouseMove(Event evt, int x, int y) {
		whichButton(y);
		return true;
	}

	private boolean whichButton(int y) {

		// This figures out what button the mouse is over 
		int past = y % ButtonClass.buttonHeight;
		int buttonNumber = (y - past) / ButtonClass.buttonHeight;

		if(buttonNumber >= buttons.length || buttonNumber < 0)
			return false;

		ButtonClass.curButton = buttonNumber;
		return true;
	}

	// Member class MovingBarClass
	private class MovingBarClass {

		static final float REF_VEL = .0036F;

		float y;
		float yVel;
		BufferedImage barImage;
		BufferedImage chainImage;

		public MovingBarClass() {
			y = 100;

			barImage = new BufferedImage(getWidth(), ButtonClass.buttonHeight, BufferedImage.TYPE_INT_ARGB);
			for(int x = 0; x < getWidth(); ++x)
				for(int y = 0; y < ButtonClass.buttonHeight; ++y)
					barImage.setRGB(x, y, 0x8F000000);

			chainImage = new BufferedImage(20, ButtonClass.buttonHeight, BufferedImage.TYPE_INT_ARGB);
			for(int x = 0; x < 20; ++x)
				for(int y = 0; y < ButtonClass.buttonHeight; ++y)
					chainImage.setRGB(x, y, 0x8F000000);


		}

		public boolean updateYourself(long delta) {
			
			float buttonY = ButtonClass.curButton * ButtonClass.buttonHeight;
			float dif     = Math.abs(y - buttonY);
			boolean repaint = true;
	
			if(dif < 0.001F)
			{
				yVel = 0;
				y    = buttonY;
				repaint = false;
			}
			else if(y < buttonY)
				yVel =  REF_VEL * dif;
			else if(y > buttonY)
				yVel = -REF_VEL * dif;
	
			y += yVel * delta;

			return repaint;

		}

	} // end class

	// Member Class StaticClass
	private class StaticClass {

		BufferedImage   staticImage;
		short   	timeOnFrame;

		final short frameLength = 10;

		public StaticClass() {
			
			timeOnFrame = 0;
			staticImage = new BufferedImage(100, 100, BufferedImage.TYPE_INT_RGB);
		}

		public void updateYourself(long delta) {
 			timeOnFrame += (short)delta;

			if(timeOnFrame > frameLength)
			{
				timeOnFrame = 0;
				for(int x = 0; x < 100; ++x)
					for(int y = 0; y < 100; ++y)
					{
						int r = randGen.nextBoolean() ? 0 : 0xFFFFFFFF;
						if(r == 0xFFFFFFFF)
							r = randGen.nextBoolean() ? 0 : 0xFFFFFFFF;
						if(r == 0xFFFFFFFF)
							r = randGen.nextBoolean() ? 0 : 0xFFFFFFFF;
						staticImage.setRGB(x, y, r);
					}
			}
			
		}

	} // end class

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
			System.exit(1);
		}

		return im;

	}


} // end class
