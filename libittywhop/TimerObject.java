import java.util.TimerTask;
import java.util.Timer;

public class TimerObject extends TimerTask {

	private GameObject gameObject;
	private Timer      timer;
	private long       lastTime;

	public TimerObject(GameObject GO, long interval) {
		gameObject     = GO;
		lastTime = System.currentTimeMillis();
		timer    = new Timer();
		timer.scheduleAtFixedRate(this, 0L, interval);
	} // end TimerClass()

	public void run() {
		// Figure out how much time has elapsed since we were last
		// called.  Should be every 25ms, but that is not guaranteed...
		long  time = System.currentTimeMillis();
		long delta = time - lastTime;
		lastTime  = time;
		gameObject.updateEverything(delta);
	} // end run()

} // end class TimerObject
