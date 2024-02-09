package com.ipvsserv.dmsapp;

import java.io.File;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.EngineMDCUtils;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.SchemaConst;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DMSUserManagementModule extends IpvsServBasicModule {
    public DMSUserManagementModule() {
        super("DMSUserManagement");
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
    //private boolean executed = false;
    private String user="";
    private String password="";
    private String resource="";
    //private IpvsServClassLoader pluginLoader = null;
    private Element loadXML(String filePath){
        File file = new File(filePath);
        if(file.exists()){
            try{
                Document doc = Dom4jUtils.getDocument(file);
                return doc.getRootElement();
            } catch (Exception ex) {
               MRLog.error("loadXML filePath", ex);
            }   
        }else{
        	try {
                //Document doc = Dom4jUtils.loadXMLResource(filePath, IpvsServer.getInstance().getMRClass().getClass());
                Document doc = Dom4jUtils.loadXMLResource(filePath);
	        	return doc.getRootElement();
        	} catch (Exception ex) {
                MRLog.error("loadXML filePath", ex);
             }
        }
        return null;
    }
    /*private Element loadXML2(String filePath){
        File file = new File(filePath);
        if(file.exists()){
            try{
                Document doc = Dom4jUtils.getDocument(file);
                return doc.getRootElement();
            } catch (Exception ex) {
               MRLog.error("loadXML filePath", ex);
            }   
        }else{
        	 ClassLoader oldLoader = Thread.currentThread().getContextClassLoader();
             try {
                 Thread.currentThread().setContextClassLoader(pluginLoader);
                 Object cls = pluginLoader.loadClass("com.ipvs.test.TestDBCache").newInstance();
                 URL url = cls.getClass().getResource(filePath);                
                 Document doc = Dom4jUtils.getDocument(url);
                 return doc.getRootElement();
             } catch (Exception ex) {
                MRLog.error("loadXML filePath", ex);
             }finally{
                 Thread.currentThread().setContextClassLoader(oldLoader);                
             } 
	    
        }
        return null;
    }  */  
    private boolean userAdded=false;
    private boolean assetAdded=false;
    private boolean done=false;
    
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

            if(userAdded && assetAdded){
                done=true;
            }
            
            if(done){
                try{
                    destroy();
                    MRLog.debug("DMS initiation is upto date ........");
                    return;
                } catch (Exception ex) {
                    
                }
            }

            IpvsServer instance= IpvsServer.getInstance();
            IpvsDBDriver db = instance.getDefaultIpvsDBDriver();

            /*if(isInitdbCompleted(db)){
                //updateServiceResource(db);
                done = true;
                return ;
            }*/
            
            //
            //loadObjectXmls();
            //read user info 
            parseLoginXML();
            
            //load user 
            createRecords(db);
            if(userAdded && assetAdded) {
                MRLog.debug("Completed DMS initiation........");
            }
            executed=true;
            
        } catch (Exception ex) {
            MRLog.error("DMSUserManagement", ex);
        } finally {
            synchronized (this) {
                runningInsert = false;
            }
        }
    }
   /* private void updateServiceResource(IpvsDBDriver db){
         try{
            List<String> list = db.getDomainNames();
            for (Iterator<String> domianiterator = list.iterator(); domianiterator.hasNext();) {
                String domainName = domianiterator.next();
                StringBuffer query =new StringBuffer();
                RequestMDCParams params = new RequestMDCParams(domainName);
                query.append("update replace $b in  index-scan('"+params.getIndexNameCol()+"'");
                query.append(", '"+domainName+".serviceresourcelist', 'EQ')/../ServiceResource/Contact/OwnerUserJID ");
                query.append(" with <OwnerUserJID></OwnerUserJID>");
                db.executeXQueryToUpdate("updateServiceResource", params.getCollectionName(), query.toString());
            }
        }catch(Exception ex){
            MRLog.error(ex);
        }
    }*/
    public static String getUUID() {
        UUID objUUID = UUID.randomUUID();

        return objUUID.toString();
    }
    public static String getAutoNID(String parentNID, String listPrefix) throws Exception {
        NIDSchemeMDC nidScheme = new NIDSchemeMDC();
        nidScheme.parse(parentNID);
        if (nidScheme.getType() != NIDSchemeMDC.COLLECTION) {
            MRLog.debug("getAutoNID Problem ????????1111????? parentNID  " + parentNID);
            throw new Exception("Invalid list parentNID ='" +parentNID+"'");
        }
        String NID = listPrefix + "_" + getUUID();
        return nidScheme.getCollectionNodeID() + "." + NID.toLowerCase();
    }
    
    private void createRecords(IpvsDBDriver db){
        ensureUser(db);
        ensureAssetResource(db);
    }
    private void ensureUser(IpvsDBDriver db){
        String ID="addDMSUser";
        if(!userAdded){
            try{
                NIDSchemeMDC node = getUserNode();
                // if already exists .. neglect error
                String list_nodeID= node.getCollectionNodeID();
                String whereXpath = "//User[Username='"+user+"']";
                Element EL = readNode(db, list_nodeID, whereXpath,"//User");
                if(EL == null){
                    String nodeID = getAutoNID(node.getCollectionNodeID(), "ul");
                    node =new NIDSchemeMDC();
                    node.parse(nodeID);
                    EL = getUser(node);
                    db.addDocument(ID, node.getDomain(), SchemaConst.DOC_TYPE_DATA, node.getDocUri(), node.getCollectionName(), EL.asXML());
                    userAdded=true;
                    return ;
                }else {
                    String password_DB = Dom4jUtils.getValueXpath(EL, "Password", "").trim();
                    if(!password_DB.equals(password.trim())){
                        Element passwordEL = Dom4jUtils.getElementXpath(EL, "Password");
                        passwordEL.detach();
                        Dom4jUtils.addElement(EL, "Password",password );
                        String nodeID = EL.attributeValue("NID");
                        node =new NIDSchemeMDC();
                        node.parse(nodeID);
                        db.updateNode("DMS_UPDATE_"+user, node.getDomain(), node.getType(), 
                                node.getIndexName(), nodeID, node.getCollectionName(), 
                                EL.asXML(), null, "", "");
                        userAdded=true;
                        return ;
                    }
                    userAdded=true;
                    return ;
                }
            } catch (Exception ex) {
                MRLog.error(ID, ex);
            }
        }
        
    }
    private void ensureAssetResource(IpvsDBDriver db){
        String ID="addDMSAssetResource";
        if(!assetAdded){
            try{
                NIDSchemeMDC node = getAssetResourceNode();
                String list_nodeID = node.getCollectionNodeID();
                String whereXpath = "//AssetResource//Info[Title='"+resource+"']";
                Element EL =  readNode(db, list_nodeID, whereXpath, "//AssetResource");
                if(EL == null ){
                    String nodeID = getAutoNID(node.getCollectionNodeID(), "arl");
                    node = new NIDSchemeMDC();
                    node.parse(nodeID);
                    EL = getAssetResource(node);
                    db.addDocument(ID, node.getDomain(), SchemaConst.DOC_TYPE_DATA, node.getDocUri(), node.getCollectionName(), EL.asXML());
                    assetAdded=true;
                    return ;
                }else {
                    assetAdded=true;
                    return ;
                }
            } catch (Exception ex) {
                MRLog.error(ID, ex);
            }            
        }
    }    
    private NIDSchemeMDC getUserNode(){
        String usernid=IpvsServGlobals.getProperty(getIpvsModuleName() + ".user.nid", "default.userlist");
        NIDSchemeMDC user_node = new NIDSchemeMDC();
        user_node.parse(usernid);
        return user_node;
    }
    private NIDSchemeMDC getAssetResourceNode(){
        String assetrecourcenid=IpvsServGlobals.getProperty(getIpvsModuleName() + ".asset.nid", "");
        NIDSchemeMDC assetrecource_node = new NIDSchemeMDC();
        assetrecource_node.parse(assetrecourcenid);
        return assetrecource_node;
    }    
    private boolean isInitdbCompleted(IpvsDBDriver db) throws Exception {
        String nodeID= IpvsServGlobals.getProperty(getIpvsModuleName() + ".verify_initdb.list_nid", "");
        String whereXpath = IpvsServGlobals.getProperty(getIpvsModuleName() + ".verify_initdb.where_xpath", "//User[UserName='superuser-default']");
        return isDocExists(db, nodeID, whereXpath);
    }
    
    private boolean isDocExists(IpvsDBDriver db,String nodeID, String  whereXpath) throws Exception {
        NIDSchemeMDC node = new NIDSchemeMDC();
        node.parse(nodeID);
        List<String> whereXpathlist = new ArrayList<String>();
        whereXpathlist.add(whereXpath);
        
        String ID="exists_"+nodeID;
        List<String> list = db.getChildren(ID, node.getDomain(), node.getType(),
                node.getIndexNameCol(), nodeID, node.getCollectionName(), "", whereXpathlist, "@NID", "");
        for (Iterator<String> i = list.iterator(); i.hasNext();) {
            String NID = i.next();
            NID = EngineMDCUtils.removeAttribute(NID).trim();
            if(!NID.trim().equals("")){
                MRLog.debug(ID +" " + whereXpath+ " found NID " +NID );
                return true;
            }
        }
        return false;
    }
     
    private Element  readNode(IpvsDBDriver db,String nodeID, String  whereXpath, String elementName) throws Exception {
        NIDSchemeMDC node = new NIDSchemeMDC();
        node.parse(nodeID);
        
        String colNodeID = node.getCollectionNodeID();
        node = new NIDSchemeMDC();
        node.parse(colNodeID);
        
        List<String> whereXpathlist = new ArrayList<String>();
        whereXpathlist.add(whereXpath);

        String packetID="readNode_"+whereXpath;
        String xml = db.getNodesCol(packetID, node.getDomain(), node.getType(), node.getIndexNameCol(), nodeID, 
                node.getCollectionName(), 0, 1, whereXpathlist, "", "", "<DBResp>", "</DBResp>", false, null, false, "");        
        try{
            if(xml != null ){
                Document doc = Dom4jUtils.stringToDocument(xml);
                Element El = Dom4jUtils.getElementXpath(doc.getRootElement(), elementName);
                MRLog.debug(packetID + " not found ?"+ (El==null)+ " "  );
                return El;
            }
        }catch(Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    private void parseLoginXML() throws Exception {
        String loginxml = IpvsServGlobals.getProperty(getIpvsModuleName() + ".loginxml", "/etc/config/LoginRequest.xml");
        Element loginXMLEL = loadXML(loginxml);
        if(loginXMLEL == null){
             throw new Exception("loginxml not found '" +loginxml+"'"); 
        }
        user = Dom4jUtils.valueXpath(loginXMLEL, "data/LoginRequestData/user", "");
        password = Dom4jUtils.valueXpath(loginXMLEL, "data/LoginRequestData/password", "");
        resource = Dom4jUtils.valueXpath(loginXMLEL, "data/LoginRequestData/resource", "");
    }
    
    private void loadObjectXmls() throws Exception {
        /*
    	pluginLoader = IpvsServer.getInstance().getClassLoader(); 
        String objectxmljar=IpvsServGlobals.getProperty(getIpvsModuleName() + ".lib.objectxmljarpath", "/home/onair/modules/mediaroom/public/objectxml.jar");
        File objectxmljarFile = new File(objectxmljar);
        pluginLoader.addURLFile(objectxmljarFile.toURI().toURL());
        objectxmljar=IpvsServGlobals.getProperty(getIpvsModuleName() + ".lib.mediaroomjarpath", "/home/onair/modules/mediaroom/public/mediaroom.jar");
        objectxmljarFile = new File(objectxmljar);
        pluginLoader.addURLFile(objectxmljarFile.toURI().toURL());   
        */
    }
   
    public Element getAssetResource(NIDSchemeMDC node) throws Exception{
        ///////////// load asset object xml ///////////////
        String assetobjectxml = IpvsServGlobals.getProperty(getIpvsModuleName() + ".objectxml.asset", "/com/ipvs/xsd/objectXml/AssetResource.xml");
        Element objectassetEL = loadXML(assetobjectxml);
        if(objectassetEL==null){
            throw new Exception("AssetResource xml is not found.");
        }
        
        // set NID and parentNID
        Dom4jUtils.addAttribute(objectassetEL,"NID", node.getDocNodeID());
        Dom4jUtils.addAttribute(objectassetEL,"parentNID", node.getCollectionNodeID());
        
        //set Groups tag
        String groupsELPath = IpvsServGlobals.getProperty(getIpvsModuleName() + ".asset.groups_el_parent_path","//Info");
        Element groupsELNew = IpvsServGlobals.getElement(getIpvsModuleName() + ".asset.groups_el.Groups");
        Dom4jUtils.replaceElement(objectassetEL, groupsELPath, groupsELNew);
        
        //set assetPrivateKey
        groupsELPath = IpvsServGlobals.getProperty(getIpvsModuleName() + ".asset.assetPrivateKey_el_parent_path","//Info");
        groupsELNew = IpvsServGlobals.getElement(getIpvsModuleName() + ".asset.assetPrivateKey_el.AssetPrivateKey");
        Dom4jUtils.replaceElement(objectassetEL, groupsELPath, groupsELNew);
        
        //set : Title, Type.
        String type = IpvsServGlobals.getProperty(getIpvsModuleName() + ".asset.Type", "MS-Device");
        Dom4jUtils.replaceValueXpath(objectassetEL, "//Title", resource);
        Dom4jUtils.replaceValueXpath(objectassetEL, "//Type", type);
        return objectassetEL;
    }
    
    public Element getUser( NIDSchemeMDC node) throws Exception{
         
        ///////////// load user object xml ///////////////
        String userobjectxml = IpvsServGlobals.getProperty(getIpvsModuleName() + ".objectxml.user", "/com/ipvs/xsd/objectXml/User.xml");
        Element objectUserEL = loadXML(userobjectxml);
        if(objectUserEL==null){
            throw new Exception("User xml is not found.");
        }

        //get user NID
        
        // set NID and parentNID
        Dom4jUtils.addAttribute(objectUserEL,"NID", node.getDocNodeID());
        Dom4jUtils.addAttribute(objectUserEL,"parentNID", node.getCollectionNodeID());
        
        
        //set Group tag
        String groupsELPath = IpvsServGlobals.getProperty(getIpvsModuleName() + ".user.group_el_parent_path","//Roster/Item");
        Element groupsELNew = IpvsServGlobals.getElement(getIpvsModuleName() + ".user.group_el.Group");
        Dom4jUtils.replaceElement(objectUserEL, groupsELPath, groupsELNew);

        //set Groups tag
        groupsELNew = IpvsServGlobals.getElement(getIpvsModuleName() + ".user.groups_el.Groups");
        Dom4jUtils.replaceChildElement(objectUserEL, groupsELNew);

        //set : user, pwd , name , creation date, etc.
        String emailSuffix = IpvsServGlobals.getProperty(getIpvsModuleName() + ".user.email_suffix","local.com");
        Dom4jUtils.replaceValueXpath(objectUserEL, "//Email", user+"@"+emailSuffix);
        Dom4jUtils.replaceValueXpath(objectUserEL, "//Name", user);
        Dom4jUtils.replaceValueXpath(objectUserEL, "//Username", user);
        Dom4jUtils.replaceValueXpath(objectUserEL, "//Password", password);
        
        //todo: set date -- date format is not fixed. need to get date foramt form UI
        String date = (new Date()).toString();
        Dom4jUtils.replaceValueXpath(objectUserEL, "//CreationDate",date );
        Dom4jUtils.replaceValueXpath(objectUserEL, "//ModifiedDate", date);
        return objectUserEL;
    }
  
    @Override
    public int getLastKnownState() throws Exception {
        
        return 0;
    }
    
}

 