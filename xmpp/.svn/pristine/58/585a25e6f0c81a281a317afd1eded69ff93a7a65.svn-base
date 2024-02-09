package com.ipvsserv.dmsapp;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.xslt.XsltTransformUtils;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */


public class DMSUserRegistrationModule extends IpvsServBasicModule {
    public DMSUserRegistrationModule() {
        super("DMSUserRegistration");
    }

    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
    }

    @Override
    public void start() throws Exception {
        super.startMonitor();
    }

    @Override
    public void stop() throws Exception {
        destroy();
    }

    @Override
    public void destroy() throws Exception {
        super.destroy();
    }

    private boolean runningInsert = false;
    
    private boolean done = false;

    @Override
    public void verifyState() throws Exception {

        synchronized (this) {
            if (runningInsert) {
                return;
            }
            runningInsert = true;
        }
        try {

            if (!IpvsServer.getInstance().isDBReady()) {
                return;
            }

            done = checkDMSUserRegistration();
            if (done) {
                started=true;
                initialized = true;
                try {
                    destroy();
                    MRLog.debug("DMS registration is upto date ........");
                    return;
                } catch (Exception ex) {

                }
            }
            executed = true;
        } catch (Exception ex) {
            MRLog.error("DMSUserRegistration", ex);
        } finally {
            synchronized (this) {
                runningInsert = false;
            }
        }
    }

    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }
    boolean debug = false;
    private boolean checkDMSUserRegistration() {
        Document doc =null;
        String ipvscomphome = "";
        try {
            debug = CompUtils.booleanValue(System.getProperty("ipvsDebug", "false"),false);
            DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
            if (!dbadmin.isDBModuleStart()) {
                return false;
            }

            //// update serviceresourcelist //////////// 
            IpvsServer instance= IpvsServer.getInstance();
            IpvsDBDriver db = instance.getDefaultIpvsDBDriver();
            String serviceresourcelist = IpvsServGlobals.getProperty("admindbapi.serviceresourcelistNID", "default.serviceresourcelist");
            //String serviceresourcelist = IpvsServGlobals.getProperty(getIpvsModuleName()+".serviceresourcelistNID", "default.serviceresourcelist");
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.parse(serviceresourcelist);
            if(debug){
                //skip updating service resource
            }else{
                List<String> arr= new ArrayList<String>();
                db.updateNode("reset_serviceresource", node.getDomain(), 1, node.getIndexNameCol(), serviceresourcelist, 
                        node.getCollectionName(), "<OwnerUserJID></OwnerUserJID>", arr, "//OwnerUserJID", "");
                db.updateNode("reset_serviceresource", node.getDomain(), 1, node.getIndexNameCol(), serviceresourcelist, 
                        node.getCollectionName(), "<StateFlag></StateFlag>", arr, "//StateFlag", "");
            }
            
            //// create dms user //////////// 
            String dbMigrationBackupArea = IpvsServGlobals.getProperty("admindbapi.dbBackupArea", "/data/sedna_dbsnapshot/backup");
            ipvscomphome= IpvsServGlobals.getHomeDirectory();
            String xsl =ipvscomphome+ "/conf/xmls/dmsappregistration/registerDMSAppUser.xsl";
            String xml =ipvscomphome+ "/conf/xmls/dmsappregistration/serviceDomain.xml";
            String result = dbMigrationBackupArea+ "/resultRegisterDMSAppUser.xml";
            
            File resultFile = new File(result);
            XsltTransformUtils.transform_Xalan(xsl, new File(xml), resultFile);
            doc = Dom4jUtils.getDocument(resultFile);
            Element root = doc.getRootElement();
            String  db_AssetResource_registration = Dom4jUtils.valueXpath(root, "db_AssetResource_registration/result", "false");
            String  db_Device_registration = Dom4jUtils.valueXpath(root, "db_Device_registration/result", "false");
            if(db_AssetResource_registration.equals("false")
                    || db_Device_registration.equals("false")
                    ){
                MRLog.error("Failed checkDMSUserRegistration  " + Dom4jUtils.documentToString(doc));
                return false;
            }
            
            return true;
        } catch (Exception ex) {
            MRLog.debug("Failed checkDMSUserRegistration  " );
            try{
                MRLog.error("Failed checkDMSUserRegistration  "  + CompUtils.readFile(ipvscomphome+ "/dmsappregistration/resultRegisterDMSAppUser.xml"), ex);
            }catch(Exception e){
            }
            return false;
        }
       
    }
}
/*
<?xml version="1.0" encoding="UTF-8"?>
<DMSRegistartion xmlns:xalan="http://xml.apache.org/xalan">
    <user_whereXpath>/User[Username='pravinpc']</user_whereXpath>
    <assetResource_whereXpath>/AssetResource[//Title='pravinpc']</assetResource_whereXpath>
    <user>pravinpc</user>
    <password>pravinpc</password>
    <resource>pravinpc</resource>
    <assetType>DMS-1500-Device</assetType>
    <db_assetNID>default.assetresourcelist.arl_aeb96d36-f5c1-4e43-ad0d-2c583656d745</db_assetNID>
    <db_assetType>DMS-1500-Device</db_assetType>
    <db_resource>pravinpc</db_resource>
    <db_password>pravinpc</db_password>
    <db_userName>pravinpc</db_userName>
    <device_whereXpath>/*[ (contains(name(),'Device') ) and (//AssetResourceNID='default.assetresourcelist.arl_aeb96d36-f5c1-4e43-ad0d-2c583656d745') ]/*[ (contains(name(),'Device') ) and
        (//AssetResourceNID='default.assetresourcelist.arl_aeb96d36-f5c1-4e43-ad0d-2c583656d745') ]</device_whereXpath>
    <db_User_registration>
        <one_record_same_pass/>
        <records>1</records>
        <nid>default.userlist.ul_8c696878-7dc5-4d92-a36c-8490f130f98f</nid>
        <result>true</result>
    </db_User_registration>
    <db_AssetResource_registration>
        <one_record/>
        <one_record_same_type/>
        <records>1</records>
        <nid>default.assetresourcelist.arl_aeb96d36-f5c1-4e43-ad0d-2c583656d745</nid>
        <result>true</result>
    </db_AssetResource_registration>
    <db_Device_registration>
        <one_record/>
        <one_record_same_type/>
        <records>1</records>
        <nid>default.devicelist.dl_57fbeeb5-6487-4450-bc2f-c5e0f88ee25e</nid>
        <result>true</result>
    </db_Device_registration>
</DMSRegistartion>
*/