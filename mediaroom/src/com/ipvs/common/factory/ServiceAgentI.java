package com.ipvs.common.factory;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;


public interface ServiceAgentI {
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,String userJID,Document setServiceDomainResponseDoc) throws Exception;
    public void reset() throws Exception;
    public boolean handleEvent(MRClient client, Document doc, Element event) throws Exception;
    public String handleMessage(MRClient client, String fromJID, String message) throws Exception;
}
