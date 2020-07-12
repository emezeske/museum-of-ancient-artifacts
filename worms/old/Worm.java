import java.awt.*;
import java.awt.geom.AffineTransform;

public class Worm extends FreeRectBody {
	static Image 	restingL, restingR,
			jumpingL, jumpingR;

	short facing;
	final short LEFT  = 0;
	final short RIGHT = 1;

	short jumping;
	long  jumpTime;
	boolean shortJump;
	final short J_UP  = 0;
	final short J_LFT = 1;
	final short J_RGT = 3;
	final short J_NONE = 4;
	final short J_WAIT = 5;

	boolean movingLeft;
	boolean movingRight;

	// Weapon information
	boolean inWeaponMode;
	boolean chargingWeapon;
	double  weaponAngle;
	double  weaponMagnitude;
	long weaponChargeStartTime;
	long mouseHeldDownTime;

	final double MAX_WEP_MAG = 0.8;

	// WEAPONS !
	static final int NUM_WEAPONS = 3;
	Weapon[] weapons;
	int curWeap;

	public Worm(double X, double Y) {
		super(X, Y);
		jumping = J_NONE;
		weaponAngle = 0.0;
		weapons = new Weapon[NUM_WEAPONS];
		weapons[0] = new Bazooka();
		weapons[1] = new Grenade();
		weapons[2] = new Firework();
		curWeap = 0;
	} // end constructor Worm()

	static void getImages() {
		restingL = WormGame.refToSelf.getTrackedImage("wormrestl.png");
		restingR = WormGame.refToSelf.getTrackedImage("wormrestr.png");
		jumpingL = WormGame.refToSelf.getTrackedImage("wormjumpl.png");
		jumpingR = WormGame.refToSelf.getTrackedImage("wormjumpr.png");
	} // end getImages()

	public void drawYourself(Graphics2D g2d) {
		AffineTransform oldXForm = g2d.getTransform();
		g2d.translate(x, y);
		if(atRest) {
			switch(facing) {
				case LEFT:
				g2d.drawImage(restingL, 0, 0, null);
				break;
				case RIGHT:
				g2d.drawImage(restingR, 0, 0, null);
				break;
			}
			if(inWeaponMode) {
				double powerBarLen = 70*(weaponMagnitude+0.4);
				double maxPowerBarLen = 75*(MAX_WEP_MAG+0.4);
				double negAngle   = weaponAngle-Math.PI/16;
				double posAngle   = weaponAngle+Math.PI/16;
				int[] xpoints = { (int)(0.5*width),  (int)(width*0.5 +Math.cos(negAngle)*powerBarLen) ,(int)(width*0.5 +Math.cos(posAngle)*powerBarLen)};
				int[] ypoints = { (int)(0.5*height), (int)(height*0.5+Math.sin(negAngle)*powerBarLen) ,(int)(height*0.5+Math.sin(posAngle)*powerBarLen)};
				g2d.setPaint(new GradientPaint(	(float)(0.5*width), (float)(0.5*height), Color.orange,
								(float)(width*0.5+Math.cos(weaponAngle)*maxPowerBarLen), (float)(width*0.5+Math.sin(weaponAngle)*maxPowerBarLen), Color.red));
				g2d.fill(new Polygon(xpoints, ypoints, 3));
			}
		} else {
			switch(facing) {
				case LEFT:
				g2d.drawImage(jumpingL, 0, 0, null);
				break;
				case RIGHT:
				g2d.drawImage(jumpingR, 0, 0, null);
				break;
			}
		}
		g2d.setTransform(oldXForm);
		weapons[curWeap].drawYourself(g2d);
	} // end drawYourself()

	public void updateYourself(long delta) {
		if(chargingWeapon) {
			mouseHeldDownTime = System.currentTimeMillis() - weaponChargeStartTime;
			weaponMagnitude = 0.0007 * mouseHeldDownTime;
			if(weaponMagnitude > MAX_WEP_MAG)
				weaponMagnitude = MAX_WEP_MAG;
		} else
			weaponMagnitude = 0.0;
		waitToJump();
		if(!atRest) {
			if(xVel < 0)
				facing = LEFT;
			else 
				facing = RIGHT;
		}
		super.updateYourself(delta);
		weapons[curWeap].updateYourself(delta);
	} // end updateYourself()

	public void collisionHandler(Point2D closestPoint, Point2D closestPointOnRec, double lastRealX, double lastRealY) {
		if(movingLeft || movingRight || jumping == J_RGT || jumping == J_LFT || jumping == J_UP ) {
			x = lastRealX;
			y = lastRealY;
			xVel = yVel = 0.0;
			movingLeft = movingRight = false;
			atRest = true;
			jumping = J_NONE;
			return;
		}
		super.collisionHandler(closestPoint, closestPointOnRec, lastRealX, lastRealY); 
	} // end collisionEvent()

	// DEBUG
	public void incWeap() {
		curWeap++;
		if(curWeap == NUM_WEAPONS)
			curWeap = 0;
	} // end incWeap()

	public void changeAim(int mX, int mY) {
		double aimX = mX - x - 0.5*width;
		double aimY = mY - y - 0.5*height;
		weaponAngle = Math.atan2(aimY, aimX);
	} // end changeAim()

	public void beginChargingWeapon(int mX, int mY) {
		if(!inWeaponMode)
			return;
		weaponChargeStartTime = System.currentTimeMillis();
		chargingWeapon = true;
	} // end beginChargingWeapon()

	public void dischargeWeapon() {
		if(!inWeaponMode)
			return;
		weapons[curWeap].fire(x + 0.5*width, y + 0.5*height, weaponAngle, weaponMagnitude);
		chargingWeapon = false;
		inWeaponMode = false;
	} // end dischargeWeapon()

	public void toggleFireMode() {
		if(atRest && !inWeaponMode)
			inWeaponMode = true;
		else if(inWeaponMode)
			inWeaponMode = false;
	} // end toggleFireMode()

	public void beginMovingLeft(boolean shiftWasDown) {
		if(jumping == J_WAIT)
		{
			jumping = J_LFT;
			return;
		}
		if(!atRest || movingRight || movingLeft)
			return;
		if(facing == RIGHT) {
			facing = LEFT;
			return;
		}
		atRest 		= false;
		movingLeft 	= true;
		xVel 		=  (shiftWasDown) ? -0.02 : -0.04;
		yVel 		= -0.10;
	} // end beginMovingLeft()

	public void beginMovingRight(boolean shiftWasDown) {
		if(jumping == J_WAIT)
		{
			jumping = J_RGT;
			return;
		}
		if(!atRest || movingRight || movingLeft)
			return;
		if(facing == LEFT) {
			facing = RIGHT;
			return;
		}
		atRest 		= false;
		movingRight 	= true;
		xVel 		=  (shiftWasDown) ? 0.02 : 0.04;
		yVel 		= -0.10;
	} // end beginMovingRight()

	public void beginJump(boolean shiftWasDown) {
		if(!atRest || movingRight || movingLeft || jumping != J_NONE)
			return;
		shortJump = shiftWasDown;
		jumping = J_WAIT;
		jumpTime = System.currentTimeMillis();
	} // end beginJump()

	public void waitToJump() {
		if(jumpTime + 150 > System.currentTimeMillis() || jumping == J_NONE)
				return;
		if(jumping == J_WAIT)
			jumping = J_UP;
		switch(jumping) {
			case J_UP:
				yVel = -0.20;
				break;
			case J_LFT:
				yVel = -0.20;
				xVel = (shortJump) ? -0.02: -0.08;
				break;
			case J_RGT:
				yVel = -0.20;
				xVel = (shortJump) ? 0.02: 0.08;
				break;
		}
		jumping = J_NONE;
		atRest = false;
	} // end waitToJump()
} // end class
