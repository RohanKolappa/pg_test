package com.ipvs.xmpp.client.smack;

import org.jivesoftware.smack.packet.Packet;

import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;


public class XMPPRequestHandler {
    protected XMPPI xmpp;
    protected XMPPTransaction t;
    protected int state;
    protected String msgid;
    protected boolean previousCheck = false;
    public boolean getResult(Packet response) {
        boolean result = false;

        if (response.getError() == null) {
            result = true;
        }

        return result;
    }

    public String getId() {
        return this.msgid;
    }

    public void setId(String msgid) {
        this.msgid = msgid;
    }
    public void process(Packet packet){}
  
}
