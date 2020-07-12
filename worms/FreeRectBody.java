import java.awt.*;

/*  This is the abstract class FreeRectBody.  It is the generic physics and collision detection
 *  handler.  It simulates a rectangular body with a certain speed and elasticity
 *  that can collide with the terrain.  Anything that needs to be effected by gravity
 *  and interact with the terrain should subclass this "pure virtual" class.
 *  */  

public abstract class FreeRectBody {
	// Width and height of the rectangle that makes up the collision body
	int width, height;
	// Elasticity -- i.e. how much of the original energy this body comes
	// out of a collision with
	double elasticity;
	// If the object's velocity becomes extremely small, it will be
	// considered at rest
	boolean atRest;
	// Location
	double x, y;
	// The location of the body after the last frame
	double lastRealX, lastRealY;
	// Speeeeed
	double xVel, yVel;

	// Don't feel like instantiating one of java.geom's humongous
	// objects, so here is a very basic struct
	public class Point2D {
		double x, y;
		public Point2D() {
			x = y = 0.0;
		}
	}

	// Objects that will be used for collision detection are new'ed in the 
	// constructor so we don't have to allocate during animation and summon
	// the evil garbage collector!
	Point2D	p1, p2, closestPoint, comparePoint, closestPointOnRec,
			result; 

	// Guess what this is! The constructor.
	public FreeRectBody(double X, double Y) {
		x 		= X;
		y 		= Y;
		xVel 		= 0.00;
		yVel 		= 0.0;
		width 		= 15;
		height 		= 25;
		elasticity	= 0.35;
		atRest 		= false;
		p1 			= new Point2D();
		p2 			= new Point2D();
		closestPoint 		= new Point2D();
		comparePoint 		= new Point2D();
		closestPointOnRec 	= new Point2D();
		result 			= new Point2D();
	} // end constructor FreeRectBody()

	// Called each frame to have the body draw itself
	public void drawYourself(Graphics2D g2d) {
		g2d.translate(x, y);
		g2d.setColor(Color.green);
		g2d.fillRect(0, 0, width, height);
		g2d.translate(-x, -y);
	} // end drawYourself()

	// This function gets called once per frame and is passed the
	// number of milliseconds since the last frame so it can update
	// the position of the body
	public void updateYourself(long delta) {
		if(atRest)
			return;
		xVel += WormGame.WIND_PWR * delta;
		yVel += WormGame.GRAV_ACC * delta;

		lastRealX = x;
		lastRealY = y;
		x += xVel * delta;
		y += yVel * delta;

		checkForOutOfBounds();
		checkForCollisions(delta);
	} // end updateYourself()

	// This should probably be overridden in subclasses
	public void checkForOutOfBounds() {
		if(y >= Terrain.HEIGHT - height) {
			xVel = elasticity * xVel;
			yVel = elasticity * -yVel;
			x = lastRealX;
			y = lastRealY;
		}
	} // end checkForOutOfBounds()

	// This function checks to see if the body has crossed any terrain between
	// this frame and the last.  If it has, the collisionHandler() function for 
	// this body is called
	public void checkForCollisions(long delta) {
		// So, find the equations of the two lines which we are checking
		// between and then.... check... between them and stuff.
		int	lastX = (int)Math.round(lastRealX),
			lastY = (int)Math.round(lastRealY),
			curX  = (int)Math.round(x),
			curY  = (int)Math.round(y);
		boolean xVelIsZero = (lastX - curX == 0),
			yVelIsZero = (lastY - curY == 0);
		// Find the slope of the two lines that define the collision volume
		double  slope = (double)(curY - lastY) / (double)(curX - lastX);
		// Find the two points where the lines that define the collision volume
		// intersect with the rectangle 
		int intersect1X, intersect1Y,
		    intersect2X, intersect2Y;
		if( ((curX > lastX) && (curY > lastY)) || ((curX < lastX) && (curY < lastY)) )
		{
			intersect1X = curX;
			intersect1Y = curY + height;
			intersect2X = curX + width;
			intersect2Y = curY;
		//} else if( ((curX > lastX) && (curY < lastY)) || ((curX < lastX) && (curY > lastY)) )
		} else	
		{
			intersect1X = curX;
			intersect1Y = curY;
			intersect2X = curX + width;
			intersect2Y = curY + height;
		}
		// Find the two 'b' (y = mx + b) values for the lines
		double 	b1 = intersect1Y - (double)intersect1X*slope,
			b2 = intersect2Y - (double)intersect2X*slope;
		// Determine the bounding within which we must test for terrain
		// bb = bounding box
		int bbX, bbY, bbW, bbH;
		bbX = (curX < lastX) ? curX : lastX;
		bbY = (curY < lastY) ? curY : lastY;
		bbW = (curX < lastX) ? lastX - curX + width: curX - lastX + width;
		bbH = (curY < lastY) ? lastY - curY + height: curY - lastY + height;
		boolean b1IsGreater = false;
		if(b1 > b2)
			b1IsGreater = true;

		boolean collided    = false;
		double closestDistSquared = Float.MAX_VALUE;
		for(int terX=bbX; terX<=(bbX+bbW); ++terX) {
			// Find the range of Y values at this X value that the rectangle
			// passed through
			int leastY, greatestY;
			if(b1IsGreater) {
				leastY    = (int)(slope*terX + b2);
				greatestY = (int)(slope*terX + b1);
			} else {
				leastY    = (int)(slope*terX + b1);
				greatestY = (int)(slope*terX + b2);
			}
			if(leastY < bbY)
				leastY = bbY;
			if(greatestY > bbY + bbH)
				greatestY = bbY + bbH;
			if(xVelIsZero || yVelIsZero) {
				leastY = bbY;
				greatestY = bbY + bbH;
			}
			for(int terY=leastY; terY<=greatestY; ++terY) {
				if( (terX >= Terrain.WIDTH) || (terX < 0) || (terY >= Terrain.HEIGHT) || (terY < 0) )
					continue;
				if(Terrain.collisionArray[terX][terY])
				{
					collided = true;
					comparePoint = nearestPointOnRect(terX, terY, lastRealX, lastRealY, width, height);
					double distToPlayerSq = (terX-comparePoint.x)*(terX-comparePoint.x)+(terY-comparePoint.y)*(terY-comparePoint.y);
					// If the current bit of terrain we are checking 
					// is in the rectangle's path AND it is the closest
					// known point we save the values of it's location
					// and relative location to the rectangle for later
					// use
					if(distToPlayerSq < closestDistSquared)
					{
						closestDistSquared = distToPlayerSq;
						closestPointOnRec.x = comparePoint.x;
						closestPointOnRec.y = comparePoint.y;
						closestPoint.x = terX;
						closestPoint.y = terY;
					} // end if
				} // end if
			} // end for
		} // end for
		// Handle the collision if one existed.  
		if(collided) {
			collisionHandler(closestPoint, closestPointOnRec, lastRealX, lastRealY);
		}
	} // end checkForCollisions()

	// This function deals with what should happen after the collision has been
	// detected.  It will most likely be overridden by any subclasses, but it
	// can be called by subclasses with the "super.collisionHandler" interface
	// and extended in subclass collisionHandler functions
	public void collisionHandler(Point2D closestPoint, Point2D closestPointOnRec, double lastRealX, double lastRealY) {
		// If the body is moving too slowly, consider it at rest
		if( (xVel*xVel + yVel*yVel < 0.0007) && (closestPointOnRec.y >= y + 0.9*height) )
		{
			xVel = yVel = 0.0;
			atRest = true;
		}
		x = lastRealX;
		y = lastRealY;
		// Now that the rectangle is at where it collided with the terrain, calculate
		// the new velocity based on the angle of impact with the particle
		double 	lineX = -(closestPoint.y - closestPointOnRec.y),
			lineY =  (closestPoint.x - closestPointOnRec.x),
			angleOfLineCollidedWith = Math.atan2(lineY, lineX);
		double velocityAngle = Math.atan2(yVel, xVel);
		double velocityMag   = Math.sqrt(xVel*xVel+yVel*yVel);
		xVel = -elasticity * Math.cos(-velocityAngle+Math.PI+2*angleOfLineCollidedWith) * velocityMag;
		yVel = -elasticity * Math.sin(-velocityAngle+Math.PI+2*angleOfLineCollidedWith) * velocityMag;
	} // end collisionHandler()

	// This function returns a Point2D which is the point on the rectangle rX rY that // is closest to the point pX pY.  It gets passed the following parameters:
	// pointX pointY rectX rectY rectWidth rectHeight
	public Point2D nearestPointOnRect(double pX, double pY, double rX, double rY, double rW, double rH) {
		if(pX <= rX) {
			// Must be on side 1
			if(pY <= rY) {
				result.x = rX;
				result.y = rY;
			} else if(pY >= rY + rH) {
				result.x = rX;
				result.y = rY + rH;
			} else {
				result.x = rX;
				result.y = pY;
			}
		} else if(pX >= rX + rW) {
			// Must be side 3
			if(pY <= rY) {
				result.x = rX + rW;
				result.y = rY;
			} else if(pY >= rY + rH) {
				result.x = rX + rW;
				result.y = rY + rH;
			} else {
				result.x = rX + rW;
				result.y = pY;
			}

		} else {
			if(pY <= rY) {
				// Must be side 4
				result.x = pX;
				result.y = rY;
			} else if(pY >= rY + rH) {
				// Must be side 2
				result.x = pX;
				result.y = rY + rH;
			} else {
				// The point was inside the object -- this should
				// never happen unless something in the collision
				// detection has gone wrong.
			} 
		}
		return result;
	} // end nearestPointOnRect()

} // end class FreeRectBody()
