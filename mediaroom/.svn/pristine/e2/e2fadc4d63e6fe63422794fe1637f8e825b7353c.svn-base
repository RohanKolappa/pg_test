package com.ipvs.common.service;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.IpvsDBUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.config.IPVSConst;

/**
 * This class represents the database schema The database implementation in this
 * case is a tree The MRDBSchema specifies the nodeIDs for key/root nodes within
 * the tree The service implementations will add nodes under these root nodes
 * 
 * @author miyer
 * 
 */
public class MRDB {
    
    int runningNumber = 1;
    XMPPF xf;
    String userJID;
    MRLog log;
    
    public static final String SERVICEROOT = "ServiceRoot";
    public static final String SOURCEGROUPROOT = "SourceGroupRoot";
    public static final String DESTGROUPROOT = "DestGroupRoot";
    public static final String PROFILEGROUPROOT = "ProfileGroupRoot";
    public static final String ROOMROOT = "MediaRoomList";
    public static final String ROOMCONFIGROOT = "MediaRoomConfigList";
    // Policy Admin Roots - Begin
    public static final String POLICYROOT = "PolicyRoot";
    public static final String ASSETGROUPROOT = "AssetGroupRoot";
    public static final String ROOMPOLICYROOT = "RoomPolicyRoot";
    public static final String STREAMPOLICYROOT = "StreamPolicyRoot";
    public static final String ASSETADMINPOLICYROOT = "AssetAdminPolicyRoot";
    public static final String MEDIASTOREADMINPOLICYROOT = "MediaStoreAdminPolicyRoot";
    public static final String ASSETADMINROLEROOT = "AssetAdminRoleRoot";
    public static final String MEDIASTOREADMINROLEROOT = "MediaStoreAdminRoleRoot";
    public static final String USERGROUPROOT = "UserGroupRoot";
    public static final String RESOURCEROOT = "ResourceGroupRoot";
    public static final String STREAMPROFILEGROUPROOT = "StreamProfileGroupRoot";
    public static final String ROOMGROUPROOT = "RoomGroupRoot";
    // Policy Admin Roots - End
    
    // Policy Admin new Tags -starts
    public static final String RESOURCETAGROOT = "ResourceTagRoot";
    public static final String ASSETTTAGROOT = "AssetTagRoot";
    public static final String STREAMPROFILETAGROOT = "StreamProfileTagRoot";
    public static final String USERTAGROOT = "UserTagRoot";
    public static final String ROOMTAGROOT = "RoomTagRoot";
    public static final String TAGROOT = "TagRoot";
    
    // Policy Admin new Tags -End
    
    // Asset Admin Roots - Start
    public static final String USER_ROOT = "UserRoot";
    public static final String DEVICE_ROOT = "DeviceRoot";
    public static final String ASSET_ROOT = "AssetRoot";
    public static final String ASSET_RESOURCE_ROOT = "AssetResourceRoot";
    public static final String MEDIA_ROOT = "MediaRoot";
    public static final String MEDIA_DIR_ROOT = "MediaDirRoot";
    public static final String ASSET_RESOURCE_GROUP_ROOT = "AssetResourceGroupRoot";
    public static final String MEDIA_STREAM_PORT_RESOURCE_GROUP_ROOT = "MediaStreamPortResourceGroupRoot";
    public static final String MEDIA_STREAM_RESOURCE_ROOT = "MediaStreamResourceRoot";
    public static final String MEDIA_STREAM_FILE_RESOURCE_ROOT = "MediaStreamFileResourceRoot";
    public static final String MEDIA_STREAM_SRC_PORT_RESOURCE_ROOT = "MediaStreamSrcPortResourceRoot";
    public static final String MEDIA_STREAM_DST_PORT_RESOURCE_ROOT = "MediaStreamDstPortResourceRoot";
    public static final String MEDIA_STREAM_RELAY_PORT_RESOURCE_ROOT = "MediaStreamRelayPortResourceRoot";
    public static final String STREAM_PROFILE_ROOT = "StreamProfileRoot";
    // Asset Admin Roots - End
    
    private String ipvsRootNID = null;
    private Hashtable<String, String> rootTable = null;
    private String serviceDomain = null;
    
    public MRDB(XMPPF xf, String userJID, MRLog log) {
        this.xf = xf;
        this.userJID = userJID;
        this.log = log;
        this.ipvsRootNID = null;
        this.rootTable = null;
    }
    
    private void initRoots(Hashtable<String, String> roots, Element el) throws Exception {
        String elementName = el.getName();
        //todo: the nidscheme will be applied from common shared function 
        //String NID = parentNID + "." + elementName;
        String NID = MRXMLUtils.getAttributeValue(el, IpvsDBUtils.NODEID_2, "");//parentNID + "." + elementName;
        if (!roots.containsKey(elementName)) { // make sure that
            roots.put(elementName, NID);
        }
        List<?> childList = MRXMLUtils.getList(el);
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element childReqEL = (Element) i.next();
            initRoots(roots, childReqEL);
        }
    }
    
    public void init(String ipvsRootNID, String serviceDomain, Document schemadoc) throws Exception {
        Hashtable<String, String> roots = new Hashtable<String, String>();
        
        //todo: move this function to the place where init is being called.
        if (IPVSConst.getInstance().isEnablePrefixColDocToAutoID()) {
            IpvsDBUtils.parseNID_BULK_SCHEMA(schemadoc.getRootElement(), serviceDomain);
        } else {
            String NID = IpvsDBUtils.parseNID_BULK(schemadoc.getRootElement(), ipvsRootNID);
            MRXMLUtils.addAttribute(schemadoc.getRootElement(), IpvsDBUtils.NODEID_2, NID);
        }
        
        initRoots(roots, schemadoc.getRootElement());
        this.ipvsRootNID = ipvsRootNID;
        this.rootTable = roots;
        this.serviceDomain = serviceDomain;
        if (this.xf.xmpp(userJID) != null) 
        	this.xf.xmpp(userJID).setServiceDomain(serviceDomain);
        
    }
    
    public void checkRoots() throws MRException {
        if (this.rootTable == null) throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "<error>" + "Database Not initialized" + "</error>");
    }
    
    public String getRootNID() {
        return getServiceDomainRootNID(MRDB.SERVICEROOT); 
    }
    
    public String getServiceDomain() {
        return serviceDomain;
    }
    
    public void setServiceDomain(String serviceDomain) {
        this.serviceDomain = serviceDomain;
    }
    
    public XMPPI getXMPPI() {
    	try {
	        if (this.xf.xmpp(userJID) != null) this.xf.xmpp(userJID).setServiceDomain(serviceDomain);
	        return this.xf.xmpp(userJID);
    	}
    	catch(Exception e) {
    		return null;
    	}
    }
    
    public String getServiceDomainRootNID(String key) {
        if (rootTable == null || !rootTable.containsKey(key)) return null;
        return rootTable.get(key);
    }
    public String getServiceDomainRootNID(String listname, String objectType) {
        if (rootTable == null) {
            return "";
        }
        if (listname  !=null && rootTable.containsKey(listname)) {
            return rootTable.get(listname);
        }
        if (objectType  !=null  ) {
            try {
                //todo:simplify to get the parentkey in single step
                MRConfig mrconfig = MRConfig.getInstance(objectType, log);
                if(mrconfig !=null){
                    return rootTable.get(mrconfig.getParentNIDKey(log));
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return "";
    }
    // ////////// MediaRoom ////////////
    public String getSourceGroupRootNID() {
        return getServiceDomainRootNID(MRDB.SOURCEGROUPROOT);
    }
    
    public String getDestGroupRootNID() {
        return getServiceDomainRootNID(MRDB.DESTGROUPROOT);
    }
    
    public String getProfileGroupRootNID() {
        return getServiceDomainRootNID(MRDB.PROFILEGROUPROOT);
    }
    
    public String getRoomRootNID() {
        return getServiceDomainRootNID(MRDB.ROOMROOT);
    }
    
    public String getRoomConfigRootNID() {
        return getServiceDomainRootNID(MRDB.ROOMCONFIGROOT);
    }
    
    //removed unused functions
  
}
