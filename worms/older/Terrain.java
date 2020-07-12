import java.awt.image.BufferedImage;
import java.awt.*;
import java.awt.geom.AffineTransform;

public class Terrain {
	// The terrain image is broken down into a bunch of smaller pieces so that we only
	// have to draw what is on the screen.  Anything in the terrain image that is 
	// at all opaque will be considered terrain.
	int 	TILE_SIZE = 150,
	    	NUM_TILES_X,
		NUM_TILES_Y;

	static int WIDTH;
	static int HEIGHT;

	// Explosion array that can be applied to the terrainCollisions array to remove
	// terrain
	boolean[][] explosion50;
	boolean[][] explosion100;

	static boolean[][] collisionArray;
	BufferedImage[][] terrainImages;

	// Temporary image to make the terrain out of
	static Image ti;

	static void getImages(String filename) {
		ti = WormGame.refToSelf.getTrackedImage(filename);
	} // end getImages()

	// Terrain constructor builds the collisionArray and terrainImages array
	// out of the filename passed to it which should be a PNG image
	public Terrain() {
		explosion50  = generateExplosionMap(50);
		explosion100 = generateExplosionMap(100);
		
		WIDTH = ti.getWidth(null);
		HEIGHT = ti.getHeight(null);

		BufferedImage fullTerrainImage = new BufferedImage(WIDTH, HEIGHT, BufferedImage.TYPE_INT_ARGB);
		collisionArray = new boolean[WIDTH][HEIGHT];

		// Generate a boolean collision array out of the terrainImage
		Graphics2D g2d = (Graphics2D)fullTerrainImage.getGraphics();
		g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC));
		g2d.drawImage(ti, 0, 0, null);

		int[] pixels = new int[WIDTH*HEIGHT];
		pixels = fullTerrainImage.getRGB(0, 0, WIDTH, HEIGHT, pixels, 0, WIDTH);
		for(int x = 0; x < WIDTH; ++x)
			for(int y = 0; y < HEIGHT; ++y)
				if((pixels[y * WIDTH + x] & 0xFF000000) != 0)
					collisionArray[x][y] = true;
		pixels = null;
		// Break the terrain image down into a bunch of smaller fullTerrainImage so that
		// less pixels have to be drawn each frame
		NUM_TILES_X = (int)Math.ceil((double)WIDTH  / (double)TILE_SIZE);
		NUM_TILES_Y = (int)Math.ceil((double)HEIGHT / (double)TILE_SIZE);
		terrainImages = new BufferedImage[NUM_TILES_X][NUM_TILES_Y];
		pixels = new int[TILE_SIZE*TILE_SIZE];
		for(int x = 0; x < NUM_TILES_X; ++x)
			for(int y = 0; y < NUM_TILES_Y; ++y)
			{
				terrainImages[x][y] = new BufferedImage(TILE_SIZE, TILE_SIZE, BufferedImage.TYPE_INT_ARGB);
				Graphics2D g = (Graphics2D)terrainImages[x][y].getGraphics();
				g.translate(-x*TILE_SIZE, -y*TILE_SIZE);
				g.drawImage(fullTerrainImage, 0, 0, null);
				g = null;
				pixels = terrainImages[x][y].getRGB(0, 0, TILE_SIZE, TILE_SIZE, pixels, 0, TILE_SIZE);
				boolean isEmpty = true;
				for(int i = 0; i < TILE_SIZE*TILE_SIZE; ++i)
					if((pixels[i] & 0xFF000000) != 0)
						isEmpty = false;
				if(isEmpty)
					terrainImages[x][y] = null;
			}
		ti = null;
		fullTerrainImage = null;
		pixels = null;
	} // end generateTerrain() 

	// This function builds an explosion map that is d in diameter, that can be applied
	// to the terrainCollisions array to remove terrain
	public boolean[][] generateExplosionMap(int d) {
		BufferedImage t = new BufferedImage(d, d, BufferedImage.TYPE_INT_ARGB);

		int dOver2 = (int)(d / 2);
		Graphics2D g2d = (Graphics2D)t.getGraphics();
		g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC));
		g2d.setColor(new Color(0, 0, 0, 0));
		g2d.fillRect(0, 0, d, d);
		g2d.setColor(new Color(0, 0, 0, 1));
		g2d.fillOval(0, 0, d, d);

		int pixels[] = new int[d*d];
		pixels = t.getRGB(0, 0, d, d, pixels, 0, d);

		boolean[][] xplsn = new boolean[d][d];
		for(int x=0; x < d; ++x)
			for(int y=0; y < d; ++y)
				if(pixels[y*d+x] != 0)
					xplsn[x][y] = true;
				else
					xplsn[x][y] = false;
		t = null;
		pixels = null;
		return xplsn;
	} // end generateExplosion()

	// Removes a piece of the collisionArray array and also cuts out
	// a hole in the map image
	public void cutHoleInTerrain(int x, int y, int d) {
		int dOver2 = (int)(d / 2);
		int startX = (int)((x-dOver2) / TILE_SIZE);
		int startY = (int)((y-dOver2) / TILE_SIZE);
		int endX = (int)Math.ceil((x+dOver2) / TILE_SIZE);
		int endY = (int)Math.ceil((y+dOver2) / TILE_SIZE);
		if(startX < 0) startX = 0;
		if(startX >= NUM_TILES_X) startX = NUM_TILES_X - 1;
		if(startY < 0) startY = 0;
		if(startY >= NUM_TILES_Y) startY = NUM_TILES_Y - 1;
		if(endX < 0) endX = 0;
		if(endX >= NUM_TILES_X) endX = NUM_TILES_X - 1;
		if(endY < 0) endY = 0;
		if(endY >= NUM_TILES_Y) endY = NUM_TILES_Y - 1;
		// Cut a hole in the images
		for(int curX = startX; curX <= endX; ++curX)
			for(int curY = startY; curY <= endY; ++curY)
				if(terrainImages[curX][curY] != null)
				{
					Graphics2D g2d = (Graphics2D)terrainImages[curX][curY].getGraphics();
					g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC));
					g2d.setColor(new Color(0, 0, 0, 0));
					g2d.fillOval(x-TILE_SIZE*curX - dOver2, y-TILE_SIZE*curY - dOver2, d, d);
				}

		// Cut a hole in the collisionArray
		switch(d) {
			case 50:
				for(int X=0; X<d; ++X)
					for(int Y=0; Y<d; ++Y)
						if(explosion50[X][Y])
							if( (X+x-dOver2 > 0) && (X+x-dOver2 < WIDTH) )
								if( (Y+y-dOver2 > 0) && (Y+y-dOver2 < HEIGHT) )
									collisionArray[X+x-dOver2][Y+y-dOver2] = false;
				break;
			case 100:
				for(int X=0; X<d; ++X)
					for(int Y=0; Y<d; ++Y)
						if(explosion100[X][Y])
							if( (X+x-dOver2 > 0) && (X+x-dOver2 < WIDTH) )
								if( (Y+y-dOver2 > 0) && (Y+y-dOver2 < HEIGHT) )
									collisionArray[X+x-dOver2][Y+y-dOver2] = false;
				break;
		}
	} // end cutHoleInTerrain()

	// Only draw what parts of the terrain that we absolutely have to
	public void drawYourself(Graphics2D g2d) {
		int startX = (int)(WormGame.cam.x / TILE_SIZE);
		int startY = (int)(WormGame.cam.y / TILE_SIZE);
		int endX = (int)Math.ceil((WormGame.cam.x + WormGame.cam.width) / TILE_SIZE);
		int endY = (int)Math.ceil((WormGame.cam.y + WormGame.cam.height) / TILE_SIZE);
		if(startX < 0) startX = 0;
		if(startX >= NUM_TILES_X) startX = NUM_TILES_X - 1;
		if(startY < 0) startY = 0;
		if(startY >= NUM_TILES_Y) startY = NUM_TILES_Y - 1;
		if(endX < 0) endX = 0;
		if(endX > NUM_TILES_X) endX = NUM_TILES_X;
		if(endY < 0) endY = 0;
		if(endY > NUM_TILES_Y) endY = NUM_TILES_Y;
		for(int x=startX; x < endX; ++x)
			for(int y=startY; y < endY; ++y)
				if(terrainImages[x][y] != null)
				{
					AffineTransform oldXForm = g2d.getTransform();
					g2d.translate(x*TILE_SIZE, y*TILE_SIZE);
					g2d.drawImage(terrainImages[x][y], 0, 0, null);
					g2d.setTransform(oldXForm);
				}
	} // end drawYourself
} // end class
