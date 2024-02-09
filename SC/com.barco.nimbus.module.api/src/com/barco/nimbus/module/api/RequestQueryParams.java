package com.barco.nimbus.module.api;

public class RequestQueryParams {

	public String byTitle;
	public String byId;
	public String userJID;
	public RequestQueryParams(String byTitle,String byId,String userJID) {
		this.byTitle = byTitle;
		this.byId = byId;
		this.userJID = userJID;
	}
	
}
