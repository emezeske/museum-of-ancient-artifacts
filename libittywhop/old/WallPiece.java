import java.awt.*;
import java.awt.image.BufferedImage;
import java.util.Random;

public class WallPiece {

	public static final int TILE_WIDTH  = 10;
	public static final int MATRIX_WIDTH = 5;

	boolean[][]  pieceMatrix;

	static Image         wallImage;
	static Image         blueWallImage;
	static Image         redWallImage;
	static Image         orangeWallImage;
	static Image         territoryImage;
	static BufferedImage wallHighlightImage;

	private float ALPHA_DELTA = 0.5F;  // EPSILON
	private int   alpha;
	public int    rotation, whichPiece;

	public void init(GameObject gameObject) {
		wallImage       = gameObject.getTrackedImage("brick.png");
		blueWallImage   = gameObject.getTrackedImage("bluebrick.png");
		redWallImage    = gameObject.getTrackedImage("redbrick.png");
		orangeWallImage = gameObject.getTrackedImage("orangebrick.png");
		territoryImage  = gameObject.getTrackedImage("territory.png");
	} // end init()

	public WallPiece() {
		pieceMatrix = new boolean[MATRIX_WIDTH][MATRIX_WIDTH];
		whichPiece = GameObject.randGen.nextInt(44);
		setPiece();
		alpha = 15;
		rotation = 0;
	} // end WallPiece()

	public void updateYourself(long delta) {
		alpha += ALPHA_DELTA * delta;
		if(alpha > 100)
		{
			alpha = 100;
			ALPHA_DELTA = -ALPHA_DELTA;
		}
		else if(alpha < 0)
		{
			alpha = 0;
			ALPHA_DELTA = -ALPHA_DELTA;
		}
	} // end updateYourself()

	public void drawYourself(Graphics2D g2d, Color playerColor, int playerX, int playerY) {
		for(short x = 0; x < MATRIX_WIDTH; ++x) 
			for(short y = 0; y < MATRIX_WIDTH; ++y)
				if(pieceMatrix[x][y])
				{
					g2d.drawImage(wallImage, TILE_WIDTH * x + playerX, TILE_WIDTH * y + playerY, null);
					g2d.setColor(new Color(playerColor.getRed(), playerColor.getGreen(), playerColor.getBlue(), alpha) );
					g2d.fillRect(TILE_WIDTH * x + playerX, TILE_WIDTH * y + playerY, TILE_WIDTH, TILE_WIDTH);
				}
	} // end drawYourself()

	public void rotateCW() {
		boolean[][] tempMatrix = new boolean[MATRIX_WIDTH][MATRIX_WIDTH];

		for(int x = 0; x < MATRIX_WIDTH; ++x)
				for(int y = 0; y < MATRIX_WIDTH; ++y)
					tempMatrix[MATRIX_WIDTH - 1 - y][x] = pieceMatrix[x][y];

		for(int x = 0; x < MATRIX_WIDTH; ++x)
				for(int y = 0; y < MATRIX_WIDTH; ++y)
					pieceMatrix[x][y] = tempMatrix[x][y];
		rotation++;
		if(rotation == 4)
			rotation = 0;
	} // end rotateCW()

	public void rotateCCW() {
		boolean[][] tempMatrix = new boolean[MATRIX_WIDTH][MATRIX_WIDTH];

		for(int x = 0; x < MATRIX_WIDTH; ++x)
				for(int y = 0; y < MATRIX_WIDTH; ++y)
					tempMatrix[y][MATRIX_WIDTH - 1 - x] = pieceMatrix[x][y];

		for(int x = 0; x < MATRIX_WIDTH; ++x)
				for(int y = 0; y < MATRIX_WIDTH; ++y)
					pieceMatrix[x][y] = tempMatrix[x][y];
		rotation--;
		if(rotation == -1)
			rotation = 3;
	} // end rotateCCW()

	public void setPiece() {
		for(int x = 0; x < MATRIX_WIDTH; ++x)
			for(int y = 0; y < MATRIX_WIDTH; ++y)
				pieceMatrix[x][y] = false;

		switch(whichPiece) {
			case 0:
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				break;
			case 1:
				pieceMatrix[2][2] = true;
				break;
			case 2:
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				break;
			case 3:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				break;
			case 4:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				break;
			case 5:
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 6:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 7:
				pieceMatrix[1][2] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				break;
			case 8:
				pieceMatrix[1][2] = true;
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				break;
			case 9:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				break;
			case 10:
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				break;
			case 11:
				pieceMatrix[1][2] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 12:
				pieceMatrix[1][2] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][1] = true;
				break;
			case 13:
				pieceMatrix[1][1] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 14:
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[3][1] = true;
				break;
			case 15:
				pieceMatrix[1][1] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				break;
			case 16:
				pieceMatrix[1][0] = true;
				pieceMatrix[1][1] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				break;
			case 17:
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				pieceMatrix[3][0] = true;
				pieceMatrix[3][1] = true;
				pieceMatrix[3][2] = true;
				break;
			case 18:
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[2][1] = true;
				break;
			case 19:
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[2][1] = true;
				break;
			case 20:
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[2][1] = true;
				break;
			case 21:
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				break;
			case 22:
				pieceMatrix[1][1] = true;
				pieceMatrix[3][1] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 23:
				pieceMatrix[1][1] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[3][3] = true;
				break;
			case 24:
				pieceMatrix[3][1] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[1][3] = true;
				break;
			case 25:
				pieceMatrix[4][1] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[0][3] = true;
				break;
			case 26:
				pieceMatrix[0][1] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[4][3] = true;
				break;
			case 27:
				pieceMatrix[0][0] = true;
				pieceMatrix[0][1] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[4][3] = true;
				pieceMatrix[4][4] = true;
				break;
			case 28:
				pieceMatrix[0][2] = true;
				pieceMatrix[0][3] = true;
				pieceMatrix[0][4] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[4][0] = true;
				pieceMatrix[4][1] = true;
				break;
			case 29:
				pieceMatrix[0][0] = true;
				pieceMatrix[0][1] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[4][0] = true;
				pieceMatrix[4][1] = true;
				break;
			case 30:
				pieceMatrix[0][1] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[4][1] = true;
				break;
			case 31:
				pieceMatrix[0][0] = true;
				pieceMatrix[0][1] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[0][3] = true;
				pieceMatrix[0][4] = true;
				pieceMatrix[4][0] = true;
				pieceMatrix[4][1] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[4][3] = true;
				pieceMatrix[4][4] = true;
				pieceMatrix[1][0] = true;
				pieceMatrix[2][0] = true;
				pieceMatrix[3][0] = true;
				pieceMatrix[1][4] = true;
				pieceMatrix[2][4] = true;
				pieceMatrix[3][4] = true;
				break;
			case 32:
				pieceMatrix[0][0] = true;
				pieceMatrix[1][0] = true;
				pieceMatrix[2][0] = true;
				pieceMatrix[3][0] = true;
				pieceMatrix[4][0] = true;
				pieceMatrix[0][1] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[0][3] = true;
				pieceMatrix[0][4] = true;
				break;
			case 33:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 34:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[1][3] = true;
				pieceMatrix[3][3] = true;
				break;
			case 35:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				pieceMatrix[1][4] = true;
				pieceMatrix[3][4] = true;
				break;
			case 36:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				pieceMatrix[0][4] = true;
				pieceMatrix[1][4] = true;
				pieceMatrix[3][4] = true;
				pieceMatrix[4][4] = true;
				break;
			case 37:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[0][3] = true;
				pieceMatrix[1][3] = true;
				pieceMatrix[3][3] = true;
				pieceMatrix[4][3] = true;
				break;
			case 38:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				break;
			case 39:
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 40:
				pieceMatrix[2][0] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				break;
			case 41:
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[3][2] = true;
				break;
			case 42:
				pieceMatrix[2][1] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				break;
			case 43:
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[4][2] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][3] = true;
				break;
			case 44:
				pieceMatrix[0][2] = true;
				pieceMatrix[1][2] = true;
				pieceMatrix[2][2] = true;
				pieceMatrix[3][2] = true;
				pieceMatrix[2][1] = true;
				pieceMatrix[2][3] = true;
				pieceMatrix[2][4] = true;
				break;
		} // end switch
	} // end setPiece()

} // end class WallPiece
