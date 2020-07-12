import java.awt.*;
import java.awt.geom.AffineTransform;

public class Bazooka extends Weapon {

	boolean active;
	static Image   bazookaImage;

	static void getImages() {
		bazookaImage = WormGame.refToSelf.getTrackedImage("bazooka.png");
	} // end getImages()

	public Bazooka() {
		super();
		active = false;
		width  = 10;
		height = 10;
	}

	public void fire(double X, double Y, double angle, double magnitude) {
		x = X - 0.5*width;
		y = Y - 0.5*height;
		xVel = Math.cos(angle) * magnitude;
		yVel = Math.sin(angle) * magnitude;
		atRest = false;
		active = true;
	} // end constructor Bazooka()

	public void checkForOutOfBounds() {
		if(y >= Terrain.HEIGHT - height)
			active = false;
	} // end checkForOUtOfBounds()

	public void collisionHandler(Point2D cP, Point2D cPOR, double LRX, double LRY) {
		WormGame.refToSelf.explodeAt((int)cP.x, (int)cP.y, 100);
		xVel = yVel = 0.0;
		x = cP.x;
		y = cP.y;
		atRest = true;
		active = false;
	} // end collisionHandler()

	public void updateYourself(long delta) {
		if(active)
			super.updateYourself(delta);
	} // end updateYourself()

	public void drawYourself(Graphics2D g2d) {
		if(!active)
			return;
		double rotation = Math.atan2(yVel, xVel) + Math.PI/2;
		AffineTransform origXForm = g2d.getTransform();
		g2d.translate(x, y);
		g2d.rotate(rotation, width/2, height/2);
		g2d.drawImage(bazookaImage, 0, 0, null);
		g2d.setTransform(origXForm);
	} // end drawYourself()
} // end class
