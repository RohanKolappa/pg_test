package com.ipvs.xmpp.client;

import java.util.Iterator;
import java.util.Vector;

import com.ipvs.common.XMPPEvent;
import com.ipvs.common.XMPPEventQueue;

public class XMPPEventQueueHandler {
    protected Vector<XMPPEventQueue> eventQueueList = new Vector<XMPPEventQueue>();
    public synchronized XMPPEventQueue getXMPPEventQueue(String userJID) {
        XMPPEventQueue queue = new XMPPEventQueue(userJID);
        this.eventQueueList.add(queue);
        return queue;
    }

    public synchronized void releaseXMPPEventQueue(XMPPEventQueue queue) {
        this.eventQueueList.remove(queue);
    }
    
    public synchronized void addEvent(String fromJID, String message) {
        Iterator<XMPPEventQueue> i = this.eventQueueList.iterator();
        while (i.hasNext()) {
            i.next().addXMPPEvent(new XMPPEvent(fromJID, message));
        }       
    }

    public void handleEvent(String fromJID, String message) {
        // TBD : Propogate this event only if it is a PubSub Event
        // System.out.println("XMPPI::handleEvent: adding XMPPEvent");
        // bGotNewHandleEvent=true;
        try {
            this.addEvent(fromJID, message);
        }catch(Exception e){
            e.printStackTrace();
        }
    }
}
