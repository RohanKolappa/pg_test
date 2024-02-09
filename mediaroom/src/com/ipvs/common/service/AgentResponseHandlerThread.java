package com.ipvs.common.service;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;


public class AgentResponseHandlerThread implements Runnable {
    private XMPPI xmpp;
    private MRLog log;
    private MRDB db;
    private MRResponseHandlerI defaultResponseHandler;
    private MRResourceFactoryI resourceFactory;
    private ServiceAgentFactoryI serviceAgentFactory;
    private String serviceName = null;
    private String responseName = null;
    private AgentResponseHandlerI responseHandler = null;
    private MRRequest request = null;

    public String getClassPath(String responseName, String serviceName) {
        return "com.ipvs." + serviceName + ".handler." + responseName + "Handler";
    }

    public void init(String responseName, String serviceName, MRRequest request, MRResponseHandlerI defaultResponseHandler, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory, String responseHandlerName) throws Exception {
        this.log = log;
        this.db = db;
        this.xmpp = xmpp;
        this.resourceFactory = resourceFactory;
        this.serviceAgentFactory = serviceAgentFactory;
        this.serviceName = serviceName;
        this.defaultResponseHandler = defaultResponseHandler;
        this.responseName = responseName;
        this.request = request;
        if("".equals(responseHandlerName))
        	responseHandlerName = getClassPath(responseName, serviceName);

        try {
            this.responseHandler = (AgentResponseHandlerI) Class.forName(responseHandlerName).newInstance();
        } catch (Exception exp) {
            //System.out.println("Exception in AgentResponseHandlerThread " + exp.toString());
            this.responseHandler = null;
        }
    }

    public void run() {
        try {
            if (this.responseHandler == null) {
                return;
            }

            log.addInfo("AgentResponseHandlerThread: Handle Agent Response ServiceName :=" + serviceName + " ResponseName:=" + responseName, MRLog.INFO, MRLog.NOTIFY);
            responseHandler.handleAgentResponse(defaultResponseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        } catch (Exception exp) {
            exp.printStackTrace();
        }
    }
}
