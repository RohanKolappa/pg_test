package com.ipvs.xmpp.client.smack.packets;

import org.jivesoftware.smack.packet.IQ;

import com.ipvs.xmpp.client.params.DBParams;
import com.ipvs.xmpp.client.params.XmppParams;
public class IpvsIQ  extends IQ {
	protected String parseSubTree;
	private XmppParams dbParams=null;
	public static IpvsIQ getIpvsIQ(DBParams dbParams) throws Exception {
        IpvsIQ xmppPacket = new IpvsIQ();
        xmppPacket.setParams(dbParams);
        xmppPacket.setTo(dbParams.getToJID());
        xmppPacket.setFrom(dbParams.getFromJID());
        xmppPacket.setType(Type.GET);
        
        return xmppPacket;
    }
	public XmppParams getParams() {
        return dbParams;
    }
    public void setParams(XmppParams dbParams) {
        this.dbParams = dbParams;
    }
	public void setContentStr(String parseSubTree) {
		this.parseSubTree= parseSubTree;
	}
	public String getContentStr() {
		return parseSubTree;
	}
	public String getChildElementXML() {
		if(getParams() !=null)
			return getParams().toXML();
		return "";
 	}
}
 
  
	 
 
  
