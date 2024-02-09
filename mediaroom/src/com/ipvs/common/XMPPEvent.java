package com.ipvs.common;

public class XMPPEvent {

	String fromJID;
	String message;

	public XMPPEvent(String message) {
		this.message = message;
		this.fromJID = null;
	}

	public XMPPEvent(String userJID, String message) {
		this.message = message;
		this.fromJID = userJID;
	}

	public String getMessage() {
		return this.message;
	}
	public String getFrom() {
		return this.fromJID;
	}

}
