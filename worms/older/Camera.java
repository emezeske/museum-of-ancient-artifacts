import java.lang.Math;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.awt.*;

public class Camera {
	double targetX, targetY;
	double x, y;
	double width, height;
	double zoom, targetZoom;

	final double BG_SCROLL_RATE 	= 0.4;
	final double CAMERA_CATCH_UP 	= 0.003;
	final double MIN_ZOOM		= 0.3;
	final double MAX_ZOOM		= 2.0;
	// Current transform
	AffineTransform trans;
	// Background variables
	TexturePaint bgTex;
	static Image bgImage;
	double bgX, bgY;
	// Variables for jostling the camera
	long jostleBeganAt;
	boolean jostling;
	static final long JOSTLE_LENGTH = 300;

	static void getImages() {
		bgImage = WormGame.refToSelf.getTrackedImage("bg1.png");
	} // end getImages()
	
	Camera() {
		x = y = 0.0;
		targetX = targetY = 0.0;
		zoom = targetZoom = 1.0;
		trans = new AffineTransform();
		width = WormGame.SCREEN_W;
		height = WormGame.SCREEN_H;
		// Get the Background image
		BufferedImage bgBufIm = new BufferedImage(100, 600, BufferedImage.TYPE_INT_RGB);
		((Graphics2D)bgBufIm.getGraphics()).drawImage(bgImage, 0, 0, null);
		bgTex = new TexturePaint(bgBufIm, new Rectangle(0, 0, 100, 600));
		bgImage = null;
	} // end constructor Camera()

	// Shakes the camera around for a bit
	public void jostle() {
		jostleBeganAt = System.currentTimeMillis();
		jostling = true;
	} // end jostle()

	public void setNewTarget(double x, double y) {
		targetX = x - 0.5*width;
		targetY = y - 0.5*height;
	} // end setNewTarget()

	public void updateYourself(long delta) {
		zoom += CAMERA_CATCH_UP*1*delta*(targetZoom - zoom);

		double cntrX = x + 0.5*width;
		double cntrY = y + 0.5*height;
		width = (double)WormGame.SCREEN_W / zoom;
		height = (double)WormGame.SCREEN_H / zoom;
		x = cntrX - 0.5*width;
		y = cntrY - 0.5*height;

		x += CAMERA_CATCH_UP*delta*(targetX - x);
		y += CAMERA_CATCH_UP*delta*(targetY - y);

		if(jostling) {
			x += 5-10*Math.random();
			y += 5-10*Math.random();
			if(System.currentTimeMillis() > jostleBeganAt + JOSTLE_LENGTH)
				jostling = false;
		}

		if(x < 0.0 - WormGame.SCREEN_W) 
			x = 0.0 - WormGame.SCREEN_W;
		else if(x > Terrain.WIDTH - width + WormGame.SCREEN_W)
			x = Terrain.WIDTH - width + WormGame.SCREEN_W;
		if(y > Terrain.HEIGHT - height) 
			y = Terrain.HEIGHT - height;

		bgX = BG_SCROLL_RATE * x;
		bgY = BG_SCROLL_RATE * y;
	} // end updateYourself()

	public void prepareScene(Graphics2D g2d) {
		// Draw the parallax scrolling background
		g2d.setPaint(bgTex);
		AffineTransform oldXForm = g2d.getTransform();
		g2d.translate(-bgX, -bgY);
		g2d.fill(new Rectangle((int)Math.round(bgX), (int)Math.round(bgY), (int)WormGame.SCREEN_W, (int)WormGame.SCREEN_H));
		g2d.setTransform(oldXForm);
		// Set up and apply the transforms for the camera
		trans.setToIdentity();
		trans.scale(zoom, zoom);
		trans.translate(-x, -y);
		g2d.transform(trans);
	} // end render()

	public void setZoomTarget(double newZoom) {
		if(newZoom >= MIN_ZOOM && newZoom <= MAX_ZOOM)
			targetZoom = newZoom;
	} // end setZoomLevel()
} // end class
