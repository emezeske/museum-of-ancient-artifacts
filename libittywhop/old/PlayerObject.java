import java.awt.*;
import java.util.Random;

public class PlayerObject {

	int x, y;
	int howMuchTerritory;
	byte myTerritory;
	byte myWall;

	WallPiece curWallPiece;
	WallPiece nextWallPiece;
	boolean[][] wallMatrix;
	boolean[][] isTerritory;

	Color playerColor;

	public PlayerObject(int id) {
		howMuchTerritory = 0;

		if(id == 0)
		{
			myTerritory = GameObject.PLAYER1_TERR;
			myWall      = GameObject.PLAYER1_WALL;
			playerColor = GameObject.PLAYER1_COL;
			x = y = 0;
		}
		else if(id == 1)
		{
			myTerritory = GameObject.PLAYER2_TERR;
			myWall      = GameObject.PLAYER2_WALL;
			playerColor = GameObject.PLAYER2_COL;
			x = GameObject.WORLD_WIDTH - WallPiece.MATRIX_WIDTH;
			y = 0;
		}
		else if(id == 2)
		{
			myTerritory = GameObject.PLAYER3_TERR;
			myWall      = GameObject.PLAYER3_WALL;
			playerColor = GameObject.PLAYER3_COL;
			x = GameObject.WORLD_WIDTH  - WallPiece.MATRIX_WIDTH;
			y = GameObject.WORLD_HEIGHT - WallPiece.MATRIX_WIDTH;
		}

		wallMatrix     = new boolean[GameObject.WORLD_WIDTH][GameObject.WORLD_HEIGHT];
		isTerritory    = new boolean[GameObject.WORLD_WIDTH][GameObject.WORLD_HEIGHT];
		curWallPiece   = new WallPiece();
		nextWallPiece  = new WallPiece();
	} // end PlayerObject()

	public void updateYourself(long delta) {
		if(curWallPiece != null)
			curWallPiece.updateYourself(delta);
	} // end updateYourself()

	public void keepPieceOnMap() {
		for(int x = 0; x < WallPiece.MATRIX_WIDTH; ++x)
			for(int y = 0; y < WallPiece.MATRIX_WIDTH; ++y)
				if(curWallPiece.pieceMatrix[x][y]) 
				{
					if(x + this.x < 0)
						this.x -= x + this.x;
					if(y + this.y < 0)
						this.y -= y + this.y;
					if(x + this.x >= GameObject.WORLD_WIDTH)
						this.x -= x + this.x - GameObject.WORLD_WIDTH + 1;
					if(y + this.y >= GameObject.WORLD_HEIGHT)
						this.y -= y + this.y - GameObject.WORLD_HEIGHT + 1;
				}
	} // end keepPieceOnMap()

	public void moveUp() {
		--y;
		keepPieceOnMap();
	} // end moveUp()

	public void moveDown() {
		++y;
		keepPieceOnMap();
	} // end moveDown()

	public void moveLeft() {
		--x;
		keepPieceOnMap();
	} // end moveLeft()

	public void moveRight() {
		++x;
		keepPieceOnMap();
	} // end moveRight()

	public void rotateCW() {
		curWallPiece.rotateCW();
		keepPieceOnMap();
	} // end rotateCW()

	public void rotateCCW() {
		curWallPiece.rotateCCW();
		keepPieceOnMap();
	} // end rotateCCW()

	public boolean isPiecePlaceable() {
		for(int x = 0; x < WallPiece.MATRIX_WIDTH; ++x)
			for(int y = 0; y < WallPiece.MATRIX_WIDTH; ++y)
				if(curWallPiece.pieceMatrix[x][y]) 
				{
					if( (x + this.x < 0) || (y + this.y < 0) || (x + this.x >= GameObject.WORLD_WIDTH) || (y + this.y >= GameObject.WORLD_HEIGHT) )
						return false;
					for(int i = 0; i < GameObject.NUM_PLAYERS; ++i)
						if(GameObject.players[i].wallMatrix[x + this.x][y + this.y])
							return false;
				}
		return true;
	} // end isPiecePlaceable()

	public void placeCurPiece() {
		for(int x = 0; x < WallPiece.MATRIX_WIDTH; ++x)
			for(int y = 0; y < WallPiece.MATRIX_WIDTH; ++y)
				if(curWallPiece.pieceMatrix[x][y])
				{
					wallMatrix[x + this.x][y + this.y] = true; 
					GameObject.drawList[x + this.x][y + this.y] = myWall;
					GameObject.drawListChanged = true;
				}
		setTerritory();
		
		curWallPiece  = nextWallPiece;
		nextWallPiece = new WallPiece();

		keepPieceOnMap();
	} // end placeCurPiece()

	public void setTerritory() {
		for(int x = 0; x < GameObject.WORLD_WIDTH; ++x)
			for(int y = 0; y < GameObject.WORLD_HEIGHT; ++y)
				isTerritory[x][y] = true;

		for(int x = 0; x < GameObject.WORLD_WIDTH; ++x)
		{
			if(!wallMatrix[x][0])
				wanderAndCheckTerritory(x, 0);
			if(!wallMatrix[x][GameObject.WORLD_HEIGHT -1])
				wanderAndCheckTerritory(x, GameObject.WORLD_HEIGHT - 1);
		}
		for(int y = 1; y < GameObject.WORLD_HEIGHT - 1; ++y)
		{
			if(!wallMatrix[0][y])
				wanderAndCheckTerritory(0, y);
			if(!wallMatrix[GameObject.WORLD_WIDTH - 1][y])
				wanderAndCheckTerritory(GameObject.WORLD_WIDTH - 1, y);
		}
		howMuchTerritory = 0;
		for(int x = 0; x < GameObject.WORLD_WIDTH; ++x)
			for(int y = 0; y < GameObject.WORLD_HEIGHT; ++y)
				if(isTerritory[x][y])
				{
					if(GameObject.drawList[x][y] == 0 || GameObject.drawList[x][y] == myWall)
						++howMuchTerritory;
					if(GameObject.drawList[x][y] == 0)
					{
						GameObject.drawList[x][y] = myTerritory;
						GameObject.drawListChanged = true;
					}
				}
	} // end setTerritory()

	public void wanderAndCheckTerritory(int x, int y) {
		isTerritory[x][y] = false;

		if(x + 1 < GameObject.WORLD_WIDTH)
			if(!wallMatrix[x+1][y] && isTerritory[x+1][y])
				wanderAndCheckTerritory(x+1, y);
		if(x - 1 >= 0)
			if(!wallMatrix[x-1][y] && isTerritory[x-1][y])
				wanderAndCheckTerritory(x-1, y);
		if(y + 1 < GameObject.WORLD_HEIGHT)
			if(!wallMatrix[x][y+1] && isTerritory[x][y+1])
				wanderAndCheckTerritory(x, y+1);
		if(y - 1 >= 0)
			if(!wallMatrix[x][y-1] && isTerritory[x][y-1])
				wanderAndCheckTerritory(x, y-1);
		if(x + 1 < (GameObject.WORLD_WIDTH) && (y + 1 < GameObject.WORLD_HEIGHT) )
			if(!wallMatrix[x+1][y+1] && isTerritory[x+1][y+1])
				wanderAndCheckTerritory(x+1, y+1);
		if( (x - 1 >= 0) && (y + 1 < GameObject.WORLD_HEIGHT) )
			if(!wallMatrix[x-1][y+1] && isTerritory[x-1][y+1])
				wanderAndCheckTerritory(x-1, y+1);
		if( (x + 1 < GameObject.WORLD_WIDTH) && (y - 1 > 0) )
			if(!wallMatrix[x+1][y-1] && isTerritory[x+1][y-1])
				wanderAndCheckTerritory(x+1, y-1);
		if( (x - 1 >= 0) && (y - 1 >= 0) )
			if(!wallMatrix[x-1][y-1] && isTerritory[x-1][y-1])
				wanderAndCheckTerritory(x-1, y-1);
	} // end wanderAndCheckTerritory()

	public void drawYourself(Graphics2D g2d) {
		curWallPiece.drawYourself(g2d, playerColor, x * WallPiece.TILE_WIDTH, y * WallPiece.TILE_WIDTH);
	} // end drawYourself()

} // end class PlayerObject