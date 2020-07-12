import java.awt.*;
import java.awt.geom.AffineTransform;

public class SmokePuff {
	boolean active;
	double x, y, xVel, yVel, scaling;
	float opacity;
	static Image smokeImage;

	public static void getImages() {
		smokeImage = WormGame.refToSelf.getTrackedImage("smokepuff.png");
	} // end getImages()

	public SmokePuff() {
		active = false;
	} // end constructor SmokePuff()

	public void startFalling(double X, double Y) {
		x = X;
		y = Y;
		xVel = 0.0;
		yVel = 0.0;
		scaling = 1.0;
		opacity = 1.0F;
		active = true;
	} // end startFalling()

	public void updateYourself(long delta) {
		if(!active)
			return;
		xVel +=  WormGame.WIND_PWR * delta; 
		yVel = -500.0 * WormGame.GRAV_ACC;
		x += xVel * delta;
		y += yVel * delta;
		scaling += 0.001 * delta;
		opacity -= 0.0001 * delta;

		if(opacity < 0.0)
			active = false;
	} // end updateYourself()

	public void drawYourself(Graphics2D g2d) {
		if(!active)
			return;
		AffineTransform origXForm = g2d.getTransform();
		Composite  oldAlpha = g2d.getComposite();

		g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, opacity));
		g2d.translate(x, y);
		g2d.scale(scaling, scaling);
		g2d.drawImage(smokeImage, 0, 0, null);
		g2d.setTransform(origXForm);
		g2d.setComposite(oldAlpha);
	} // end drawYourself()
} // end class
