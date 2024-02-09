package com.ipvs.xmpp.client.smack.packets;

import org.dom4j.Element;
import org.jivesoftware.smack.packet.IQ;


public class ResponseElement extends IQ {
    public static final String IPVSDBSERVICE ="ipvs:mediaroom:ipvsdbservice";
    
    private Element responseEL;
    public ResponseElement(){
        super();
    }
    public String getChildElementXML() {
        //never called 
        return "<response xmlns=\"" + ResponseElement.IPVSDBSERVICE + "\">" + getResponseXML()+"</response>";
    }

    public Element getResponseEL() {
        return responseEL;
    }

    public void setResponseEL(Element presponseEL) {
        this.responseEL = presponseEL;
    }

    public Element getDBRespEL() throws Exception {
      if(responseEL==null){
          return null;
      }
      return (Element)responseEL.selectSingleNode("//DBResp");
    } 
    
    private String getResponseXML() {
        if(responseEL!=null){
            return responseEL.asXML();
        }
        return "";
    }
}
