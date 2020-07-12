import java.awt.*;
import java.awt.geom.AffineTransform;
import java.util.Random;

public class Cloud {
	double x, y;
	static final short NUM_CLOUD_IMGS = 3;
	static Image[] cloudImages;
	short imgIndex;
	short MVMT_RATE;
	static Random rand = null;
	static final short width = 100, height = 50;

	public static void getImages() {
		cloudImages = new Image[NUM_CLOUD_IMGS];
		for(int i=0; i<NUM_CLOUD_IMGS; ++i)
			cloudImages[i] = WormGame.refToSelf.getTrackedImage("cloud"+(i+1)+".png");
	} // end getImages()

	public Cloud() {
		if(rand == null)
			rand = new Random(System.currentTimeMillis());
		x = Math.random() * (Terrain.WIDTH + 2*WormGame.SCREEN_W) - WormGame.SCREEN_W;
		y = Math.random() * (2*Terrain.HEIGHT) - Terrain.HEIGHT;
		imgIndex = (short)rand.nextInt(3);
		MVMT_RATE = (short)(800 + rand.nextInt(400));
	} // end constructor Cloud()

	public void updateYourself(long delta) {
		x += WormGame.WIND_PWR * delta * MVMT_RATE; 
		if(x + 100 < -WormGame.SCREEN_W)
			x = Terrain.WIDTH + WormGame.SCREEN_W;
		else if(x > Terrain.WIDTH + WormGame.SCREEN_W)
			x = -WormGame.SCREEN_W - 100;
	} // end updateYourself()

	public void drawYourself(Graphics2D g2d) {
		Camera cam = WormGame.refToSelf.cam;
		if( 	(x + width < cam.x) 	||
			(x > cam.x + cam.width) 	||
			(y + height < cam.y) 	||
			(y > cam.y + cam.height) )
			return;
		AffineTransform origXForm = g2d.getTransform();

		g2d.translate(x, y);
		g2d.drawImage(cloudImages[imgIndex], 0, 0, null);

		g2d.setTransform(origXForm);
	} // end drawYourself()
} // end class
