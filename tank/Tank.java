import java.awt.*;
import java.lang.Math;

public class Tank {

	TankGame game;
	Image    tankBody;
	Image    tankTurret;
	Image[]  tankTreads;

	double x, y;
	double xVel, yVel;
	double rotation;
	double rotationVel;
	double turretRotation;
	double turretRotationVel;
	double vertex;

	private int curTreadFrame, treadFrameDelta;
	private long treadFrameTimer;

	boolean movingForward, movingBackward, turningLeft, turningRight,
		turretRotRight, turretRotLeft;

	public Tank(TankGame g) {
		game   = g;

		// Get all the images we need to draw the tank
		tankBody 	= game.getTrackedImage("tank.png");
		tankTurret 	= game.getTrackedImage("turret.png");
		tankTreads	= new Image[3];
		for(int i = 0; i < 3; ++i)
			tankTreads[i] = game.getTrackedImage("tanktread"+i+".png");

		// Initialize the position/vel/states
		x    = y    = 32;
		xVel = yVel = 0;
		rotation    = 0.0F;
		rotationVel = 0.0F;
		turretRotation = 0.0F;
		turretRotationVel = 0.0F;
		// vertex is the distance down the tank that we should
		// rotate about
		vertex      = (tankBody.getWidth(null) * 0.5F);
		movingForward = movingBackward = turningLeft = turningRight = false;
		turretRotRight = turretRotLeft = false;
		curTreadFrame   = 0;
		treadFrameDelta = 1;
	}

	public void updateYourself(long delta) {
		final double tankAccel	= 0.00047F;
		final double maxTankVel	= 0.19F;
		final double tankFriction = 0.00035F;

		final double tankTurnAccel	= 0.0000037F;
		final double maxTankTurnVel	= 0.001829F;
		final double tankTurnFric	= 0.0000015F;

		// Change the current tread frame appropriately
		treadFrameTimer += delta;
		if(treadFrameTimer > 100)
		{
			treadFrameTimer = 0;
			if(movingForward)
				curTreadFrame++;
			else if(movingBackward)
				curTreadFrame--;
			if(curTreadFrame < 0)
				curTreadFrame = 2;
			else if(curTreadFrame > 2)
				curTreadFrame = 0;
		}

		// Change the turret rotation velocity
		if(turretRotLeft)
			turretRotationVel -= tankTurnAccel * delta;
		else if(turretRotRight)
			turretRotationVel += tankTurnAccel * delta;
		if(Math.abs(turretRotationVel) < 0.0000140)
			turretRotationVel = 0.0F;
		// Apply rotational friction
		if(turretRotationVel > 0.0)
		{
			turretRotationVel -= tankTurnFric * delta;
			if(turretRotationVel < 0.0)
				turretRotationVel = 0.0;
		}
		else if(turretRotationVel < 0.0)
		{
			turretRotationVel += tankTurnFric * delta;
			if(turretRotationVel > 0.0)
				turretRotationVel = 0.0;
		}

		// Adjust the speed of the tank appropriately
		if(movingForward)
		{
			xVel += Math.cos(rotation) * tankAccel * delta;
			yVel += Math.sin(rotation) * tankAccel * delta;
		}
		else if(movingBackward)
		{
			xVel -= Math.cos(rotation) * tankAccel * delta;
			yVel -= Math.sin(rotation) * tankAccel * delta;
		}
		// Apply motion friction
		double angleOfMotion = (double)Math.atan2(yVel, xVel);
		double dX = (double)Math.cos(angleOfMotion) * tankFriction * delta;
		double oldXVel = xVel;
		xVel -= dX;
		if(oldXVel * xVel < 0.0)
			xVel = 0.0F;
		double dY = (double)Math.sin(angleOfMotion) * tankFriction * delta;
		double oldYVel = yVel;
		yVel -= dY;
		if(oldYVel * yVel < 0.0)
			yVel = 0.0F;

		// Rotate the tank
		if(turningLeft)
			rotationVel -= tankTurnAccel * delta;
		else if(turningRight)
			rotationVel += tankTurnAccel * delta;
		if(Math.abs(rotationVel) < 0.0000140)
			rotationVel = 0.0F;
		// Apply rotational friction
		if(rotationVel > 0.0)
		{
			rotationVel -= tankTurnFric * delta;
			if(rotationVel < 0.0)
				rotationVel = 0.0;
		}
		else if(rotationVel < 0.0)
		{
			rotationVel += tankTurnFric * delta;
			if(rotationVel > 0.0)
				rotationVel = 0.0;
		}

		// Move the tank based on it's speed and make sure
		// that it hasn't exceeded the maximum velocities
		double tankVelocity = (double)Math.tan(xVel / yVel);
		if(tankVelocity > maxTankVel)
			;
		else if(tankVelocity < -maxTankVel)
			;
		if(rotationVel > maxTankTurnVel)
			rotationVel = maxTankTurnVel;
		else if(rotationVel < -maxTankTurnVel)
			rotationVel = -maxTankTurnVel;

		x += xVel * delta;
		y += yVel * delta;
		rotation += rotationVel * delta;
		turretRotation += turretRotationVel * delta;
	}

	public void moveForward() {
		movingForward = true;
	} // end function moveForward()
	public void moveBackward() {
		movingBackward = true;
	} // end function moveBackward()
	public void turnLeft() {
		turningLeft = true;
	} // end function turnLeft()
	public void turnRight() {
		turningRight = true;
	} // end function turnRight()

	public void stopMovingForward() {
		movingForward = false;
	} // end function stopMovingForward()
	public void stopMovingBackward() {
		movingBackward = false;
	} // end function stopMovingBackward()
	public void stopTurningLeft() {
		turningLeft = false;
	} // end function stopTurningLeft()
	public void stopTurningRight() {
		turningRight = false;
	} // end function stopTurningRight()

	public void rotateTurretLeft() {
		turretRotLeft = true;
	} // end function rotateTurretLeft()
	public void rotateTurretRight() {
		turretRotRight = true;
	} // end function rotateTurretRight()

	public void stopRotatingTurretLeft() {
		turretRotLeft = false;
	} // end function stopRotatingTurretLeft()
	public void stopRotatingTurretRight() {
		turretRotRight = false;
	} // end function stopRotatingTurretRight()

	public void drawYourself(Graphics2D g2d) {
		g2d.translate(x, y);
		g2d.rotate(rotation, vertex, vertex);
		g2d.drawImage(tankBody, 0, 0, null);
		g2d.drawImage(tankTreads[curTreadFrame], 0, 0, null);

		g2d.rotate(turretRotation, vertex, vertex);
		g2d.drawImage(tankTurret, 0, 0, null);
	}

} // end class Tank
