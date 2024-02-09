package com.ipvs.config;

import java.util.Enumeration;
import java.util.Hashtable;

import com.ipvs.utils.Utils;

public class IPVSConst {
	private static IPVSConst nxdconst = null;
	private  boolean enabledNewDBAPI =false;

	private  boolean enabledBulkRead =false;
	private  boolean enabledBulkFind =false;
	private int useReadDefaultDepth=100;
	
	private  boolean enabledBulkAddUpdate =false;
	private  boolean enableNewPubSub =false;
	private  boolean disablePolicyCatche=false;

    private long mediafileTimeOut= 15 ;
    private long mediafileDBTimeOut= 8 ;
    //public String ipvsxmldbctl="/etc/scripts/sednactl.sh";
    private String ipvsxmldbctl="";
	//IpvsDB.mediadir.export.file.lastAction_time_out_min

	//private  boolean enabledBulkReadChildren=false;
	//private  boolean enabledBulkFindChildren=false;
 	private  boolean enabledBulkDelete =false;
 	
	private String os="";
	private String xmppSendMessageServerName="";
	private String xmppCreateNodeServerName="";
	private  boolean bEnableLogging=false;
	private String useMRXMLDBUtilsImpl;
	private boolean enablePrefixColDocToAutoID;
	private String pubsubJidPrefix_PubsubDB;
	private String pubsubJidPrefix_IpvsDB;
	private String pubsubJidPrefix_ExtComp;
	
	private Hashtable<String,Integer > countToFetchList = new Hashtable<String,Integer >();
	private boolean enablePagination;
	private int countToFetchDefault=20;
    private String ipvsDbComponentJIDPrefix="ipvsdbservice";
    private String ipvsOpenfireUserComponentJIDPrefix="ipvsuserauthservice";
    
    private String ipvsDbExportDir="";
    private String ipvsDbImportDir="";

    private String ipvsDbExportFile="";
    private String ipvsDbImportFile="";
    private String ipvsDMSExportFile="";
    private String ipvsDMSImportFile="";

    //private String xmppClientClass = "com.ipvs.smack.XMPPUtils.IPVSSession2";
    //private String xmppClientClass = "com.ipvs.xmpp.client.smack.SmackXmppClient";
    //private String xmppClientClass = "com.ipvs.smack.XMPPUtils.IPVSSession";
    private String xmppClientClass = "";
    private boolean componentLogin=false;
    private String pluginName=null;
    private boolean mscomponentLogin=false;
    private String ipvsAppServerComponentJIDPrefix="";
	private int retryIntervalForDBMaxReqLimitError=30; 
	
    public static IPVSConst getInstance(){ 
		if(nxdconst==null){
			nxdconst= new IPVSConst();
 		}
		return nxdconst;
	}
	private IPVSConst(){
		IPVSConfig config = IPVSConfig.getInstance();
		
		
		enabledNewDBAPI =  config.getPropertyBool("new.db.api.enable",false);
		if( enabledNewDBAPI ) {
			enabledBulkRead =  config.getPropertyBool("new.db.api.bulk.read.enable",false);
			enabledBulkFind =  config.getPropertyBool("new.db.api.bulk.read.withSearch.enable",false);
			enabledBulkDelete =  config.getPropertyBool("new.db.api.bulk.delete.enable",false);
			enabledBulkAddUpdate =  config.getPropertyBool("new.db.api.bulk.addupdate.enable",false);
			useReadDefaultDepth =  config.getPropertyInt("bulk.read.default.depth",20);
			
			enableNewPubSub =  config.getPropertyBool("new.pubsub.enable",false);
			disablePolicyCatche =  config.getPropertyBool("new.pubsub.policy.cache.disable",false);
			
		 
			/*enabledBulkReadChildren =  config.getPropertyBool("new.db.api.bulk.read.children.enable",false);
			enabledBulkFindChildren =  config.getPropertyBool("new.db.api.bulk.find.children.enable",false);
			*/
		}
 
		pubsubJidPrefix_PubsubDB = config.getProperty("ipvs.db.component.jid.prefixz", "pubsub");
		pubsubJidPrefix_IpvsDB = config.getProperty("IpvsDB.pubsub.jid.prefix", "ipvs.pubusb");
		pubsubJidPrefix_ExtComp = config.getProperty("ExtComp.pubsub.jid.prefix", "ipvsext.pubsub");

		ipvsDbComponentJIDPrefix = config.getProperty("ipvs.db.component.jid.prefix", "ipvsdbservice");
		ipvsOpenfireUserComponentJIDPrefix = config.getProperty("ipvs.openfire.user.component.jid.prefix", "ipvsuserauthservice");
		ipvsDbExportDir = config.getProperty("IpvsDB.mediadir.export.dir", "/data/mediadir/export");
		ipvsDbImportDir = config.getProperty("IpvsDB.mediadir.import.dir", "/data/mediadir/import");

        ipvsDbExportFile = config.getProperty("IpvsDB.mediadir.export.file", "/data/mediafile/export");
        ipvsDbImportFile = config.getProperty("IpvsDB.mediadir.import.file", "/data/mediafile/import");
        
        ipvsDMSExportFile = config.getProperty("IpvsDB.dms.mediadir.export.file", "/data/dms/mediafile/export");
        ipvsDMSImportFile = config.getProperty("IpvsDB.dms.mediadir.import.file", "/data/dms/mediafile/import");
        mediafileTimeOut = Utils.getLongValue(config.getProperty("IpvsDB.mediadir.export.file.lastAction_time_out_min", ""),mediafileTimeOut) ;
        mediafileDBTimeOut = Utils.getLongValue(config.getProperty("IpvsDB.mediadir.export.file.lastAction_time_out_db_min", ""),mediafileDBTimeOut) ;
        mediafileTimeOut = mediafileTimeOut * 60 * 1000;
        mediafileDBTimeOut= mediafileDBTimeOut * 60 * 1000;
		if(enableNewPubSub){
			String newPubSubImpl = config.getProperty("MRXMLDBUtils.Pubsub.Impl.use", "MRXMLDBUtils.IpvsDB.Impl");
			useMRXMLDBUtilsImpl = config.getProperty(newPubSubImpl, "com.ipvs.common.db.IpvsDBImpl");
			//useMRXMLDBUtilsImpl = config.getProperty("MRXMLDBUtils.IpvsDB.Impl","com.ipvs.common.db.IpvsDBImpl");
		}else{
			useMRXMLDBUtilsImpl = config.getProperty("MRXMLDBUtils.PubsubDB.Impl","com.ipvs.common.db.PubsubDBImpl");
			
		}
		xmppCreateNodeServerName = config.getProperty("xmpp.send.message.server.name","localhost");
		xmppSendMessageServerName = config.getProperty("xmpp.create.node.server.name","localhost");
		bEnableLogging =  config.getPropertyBool("xmpp.server.agent.enable.logging",false);
		
		enablePrefixColDocToAutoID =  config.getPropertyBool("enable.nidscheme.with.prefix.domain.col.docid",false);
		
		
		
		countToFetchDefault = config.getPropertyInt("max.countToFetch.default", 20);
		enablePagination = config.getPropertyBool("enable.pagination",true);
		String keyPrefix = "read.children.countToFetch.";
		for (Enumeration<?> e = config.getProps().propertyNames() ; e.hasMoreElements() ;) {
	         String key = (String)e.nextElement();
	         if(key.startsWith(keyPrefix)){
	        	 int value = config.getPropertyInt(key, countToFetchDefault);
	        	 String listName = key.substring(key.indexOf(keyPrefix)+keyPrefix.length());
	        	 countToFetchList.put( listName, Integer.valueOf(value));
	         }
	     }
		
        xmppClientClass= config.getProperty("xmpp.client.class",xmppClientClass);
        //if(ApplicationContext.isAppletContext())
        //  bEnableLogging =  false;
        ipvsAppServerComponentJIDPrefix = config.getProperty("ipvs.appserver.component.jid.prefix", "managementserver");
        ipvsxmldbctl = config.getProperty("script.ipvsxmldbctl", "/etc/scripts/ipvsxmldbctl.sh");
        
        //
        retryIntervalForDBMaxReqLimitError = Utils.getIntValue(config.getProperty("IpvsDB.max.request.limit.error.retry.interval.sec", "30") , retryIntervalForDBMaxReqLimitError) ;
        retryIntervalForDBMaxReqLimitError = retryIntervalForDBMaxReqLimitError*60;
    }
	
    public int getRetryIntervalForDBMaxReqLimitError() {
        return retryIntervalForDBMaxReqLimitError;
    }
	
    public long getMediafileDBTimeOut() {
        return mediafileDBTimeOut;
    }
    public long getMediafileTimeOut() {
        return mediafileTimeOut;
    }
    public String getIpvsDMSImportFile() {
        return ipvsDMSImportFile;
    }
    public String getIpvsDMSExportFile() {
        return ipvsDMSExportFile;
    }
    public String getIpvsDbExportDir() {
		return ipvsDbExportDir;
	}
    public String getIpvsDbImportDir() {
		return ipvsDbImportDir;
	}
    public String getIpvsDbExportFile() {
        return ipvsDbExportFile;
    }
    public String getIpvsDbImportFile() {
        return ipvsDbImportFile;
    }
	public String getIpvsDbComponentJIDPrefix() {
        return ipvsDbComponentJIDPrefix;
    }
    public void setIpvsDbComponentJIDPrefix(String ipvsDbComponentJIDPrefix) {
        this.ipvsDbComponentJIDPrefix = ipvsDbComponentJIDPrefix;
    }
    public String getIpvsOpenfireUserComponentJIDPrefix() {
        return ipvsOpenfireUserComponentJIDPrefix;
    }
    public void setIpvsOpenfireUserComponentJIDPrefix(String ipvsOpenfireUserComponentJIDPrefix) {
        this.ipvsOpenfireUserComponentJIDPrefix = ipvsOpenfireUserComponentJIDPrefix;
    }
    public String getXmppClientClass() {
      return xmppClientClass;
    }
	
	public boolean isEnablePagination() {
		return enablePagination;
	}

	public int getCountToFetch_ByListName(String listName) {
		if(countToFetchList.containsKey(listName))
			return (int)countToFetchList.get(listName);
		return countToFetchDefault;
	}

	public int getCountToFetch_Default() {
		return countToFetchDefault;
	}
	
	public String getPubsubJidPrefix_ExtComp() {
		return pubsubJidPrefix_ExtComp;
	}
	public String getPubsubJidPrefix_IpvsDB() {
		return pubsubJidPrefix_IpvsDB;
	}
	public String getPubsubJidPrefix_PubsubDB() {
		return pubsubJidPrefix_PubsubDB;
	}
	public boolean isEnablePrefixColDocToAutoID() {
		return enablePrefixColDocToAutoID;
	}
	public String getUseMRXMLDBUtilsImpl() {
		return useMRXMLDBUtilsImpl;
	}
	public boolean isEnableNewDBAPI() {
		return enabledNewDBAPI;
	}
	public int getUseReadDepth() {
		return useReadDefaultDepth;
	}
	public String getXmppSendMessageServerName() {
		return xmppSendMessageServerName;
	}
	public String getXmppCreateNodeServerName() {
		return xmppCreateNodeServerName;
	}
	public boolean isEnableLogging() {
		return bEnableLogging;
	}
	public boolean isEnableBulkDelete() {
		return enabledBulkDelete;
	}
 	public boolean isEnableBulkAddUpdate() {
		return enabledBulkAddUpdate;
	}
	public boolean isEnableBulkRead() {
		return enabledBulkRead;
	}
	public boolean isEnableBulkFind() {
		return enabledBulkFind;
	}
	public boolean isEnableNewPubSub() {
		return enableNewPubSub;
	}
	public boolean isDisablePolicyCatche() {
		return disablePolicyCatche;
	}
	/*public boolean isEnabledBulkReadChildren() {
		return enabledBulkReadChildren;
	}
	public boolean isEnabledBulkFindChildren() {
		return enabledBulkFindChildren;
	}*/
    public String getIpvsxmldbactl() {
        return ipvsxmldbctl;
    }   


	
	public String getOS(){
		try{
			if(os !=null && !os.equals(""))
				return os;	
			
			os = System.getProperty("os.name").toLowerCase();
			//default
			if(os == null || os.equals(""))
				os = "linux";
			if (os.indexOf("nt")!=-1 || os.indexOf("windows 20")!=-1  || os.indexOf("windows 9")!=-1 || os.indexOf("windows xp")!=-1 ) {
				os = "windows";
			}else if (os.startsWith("mac")  ) {
				os = "mac";
			/*}else if (os.indexOf("hp-ux") != -1 ) {
				os = "linux";
			}else if (os.indexOf("sunos") != -1 ) {
				os = "linux";
			}else if (os.indexOf("linux") != -1 ) {
				os = "linux";*/
			} else {
				os = "linux";
			}			
		}catch(Exception ex){
		}
		return  os;
	}
   public void setMediaroomPluginName(String pluginName) {
        this.pluginName =pluginName;
        componentLogin=true;
        mscomponentLogin=true;
    }
    public String getMediaroomPluginName() {
        return pluginName;
    }
    public void setXmppClientClass(String pxmppClientClass) {
        this.xmppClientClass=pxmppClientClass;
    }
    public boolean isComponentLogin() {
        return componentLogin;
    }
    public void setComponentLogin(boolean componentLogin) {
        this.componentLogin = componentLogin;
    }
    public boolean isMSComponentLogin() {
        return mscomponentLogin;
    }
    public void setMSComponentLogin(boolean mscomponentLogin) {
        this.mscomponentLogin = mscomponentLogin;
    }
    public String getIpvsAppServerComponentJIDPrefix() {
        return ipvsAppServerComponentJIDPrefix;
    }
 
}
/*
#bEnableLogging to LogRoot
xmpp.server.agent.enable.logging=false


#set Server Names
#m.setTo(dstuser + "@localhost");
xmpp.send.message.server.name=localhost
# ce.setOwner(super.getSuperUser() + "@localhost");
xmpp.create.node.server.name=localhost


#New.db.api.enable= 0/1 or true/false
#if false all other below (read/add/update/delete) will be false
new.db.api.enable=true

#bulk read enable. deleteNodeWithParent = false;
new.db.api.bulk.read.enable=true
new.db.api.bulk.read.default.depth=20

#bulk delete enable. deleteNodeWithParent = false;
new.db.api.bulk.delete.enable=true

#bulk add update enable. addUpdateBulkEnable =false;
new.db.api.bulk.addupdate.enable=true

  
 */

/*
        System.out.println("java.home: " + System.getProperty("java.home"));
        System.out.println("java.class.path: " + "java.home" + System.getProperty("java.class.path"));
        System.out.println("java.specification.version: " + System.getProperty("java.specification.version"));
        System.out.println("java.specification.vendor: " + System.getProperty("java.specification.vendor"));
        System.out.println("java.specification.name: " + System.getProperty("java.specification.name"));
        System.out.println("java.version: " + System.getProperty("java.version"));
        System.out.println("java.vendor: " + System.getProperty("java.vendor"));
        System.out.println("java.vendor.url: " + System.getProperty("java.vendor.url"));
        System.out.println("java.vm.specification.version: " + System.getProperty("java.vm.specification.version"));
        System.out.println("java.vm.specification.vendor: " + System.getProperty("java.vm.specification.vendor"));
        System.out.println("java.vm.specification.name: " + System.getProperty("java.vm.specification.name"));
        System.out.println("java.vm.version: " + System.getProperty("java.vm.version"));
        System.out.println("java.vm.vendor: " + System.getProperty("java.vm.vendor"));
        System.out.println("java.vm.name: " + System.getProperty("java.vm.name"));
        System.out.println("java.class.version: " + System.getProperty("java.class.version"));
        System.out.println("os.home: " + System.getProperty("os.name"));
        System.out.println("os.arch: " + System.getProperty("os.arch"));
        System.out.println("os.version: " + System.getProperty("os.version"));
        System.out.println("file.separator: " + System.getProperty("file.separator"));
        System.out.println("path.separator: " + System.getProperty("path.separator"));
        System.out.println("line.separator: " + System.getProperty("line.separator"));
        System.out.println("user.name: " + System.getProperty("user.name"));
        System.out.println("user.home: " + System.getProperty("user.home"));
        System.out.println("user.dir: " + System.getProperty("user.dir"));
SOLARIS 2.6 (32-bit)
====================
Operating System: SunOS
Operating System version: 5.6
System Architecture: sparc

SOLARIS 8 (32-bit or 64-bit)
============================
Operating System: SunOS
Operating System version: 5.8
System Architecture: sparc

HPUX 11.0 (64-bit)
==================
Operating System: HP-UX
Operating System version: B.11.00
System Architecture: PA-RISC

HPUX 11.11 (64-bit)
===================
Operating System: HP-UX
Operating System version: B.11.11
System Architecture: PA-RISC

HPUX 11.22 (IA64)
=================
Operating System: HP-UX
Operating System version: B.11.22
System Architecture: IA64

AIX 4.3.3 (64-bit)
==================
Operating System: AIX
Operating System version: 4.3
System Architecture: POWER_PC

AIX 5L 5.2 (64-bit)
===================
Operating System: AIX
Operating System version: 5.2
System Architecture: POWER_PC

TRU64 5.1B (64-bit)
===================
Operating System: OSF1
Operating System version: V5.1
System Architecture: alpha

REDHAT 2.1 (x86 32-bit)
=======================
Operating System: Linux
Operating System version: 2.4.9-e.25enterprise
System Architecture: x86

REDHAT 3.0 (x86 32-bit)
=======================
Operating System: Linux
Operating System version: 2.4.21-4.EL
System Architecture: i386

REDHAT 3.0 (x64-64 64-bit)
==========================
Operating System: Linux
Operating System version: 2.4.21-4.EL
System Architecture: i386
  
 */
/*	
private int maxDBConnPool=20;
private int insertChildNodeMaxSise=8000;
private  boolean bSaveDeleteToDB =false;
private  boolean bEnableDB =false;
private String databaseVendor="senda";
private String databaseHostName, databaseName, databaseUser, databasePassword;
public static IPVSConst getInstance(){ 
	if(nxdconst==null){
		nxdconst= new IPVSConst();
		}
	return nxdconst;
}
private IPVSConst(){
	IPVSConfig config = IPVSConfig.getInstance();
	bSaveDeleteToDB = config.getPropertyBool("pubsubitems.savetodb.enable",false);
	//check if db enabled 
	bEnableDB =  config.getPropertyBool("ipvs.nxd.database.enable",false);
	if(bEnableDB ){
		//check databaseVendor
		databaseVendor =  config.getProperty("ipvs.nxd.database.vendor","senda");
		//databaseURI, database, databaseUsername, databasePassword;
		databaseHostName   =  config.getProperty("ipvs.nxd.host","localhost");
		databaseName   =  config.getProperty("ipvs.nxd.databse.name","msdb");
		databaseUser   =  config.getProperty("ipvs.nxd.user","SYSTEM");
		databasePassword   =  config.getProperty("ipvs.nxd.pwd","MANAGER");
		maxDBConnPool   =  config.getPropertyInt("ipvs.nxd.maxConn",maxDBConnPool);
		insertChildNodeMaxSise =  config.getPropertyInt("insert.childnode.max.size",insertChildNodeMaxSise);
	}
	}
public boolean isSaveDeleteToDB(){
	return bSaveDeleteToDB;
}
public boolean isDBEnabled(){
	return bEnableDB;
}	
public String getDatabaseVendor(){
	return databaseVendor;
}
public String getDatabaseHostName() {
	return databaseHostName;
}
public String getDatabaseName() {
	return databaseName;
}
public String getDatabaseUser() {
	return databaseUser;
}
public String getDatabasePassword() {
	return databasePassword;
}
public int getMaxDBConnPool() {
	return maxDBConnPool;
}
public void setMaxDBConnPool(int maxDBConnPool) {
	this.maxDBConnPool = maxDBConnPool;
}
public int getInsertChildNodeMaxSise() {
	return insertChildNodeMaxSise;
}
public void setInsertChildNodeMaxSise(int insertChildNodeMaxSise) {
	this.insertChildNodeMaxSise = insertChildNodeMaxSise;
}	*/