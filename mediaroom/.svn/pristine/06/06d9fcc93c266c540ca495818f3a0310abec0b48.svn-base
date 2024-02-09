package com.ipvs.mediaroomservice.handler;

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
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;

public class GetMRSourceGroupListRequestHandler implements MRRequestHandlerI {
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addTraceBegin(request);
		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL,
					authType, role, roomPolicyNID, streamPolicyNID,
					token, authority);
		log.addTraceEnd(request);
		return auth;
	}


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		// Check the data
    Element data = MediaRoomServiceUtils.cloneDataElement(request);
    String destNID = null;
		if(data.element("destNID") != null)
		  destNID = MRXMLUtils.getValue(data,"destNID","");

		//String[] destGroupList;
		if( (destNID != null) && (!"".equals(destNID.trim()))){
		  //MRResourceI destPort = resourceFactory.getResource(destNID);
		  //destGroupList = destPort.getGroupList();
		}
		else {
	    //String destGroup = request.getResourceGroup();
	    //destGroupList = new String[1];
	    //destGroupList[0] = destGroup;
		}
		
		// PolicyManager.getSourceGroupList(userGroup, destGroupList);
		// MRXMLDBUtils.validateElement(request, log, db, roomNID, "mediaroomservice"); // TBR

		String xml = "<GroupList>"
		  + "<Tag Name='location' Value='sunnyvale'/>" 
      + "<Tag Name='location' Value='stanford'/>" 
			+	"</GroupList>";
		return request.createResponseData( 
		    MRXMLUtils.stringToDocument(xml).getRootElement());

	}

}
