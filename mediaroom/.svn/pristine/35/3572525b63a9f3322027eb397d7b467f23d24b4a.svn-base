package com.ipvs.test;

import com.ipvs.common.factory.MRResourceI;

public class TestResource implements MRResourceI {
	
	private String NID;
	private String name;
	private int resourceType;
	private int streamType;
	private String[] groupList;
	private String userJID;
	private String agentID;
	private String confirmationAgentFullJID;
	
	public String getNID() {
		return NID;
	}
	public void setNID(String nid) {
		NID = nid;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public int getResourceType() {
		return resourceType;
	}
	public void setResourceType(int resourceType) {
		this.resourceType = resourceType;
	}
	public int getStreamType() {
		return streamType;
	}
	public void setStreamTypeID(int streamType) {
		this.streamType = streamType;
	}
	public String[] getGroupList() {
		if(groupList!=null) {
			return groupList.clone();
		}
		else {
			return null;
		}
	}
	public void setGroupList(String[] groupList) {
		if(groupList!=null) {
			this.groupList = groupList.clone();
		}
		else {
			this.groupList = null;
		}
	}
	public String getUserJID() {
		return userJID;
	}
	public void setUserJID(String userJID) {
		this.userJID = userJID;
	}
	public String getAgentID() {
		return agentID;
	}
	public void setAgentID(String agentID) {
		this.agentID = agentID;
	}
	public void setConfirmationAgentFullJID(String confirmationAgentFullJID) {
		this.confirmationAgentFullJID = confirmationAgentFullJID;
	}
	public String getConfirmationAgentFullJID() {
		return confirmationAgentFullJID;
	}
	
}
