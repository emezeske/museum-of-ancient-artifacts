import java.util.TimerTask;
import java.util.Timer;

public class TimerClass extends TimerTask {

	LinkingButton lb;
	Timer timer;
	long lastTime;

	public TimerClass(LinkingButton LB, long interval) {
		lb     = LB;
		lastTime = System.currentTimeMillis();
		timer    = new Timer();
		timer.scheduleAtFixedRate(this, 0L, interval);
	}

	public void run() {
		// Figure out how much time has elapsed since we were last
		// called.  Should be every 25ms, but that is not guaranteed...
		long  time = System.currentTimeMillis();
		long delta = time - lastTime;
		lastTime  = time;
		lb.updateEverything(delta);
	}

}
