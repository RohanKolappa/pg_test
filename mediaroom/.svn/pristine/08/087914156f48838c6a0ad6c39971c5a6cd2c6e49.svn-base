package com.ipvs.app;

import java.applet.Applet;
import java.security.AccessController;
import java.security.PrivilegedActionException;
import java.security.PrivilegedExceptionAction;
import java.util.Vector;

import netscape.javascript.JSException;
import netscape.javascript.JSObject;


class AppletLog {
	static final int ERROR = 0;
	static final int OK = 200;
	static final void println(String s) {
		System.out.println(s);
	}
}
class Notification {
	String methodName;
	String message;
	Notification(String methodName, String message) {
		this.methodName = methodName;
		this.message = message;
	}
}

class JSNotifier extends Thread {

	Vector queue;
	JSObject win;
	Applet applet;
	
	JSNotifier(Applet applet) {
		this.applet = applet;
		this.win = null;
		this.queue = new Vector();
	}


	synchronized void send(String methodName, String message) {
		Notification n = new Notification(methodName, message);
		this.queue.add(n);
		synchronized(this){
			this.notify();
		}
        AppletLog.println("JSNotifier:send queue size=" + this.queue.size());
	}
	
	synchronized Notification getNext() {
		if(queue.isEmpty())
			return null;
		Notification notification = (Notification) queue.elementAt(0);
		queue.remove(0);
		return notification;
	}

	void flushNotifications() throws Exception {
		while(true) {
			Notification notification = this.getNext();
			if(notification == null)
				return;
			String[] args = new String[1];
	        args[0] = notification.message;
	        long now = System.currentTimeMillis();
	        this.getWindow();
	        this.win.call(notification.methodName, args);
	        long duration = System.currentTimeMillis() - now;
	        AppletLog.println("JSNotifier:flushNotifications Notified JS duration=" + duration  + " message=" + notification.message);
		}
	}
	
	synchronized void getWindow() {
		if(this.win == null)
			this.win = JSObject.getWindow(this.applet);
	}
	
	synchronized void resetWindow() {
		this.win = null;
	}
	
	public void run() {
		while(true) {
			AppletLog.println("JSNotifier:run waiting ...");
			try {
				this.flushNotifications();
				synchronized(this){
					this.wait();
				}
			}
			catch(InterruptedException e) {
				AppletLog.println("JSNotifier:run interrupted exiting ...");
				e.printStackTrace();
				break;
			}
			catch(JSException e) {
				AppletLog.println("JSNotifier:run jsexception ignoring ...");
				e.printStackTrace();
				// break;
			}
			catch(Exception e) {
				AppletLog.println("JSNotifier:run exception exiting ...");
				e.printStackTrace();
				break;
			}
		}
	}
}

/////////////////////////// wrap around AccessController.doPrivileged call ///////////////////
class WrapperJSMethod implements PrivilegedExceptionAction{
    private String methodName=null;
    private String methodArg1=null;
    private String methodArg2=null;
    private BaseApplet parent=null;
    public WrapperJSMethod(BaseApplet parent, String methodName, String methodArg1, String methodArg2){
        this.parent=parent;
        this.methodName=methodName;
        this.methodArg1=methodArg1;
        this.methodArg2=methodArg2;
    }
        
    public Object run() throws Exception {
        //AppletLog.println("WrapperJSMethod method:" + methodName);
    	return this.parent.handleJSCall(methodName, methodArg1, methodArg2);
        
    }
}   
public abstract class BaseApplet extends Applet {
		
	private static final long serialVersionUID = -5096454920320489249L;	
    String callback = "handleMessage";
    String initCallback = "handleInit";
    String version = null;
            
	JSNotifier jsNotifier = null;
	
	
	public void setCallback(String callback) {
        this.callback = callback;
    }
    
	public String getVersion() {
        return version;
    }

	protected void callJSMethod(String methodName, String methodArgs) throws Exception {
        AppletLog.println("Calling js method begin:" + methodName);
        if (this.jsNotifier == null) {
            this.jsNotifier = new JSNotifier(this);
            this.jsNotifier.start();
        }
        this.jsNotifier.send(methodName, methodArgs);
    }

	public void init() {
        try {
            AccessController.doPrivileged(new WrapperJSMethod( this, "init", null, null)) ;
        } catch (PrivilegedActionException e) {
            e.printStackTrace();
        }   
    }
    public String login(String request) {
        try {
            return (String) AccessController.doPrivileged(new WrapperJSMethod( this, "login", request, null)) ;
        } catch (PrivilegedActionException e) {
            e.printStackTrace();
            return e.getMessage();
        }
    }
    public String setServiceDomain(String userJID, String request) {    	    	
        try {
            return (String) AccessController.doPrivileged(new WrapperJSMethod( this, "setServiceDomain", userJID, request)) ;
        } catch (PrivilegedActionException e) {
            e.printStackTrace();
            return e.getMessage();
        }
    }
    public String logout(String userJID, String message) {    
        try {
            return (String) AccessController.doPrivileged(new WrapperJSMethod( this, "logout", userJID, message)) ;
        } catch (PrivilegedActionException e) {
            e.printStackTrace();
            return e.getMessage();
        }
    }
    public  String sendRequest(String userJID, String message) {
        AppletLog.println("BaseApplet:sendRequest message=" + message);        
        try {
            return (String) AccessController.doPrivileged(new WrapperJSMethod( this, "sendRequest", userJID, message)) ;
        } catch (PrivilegedActionException e) {
            e.printStackTrace();
            return e.getMessage();
        }
    }       
    
    public void destroy() {
    	AppletLog.println("BaseApplet:Destroy Method");  
    	killAllThreads();
    	super.destroy();
    }
    
    public void stop() {
    	AppletLog.println("BaseApplet:Stop Method");
    	killAllThreads();    	
    	super.stop();
    }
    
    protected abstract void killAllThreads(); 
    
    protected abstract String handleJSCall(String methodName, String arg1, String agr2);

}
