import java.awt.*;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.util.Random;

public class Flame {

	BufferedImage flameImage;
	Random rand;

	int[] pixels;
	static final int 	W = 100,
		     		H = 50;

	public Flame() {
		pixels = new int[W * H];
		flameImage = new BufferedImage(W, H, BufferedImage.TYPE_INT_ARGB); 
		rand = new Random();
	} // end Flame()

	public void seedBottomRow() {
		for(int x=0; x<W; ++x) {
			if(rand.nextBoolean())
				pixels[(H-1) * W + x] = 0x00000000;
			else 
				pixels[(H-1) * W + x] = 0xFF000000;
		}
	} // end seedBottomRow()

	public void recalculateFlame() {
		for(int x=0; x<W; ++x)
			for(int y=H-2; y>0; --y) {
				int 	i1 = (x-1 < 0) ? (y+1)*W + W - 1 : (y+1)*W + x - 1,
					i2 = (y+1) * W + x,
					i3 = (x+1 == W) ? (y+1) * W : (y+1) * W + x + 1,
					i4 = (y+2 < H) ? (y+2)*W + x : (y+1)*W + x;

				int alpha =	((pixels[i1] >> 24)&0x000000FF) +
						((pixels[i2] >> 24)&0x000000FF) +
						((pixels[i3] >> 24)&0x000000FF) + 
						((pixels[i4] >> 24)&0x000000FF);
				alpha /= 4;
				alpha = (alpha < 0) ? 0 : alpha-(int)((float)255 / (float)H);
				if(alpha < 0) alpha = 0;

				pixels[y * W + x] = (alpha << 24) | 0x00FB6100;
			} // end for
	} // end recalculateFlame()

	long timeAccum = 0;
	final int UPDATE_TIME = 30;
	public void updateYourself(long delta) { 
		timeAccum += delta;
		if(timeAccum > UPDATE_TIME) {
			seedBottomRow();
			recalculateFlame();
			flameImage.setRGB(0, 0, W, H, pixels, 0, W);
			timeAccum = 0;
		}
	}

	public void drawYourself(Graphics2D g2d) { 
		AffineTransform oldXForm = g2d.getTransform();

		g2d.translate(-100, Terrain.HEIGHT - H + 1);
		for(int i=0; i<30; ++i) {
			g2d.translate(100, 0);
			g2d.drawImage(flameImage, 0, 0, null);
		}

		g2d.setTransform(oldXForm);
	}
} // end class
