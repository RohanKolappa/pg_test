package com.ipvsserv.nxd.bulkupload;

import java.io.File;
import java.util.Map;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.api.xmldb.sedna.SednaXmlDBModule;
import com.ipvsserv.nxd.bulkupload.template.TemplateDefination;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

public class BulkUploadModule extends IpvsServBasicModule {
    protected String taskName = "TestModule";
    protected String templateDefinationXmlPath;
    protected String bulkUploadXmlPath = "bulkupload.xml";
    protected TemplateDefination templateDefination = null;
    
    private long startDate;
    private SednaXmlDBModule xmlDBModule = null;
    public BulkUploadModule() {
        super("BulkUploadModule");
    }
    
    public BulkUploadModule(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            xmlDBModule = (SednaXmlDBModule) IpvsServer.getInstance().getModule(SednaXmlDBModule.class);
            if (xmlDBModule == null) {
                throw new Exception("Database could not connect.");
            }
            
            templateDefinationXmlPath = params.get("templateDefinationXmlPath");
            //testDBXmlPath = IpvsServGlobals.getProcessArguments().getArgumentParam("testdbxml", params.get("testDBXmlPath"));
            bulkUploadXmlPath = IpvsServGlobals.getProcessArguments().getArgumentParam("bulkuploadxml", params.get("bulkUploadXmlPath"));
            
            try {
                bulkUploadXmlPath = IpvsServGlobals.verifyAndGetTestXmlPath(bulkUploadXmlPath);
            } catch (Exception ex) {}
             
            
            templateDefinationXmlPath = IpvsServGlobals.verifyAndGetFilePath(templateDefinationXmlPath);
            
            startDate = System.currentTimeMillis();
            ////////////////////////////////////
             
            initialized = true;
        } catch (Exception ex) {
            MRLog.error(" " + bulkUploadXmlPath + " app: " + IpvsServGlobals.getIpvsAppName());
            ex.printStackTrace();
        }
    }
    
    @Override
    public void start() throws Exception {
        try {
            MRLog.debug(name + " start  ");
            //wait until the sendna connects 
            try {
                while (!xmlDBModule.isStarted()) {
                    Thread.sleep(1 * 1000);
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            } finally {
            }
            
            templateDefination = new TemplateDefination();
            File file = new File(templateDefinationXmlPath);
            if (!file.exists()) {
                try {
                    templateDefinationXmlPath = IpvsServGlobals.verifyAndGetTestXmlPath(templateDefinationXmlPath);
                } catch (Exception ex) {}
            }
            templateDefination.setTemplateDefinationXmlPath(templateDefinationXmlPath);
            templateDefination.loadXML();
            MRLog.debug(name + " templateDefination loaded  " + templateDefinationXmlPath);
            
            MRLog.debug(name + " BulkUploadService starting   ");
            BulkUploadService importDocuments = new BulkUploadService();
            importDocuments.createXmls(bulkUploadXmlPath, templateDefination, xmlDBModule);
            MRLog.debug(name + " BulkUploadService done   started at " + startDate);
            
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            //Thread.sleep(3000);
            String appname = IpvsServGlobals.getIpvsAppName();
            if (appname != null && appname.equals(IpvsServGlobals.BULK_UPLOAD )) {
                IpvsServer.getInstance().stop();
            }
        }
    }
    
    @Override
    public void destroy() throws Exception {
        //stop if the app is started as taskName = SednaNativeDBTest /exist test / back up /bulk load only
        try {
            if (!IpvsServGlobals.isIpvsComponetApp()) {
                IpvsServer.getInstance().stop();
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    ///////////////
    
    /////////////////////////loadModule/////////////////////////
    
    ////////////////////////////
    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }
    
    @Override
    public void verifyState() throws Exception {

    }
    
    //////////////////////////
    public String getTaskName() {
        return taskName;
    }
    
    public String getTemplateDefinationXmlPath() {
        return templateDefinationXmlPath;
    }
    
    public String getTestDBXmlPath() {
        return bulkUploadXmlPath;
    }
    
    public TemplateDefination getTemplateDefination() {
        return templateDefination;
    }
    
}
