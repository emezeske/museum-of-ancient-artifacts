import java.applet.Applet;
import java.awt.*;

public class TankGame extends Applet {

	Tank tank;
	GameTimer  gameTimer;

	Image  backBufferImage;
	RenderingHints rh;

	public void init() {
		tank  = new Tank(this);
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
		tank.drawYourself(g2d);
	}

	public void updateEverything(long delta) {
		tank.updateYourself(delta);
		repaint();
	}

	public boolean keyDown(Event evt, int key) {
		switch(key) {
			case Event.UP:
				tank.moveForward();
				break;
			case Event.DOWN:
				tank.moveBackward();
				break;
			case Event.LEFT:
				tank.turnLeft();
				break;
			case Event.RIGHT:
				tank.turnRight();
				break;
			case 'd':
				tank.rotateTurretLeft();
				break;
			case 'f':
				tank.rotateTurretRight();
				break;
		}
		return true;
	}

	public boolean keyUp(Event evt, int key) {
		switch(key) {
			case Event.UP:
				tank.stopMovingForward();
				break;
			case Event.DOWN:
				tank.stopMovingBackward();
				break;
			case Event.LEFT:
				tank.stopTurningLeft();
				break;
			case Event.RIGHT:
				tank.stopTurningRight();
				break;
			case 'd':
				tank.stopRotatingTurretLeft();
				break;
			case 'f':
				tank.stopRotatingTurretRight();
				break;
		}
		return true;
	}

} // end class
