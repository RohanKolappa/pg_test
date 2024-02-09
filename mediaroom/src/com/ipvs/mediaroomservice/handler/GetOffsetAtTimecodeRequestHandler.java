package com.ipvs.mediaroomservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.handler.base.AsyncBaseRequestHandler;


public class GetOffsetAtTimecodeRequestHandler extends AsyncBaseRequestHandler  {
  
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        String streamNID = null;
        Element data = MediaRoomServiceUtils.cloneDataElement(request);
        streamNID = MRXMLUtils.getValueXpath(data, "streamNID", "");      

        if ((streamNID == null) || (streamNID.trim().length() == 0)) {
            throw new MRException(MediaRoomServiceExceptionCodes.STREAMNIDNOTFOUND, "StreamNID Not Found");
        }

        String roomNID = MRXMLUtils.getValueXpath(data, ".//roomNID", "");
  	  	if("".equals(roomNID))
  	  		roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, streamNID);

        Document streamDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID));
        String sourceStreamAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//sourceAgentJID", "");
        if ((sourceStreamAgentJID == null) || (sourceStreamAgentJID.trim().length() == 0)) {
            throw new MRException(MediaRoomServiceExceptionCodes.SOURCEAGENTNOTFOUND, "Source Agent JID Not Found");
        }
        
        //get the media uuid from media stream file doc
        MediaRoomServiceUtils.setMediaUUIDInDataElement(streamDataDoc, request, data, log, db);

        
        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
        String toAgentJID = sourceStreamAgentJID;
        String toServiceName = MRConst.MEDIA_SERVICE;
        String dataStr = MRXMLUtils.elementToString(data);
        String responseHandlerName = this.getClass().getName();
        AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
        		fromAgentName, toAgentJID, toServiceName, dataStr, responseHandlerName);
    	return null;
    }
}
