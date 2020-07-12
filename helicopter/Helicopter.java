import java.awt.*;
import java.lang.Math;

public class Helicopter {

	HelicopterGame  game;
	Image           hcBody;
	Image[]         chopperFrames;

	float x, y;
	float xVel, yVel;
	float rotation;
	float vertex;

	private int curChopperFrame, chopperFrameDelta;
	private long chopperFrameTimer;

	boolean rising, falling, movingLeft, movingRight;

	public Helicopter(HelicopterGame g) {
		game   = g;

		// Get all the images we need to draw the helicopter
		hcBody = game.getTrackedImage("helicopter.png");
		chopperFrames    = new Image[3];
		for(int i = 0; i < 3; ++i)
			chopperFrames[i] = game.getTrackedImage("chopper"+i+".png");

		// Initialize the position/vel/states
		x    = y    = 32;
		xVel = yVel = 0;
		rotation    = 0.0F;
		// vertex is the distance down the helicopter that we should
		// rotate about
		vertex      = (hcBody.getWidth(null) * 0.75F);
		rising = falling = movingLeft = movingRight = false;
		curChopperFrame   = 0;
		chopperFrameDelta = 1;
	}

	public void updateYourself(long delta) {
		final float yAccel  = 0.00037F;
		final float maxYVel = 0.19F;

		final float xAccel  = 0.00037F;
		final float maxXVel = 0.19F;

		// Change the current chopper frame appropriately
		chopperFrameTimer += delta;
		if(chopperFrameTimer > 30)
		{
			chopperFrameTimer = 0;
			curChopperFrame  += chopperFrameDelta;
			if((curChopperFrame == 0) || (curChopperFrame == 2))
				chopperFrameDelta = -chopperFrameDelta;

		}

		// Adjust the speed of the helicopter appropriately
		if(rising)
			yVel -= yAccel * delta;
		else if(falling)
			yVel += yAccel * delta;

		if(movingLeft)
			xVel -= xAccel * delta;
		else if(movingRight)
			xVel += xAccel * delta;

		// Move the helicopter based on it's speed and make sure
		// that it hasn't exceeded the maximum velocities
		if(yVel > maxYVel)
			yVel =  maxYVel;
		else if(yVel < -maxYVel)
			yVel = -maxYVel;

		if(xVel > maxXVel)
			xVel =  maxXVel;
		else if(xVel < -maxXVel)
			xVel = -maxXVel;

		y += yVel * delta;
		x += xVel * delta;

		// Finally, adjust the rotation of the helicopter
		// according to it's velocity
		rotation = xVel * 0.8F;


	}

	public void drawYourself(Graphics2D g2d) {
		g2d.translate(x + vertex - Math.cos(rotation) * vertex, y - Math.sin(rotation) * vertex);
		g2d.rotate(rotation);
		g2d.drawImage(hcBody, 0, 0, null);
		g2d.drawImage(chopperFrames[curChopperFrame], 0, 0, null);
	}

}
