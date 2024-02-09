package com.ipvs.mediaroomservice.impl;



import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.xsd.AddMediaRoomConfigRequest;
import com.ipvs.mediaroomservice.xsd.AddMediaSourceRequest;
import com.ipvs.mediaroomservice.xsd.AddStreamRequest;
import com.ipvs.mediaroomservice.xsd.AddViewerRequest;
import com.ipvs.mediaroomservice.xsd.CreateMediaRoomRequest;

public class MediaRoomPEM  {
	
	
	public static final int OWNER_AFFILIATION = 1;
	public static final int MEMBER_AFFILIATION = 2;
	public static final int NONE_AFFILIATION = 3;

	public static final int MODERATOR_ROLE = 1;
	public static final int PARTICIPANT_ROLE = 2;
	public static final int VISITOR_ROLE = 3;
	
	//private XMPPI xmpp;
	private MRLog log;
	//private MRDB schema;
	//private MRResourceFactoryI resourceFactory;
	
	public MediaRoomPEM(XMPPI xmpp, MRLog log, MRDB schema, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) {
		//this.xmpp = xmpp;
		this.log = log;
		//this.schema = schema;
		//this.resourceFactory = resourceFactory;
		//this.serviceAgentFactory = serviceAgentFactory;
	}
	
	/**
	 * Get the room group for the given room
	 * @param roomNID the room NID
	 * @return the room Group
	 *//*
	private String getRoomGroup(String roomNID) {
		// TBD read the room node to find the roomgroup ;
		return "roomGroup";
	}*/
	
	/**
	 * Get the room group for the give room config
	 * @param roomConfigNID the room config NID
	 * @return the room Config
	 */
	private String getRoomConfigGroup(String roomConfigNID) {
		// TBD read the room config node to find the roomgroup ;
		return "roomGroup";
	}
	
	/**
	 * Gets room authorization for an Affiliation to start/stop a room
	 * 
	 * @param request	the Session of the requesting user
	 * @param roomGroup the rooom Group for which room authorization is requested
	 * @param affiliation the affiliation being requested
	 * @return room Authorization
	 */
	private MediaRoomAuthorization getRoomAuthorizationP(MRRequest request, 
			String roomGroup, int affiliation, int authType) throws Exception {
		this.log.addTraceBegin(request);
		
		// TBD Lookup the policy for userGroup + roomGroup + affiliation
		int role = MediaRoomPEM.MODERATOR_ROLE;
		String token = "testRoomToken";
		String authority = "testAuthority";
		String roomPolicyNID = "1";
		String streamPolicyNID = MRConst.NULL;

		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL,
				authType, role, roomPolicyNID, streamPolicyNID,
				token, authority);
		
		this.log.addTraceEnd(request);
		return auth;
	}
	
	/**
	 * Gets room authorization for for adding a room config
	 * 
	 * @param request	the Session of the requesting user
	 * @param rq the add media room config request
	 * @return room Authorization
	 */
	public MediaRoomAuthorization getAuthorization(MRRequest rq, AddMediaRoomConfigRequest message) throws Exception {
		return getRoomAuthorizationP(rq, message.group, MediaRoomPEM.OWNER_AFFILIATION, MediaRoomObject.ROOMCONFIG_TYPEID);
	}

	/**
	 * Gets room authorization for for creating a room
	 * 
	 * @param request	the Session of the requesting user
	 * @param message the create media room request
	 * @return room Authorization
	 */
	public MediaRoomAuthorization getAuthorization(MRRequest rq, CreateMediaRoomRequest message) throws Exception {
		String roomGroup = getRoomConfigGroup(message.roomConfigNID);
		return getRoomAuthorizationP(rq, roomGroup, MediaRoomPEM.OWNER_AFFILIATION, MediaRoomObject.ROOM_TYPEID);
	}

	/**
	 * Gets the media authorization to add media to a room
	 * 
	 * @param request	the request of the requesting user
	 * @param message the add media request
	 * @return media Authorization
	 */
	public MediaRoomAuthorization getAuthorization(MRRequest rq, AddMediaSourceRequest message) {
		this.log.addTraceBegin(rq);

		//String roomGroup = MRConst.NULL;
		String roomNID = MRConst.NULL;
		if(message.getRequestType() == MediaRoomRequestTypes.ADDMEDIACONFIG) {
			//AddMediaSourceConfigRequest cmessage = (AddMediaSourceConfigRequest)message;
			//roomGroup = getRoomConfigGroup(cmessage.roomConfigNID);
		}
		else {
			//roomGroup = getRoomGroup(message.roomNID);
			roomNID = message.roomNID;
		}

		// TBD Lookup the policy for userGroup + roomGroup + affiliation=MEMBER_AFFILIATION
		String token = "testMediaToken";
		String authority = "testAuthority";
		String roomPolicyNID = "1";
		String streamPolicyNID = "1";
		//String authAgentJID = MRConst.NULL;

		MediaRoomAuthorization auth = new MediaRoomAuthorization(roomNID, MRConst.NULL,
				MediaRoomObject.MEDIA_TYPEID, MediaRoomPEM.PARTICIPANT_ROLE,roomPolicyNID, streamPolicyNID,
				token, authority);

		this.log.addTraceEnd(rq);
		return auth;
	}
	
	/**
	 * Gets the viewer authorization to add viewer to the room and connect to media
	 * @param request 	the request of the requesting user
	 * @param message add viewer request
	 * @return viewer Authorization
	 */
	public MediaRoomAuthorization getAuthorization(MRRequest rq, AddViewerRequest message) throws Exception {
		
		this.log.addTraceBegin(rq);

		String roomNID = MRConst.NULL;
		//String roomGroup = MRConst.NULL;
		if(message.getRequestType() == MediaRoomRequestTypes.ADDVIEWERCONFIG) {
			//AddViewerConfigRequest cmessage = (AddViewerConfigRequest)message;
			//roomGroup = getRoomConfigGroup(cmessage.roomConfigNID);
		}
		else {
			//roomGroup = getRoomGroup(message.roomNID);
			roomNID = message.roomNID;
		}

		// TBD Lookup the policy for userGroup + roomGroup + affiliation=NONE_AFFILIATION
		String token = "testViewerToken";
		String authority = "testAuthority";
		String roomPolicyNID = "1";
		String streamPolicyNID = "1";

		MediaRoomAuthorization auth = new MediaRoomAuthorization(roomNID, MRConst.NULL,
				MediaRoomObject.VIEWER_TYPEID, MediaRoomPEM.VISITOR_ROLE, roomPolicyNID, streamPolicyNID,
				token, authority);

		this.log.addTraceEnd(rq);
		return auth;
	}
	
	/**
	 * Gets the stream authorization to connect a viewer to media
	 * @param request 	the request of the requesting user
	 * @param message the update stream request
	 * @return viewer Authorization
	 */
	public MediaRoomAuthorization getAuthorization(MRRequest rq, AddStreamRequest message) throws Exception {
		this.log.addTraceBegin(rq);

		String roomNID = MRConst.NULL;
		//String roomGroup = MRConst.NULL;
		if(message.getRequestType() == MediaRoomRequestTypes.ADDSTREAMCONFIG) {
			//AddStreamConfigRequest cmessage = (AddStreamConfigRequest)message;
			//roomGroup = getRoomConfigGroup(cmessage.roomConfigNID);
		}
		else {
			//roomGroup = getRoomGroup(message.roomNID);
			roomNID = message.roomNID;
		}

		// TBD Lookup the policy , Get confirmation from the authAgentJID and set these values
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testStreamToken";
		String authority = "testAuthority";
		String roomPolicyNID = "1";
		String streamPolicyNID = "1";

		MediaRoomAuthorization auth = new MediaRoomAuthorization(roomNID, MRConst.NULL, 
				MediaRoomObject.STREAM_TYPEID, role,roomPolicyNID, streamPolicyNID,
				token, authority);
		
		this.log.addTraceEnd(rq);
		return auth;
	}
	
	public MediaRoomAuthorization getCommonAuthorization(MRRequest rq) throws Exception {
		this.log.addTraceBegin(rq);
		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;

		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL,
				authType, role, roomPolicyNID, streamPolicyNID,
				token, authority);
		
		this.log.addTraceEnd(rq);
		return auth;
	}
	
	public MRAuthorization authorizeRequest(MRRequest rq, int requestType) throws Exception {
		this.log.addTraceBegin(rq);
		MRAuthorization auth = null;
		/*
		switch(requestType) {
		case MediaRoomRequestTypes.ADDMEDIAROOMCONFIG:
			auth = getAuthorization(rq, (AddMediaRoomConfigRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.ADDMEDIACONFIG:
			auth = getAuthorization(rq, (AddMediaSourceConfigRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.ADDVIEWERCONFIG:
			auth = getAuthorization(rq, (AddViewerConfigRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.ADDSTREAMCONFIG:
			auth = getAuthorization(rq, (AddStreamConfigRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.CREATEMEDIAROOM:
			auth = getAuthorization(rq, (CreateMediaRoomRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.ADDTRACK:
			auth = getCommonAuthorization(rq);
			break;
		case MediaRoomRequestTypes.ADDMEDIA:
			auth = getAuthorization(rq, (AddMediaSourceRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.ADDVIEWER:
			auth = getAuthorization(rq, (AddViewerRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.ADDSTREAM:
			auth = getAuthorization(rq, (AddStreamRequest)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.CREATECONNECTION:
			auth = getCommonAuthorization(rq); // TBD
			break;
		case MediaRoomRequestTypes.UPDATESTREAM:
			// auth = getAuthorization(rq, (UpdateStreamRequestHandler)(rq.getMessage()));
			break;
		case MediaRoomRequestTypes.CONTROLMEDIA:
			auth = getCommonAuthorization(rq);
			break;
		case MediaRoomRequestTypes.DELETEMROBJECT:
			auth = getCommonAuthorization(rq);
			break;
		default:
		}
		*/
		this.log.addTraceEnd(rq);

		return auth;
	}


}
