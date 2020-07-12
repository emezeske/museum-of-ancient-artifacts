import java.awt.*;
import java.awt.geom.AffineTransform;

public class Firework extends Weapon {

	boolean active;
	static Image   fireworkImage;
	double angle;
	short fuseTime;
	long  launchTime;

	static final double ROCKET_POWER = 0.0007;

	static final int NUM_CONFETTI = 6;
	Confetti[] confetti;

	static void getImages() {
		fireworkImage = WormGame.refToSelf.getTrackedImage("firework.png");
	} // end getImages()

	public Firework() {
		super();
		active = false;
		width  = 15;
		height = 15;
		fuseTime = 1000; // in millis
		elasticity = 0.45;
		confetti = new Confetti[NUM_CONFETTI];
		for(int i=0; i<NUM_CONFETTI; ++i)
			confetti[i] = new Confetti();
	}

	public void fire(double X, double Y, double angle, double magnitude) {
		magnitude /= 2;
		launchTime = System.currentTimeMillis();
		x = X - 0.5*width;
		y = Y - 0.5*height;
		xVel = Math.cos(angle) * magnitude;
		yVel = Math.sin(angle) * magnitude;
		angle = Math.atan2(yVel, xVel) + Math.PI/2;
		atRest = false;
		active = true;
	} // end constructor Bazooka()

	public void collisionHandler(Point2D cP, Point2D cPOR, double LRX, double LRY) {
		if(!active)
			return;
		super.collisionHandler(cP, cPOR, LRX, LRY);
	} // end collisionHandler()

	public void checkForOutOfBounds() {
		if(!active)
			return;
		if(y >= Terrain.HEIGHT - height)
			active = false;
	} // end checkForOUtOfBounds()

	public void updateYourself(long delta) {
		if(active) {
			super.updateYourself(delta);
			angle = Math.atan2(yVel, xVel);

			xVel += Math.cos(angle) * ROCKET_POWER * delta;
			yVel += Math.sin(angle) * ROCKET_POWER * delta;

			if(launchTime + fuseTime < System.currentTimeMillis()) {
				WormGame.refToSelf.explodeAt((int)(x+0.5*width), (int)(y+0.5*height), 100);
				for(int i=0; i<NUM_CONFETTI; ++i)
					confetti[i].beginFalling(x+0.5*width, y+0.5*height, -0.06+i*0.02+0.2*xVel, 0.2*yVel);
				active = false;
			}
		}
		for(int i=0; i<NUM_CONFETTI; ++i)
			confetti[i].updateYourself(delta);
	} // end updateYourself()

	public void drawYourself(Graphics2D g2d) {
		if(active) {
			AffineTransform origXForm = g2d.getTransform();
			g2d.translate(x, y);
			g2d.rotate(angle + Math.PI/2, width/2, height/2);
			g2d.drawImage(fireworkImage, 0, 0, null);
			g2d.setTransform(origXForm);
		}
		for(int i=0; i<NUM_CONFETTI; ++i)
			confetti[i].drawYourself(g2d);
	} // end drawYourself()
} // end class
