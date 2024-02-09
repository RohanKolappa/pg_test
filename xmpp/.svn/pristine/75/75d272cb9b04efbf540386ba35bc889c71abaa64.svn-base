package com.ipvsserv.common.config.watch;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public abstract class IntervalThread implements Runnable {
    
    private boolean active = false;
    
    private int interval = -1;
    
    private String name;
    
    private Thread runner;
    
    public IntervalThread(int intervalSeconds, String name) {
        
        this.interval = intervalSeconds * 1000;
        this.name = name;
    }
    
    public void start() {
        
        active = true;
        
        //If we don't have a thread yet, make one and start it.
        if (runner == null && interval > 0) {
            runner = new Thread(this);
            runner.start();
        }
    }
    
    public void stop() {
        active = false;
    }
    
    public void run() {
        
        //Make this a relatively low level thread
        Thread.currentThread().setPriority(Thread.MIN_PRIORITY);
        
        //Pause this thread for the amount of the interval
        while (active) {
            try {
                doInterval();
                Thread.sleep(interval);
                
            } catch (InterruptedException ex) {
                //Ignore
            }
        }
    }
    
    @Override
    public String toString() {
        return name;
    }
    
    protected abstract void doInterval();
}
