import java.applet.Applet;
import java.awt.*;

public class HelicopterGame extends Applet {

	Helicopter helicopter;
	GameTimer  gameTimer;

	Image  backBufferImage;
	RenderingHints rh;

	public void init() {

		helicopter  = new Helicopter(this);
		gameTimer   = new GameTimer(this);
		backBufferImage    = createImage(getSize().width, getSize().height);
		// Set up the rendering hints to get the best quality
		rh = new RenderingHints(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		rh.put(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
		rh.put(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);

	}

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

	public void update(Graphics g) {

		// get a Graphics2D for the backbuffer, and clear it
		Graphics2D backBufferGraphics = (Graphics2D)backBufferImage.getGraphics();
		backBufferGraphics.setRenderingHints(rh);
		backBufferGraphics.setColor(Color.white);
		backBufferGraphics.fillRect(0, 0, getSize().width, getSize().height);
		// draw everything to the backbuffer
		paint(backBufferGraphics);
		// Get the real Graphics2D and draw the backbuffer onto it
		Graphics2D g2d = (Graphics2D)g;
		g2d.setRenderingHints(rh);
		g2d.drawImage(backBufferImage, 0, 0, null);

	}

	public void paint(Graphics2D g2d) {

		helicopter.drawYourself(g2d);
		
	}

	public void updateEverything(long delta) {

		helicopter.updateYourself(delta);
		repaint();

	}

	public boolean keyDown(Event evt, int key) {
		
		switch(key) {
			case Event.UP:
				helicopter.rising      = true;
				break;
			case Event.DOWN:
				helicopter.falling     = true;
				break;
			case Event.LEFT:
				helicopter.movingLeft  = true;
				break;
			case Event.RIGHT:
				helicopter.movingRight = true;
				break;
		}

		return true;

	}

	public boolean keyUp(Event evt, int key) {
		
		switch(key) {
			case Event.UP:
				helicopter.rising      = false;
				break;
			case Event.DOWN:
				helicopter.falling     = false;
				break;
			case Event.LEFT:
				helicopter.movingLeft  = false;
				break;
			case Event.RIGHT:
				helicopter.movingRight = false;
				break;
		}

		return true;

	}

} // end class
