package com.ipvs.common;

import java.util.Iterator;
import java.util.Vector;

public class XMPPEventHandlerThread implements Runnable {

  String userJID;
  XMPPEventHandlerI handler;
  XMPPI xmpp;
  Thread thread; 
  MRLog  log = null;
  
  public XMPPEventHandlerThread(String userJID, XMPPI xmpp, XMPPEventHandlerI handler,MRLog log) {
    this.userJID = userJID;
    this.xmpp = xmpp;
    this.handler = handler;
    this.thread = null; 
    this.log = log;
  }
  
  public void startListening() {
    thread = new Thread(this);
    thread.start();
  }
  
  public void stopListening() {
    this.thread.interrupt();
  }

/*  private void processEventQueue() {
  }
*/
  public void run() {
    XMPPEventQueue queue = this.xmpp.getXMPPEventQueue(this.userJID);
    log.addInfo("XMPPEventHandlerThread:: started");
    while(true) {
      try {
    	// System.out.println("XMPPEventHandlerThread:: waitForEvents");
        queue.waitForEvents();
        Vector<XMPPEvent> events = queue.flushEvents();

        Iterator<XMPPEvent> i = events.iterator();
        while(i.hasNext()) {
            XMPPEvent event = i.next();
	    // System.out.print("+");
            // System.out.flush();
            handler.handleXMPPEvent(userJID, event);
	    // System.out.print(".");
            // System.out.flush();
        }
        events = null;          
      }
      catch(InterruptedException e) {
        // e.printStackTrace();
        break;
      }
      catch(Exception e) {
        e.printStackTrace();
        break;
      }
    }
    log.addInfo("XMPPEventHandlerThread:: exiting");
    this.xmpp.releaseXMPPEventQueue(queue);
  }
  
}
