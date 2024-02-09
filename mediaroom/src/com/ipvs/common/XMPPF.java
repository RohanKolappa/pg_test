package com.ipvs.common;

import com.ipvs.common.factory.ServiceAgentFactoryI;

public interface XMPPF {
	XMPPI xmpp(String userJID) throws Exception;
	ServiceAgentFactoryI sf(String userJID) throws Exception;
}
