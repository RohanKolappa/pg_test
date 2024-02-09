package com.ipvsserv.server;

import java.util.Map;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public interface IpvsServModule {
    String getName() throws Exception;
    
    void addProcessingDoneEventListener(ProcessingDoneEventListener listner);
    
    void removeProcessingDoneEventListener(ProcessingDoneEventListener listner);
    
    void initialize(Map<String, String> params) throws Exception;
    
    void start() throws Exception;
    
    void stop() throws Exception;
    
    void destroy() throws Exception;
    
    boolean isInitialized();
    
    boolean isStarted();
    
    void setStarted(boolean started);
    
    int getLastKnownState() throws Exception;
    
    void verifyState() throws Exception;
    
    String getIpvsModuleName();
    
    void setIpvsModuleName(String ipvsModuleName);
    
    int getModulePriority();
    
    void setModulePriority(int modulePriority);
    
    boolean isModuleEnabled();
    
    void setModuleEnabled(boolean moduleEnabled);
    
    void setCompletBeforeStartNext(boolean completBeforeStartNext);
    
    String getRootIpvsModuleName();
    void setRootIpvsModuleName(String ipvsRootModuleName);
    void setParent(IpvsServModule parent);
    IpvsServModule getParent();
    
}
