import java.awt.*;
import java.awt.geom.AffineTransform;

public class Confetti extends FreeRectBody {

	boolean active;
	static Image confettiImage;
	final static double WIND_RESIST = 0.00006;

	public Confetti() {
		super(0.0, 0.0);
		active = false;
		width = height = 10;
	} // end constructor Confetti

	static void getImages() {
		confettiImage = WormGame.refToSelf.getTrackedImage("confetti.png");
	} // end getImages()

	public void beginFalling(double x, double y, double xVel, double yVel) {
		this.x = x;
		this.y = y;
		this.xVel = xVel;
		this.yVel = yVel;
		active = true;
	} // end beginFalling()

	public void collisionHandler(Point2D cP, Point2D cPOR, double LRX, double LRY) {
		if(!active)
			return;
		else {
			WormGame.refToSelf.explodeAt((int)(x+0.5*width), (int)(y+0.5*height), 50);
			active = false;
		} 
	} // end collisionHandler()

	public void checkForOutOfBounds() {
		if(!active)
			return;
		if(y >= Terrain.HEIGHT - height)
			active = false;
	} // end checkForOUtOfBounds()

	public void updateYourself(long delta) {
		if(!active)
			return;
		super.updateYourself(delta);
		yVel -= WIND_RESIST * delta;
	} // end function updateYourself()

	public void drawYourself(Graphics2D g2d) {
		if(!active)
			return;
		AffineTransform origXForm = g2d.getTransform();
		g2d.translate(x, y);
		g2d.drawImage(confettiImage, 0, 0, null);
		g2d.setTransform(origXForm);
	} // end drawYourself()
} // end class
