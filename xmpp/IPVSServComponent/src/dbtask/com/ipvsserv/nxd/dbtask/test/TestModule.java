package com.ipvsserv.nxd.dbtask.test;

import java.io.File;
import java.util.Map;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.bulkupload.template.TemplateDefination;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public abstract class TestModule extends IpvsServBasicModule {
    protected String taskName = "TestModule";
    protected String templateDefinationXmlPath;
    protected String testDBXmlPath = "testDB.xml";
    protected TemplateDefination templateDefination = null;
    private ClassLoader loader;
    private DBTestActionsSequence dbTestActions = null;
    private long startDate;
    
    public TestModule() {
        super("TestModule");
    }
    
    public TestModule(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            templateDefinationXmlPath = params.get("templateDefinationXmlPath");
            testDBXmlPath = IpvsServGlobals.getProcessArguments().getArgumentParam("testdbxml", params.get("testDBXmlPath"));
            try {
                testDBXmlPath = IpvsServGlobals.verifyAndGetTestXmlPath(testDBXmlPath);
            } catch (Exception ex) {}
            TestGlobals.setConfigPath(testDBXmlPath);
            
            templateDefinationXmlPath = IpvsServGlobals.verifyAndGetFilePath(templateDefinationXmlPath);
            
            startDate = System.currentTimeMillis();
            ////////////////////////////////////
            loader = Thread.currentThread().getContextClassLoader();
            initialized = true;
        } catch (Exception ex) {
            MRLog.error(" " + testDBXmlPath + " app: " + IpvsServGlobals.getIpvsAppName());
            ex.printStackTrace();
        }
    }
    
    @Override
    public void start() throws Exception {
        try {
            MRLog.debug(name + " start  ");
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
            
            DBTestActionShared shared = DBTestActionShared.getInstance();
            String collectionPrefix = getProperty("collectionPrefix", TestGlobals.getProperty("DBTestActionsSequence.collectionPrefix", "ipvsCol"));
            try {
                IpvsDBDriver db = IpvsServer.getInstance().getDefaultIpvsDBDriver();
                Map<String, Integer> resourceList = db.getChildCollectionResourceCount(name, "", "startswith", collectionPrefix);
                shared.init(resourceList, collectionPrefix);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            MRLog.debug(name + " DBTestActionShared loaded ");
            dbTestActions = new DBTestActionsSequence(loader);
            dbTestActions.start();
            MRLog.debug(name + " DBTestActionsSequence started   ");
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void destroy() throws Exception {
        //stop if the app is started as taskName = SednaNativeDBTest /exist test / back up /bulk load only
        try {
            dbTestActions.stop();
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
        return testDBXmlPath;
    }
    
    public TemplateDefination getTemplateDefination() {
        return templateDefination;
    }
    
}
