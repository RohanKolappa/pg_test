package com.ipvs.proxy.app;

import org.dom4j.Document;

public interface ProxyClientListenerI {

	public void handleProxyClientRequest(Document request);
	public boolean loginProxyUser();
	public void handleProxyDisconnected();
	public void handleXAuthResponse(Document response);
}
