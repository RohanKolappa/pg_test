package com.ipvs.xmpp.client.smack;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.packet.Packet;

import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;


public class XmppRequest extends XMPPRequestHandler {
	protected String description="";
	
    public void init (XMPPI xmpp, String description, XMPPTransaction t) {
        this.xmpp = xmpp;
        this.t = t;
        this.state=0;
        this.description=description;
    }        
    
    public void process(Packet packet){
        switch(state) {
        case 0:
            try{
                state++;
                xmpp.addRequest(getRequest(packet), this);
            }catch(Exception ex){
                t.setException(ex);
                return;
            }
            break;
        case 1:
            state++;
            xmpp.delRequest(this);
            if (!getResult(packet)) {
                try{
                    setResponseError(packet);
                }catch(Exception ex){
                    t.setException(ex);
                    return;
                }                    
                break; 
            }
            try{
                setResponse(packet);
            }catch(Exception ex){
                t.setException(ex);
                return;
            }            
            break;
        }
    }
    
    public void setResponseError(Packet packet) throws Exception{
        t.setException(new XMPPException(packet.getError() + " "+  " description : '" + description +"'"));
    }    
    
    public void setResponse(Packet packet) throws Exception{
        t.setResponse(packet);
    }
    
    public Packet getRequest(Packet packet) throws Exception{
        return packet;
    }
    
}
