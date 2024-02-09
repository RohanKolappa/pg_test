package com.ipvs.proxy.handler;

import java.net.URLEncoder;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.XAuthServerAgent;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.proxy.impl.ProxyClientAppUtils;
import com.ipvs.proxy.impl.ProxyConstants;


public class GetStreamListRequestHandler implements MRRequestHandlerI {
    
	protected XAuthServerAgent xAuthAgent;
	
	public GetStreamListRequestHandler(XAuthServerAgent xAuthAgent) {
		this.xAuthAgent = xAuthAgent; 
	}

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.STREAM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }
    
   
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)  throws Exception {

    	StringBuffer strObject = new StringBuffer();
    	
    	
    	
    		List<Element> streamList = xAuthAgent.getXAuthEntryList(null);
    		for(int i=0;i<streamList.size();i++) {
    			Document streamDoc = MRXMLUtils.elementToDocument(streamList.get(i));
    			if(streamDoc == null) {
    				continue;
    			}
    			int state = 0;
    			try {
    				state = Integer.valueOf(MRXMLUtils.getValueXpath(streamDoc, "//state", ""));
    			}catch(Exception e) {
    				e.printStackTrace();
    			}
    			String proxyStream = null;
    			if(state == MediaRoomServiceConst.STREAM_STARTED) {
    				proxyStream = ProxyClientAppUtils.generateProxyStream(streamDoc, null, ProxyConstants.STATE_CONNECTED, log, db);
    			} else {
    				proxyStream = ProxyClientAppUtils.generateProxyStream(streamDoc, null,ProxyConstants.STATE_DISCONNECTED, log, db);
    			}
    			strObject.append(proxyStream);
    			strObject.append("&" + ProxyConstants.SEPERATOR);
    		}
    		
    	String response = "send" + ProxyConstants.SEPERATOR + "getstreamlistresponse" + ProxyConstants.SEPERATOR + strObject.toString();
    	response = URLEncoder.encode(response, "UTF-8");
    	return MRXMLUtils.stringToDocument("<Response>" +  "<![CDATA[" + response + "]]>" + "</Response>");
    	
    } 
    
    
}



