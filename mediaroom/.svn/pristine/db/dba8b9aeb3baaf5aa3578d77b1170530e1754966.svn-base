package com.ipvs.assetadminservice.dir.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
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
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.Utils;

public class PublishDirSizeRequestHandler implements MRRequestHandlerI {
	@Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
            MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);
        return auth;
    }

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Element dirSizeEl =MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//PublishDirSize");
		updateDirSize(dirSizeEl, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				
		return request.createResponseData();
	}
	
	
	public void updateDirSize(Element requestEl,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		String dirNID = MRXMLUtils.getAttributeValue(requestEl,SessionServiceResourceConst.BY_NID_ATTRIBUTE,"");
		String strSizeUsed = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.SIZE_USED, "");
		if(dirNID.isEmpty() || strSizeUsed.isEmpty()){
			throw new MRException(AMExceptionCodes.INVALID_DATA, "Invalid Input");
		}
		long sizeUsed = Utils.getLongValue(strSizeUsed,0);
		MediaStoreServiceUtils.updateMediaStreamDirSizeUsed(dirNID, sizeUsed, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
}
