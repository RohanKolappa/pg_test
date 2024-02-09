package com.ipvs.mediaroomservice.datamodel;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRNodeI;

/**
 * The Authorization for a stream between media and a viewer
 * @author miyer
 *
 */
public class MediaRoomAuthorization extends MRAuthorization implements MRNodeI {
	public String NID;
	public String roomNID;
	public String description;
	public int authType;
	public String roomPolicyNID;
	public String streamPolicyNID;
	public MediaRoomAuthorization(String roomNID, String description, 
			int authType, int role, String roomPolicyNID, String streamPolicyNID,
			String token, String authority) {
		setDescription(description);
		setRole(role);
		setToken(token);
		setAuthority(authority);
		setXAuthAgentFullJID(MRConst.NULL);
		setXAuthToken(MRConst.NULL);
		setNID(MRConst.NULL);
		this.authType = authType;
		this.roomPolicyNID = roomPolicyNID;
		this.streamPolicyNID = streamPolicyNID;
	}

	public void setNID(String NID) {
		this.NID = NID;
	}
	
	public String getNID() {
		return this.NID;
	}

	public int getType() { return MediaRoomObject.AUTHORIZATION_TYPEID; }
}
