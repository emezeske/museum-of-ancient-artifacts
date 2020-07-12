import java.awt.*;
import java.awt.geom.AffineTransform;

public class Explosion {
	static final int LINGER_TIME = 300;
	static Image[] explosionImages;

	long startTime;
	int x, y, curImage; // position, diameter
	boolean active;

	static void getImages() {
		explosionImages = new Image[2];
		explosionImages[0] = WormGame.refToSelf.getTrackedImage("explosion50.png");
		explosionImages[1] = WormGame.refToSelf.getTrackedImage("explosion100.png");
	} // end getImages();

	public Explosion() {
	} // end constructor Explosion()

	public void explode(int X, int Y, int R) {
		startTime = System.currentTimeMillis();
		x = X - (int)(0.5*R);
		y = Y - (int)(0.5*R);
		switch(R) {
			case 50:
				curImage = 0;
				WormGame.refToSelf.emitSmoke((int)X, (int)Y, 2, 25);
				break;
			case 100:
				curImage = 1;
				WormGame.refToSelf.emitSmoke((int)X, (int)Y, 4, 50);
				break;
		} // end switch()
		active = true;
	} // end explode()

	public void updateYourself(long delta) {
		if(!active)
			return;
		if(System.currentTimeMillis() > startTime+LINGER_TIME)
			active = false;
	} // end updateYourself()

	public void drawYourself(Graphics2D g2d) {
		if(!active)
			return;
		AffineTransform oldXForm = g2d.getTransform();
		Composite  oldAlpha = g2d.getComposite();

		long elapsedTime = System.currentTimeMillis() - startTime;
		float alpha = 0;
		if(elapsedTime != 0)
			alpha = 0.1F/((float)elapsedTime/(float)LINGER_TIME);
		if(alpha < 0)
			alpha = 0.0F;
		else if(alpha > 1.0)
			alpha = 1.0F;
		g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, alpha));
		g2d.translate(x, y);
		g2d.drawImage(explosionImages[curImage], 0, 0, null);

		g2d.setTransform(oldXForm);
		g2d.setComposite(oldAlpha);
	} // end drawYourself()
} // end class
