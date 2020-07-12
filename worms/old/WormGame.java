import java.applet.*;
import java.awt.image.BufferedImage;
import java.awt.event.*;
import java.awt.geom.RoundRectangle2D;
import java.awt.*;

public class WormGame extends Applet implements Runnable {
	// Declarations for the pretty much 'global' variables
	static final int 	SCREEN_W 	= 800;
	static final int 	SCREEN_H 	= 600;
	static final int 	NUM_WORMS 	= 8;

	static double GRAV_ACC  = 0.0003;
	static double WIND_PWR	= 0.00003;
	// Which worm should the keyboard commands be sent to?
	int CUR_WORM = 0;
	// This is a lame way to give everything access
	// to the functions that are part of the WormGame class
	// without the other classes needing to get passed a reference
	// in their constructors.  Call it a hack, I call it ingenious!
	static WormGame refToSelf;
	// The camera class
	static Camera cam;
	// This is the array of freely moving rectangular bodies
	Worm[] worms;
	// Instead of paint() getting called, update() gets called so it can draw
	// to the back buffer and then flip
	Image backBufferImage;
	// Stupid object to set up how the game should try to look
	RenderingHints rh;
	// The terrain object -- this manages the collision arrays and the terrain
	// images
	Terrain terrain;
	// The thread that calls repaint() and updates the states of the objects
	Thread workThread;
	// Condition under which the workthread will run
	boolean workThreadRunning;
	// Keeps track of last time a frame was drawn 
	long lastTime; 
	// The color to clear the terrain to when it is blown up
	static Color clearColor;
	// Loading screen stuff
	MediaTracker mt;
	int numImagesToTrack;
	int numImagesLeftToLoad;
	boolean stillLoading;
	// The explosion graphics manager
	Explosion[] explosions;
	static final int MAX_EXPLOSIONS = 8;
	// Wind Particles so that player can judge the speed of the wind
	SmokePuff[] smokePuffs;
	static final int MAX_SMOKE_PUFFS = 100;

	Flame flame;

	// DEBUG VARS
	static boolean DEBUG_MODE_ON;
	short framesDrawn = 0;
	short timer = 0;
	// END DEBUG VARS

	// Set up initial objects and start the worker thread
	public void init() {
		stillLoading = true;

		backBufferImage	= createImage(getSize().width, getSize().height);

		workThread = new Thread(this);
		workThread.start();
	} // end init()

	// Load everything and display the loading screen when necessary
	public void loadEverything() {
		// Load all the images
		for(int i = 0; i < numImagesToTrack; ++i) {
			numImagesLeftToLoad = numImagesToTrack-i;
			try { 
				mt.waitForID(i);
			} catch (InterruptedException ie) {
				System.exit(1);
			}
		}
		// Create game objects
		terrain = new Terrain();
		cam = new Camera();
		flame = new Flame();
		worms = new Worm[8];
		for(int i=0; i<NUM_WORMS; ++i)
			worms[i] = new Worm(100 + i * 85, 1);
		explosions = new Explosion[MAX_EXPLOSIONS];
		for(int i=0; i<MAX_EXPLOSIONS; ++i)
			explosions[i] = new Explosion();
		smokePuffs = new SmokePuff[MAX_SMOKE_PUFFS];
		for(int i=0; i<MAX_SMOKE_PUFFS; ++i)
			smokePuffs[i] = new SmokePuff();
		// Call the garbage collector so that we have as much memory as 
		// possible, and to minimize the chance that it will get called during
		// paint()
		System.gc();
		lastTime = System.currentTimeMillis();
		stillLoading = false;
	} // end function loadEverything()

	long loadingTimeKeeper;
	short gradientOffset = 0;
	// Simply draws the basic loading screen
	void drawLoadingScreen(Graphics2D g2d) {
		long delta = System.currentTimeMillis() - loadingTimeKeeper;
		loadingTimeKeeper = System.currentTimeMillis();
		gradientOffset += (short)(delta*0.1);

		final int BAR_W = 150;
		final int BAR_H = 25;
		g2d.setColor(Color.black);
		g2d.fillRect(0, 0, SCREEN_W, SCREEN_H);

		g2d.setPaint(new GradientPaint(0 + gradientOffset, 0, Color.gray, 200 + gradientOffset, 100, Color.blue, true) );
		g2d.fill(new RoundRectangle2D.Double(SCREEN_W*0.5-100, SCREEN_H*0.5-50, 200, 100, 10, 10) );

		g2d.setColor(Color.white);
		g2d.drawString("Loading...", (int)(SCREEN_W*0.5-20), (int)(SCREEN_H*0.5-20)); 
		g2d.setColor(Color.black);
		g2d.fillRect((int)(SCREEN_W*0.5-BAR_W*0.5), (int)(SCREEN_H*0.5-BAR_H*0.5), BAR_W, BAR_H);
		
		int percentFinished = (int)(100 - 100*((float)numImagesLeftToLoad/(float)numImagesToTrack));
		g2d.setColor(Color.blue);
		g2d.fillRect((int)(SCREEN_W*0.5-BAR_W*0.5), (int)(SCREEN_H*0.5-BAR_H*0.5), (int)(BAR_W*percentFinished/100), BAR_H);
		g2d.setColor(Color.white);
		g2d.drawString(Integer.toString(percentFinished)+"%", (int)(SCREEN_W*0.5-8), (int)(SCREEN_H*0.5 + 5)); 
	} // end drawLoadingScreen()

	// Use the media tracker to make sure all the images are finished 
	// before going on
	public Image getTrackedImage(String filename) {
		// Start getting the image 
		Image im = getImage(getCodeBase(), "gfx/"+filename); 
		// Add the image to the queue
		mt.addImage(im, numImagesToTrack++); 
		// Return the reference
		return im;
	} // end getTrackedImage()

	// This clears the back buffer and draws the scene onto it, and then flips 
	// buffers to avoid tearing
	public void update(Graphics g) {
		// Get a Graphics2D for the backbuffer, and clear it
		Graphics2D backBufferGraphics = (Graphics2D)backBufferImage.getGraphics();
		if(stillLoading) {
			drawLoadingScreen(backBufferGraphics);
			g.drawImage(backBufferImage, 0, 0, null);
			return;
		}
		// Update the state of everything
		updateEverything();
		// Draw everything to the backbuffer
		paint(backBufferGraphics);
		// Draw the backbuffer to the screen
		g.drawImage(backBufferImage, 0, 0, null);
	} // end update()

	// Draw the scene
	public void paint(Graphics2D g2d) {
		cam.prepareScene(g2d);
		terrain.drawYourself(g2d);
		for(int i=0; i<NUM_WORMS; ++i)
			worms[i].drawYourself(g2d);
		for(int i=0; i<MAX_EXPLOSIONS; ++i)
			explosions[i].drawYourself(g2d);
		for(int i=0; i<MAX_SMOKE_PUFFS; ++i)
			smokePuffs[i].drawYourself(g2d);
		flame.drawYourself(g2d);
	} // end paint()

	// This is the thread that runs the animation and physics
	public void run() {
		// Set up necessary stuff 
		loadingTimeKeeper = System.currentTimeMillis();
		refToSelf = this;
		DEBUG_MODE_ON = false;
		clearColor = new Color(0, 0, 100);
		// Set up a mediatracker and count how many images have to be tracked
		mt = new MediaTracker(this);
		numImagesToTrack = 0;
		Worm.getImages();
		Terrain.getImages("terrain1.png");
		Camera.getImages();
		Bazooka.getImages();
		Grenade.getImages();
		Explosion.getImages();
		Firework.getImages();
		Confetti.getImages();
		SmokePuff.getImages();
		// Start loading thread so that repaint can be called and animate
		// the loading screen
		Thread loadingThread = new Thread() {
							public void run() {
								loadEverything();
							}
						};
		loadingThread.start();

		// Start the thread that does alllllll the work
		workThreadRunning = true;
		while(workThreadRunning) {
			repaint(0);
			Thread.yield();
		}
	} // end run()

	// Move everything appropriately and detect collisions and 
	// then ask for the update function to be called
	public synchronized void updateEverything() {
		// Find out how much time has elapsed since last frame
		long time = System.currentTimeMillis();
		long delta = time - lastTime;
		lastTime = time;
		// If no time has elapsed, bail 
		if(delta == 0) return;
		// Move all the Worms
		for(int i=0; i<NUM_WORMS; ++i)
			worms[i].updateYourself(delta);
		cam.setNewTarget(worms[CUR_WORM].x, worms[CUR_WORM].y);
		// Move the camera
		cam.updateYourself(delta);
		// Update the explosions
		for(int i=0; i<MAX_EXPLOSIONS; ++i)
			explosions[i].updateYourself(delta);
		// Update the flame
		flame.updateYourself(delta);
		for(int i=0; i<MAX_SMOKE_PUFFS; ++i)
			smokePuffs[i].updateYourself(delta);
		// Show FPS if debug mode is on
		if(DEBUG_MODE_ON) {
			framesDrawn++;
			timer+=delta;
			if(timer > 1000) {
				timer = 0;
				System.out.println("FPS: "+framesDrawn);
				framesDrawn = 0;
			}
		}
	} // end updateEverything()

	public void emitSmoke(int x, int y, int numPuffs, int radius) {
		int numNewParticles = 0;
		for(int i=0; i<MAX_SMOKE_PUFFS; ++i) {
			if(!smokePuffs[i].active)
			{
				double r = (double)radius*Math.random();
				double angle = 2*Math.PI*Math.random();
				x += (int)(Math.cos(angle)*r);
				y += (int)(Math.sin(angle)*r);
				smokePuffs[i].startFalling(x, y);
				numNewParticles++;
				if(numNewParticles == numPuffs)
					break;
			}
		}
	} // end emitSmoke

	// This function causes all the Worms within the radius to fly
	// away from an explosion, and then carves a hole in the terrain
	public void explodeAt(int x, int y, int diameter) {
		int diameterSq = diameter*diameter;
		// Cycle through all the Worms and see if any are within the
		// blast radius, if so, send them flying!!
		double EXPLOSION_PWR = 0.0;
		if(diameter == 100)
			EXPLOSION_PWR = 0.08;
		else if(diameter == 50)
			EXPLOSION_PWR = 0.04;
		for(int i=0; i<NUM_WORMS; ++i)
		{
			double distToBodySq = (worms[i].x+0.5*worms[i].width-x)*(worms[i].x+0.5*worms[i].width-x) + (worms[i].y+0.5*worms[i].height-y)*(worms[i].y+0.5*worms[i].height-y);
			if(distToBodySq < diameterSq/2) {
				double proximity = diameterSq / distToBodySq;
				if(proximity > 6)
					proximity = 6;
				double explodeVecX = worms[i].x+0.5*worms[i].width-x;
				double explodeVecY = worms[i].y+0.5*worms[i].height-y;
				double angle = Math.atan2(explodeVecY, explodeVecX);
				worms[i].xVel += Math.cos(angle) * proximity * EXPLOSION_PWR;
				worms[i].yVel += Math.sin(angle) * proximity * EXPLOSION_PWR;
				worms[i].atRest = false;
			}
		}
		terrain.cutHoleInTerrain(x, y, diameter);
		for(int i=0; i<MAX_EXPLOSIONS; ++i)
			if(!explosions[i].active) {
				explosions[i].explode(x, y, diameter); 
				break;
			}
		cam.jostle();
	} // end explodeAt()

	// DEBUG
	public void toggleDebugMode() {
		DEBUG_MODE_ON = !DEBUG_MODE_ON;
	}

	// Override the handleEvent() function so we can turn off events
	// while loading
	public boolean handleEvent(Event evt) {
 		// If loading don't deal with the input event 
		if(stillLoading) {
			return false;
		}
		return super.handleEvent(evt);
	} // end handleEvent()

	public boolean mouseDown(Event evt, int x, int y) {
		// Transform the mouse screen coordinates into world coords
		x = (int)(cam.x + (double)x/SCREEN_W*cam.width);
		y = (int)(cam.y + (double)y/SCREEN_H*cam.height);
		// Tell the worm that the mouse was clicked
		worms[CUR_WORM].beginChargingWeapon(x, y);
		return true;
	} // end mousePressed()

	public boolean mouseUp(Event evt, int x, int y) {
		worms[CUR_WORM].dischargeWeapon();
		return true;
	} // end mouseUp()

	public boolean mouseMove(Event evt, int x, int y) {
		// Transform the mouse screen coordinates into world coords
		x = (int)(cam.x + (double)x/SCREEN_W*cam.width);
		y = (int)(cam.y + (double)y/SCREEN_H*cam.height);
		worms[CUR_WORM].changeAim(x, y);
		return true;
	} // end mouseMove()

	public boolean mouseDrag(Event evt, int x, int y) {
		// Transform the mouse screen coordinates into world coords
		x = (int)(cam.x + (double)x/SCREEN_W*cam.width);
		y = (int)(cam.y + (double)y/SCREEN_H*cam.height);
		worms[CUR_WORM].changeAim(x, y);
		return true;
	} // end mouseMove()

	public double zoom = 1.0;
	// Called whenever a key is pushed, and sends out commands based on what key
	public boolean keyDown(Event evt, int key) {
		boolean shiftWasDown = ( (evt.modifiers & evt.SHIFT_MASK) != 0) ? true : false;
		switch(key) {
			case 'q':
			case 'Q':
				worms[CUR_WORM].incWeap();
				break;
			case 'a':
			case 'A':
				worms[CUR_WORM].beginMovingLeft(shiftWasDown);
				break;
			case 'd':
			case 'D':
				worms[CUR_WORM].beginMovingRight(shiftWasDown);
				break;
			case 'e':
			case 'E':
				worms[CUR_WORM].toggleFireMode();
				break;
			case 'w':
			case 'W':
				worms[CUR_WORM].beginJump(shiftWasDown);
				break;
			case 'z':
				toggleDebugMode();
				break;
			case 'i':
				if(zoom + 0.1 <= cam.MAX_ZOOM)
				{
					zoom += 0.1;
					cam.setZoomTarget(zoom);
				}
				break;
			case 'o':
				if(zoom - 0.1 >= cam.MIN_ZOOM)
				{
					zoom -= 0.1;
					cam.setZoomTarget(zoom);
				}
				break;
			case Event.LEFT:
				CUR_WORM--;
				if(CUR_WORM < 0)
					CUR_WORM = NUM_WORMS - 1;
				break;
			case Event.RIGHT:
				CUR_WORM++;
				if(CUR_WORM == NUM_WORMS)
					CUR_WORM = 0;
				break;
		}
		return true;
	} // end keyDown()

	// Clean up and exit!
	public void destroy() {
		workThreadRunning = false;
		try{ Thread.sleep(200); }
		catch(InterruptedException ie) {}
		workThread = null;
		clearColor = null;
		worms = null;
		backBufferImage	= null;
		terrain = null;
		rh = null;
		cam = null;
	} // end destroy()

} // end class WormGame
