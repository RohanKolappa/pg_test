package com.ipvs.common;

import java.util.Vector;

import com.ipvs.client.ServiceAgentUtils;

public class XMPPEventQueue  {
  
  Vector<XMPPEvent> events;
  //String userJID;
  
  public XMPPEventQueue(String userJID) {
	//this.userJID = userJID;
    events = new Vector<XMPPEvent>();
  }
  
  public synchronized void waitForEvents() throws Exception {
      while (events.isEmpty()) {
          wait();
      }
  }
  
  public synchronized void addXMPPEvent(XMPPEvent event) {
   
	String message = event.getMessage();
	if(ServiceAgentUtils.isShutdownEvent(message)) {
		this.flushEvents();
	}
    this.events.add(event); 
    notify();   
  }
  
  public synchronized Vector<XMPPEvent> flushEvents() {
    Vector<XMPPEvent> result = this.events;
    this.events = new Vector<XMPPEvent>();
    return result;
  }
}
