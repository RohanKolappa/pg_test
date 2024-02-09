package com.ipvs.common.service;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;


public interface MRClient {
    public XMPPI getXmpp() ;
    public MRLog getLog() ;
    public MRDB getDb();
    public MRResourceFactoryI getResourceFactory() ;
    public ServiceAgentFactoryI getServiceAgentFactory();
    public String getResourceGroup();
    public String getUserJID();       
    public void reset();
}
