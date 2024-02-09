package com.ipvs.systemservice.handler;

import org.dom4j.Document;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.DeviceAdminClientAgent;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;

public class DeviceStatusRequestHandler implements MRRequestHandlerI {

  public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
      MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
      ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    log.addTraceBegin(request);
    int authType = 1;
    int role = 2;
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

  public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
      MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
      throws Exception {
    
    String agentName = ServiceAgentUtils.getAgentName(
        MRServiceConst.DEVICEADMINCLIENTAGENT, request.getUserJID());
    ServiceAgentI serviceAgent = serviceAgentFactory.getServiceAgent(agentName);
    DeviceAdminClientAgent agent = (DeviceAdminClientAgent)serviceAgent;    
    String user = "";    
    if(agent != null)
      user = agent.doDeviceAdminRequest(request,xmpp, log, db, resourceFactory, serviceAgentFactory);
    return request.createResponseData("user", user);

  }
  
  


}
