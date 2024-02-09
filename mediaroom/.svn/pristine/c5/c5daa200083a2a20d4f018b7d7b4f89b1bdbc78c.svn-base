package com.ipvs.xmpp.client.smack;
import java.util.ArrayList;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.packet.Packet;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.config.IPVSConst;
import com.ipvs.xmpp.client.params.DBParams;
import com.ipvs.xmpp.client.smack.packets.IpvsIQ;
import com.ipvs.xmpp.client.smack.packets.ResponseElement;


public class DBRequest extends XmppRequest {
    protected DBParams dbParams=null;
    
    public void init (XMPPI xmpp, DBParams dbParams , XMPPTransaction t) {
        this.xmpp = xmpp;
        this.dbParams=dbParams;
        this.t = t;
        this.state=0;
    }        

    public Packet getRequest(Packet packet) throws Exception{
        if(packet !=null) {
            return packet;
        } else {
            return IpvsIQ.getIpvsIQ(dbParams);
        }
    }  
    
    public void setResponseError(Packet packet) throws Exception{
        if(!isRetryNeeded(packet)) {
            t.setException(new XMPPException(dbParams.getAction() + " - " + packet.getError() + " "+  " Node : '" + dbParams.getNodeID() +"'"));
        }else{
            state=1;
            IpvsIQ xmppPacket;
            try {
                //todo:if original packet was not created here (getRequest), than preserve the packet... for now its always created here (getRequest).
                xmppPacket = IpvsIQ.getIpvsIQ(dbParams);
                xmpp.addRequest(xmppPacket, this);
            } catch (Exception e) {
                t.setException(e);
            }
        }
    }    
    
    public boolean isRetryNeeded(Packet packet){
        //int code= packet.getError().getCode();
        String msg= packet.getError().getMessage();
        String condition= packet.getError().getCondition();
        if("ipvs-max-request-limit-reached".equals(condition)){ //"ipvs-db-connection-timed-out".equals(condition)){
            MRLog.println(" ***************** Retry DB request after "+IPVSConst.getInstance().getRetryIntervalForDBMaxReqLimitError() +" ms : '"+condition+"'  Error '" + msg+"'");
            try {
                //wait for some time before proceed
                Thread.sleep(IPVSConst.getInstance().getRetryIntervalForDBMaxReqLimitError());
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            MRLog.println(" ***************** Retring DB request after "+IPVSConst.getInstance().getRetryIntervalForDBMaxReqLimitError() +" ms : '"+condition+"'  Error '" + msg+"'" +" " + packet.toXML());
            return true;
        }else{
            //MRLog.println(" ***************** FAILED DB request : "+condition+"  Error " + msg);
            return false;
        }
    }           
         
    public void setResponse(Packet packet) throws Exception{
        if(dbParams.getAction().equals(DBParams.GETCHILDREN)   ){
            ArrayList<String> children = new ArrayList<String>();
            String responseStr = processDiscoverItems(packet);
            StringTokenizer st = new StringTokenizer(responseStr);
            while (st.hasMoreTokens()) {
                String child = st.nextToken();
                children.add(child);
            }
            t.setResponse(children);
        }else if(dbParams.getAction().equals(DBParams.READ_LIST ) || dbParams.getAction().equals(DBParams.READ_SINGLE )){
            t.setResponse(processItemDataEL(packet));
        }else if(dbParams.getAction().equals(DBParams.READ_QUERY) ){
            t.setResponse(processItemDataEL(packet));
        }else if(dbParams.getAction().equals(DBParams.DELETE) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.ADD) ){
            t.setResponse(processItemData(packet));                
        }else if(dbParams.getAction().equals(DBParams.REPLACE) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.EXPORT_DB) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.IMPORT_DB) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.DELETE_DB) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.GET_SUBSCRIBERS) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.GET_SUBSCRIPTIONS) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.SUBSCRIBE) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.UNSUBSCRIBE) ){
            t.setResponse(processItemData(packet));
        }else if(dbParams.getAction().equals(DBParams.UPDATESUBSCRIPTIONS) ){
            t.setResponse(processItemData(packet));
        } else {
            t.setResponse(dbParams.getNodeID());
        }
    }
    
    private String processDiscoverItems(Packet packet) {
        ResponseElement pubsub = (ResponseElement) packet;
        Element el;
        try {
            el = pubsub.getDBRespEL();
            if(el !=null){
                return el.asXML();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }
    
    private Document processItemData(Packet packet) {
        ResponseElement pubsub = (ResponseElement) packet;
        try {
            Element el = pubsub.getDBRespEL();
            if(el !=null){
                Document doc = DocumentHelper.createDocument();
                doc.add(el.detach());
                return doc;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }    
    
    private Element processItemDataEL(Packet packet) {
        ResponseElement pubsub = (ResponseElement) packet;
        try {
            return pubsub.getDBRespEL();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }    
    
}
