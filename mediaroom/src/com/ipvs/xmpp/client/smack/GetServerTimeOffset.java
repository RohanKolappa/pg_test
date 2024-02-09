 package com.ipvs.xmpp.client.smack;

import java.text.ParseException;
import java.text.SimpleDateFormat;

import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.packet.Packet;

import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.xmpp.client.params.TimeParams;
import com.ipvs.xmpp.client.smack.packets.IpvsIQ;


public class GetServerTimeOffset extends XmppRequest {
    private String myjid;

    public Packet getRequest(Packet packet) {
        if(packet !=null){
            return packet;
        }
        IpvsIQ iq = new IpvsIQ();
        iq.setFrom(this.myjid);
        TimeParams tmParams = new TimeParams();
        iq.setParams(tmParams);
        return iq;
    }    
    
    public void setResponseError(Packet packet) throws Exception{
        t.setException(new XMPPException("GetServerTimeOffset " + packet.getError() ));
    }  
    
    public void init(XMPPI xmpp, XMPPTransaction t, String myjid) {
        this.myjid = myjid;
        this.xmpp = xmpp;
        this.t = t;
        this.state = 0;
    }
    // Relies on the server sending the UTC in the following date format
    // "yyyyMMdd'T'HH:mm:ss.SSS" or
    // "yyyyMMdd'T'HH:mm:ss"
    public void setResponse(Packet packet) {
		// TODO Auto-generated method stub
    	//Time t = (Time)packet;
        TimeParams t = new TimeParams();
        t.parseXML(packet.toXML());
    	long ms = 0;
    	SimpleDateFormat utcFormat=
    		new SimpleDateFormat("yyyyMMdd'T'HH:mm:ss.SSSz");
    	try {
			ms = utcFormat.parse (t.getUtc() + "GMT").getTime();
		} catch (ParseException e) {
			// TODO Auto-generated catch block
		}
		if (ms == 0) {
			utcFormat = new SimpleDateFormat("yyyyMMdd'T'HH:mm:ssz");
	    	try {
				ms = utcFormat.parse (t.getUtc() + "GMT").getTime();
			} catch (ParseException e) {
				// TODO Auto-generated catch block
			}
		}
		// Account for the timezone
        ms = ms - System.currentTimeMillis();
		this.t.setResponse( String.valueOf(ms) );
	}


}
