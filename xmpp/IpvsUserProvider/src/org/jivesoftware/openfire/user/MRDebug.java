package org.jivesoftware.openfire.user;

import java.io.PrintWriter;
import java.io.StringWriter;

import org.jivesoftware.util.Log;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class MRDebug {
    private static long lastTime = System.currentTimeMillis();

    public static synchronized void log(String msg) {
        log("", msg);
    }

    public static synchronized void log() {
        lastTime = System.currentTimeMillis();
    }

    public static synchronized void log(String prefix, String msg) {
        //System.out.println(prefix + lastTime+  " " +(System.currentTimeMillis()- lastTime) +" " + msg);
        try {
            Log.debug(prefix + lastTime + " " + (System.currentTimeMillis() - lastTime) + " " + msg);
        } catch (Exception ex) {
        }
        lastTime = System.currentTimeMillis();
    }
    public static synchronized void log(String prefix, Exception  ex) {
        //System.out.println(prefix + lastTime+  " " +(System.currentTimeMillis()- lastTime) +" " + msg);
        try {
            StringWriter sw = new StringWriter();
            if(ex !=null){
                ex.printStackTrace(new PrintWriter(sw));
            }
            Log.debug(prefix + lastTime + " " + (System.currentTimeMillis() - lastTime) + " " + sw.toString());
        } catch (Exception ex2) {
        }
        lastTime = System.currentTimeMillis();
    }
}
