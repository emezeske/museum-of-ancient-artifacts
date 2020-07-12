import java.awt.*;
import java.awt.geom.AffineTransform;

public class Grenade extends Weapon {

	boolean active;
	static Image   grenadeImage;
	short fuseTime;
	long  pinPulledTime;

	double angle;
	double angularVel;

	static void getImages() {
		grenadeImage = WormGame.refToSelf.getTrackedImage("grenade.png");
	} // end getImages()

	public Grenade() {
		super();
		active = false;
		width  = 15;
		height = 15;
		fuseTime = 3000; // in millis
		elasticity = 0.45;
	}

	public void fire(double X, double Y, double angle, double magnitude) {
		pinPulledTime = System.currentTimeMillis();
		x = X - 0.5*width;
		y = Y - 0.5*height;
		xVel = Math.cos(angle) * magnitude;
		yVel = Math.sin(angle) * magnitude;
		this.angle = angle;
		angularVel = 0.0;
		atRest = false;
		active = true;
	} // end constructor Bazooka()

	public void collisionHandler(Point2D cP, Point2D cPOR, double LRX, double LRY) {
		if(!active)
			return;
		if(!atRest) {
			// Calculate the amount of rotational velocity to add
			double angleOfTravel = Math.atan2(yVel, xVel);
			double angleOfImpact = Math.atan2(0.5*height+LRY - cP.y, 0.5*width+LRX - cP.x);
			angularVel += (angleOfTravel - angleOfImpact) * 0.005;
		}
		super.collisionHandler(cP, cPOR, LRX, LRY);
	} // end collisionHandler()

	public void checkForOutOfBounds() {
		if(y >= Terrain.HEIGHT - height)
			active = false;
	} // end checkForOUtOfBounds()

	public void updateYourself(long delta) {
		if(!active)
			return;

		super.updateYourself(delta);

		if(!atRest)
			angle += angularVel * delta;
		if(angularVel > 0.003)
			angularVel = 0.003;
		if(angularVel < -0.003)
			angularVel = -0.003;

		if(pinPulledTime + fuseTime < System.currentTimeMillis()) {
			WormGame.refToSelf.explodeAt((int)(x+0.5*width), (int)(y+0.5*height), 100);
			active = false;
		}
	} // end updateYourself()

	public void drawYourself(Graphics2D g2d) {
		if(!active)
			return;
		AffineTransform origXForm = g2d.getTransform();
		g2d.translate(x, y);
		g2d.rotate(angle, width/2, height/2);
		g2d.drawImage(grenadeImage, 0, 0, null);
		g2d.setTransform(origXForm);
	} // end drawYourself()
} // end class
