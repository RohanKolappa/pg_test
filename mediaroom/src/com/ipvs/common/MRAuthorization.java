package com.ipvs.common;

/**
 * This is the interface for the authorization class
 * Each service needs to implement its own authorization class
 * @author miyer
 *
 */
public class MRAuthorization {
	protected String description;
	protected int role;
	protected String token;
	protected String authority;
	protected boolean xAuthRequired;
	protected String xAuthAgentFullJID;
	protected String xAuthToken;
	
	public MRAuthorization() {
		description = MRConst.NULL;
		role = 0;
		token = MRConst.NULL;
		authority = MRConst.NULL;
		xAuthRequired = false;
		xAuthAgentFullJID = MRConst.NULL;
		xAuthToken = MRConst.NULL;
	}
	
	public String getDescription() {
		return description;
	}
	public void setDescription(String description) {
		this.description = description;
	}
	public int getRole() {
		return role;
	}
	public void setRole(int role) {
		this.role = role;
	}
	public String getToken() {
		return token;
	}
	public void setToken(String token) {
		this.token = token;
	}
	public String getAuthority() {
		return authority;
	}
	public void setAuthority(String authority) {
		this.authority = authority;
	}
	public String getXAuthAgentFullJID() {
		return xAuthAgentFullJID;
	}
	public void setXAuthAgentFullJID(String xAuthAgentFullJID) {
		this.xAuthRequired = true;
		this.xAuthAgentFullJID = xAuthAgentFullJID;
	}
	public String getXAuthToken() {
		return xAuthToken;
	}
	public void setXAuthToken(String xAuthToken) {
		this.xAuthRequired = false;
		this.xAuthToken = xAuthToken;
	}
	public boolean getXAuthRequired() {
		return xAuthRequired;
	}
}
