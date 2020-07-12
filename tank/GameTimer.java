import java.util.TimerTask;
import java.util.Timer;

public class GameTimer extends TimerTask {

	TankGame 	game;
	Timer 		timer;
	long 		lastTime;

	public GameTimer(TankGame tg) {
		game     = tg;
		lastTime = System.currentTimeMillis();
		timer    = new Timer();
		timer.scheduleAtFixedRate(this, 0L, 25);
	}

	public void run() {
		// Figure out how much time has elapsed since we were last
		// called.  Should be every 25ms, but that is not guaranteed...
		long  time = System.currentTimeMillis();
		long delta = time - lastTime;
		lastTime  = time;
		game.updateEverything(delta);
	}

}
