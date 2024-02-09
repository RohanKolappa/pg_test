package com.ipvs.proxy.app;

import java.util.TimerTask;


public class IPVSTimer implements Runnable{

	
	int frequency = 0;
	TimerTask timerTask  = null;
	boolean stop = false;
	
	public void schedule(TimerTask timerTask, int frequency) {
		this.frequency = frequency;
		this.timerTask = timerTask;
		stop = false;
		Thread t = new Thread (this);
        t.start();
	}

	@Override
	public void run() {
		while(!isStop()) {
			try {
				timerTask.run();
				Thread.sleep(frequency);
			}catch(Exception e ) {
				e.printStackTrace();
				break;
			}
		}	
	}
	
	public void cancel (){
		setStop(true);
	}

	public synchronized boolean isStop() {
		return stop;
	}

	public synchronized void setStop(boolean stop) {
		this.stop = stop;
	}
	
}
