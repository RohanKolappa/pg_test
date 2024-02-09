package org.jivesoftware.openfire.user;

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.Node;
import org.dom4j.io.SAXReader;
import org.jivesoftware.openfire.SessionManager;
import org.jivesoftware.openfire.XMPPServer;
import org.jivesoftware.openfire.event.UserEventDispatcher;
import org.jivesoftware.openfire.session.ClientSession;
import org.jivesoftware.util.JiveGlobals;
import org.jivesoftware.util.Log;
import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
import org.xmpp.packet.Packet;
import org.xmpp.packet.PacketError;
import org.xmpp.packet.Presence;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class IpvsUserProvider implements UserProvider {
    //private ScheduledExecutorService executor = null;
    // private ReadAllUsersMonitor readAllUsersMonitor=null;
    private IpvsUserComponent comp = null;
    private List<String> listDomains = new ArrayList<String>();
    private String action = "readChildren";
    private String nid = "userlist";
    private String deviceUsersNID = "assetresourcelist";
    private String to = "";
    private String jid = "";
    private boolean read_super_user_pwd = false;
    private boolean read_ms_user_pwd = false;

    private static IpvsUserProvider intance = null;
    //private Hashtable<String, String> passwords = new Hashtable<String, String>();
    private Hashtable<String, String> passwordsHardcoded = new Hashtable<String, String>();
    //private Hashtable<String, User> users = new Hashtable<String, User>();
    private Hashtable<String, User> usersHardcoded = new Hashtable<String, User>();

    private Hashtable<String, String> userNIDSCache = new Hashtable<String, String>();
    //private Hashtable<String, String> nidUsersCache = new Hashtable<String, String>();
    private Hashtable<String, User> usersCache = new Hashtable<String, User>();
    private boolean initHardcoded = false;
    private Date startDate = new Date();
    private long timeout_pass = 10 * 1000;
    private long timeout_users = 40 * 1000;

    protected ThreadPoolExecutor threadPool;
    
    private String ANY = "ANY";
    private String LISTNAME_USERS = "User";
    private String LISTNAME_ASSET = "AssetResource";
    private String TYPE_READ = "readList";
    private String TYPE_ADD = "add";
    private String TYPE_REPLACE = "replace";
    private String STATE_AUTOREGISTERED = "AutoRegistered";
    private String ASSETXMLFILEPATH = "/home/onair/objectXml/AssetResource.xml";

    public static IpvsUserProvider getInstance() {
        return intance;
    }

    private void Log_error(String msg) {
        //System.out.println(msg);
        Log.error(msg);
    }

    private void Log_error(String msg, Exception ex) {
        //System.out.println(msg);
        //ex.printStackTrace();
        Log.error(msg, ex);
    }

    public IpvsUserProvider() {
        if (intance == null) {
            intance = this;
        }
        comp = new IpvsUserComponent(this);
        /*
            ipvs.db.component.jid.prefix=ipvsdbservice
            ipvs.openfire.user.component.jid.prefix=ipvsuserauthservice          
         */
        JiveGlobals.migrateProperty("provider.auth.className");
        JiveGlobals.migrateProperty("provider.user.className");

        JiveGlobals.migrateProperty("ipvs.component.user.name");
        JiveGlobals.migrateProperty("ipvs.component.user.domains_comma_separated");
        JiveGlobals.migrateProperty("ipvs.component.pubsub.name");

        JiveGlobals.migrateProperty("ipvs.component.user.pwd_read.superuser");
        JiveGlobals.migrateProperty("ipvs.component.user.pwd_read.managementserver");
        
        JiveGlobals.migrateProperty("ipvs.component.user.timeout.pass");
        JiveGlobals.migrateProperty("ipvs.component.user.timeout.user");
        
        timeout_pass = JiveGlobals.getLongProperty("ipvs.component.user.timeout.pass", timeout_pass);
        timeout_users = JiveGlobals.getLongProperty("ipvs.component.user.timeout.user", timeout_users);

        String domains = JiveGlobals.getProperty("ipvs.user.component.domains_comma_separated", "default");
        String[] arr = domains.split(",");
        for (int i = 0; i < arr.length; i++) {
            String domain = arr[i].trim();
            if (!domain.equals(""))
                listDomains.add(domain);
        }
        action = "readList";
        nid = "userlist";
        deviceUsersNID = "assetresourcelist";
        String pubsub_jid = JiveGlobals.getProperty("ipvs.component.pubsub.name", "ipvsdbservice");
        if (pubsub_jid.endsWith("localhost") || pubsub_jid.endsWith("127.0.0.1"))
            to = pubsub_jid;
        else
            to = pubsub_jid + "." + XMPPServer.getInstance().getServerInfo().getXMPPDomain();
        jid = comp.getAddress().toBareJID();
        comp.setDbcomponent(to);

        String read_super_user_pwd_str = JiveGlobals.getProperty("ipvs.component.user.pwd_read.superuser", "false").trim();
        if (read_super_user_pwd_str.equals("true")) {
            read_super_user_pwd = true;
        }
        String read_ms_user_pwd_str = JiveGlobals.getProperty("ipvs.component.user.pwd_read.managementserver", "false").trim();
        if (read_ms_user_pwd_str.equals("true")) {
            read_ms_user_pwd = true;
        }
        int processInThreadCorePoolSize = 1;
        int processInThreadMaximumPoolSize = 1;
        int processInThreadKeepAliveTime = 60;

        threadPool = new ThreadPoolExecutor(processInThreadCorePoolSize, processInThreadMaximumPoolSize, processInThreadKeepAliveTime, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(), new ThreadPoolExecutor.CallerRunsPolicy());

        try{
            new CreateServerStartedStatus().start();
        } catch (Exception e) {
        }
        //threadPool.execute(new ReadAllUsersMonitor());

    }

    //////////////////////////////////////////////////////////// Notifications //////////////////////////////////////////////////
    public void processNotifications(Packet packet) {
        Log_error("IpvsUserComponent processNotifications(Packet packet) " + packet.toXML());
        if (packet instanceof IQ) {
            IQ reply = (IQ) packet;
            processReply(reply);
        } else if (packet instanceof Presence) {
            /*Presence reply = (Presence) packet;
            if( reply.getType().equals(Presence.Type.error)){
            } else  if( reply.getType().equals(Presence.Type.subscribe)){
                if(reply.getFrom().toString().equals(to)){
                    comp.subscribeToPresence(jid, to, timeout_users);
                }
            } else  if( reply.getType().equals(Presence.Type.subscribed)){
                
            } else  if( reply.getType().equals(Presence.Type.unavailable)){ 
                
            } else  if( reply.getType().equals("")){ 
                
            }*/
        } else if (packet instanceof Message) {
            /*
             <message to="ipvs.user.component.localhost" from="ipvs.pubsub.localhost" 
             id="default.userlist__ipvs.user.component.localhost__1uV2V">
             <event xmlns="http://jabber.org/protocol/pubsub#event">
             <items><item id="default.userlist.ul_4b954edc-8521-416e-936d-a8b6398b27d3"><x xmlns="jabber:x:data" type="result"><field var="FORM_TYPE" type="hidden"><value>http://jabber.org/protocol/pubsub#meta-data</value></field>
             <field var="pubsub#title" type="text-single" label="Short name for the node">
             <value>default.userlist.ul_4b954edc-8521-416e-936d-a8b6398b27d3</value></field>
             <field var="pubsub#description" type="text-single" label="Description of the node"><value>default.userlist.ul_4b954edc-8521-416e-936d-a8b6398b27d3</value></field><field var="ipvs_addupdate" type="text-single" label="IPVS AddUpdate"><value>udpate</value></field><field var="ipvs_revision" type="text-single" label="ipvs_revision"><value>91</value></field></x></item></items></event></message>
             */
            /*
                DataForm authForm = (DataForm) reply.getExtension("x", "jabber:x:data");
                FormField NIDField= authForm.getField("pubsub#title");
                List<String> values = NIDField.getValues();
                String NID="";
                if(values.size()>0){
                    NID= values.get(0);
                }
             */
            /*
             <message to="ipvs.user.component.localhost" 
             from="ipvs.pubsub.localhost" 
             id="default.userlist__ipvs.user.component.localhost__j9n4c">
             <event xmlns="http://jabber.org/protocol/pubsub#event">
             <delete node="default.userlist.ul_4b954edc-8521-416e-936d-a8b6398b27d3" 
             ipvs_revision="694"/></event></message>
             */
            Message reply = (Message) packet;
            Element eventEL = reply.getChildElement("event", "http://jabber.org/protocol/pubsub#event");
            if (eventEL != null) {
                Element deleteEL = eventEL.element("delete");
                if (deleteEL != null) {
                    String NID = deleteEL.attributeValue("node");
                    if (NID != null) {
                        userDeleted(NID);
                    }
                }
            } else {
                eventEL = reply.getChildElement("updateEvent", IpvsUsersIQ.MEDIAROOM_NAMESPACE);
                if (eventEL != null) {
                    String updateType = eventEL.attributeValue("type");
                    String NID = eventEL.attributeValue("nodeID");
                    MRDebug.log("NID " + updateType + " " + NID + "");
                    if (NID != null && updateType != null && updateType.equalsIgnoreCase("delete")) {
                        userDeleted(NID);
                    }

                }
            }

        }

    }

    ///////////////////////// iq ////////////////////

    ///////////////////////// private ////////////////////
    private void userDeleted(String NID) {
        String userName = userNIDSCache.get(NID);
        if (userName != null) {
            MRDebug.log("userDeleted " + userName + " " + NID + "");
            User user;
            try {
                synchronized (usersCache) {
                    usersCache.remove(userName);
                }
                try{
                	Collection<ClientSession> sessions =  SessionManager.getInstance().getSessions(userName);
                	for (ClientSession clientSession : sessions) {
                		try{
	                		if(!clientSession.isClosed()){
	                			MRDebug.log("deleted user, logging out user  " + userName + " " + NID + "" );
	                			clientSession.close();
	                		} 
                		}catch(Exception ex){
                			MRDebug.log("userDeleted " + userName + " " + NID + "" , ex);
                        }
					}
                }catch(Exception ex){
                	MRDebug.log("userDeleted " + userName + " " + NID + "", ex);
                }
                //MRDebug.log("Raise user user_deleting event ");
                // Gets current user
                user = UserManager.getInstance().getUser(userName);
                Map<String, Object> params = Collections.emptyMap();
                UserEventDispatcher.dispatchEvent(user, UserEventDispatcher.EventType.user_deleting, params);
            } catch (UserNotFoundException e) {
                // It's ok, user doesn't exist, so deleting it is nothing
            }
        }
        //nidUsersCache.remove(userName);
        userNIDSCache.remove(NID);
    }
    
    private void printStackStrace(Exception ex, StringBuffer buf) {
        StringWriter sw = new StringWriter();
        if(ex !=null){
            ex.printStackTrace(new PrintWriter(sw));
        }
        buf.append(sw.toString());
    }
    
    public List<?> getIQResponse(String type, String NID, String username, String query, String serviceDomain, String listName, String whereXPath, Element dataEl, String caller) throws Exception {
    	IQ reply = null;
    	List<?> list = null;
    	IpvsUsersIQ packet = new IpvsUsersIQ();
    	packet.setFrom(jid);
        packet.setTo(to);
        packet.addPubsubElement(type, NID, jid, serviceDomain, action, query, whereXPath, 1, -1, dataEl);
        reply = comp.query(packet, caller + " " + username + " " + query + "(" + packet.getID() + ") " + System.currentTimeMillis(), timeout_pass);
        
        if (reply != null) {
        	PacketError error = reply.getError();
            //if error -- return null
            if (error != null) {
                MRDebug.log("ERROR: " +  caller + username + " " + error.toXML());
                //errorBuf.append("" + error.getText());
                throw new Exception( error.getText()); 
            }                        
            //process packet
            Element pubsub = reply.getChildElement();
            if (pubsub != null) {
                Element read = pubsub.element(type);
                if (read != null) {
                    Element dbresp = read.element("DBResp");
                    if (dbresp != null) {
                        list = dbresp.elements(listName);
                        
                    }
                }
            }
        }
    	return list;
    }
    
    public String getPassword_ipvs(String userName, String query) throws Exception {
        //MRDebug.log("\n\n ","getPassword_ipvs start "  + userName +" " + query);
        String password = "";
        if(userName==null || userName.trim().equals("")) {
            return null;
        }
        StringBuffer errorBuf = new StringBuffer();
        userName=userName.trim();
        boolean readFirstPwd = isFirstReadFromDB(userName);
        IQ reply = null;
        try {
            if (XMPPServer.getInstance().isShuttingDown())
                return null;
            if (!XMPPServer.getInstance().isStarted())
                return null;
            if (!initHardcoded) {
                loadUsersHarcoded();
                initHardcoded = true;
            }
            //if (read_super_user_pwd) {
            //    MRDebug.log("readFirstPwd " + readFirstPwd + " read_super_user_pwd " + read_super_user_pwd);
            //}
            if (!readFirstPwd) {
                synchronized (passwordsHardcoded) {
                    if (passwordsHardcoded.containsKey(userName)) {
                        return passwordsHardcoded.get(userName);
                    }
                }
            }
            synchronized (usersCache) {
                usersCache.clear();
            }
            for (Iterator<String> iterator = listDomains.iterator(); iterator.hasNext();) {
                String serviceDomain = iterator.next();
                try {
                	List<?> usersList = null;
                	boolean regularUser= false;       
                	
                	usersList = getIQResponse(TYPE_READ, serviceDomain + "." + nid, userName, query, serviceDomain, LISTNAME_USERS, query, null, "getPassword_ipvs");
                	if(usersList!= null && !usersList.isEmpty()){
                    	regularUser = true;
                    }
                    
                    //if its not a regular user then find in device users list
                    if(!regularUser) {
                    	String deviceQuery = "//AssetResource[Info/Title='"+userName+"' and BotUser/@enable='true']";
                    	usersList = getIQResponse(TYPE_READ, serviceDomain + "." + deviceUsersNID, userName, deviceQuery, serviceDomain, LISTNAME_ASSET, deviceQuery, null, "getPassword_ipvs");
                    }
                	
                    if (readFirstPwd) {
                        // if super user is not returned from db and no error can return hard coded password.   
                        if (usersList.size() == 0) {
                            synchronized (passwordsHardcoded) {
                                if (passwordsHardcoded.containsKey(userName)) {
                                    return passwordsHardcoded.get(userName);
                                }
                            }
                        }
                    }
                    
                    for (Iterator<?> iterator2 = usersList.iterator(); iterator2.hasNext();) {
                        Element user = (Element) iterator2.next();
                        
                        String userNameFromDbReply = "";
                        password  = "";
                        String email = "";
                        MRDebug.log("user"+ user.asXML());
                        if(!regularUser) {
                        	Element infoElement = user.element("Info");
                        	Element assetPrivateKeyEl  = infoElement.element("AssetPrivateKey");
                        	userNameFromDbReply = infoElement.elementTextTrim("Title");
                        	password = assetPrivateKeyEl.elementTextTrim("KeyData");
                        } else {
                        	userNameFromDbReply = user.elementTextTrim("Username");
                        	password = user.elementTextTrim("Password");
                        	email = user.elementTextTrim("Email");
                        }                        
                         //todo: convert to date.
                        //String creationDate = user.elementTextTrim("CreationDate");
                        //String modifiedDate = user.elementTextTrim("ModifiedDate");                                        
                        String NID = user.attributeValue("NID");
                        if (NID != null) {
                            userNIDSCache.put(NID, userNameFromDbReply);
                        }
                        synchronized (usersCache) {
                            usersCache.put(userName, new User(userNameFromDbReply, userNameFromDbReply, email, startDate, startDate));
                        }
                        if (userNameFromDbReply.equals(userName)) {
                            return password;
                        }
                    }               	
           
                } catch (Exception ex) {
                    Log_error(" getPassword_ipvs   " + userName +" "+ serviceDomain, ex);
                    errorBuf.append(" serviceDomain " + serviceDomain + " ");
                    printStackStrace( ex, errorBuf);
                }
            }

            /*
            if (readFirstPwd) {
                //should never come here.  if super user is not returned from db and no error can return hard coded password.    
                synchronized (passwordsHardcoded) {
                    if (passwordsHardcoded.containsKey(userName)) {
                        return passwordsHardcoded.get(userName);
                    }
                }
            } */

        } catch(Exception ex){
            Log_error(" getPassword_ipvs   " + userName , ex);
            printStackStrace( ex, errorBuf);
        } finally {
            //MRDebug.log("getPassword_ipvs end "   + userName +" - " +Password +" " + query + "\n\n ");
        }
        
        if(reply !=null) {
            //MRDebug.log("(getPassword_ipvs) DID NOT FIND USER ? '" + userName + "' reply= "+ reply.toXML() +" "+ errorBuf.toString());
            MRDebug.log("(getPassword_ipvs) DID NOT FIND USER ? '" + userName +"' REPLY NOT NOT. "+ errorBuf.toString());
            throw new UserNotFoundException("ERROR: Did not find user '" + userName + "'." +" "+ errorBuf.toString());
        } else {
            MRDebug.log("ERROR: (getPassword_ipvs) DB READ FAILED ? '" + userName + "' REPLY IS NULL. " +" "+ errorBuf.toString());
            throw new UserNotFoundException("ERROR: Database read failed or timed out. Did not find user '" + userName + "'." +" "+ errorBuf.toString());
        }
    }

    private Collection<String> getUsernames_ipvs() throws Exception {
    	String deviceUsersQuery = "//AssetResource[BotUser/@enable='true']"; 
        return getUsernames_ipvs("", deviceUsersQuery,0, -1);
    }

    private Collection<String> getUsernames_ipvs(String regularUserQuery, String deviceUserQuery,int startfrom, int countToFetch) throws Exception {
        //MRDebug.log("\n\n" , "getUsernames_ipvs start query " + query +" startfrom "+ startfrom+" countToFetch "+countToFetch);
        //MRDebug.log("\n\n" , "");
        List<String> usernames = new ArrayList<String>();
        boolean found = false;
        try {
            if (XMPPServer.getInstance().isShuttingDown())
                return null;
            if (!XMPPServer.getInstance().isStarted())
                return null;
            if (!initHardcoded) {
                loadUsersHarcoded();
                initHardcoded = true;
            }

            //synchronized (users) {
            Hashtable<String, User> users = getUsersList_ipvs(regularUserQuery, deviceUserQuery, startfrom, countToFetch);
            if(!users.isEmpty()) {
            	found = true;
            }                    

            int end = startfrom + countToFetch;
            int icounter = 0;
            for (Enumeration<String> e = usersHardcoded.keys(); e.hasMoreElements();) {
                if (icounter < startfrom)
                    continue;
                if (icounter > end)
                    break;
                usernames.add(e.nextElement());
                icounter++;
            }

            for (Enumeration<String> e = users.keys(); e.hasMoreElements();) {
                if (icounter < startfrom)
                    continue;
                if (icounter > end)
                    break;
                usernames.add(e.nextElement());
                icounter++;
            }
            if (!found) {
                MRDebug.log(" getUsernames_ipvs " + usernames.size() );//+ " query " + query);
            }
            return usernames;
        } finally {
            //MRDebug.log("getUsernames_ipvs end "+ usernames.size() + " "+ query +" startfrom "+ startfrom+" countToFetch "+countToFetch + "\n\n ");
        }
    }
    
    
    private Hashtable<String, User> getUsersList_ipvs(String regularUserQuery, String deviceUserQuery,int startfrom, int countToFetch) throws Exception {
        //MRDebug.log("\n\n" , "getUsernames_ipvs start query " + query +" startfrom "+ startfrom+" countToFetch "+countToFetch);
        //MRDebug.log("\n\n" , "");
        try {
  
            //synchronized (users) {
            Hashtable<String, User> users = new Hashtable<String, User>();
            for (Iterator<String> iterator = listDomains.iterator(); iterator.hasNext();) {
                String serviceDomain = iterator.next();
                try {
                	
                	List<?> deviceUsersList = getIQResponse(TYPE_READ, serviceDomain + "." + deviceUsersNID, "", deviceUserQuery, serviceDomain, LISTNAME_ASSET, deviceUserQuery, null, "getUsernames_ipvs");
                	for (Iterator<?> iterator2 = deviceUsersList.iterator(); iterator2.hasNext();) {
                        Element deviceUser = (Element) iterator2.next();                                       
                        Element infoElement = deviceUser.element("Info");
                      	String Username = infoElement.elementTextTrim("Title");
                        String Email = "";
                        String NID = deviceUser.attributeValue("NID");
                        if (NID != null) {
                            userNIDSCache.put(NID, Username);                                          
                        }
                        users.put(Username, new User(Username, Username, Email, startDate, startDate));
                    }
                    
                    //get regular users
                      List<?> regularUsersList = getIQResponse(TYPE_READ, serviceDomain + "." + nid, "", regularUserQuery, serviceDomain, LISTNAME_USERS, regularUserQuery, null, "getUsernames_ipvs");
                      for (Iterator<?> iterator2 = regularUsersList.iterator(); iterator2.hasNext();) {
                          Element user = (Element) iterator2.next();
                          /*
                            <Username>adminuser</Username>
                           <Password>adminuser</Password>
                           <Email/>
                           <Name>adminuser</Name>
                           <CreationDate/>
                           <ModifiedDate/>
                           */
                          String Username = user.elementTextTrim("Username");
                          String Email = user.elementTextTrim("Email");
                          String NID = user.attributeValue("NID");
                          if (NID != null) {
                              userNIDSCache.put(NID, Username);
                              //nidUsersCache.put(Username, NID);
                          }
                          //todo: convert to date.
                          //String creationDate = user.elementTextTrim("CreationDate");
                          //String modifiedDate = user.elementTextTrim("ModifiedDate");
                          users.put(Username, new User(Username, Username, Email, startDate, startDate));
                          //MRDebug.log("getUsernames_ipvs  "+ usernames.size() + " Username "+ Username +" Email "+ Email);

                      }
                   
                } catch (Exception ex) {
                    Log_error(" getUsernames_ipvs   "  +regularUserQuery + " " + serviceDomain, ex);
                }
            }           
            return users;
        } finally {
            //MRDebug.log("getUsernames_ipvs end "+ usernames.size() + " "+ query +" startfrom "+ startfrom+" countToFetch "+countToFetch + "\n\n ");
        }
    }
  
    private Hashtable<String, User> getUsers_ipvs(String regularUserQuery, String deviceUserQuery,int startfrom, int countToFetch) throws Exception {
        //MRDebug.log("\n\n" , "getUsernames_ipvs start query " + query +" startfrom "+ startfrom+" countToFetch "+countToFetch);
        //MRDebug.log("\n\n" , "");
        Hashtable<String, User> users = new Hashtable<String, User>();
        boolean found = false;
        try {
            if (XMPPServer.getInstance().isShuttingDown())
                return null;
            if (!XMPPServer.getInstance().isStarted())
                return null;

            if (!initHardcoded) {
                loadUsersHarcoded();
                initHardcoded = true;
            }
            //synchronized (users) {
            int end = startfrom + countToFetch;
            int icounter = 0;
            for (User user : usersHardcoded.values()) {
                if (icounter < startfrom)
                    continue;
                if (icounter > end)
                    break;
                users.put(user.getName(), user);
                icounter++;
            }
            users = getUsersList_ipvs(regularUserQuery, deviceUserQuery, startfrom, countToFetch);
          
            if (!found) {
                MRDebug.log(" getUsers_ipvs " + users.size() );//+ " query " + query);
            }
            return users;
        } finally {
            //MRDebug.log("getUsernames_ipvs end "+ query +" startfrom "+ startfrom+" countToFetch "+countToFetch);
        }
    }

    ///////////////////
    private int getUserCount_ipvs() {
        try {
            Collection<String> users = getUsernames_ipvs();
            if (users != null) {
                return users.size();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return usersHardcoded.size();
    }

    private User loadUser_ipvs(String userName, String query) throws Exception {
        try {
            if (!initHardcoded) {
                loadUsersHarcoded();
                initHardcoded = true;
            }
            if (usersHardcoded.containsKey(userName)) {
                return usersHardcoded.get(userName);
            }
            synchronized (usersCache) {
                if (usersCache.containsKey(userName)) {
                    return usersCache.get(userName);
                }
            }
        	String deviceUsersQuery = "//AssetResource[Info/Title='"+userName+"' and BotUser/@enable='true']"; 
            
            Collection<String> users = getUsernames_ipvs(query,deviceUsersQuery, 0, 1);
            if (users != null) {
                for (String user : users) {
                	if(users.size() == 1 || user.equals(userName)) {
	                    return new User(user, user, user, startDate, startDate);
                	}
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new User(userName, userName, userName, startDate, startDate);
    }

    @SuppressWarnings("rawtypes")
    private String getQuery(Set fields, String query) {
        String[] filters = query.split("*");
        StringBuffer buf = new StringBuffer(100);
        boolean addOr = false;
        for (int i = 0; i < filters.length; i++) {
            String filter = filters[i].trim();
            if (!filter.equals("")) {
                if (addOr)
                    buf.append("or ");
                else
                    addOr = true;
                for (Iterator iterator = fields.iterator(); iterator.hasNext();) {
                    String field = (String) iterator.next();
                    buf.append("contains (  string( " + field + " )   ,  '" + filter + "')");
                }
            }
        }
        return buf.toString();
    }

    ///////////////////////////////////////////////////////////// /////////////////////////////
    ///////////////////////////////////////////////////////////// /////////////////////////////
    ///////////////////////////////////////////////////////////// /////////////////////////////
    @Override
    public User loadUser(String username) throws UserNotFoundException {
        try {
            String query = "//User[Username='" + username + "']";
            User user = loadUser_ipvs(username, query);
            if (user != null) {
                return user;
            }
        } catch (Exception ex) {
            Log_error(" loadUser   " + username, ex);
        }
        return null;
    }

    @Override
    public int getUserCount() {
        return getUserCount_ipvs();
    }

    @Override
    public Set<String> getSearchFields() throws UnsupportedOperationException {
        return new LinkedHashSet<String>(Arrays.asList("Username", "Name", "Email"));
    }

    @Override
    public Collection<String> getUsernames() {
        try {
        	String deviceUsersQuery = "//AssetResource[BotUser/@enable='true']";
        	String regularUserQuery = "";
            return getUsernames_ipvs(regularUserQuery, deviceUsersQuery,0, -1);
        } catch (Exception ex) {
            Log_error("getUsers", ex);
        }
        return new ArrayList<String>();
    }

    @SuppressWarnings("unchecked")
    @Override
    public Collection<User> getUsers() {
        Collection<String> usernames = getUsernames();
        return new UserCollection(usernames.toArray(new String[usernames.size()]));
    }

    @Override
    public Collection<User> getUsers(int startIndex, int numResults) {
        try {
           	String deviceUsersQuery = "//AssetResource[BotUser/@enable='true']";
        	String regularUserQuery = "";
     
        	return getUsers_ipvs(regularUserQuery,deviceUsersQuery, startIndex, numResults).values();
        } catch (Exception ex) {
            Log_error("getUsers", ex);
        }
        return new ArrayList<User>();
    }

    @Override
    public Collection<User> findUsers(Set<String> fields, String pquery) throws UnsupportedOperationException {
        String query = getQuery(fields, pquery);
        try {
          	String deviceUsersQuery = "";
        	return getUsers_ipvs(query,deviceUsersQuery, 1, -1).values();
        } catch (Exception ex) {
            Log_error("findUsers", ex);
        }
        return new ArrayList<User>();
    }

    @Override
    public Collection<User> findUsers(Set<String> fields, String pquery, int startIndex, int numResults) throws UnsupportedOperationException {
        String query = getQuery(fields, pquery);
        try {
        	String deviceUsersQuery = "";
            return getUsers_ipvs(query,deviceUsersQuery, startIndex, numResults).values();
        } catch (Exception ex) {
            Log_error("findUsers indexed", ex);
        }
        return new ArrayList<User>();
    }

    ///////////////////////////////////

    private boolean processReply(IQ reply) {
        boolean gotResults = false;
        try {
            if (reply != null && reply instanceof IQ) {
            } else {
                //MRDebug.log("notification "+ reply.toXML().replace('\n',' '));
            }
        } catch (Exception ex) {
            Log_error("getReply", ex);
        }
        return gotResults;
    }

    ////////
    private boolean isFirstReadFromDB(String userName) {
        if (userName.equals("admin") && read_super_user_pwd && passwordsHardcoded.containsKey(userName)) {
            return true;
        }
        return false;
    }

    private synchronized void loadUsersHarcoded() {
        if (initHardcoded) {
            return;
        }
        try {
            //if(usersHardcoded==null){
            usersHardcoded = new Hashtable<String, User>();
            passwordsHardcoded = new Hashtable<String, String>();
            if (!read_ms_user_pwd) {
            	User user = new User("managementserver", "managementserver", "managementserver", startDate, startDate);
                usersHardcoded.put("managementserver", user);
                passwordsHardcoded.put("managementserver", "msadmin");
            }
            String username = "admin";
            User user = new User(username, username, username, startDate, startDate);
            usersHardcoded.put(username, user);
            passwordsHardcoded.put(username, username);
            
            //comp.subscribeToPresence(jid, to, timeout_users);
        } finally {
            initHardcoded = true;
        }
    }

    private Element getBotUser(String username, String query) throws Exception {
    	Element deviceUser = null;
    	
    	for (Iterator<String> iterator = listDomains.iterator(); iterator.hasNext();) {
            String serviceDomain = iterator.next();
            try {
            	
            	List<?> deviceUsersList = getIQResponse(TYPE_READ, serviceDomain + "." + deviceUsersNID, username, query, serviceDomain, LISTNAME_ASSET, query, null, "getBotUser");
            	for (Iterator<?> iterator2 = deviceUsersList.iterator(); iterator2.hasNext();) {
                    deviceUser = (Element) iterator2.next();                                       
                    Element infoElement = deviceUser.element("Info");
                  	String Username = infoElement.elementTextTrim("Title");
                    String Email = "";
                    String NID = deviceUser.attributeValue("NID");
                    if (NID != null) {
                        userNIDSCache.put(NID, Username);                                          
                    }
                    
                    if(Username.equals(username)) { return deviceUser;}
                    
                }
                
            } catch (Exception ex) {
                Log_error(" getUsernames_ipvs   "  + query + " " + serviceDomain, ex);
            }
        } 
    	return null;
    }
    
	
    public User updateAssetResource(Document deviceBotUserDoc, String NID, String username, String password, String deviceUsersQuery, String action) throws Exception {
    	XMLUtils.setValueXpath(deviceBotUserDoc, "//Info/Title", username);
    	XMLUtils.setValueXpath(deviceBotUserDoc, "//AssetPrivateKey/KeyData", password);
		
		for (Iterator<String> iterator = listDomains.iterator(); iterator.hasNext();) {
            String serviceDomain = iterator.next();
            try {
            	
                if(TYPE_ADD.equals(action)) {
        			String parentNID = serviceDomain + "." + deviceUsersNID;
        			Element deviceBotUserEl = deviceBotUserDoc.getRootElement();
        			deviceBotUserEl.addAttribute("parentNID", parentNID);
        			deviceBotUserEl.addAttribute("NID", serviceDomain + "." + NID);
        			deviceBotUserEl.element("BotUser").addAttribute("enable", "true");
        			XMLUtils.setValueXpath(deviceBotUserDoc, "//Info/Type", ANY);
	            	XMLUtils.setValueXpath(deviceBotUserDoc, "//State/StateFlag", STATE_AUTOREGISTERED);
	            	NID = serviceDomain + "." + NID;
        		}
                List<?> deviceUsersList = getIQResponse(action, NID, username, deviceUsersQuery, serviceDomain, LISTNAME_ASSET, "", deviceBotUserDoc.getRootElement(), "updateAssetResource");
                for (Iterator<?> iterator2 = deviceUsersList.iterator(); iterator2.hasNext();) {
                    Element deviceUser = (Element) iterator2.next();                                       
                    Element infoElement = deviceUser.element("Info");
                  	String Username = infoElement.elementTextTrim("Title");
                    String NIDVal = deviceUser.attributeValue("NID");
                    if (NIDVal != null) {
                        userNIDSCache.put(NIDVal, Username);                                          
                    }
                    return new User(Username, Username, Username, startDate, startDate);
                }
            } catch (Exception e) {
            	e.printStackTrace();
            }
		}
    	return null;
    }
    
    ///////////////////// n ////////////////
    @Override
    public User createUser(String username, String password, String name, String email) throws UserAlreadyExistsException {
    	if(username == null || password == null) {
    		return null;
    	}
    	
    	Element deviceBotUser = null;
    	User user = null;
    	String deviceUsersQuery = "//AssetResource[Info/Title='"+username+"' and BotUser/@enable='true']";
    	
		try {
			// Check if user exists
			deviceBotUser = getBotUser(username, deviceUsersQuery);
			if(deviceBotUser != null) {
				// update the user
				Document deviceBotUserDoc = DocumentHelper.createDocument(deviceBotUser.createCopy());
				XMLUtils.removeNameSpace(deviceBotUserDoc.getRootElement());
				String NID = deviceBotUser.attributeValue("NID");
				user = updateAssetResource(deviceBotUserDoc, NID, username, password, deviceUsersQuery, TYPE_REPLACE);
			} else {
				// add new user
				Document deviceBotUserDoc = XMLUtils.loadXMLFile(ASSETXMLFILEPATH);
				if(deviceBotUserDoc != null) {
					String NID = deviceUsersNID + "." + "arl_" + UUID.randomUUID().toString();
					user = updateAssetResource(deviceBotUserDoc, NID, username, password, deviceUsersQuery, TYPE_ADD);
				}
			}
	    } catch (Exception e) {
           e.printStackTrace();
        }
   
		return user;
    }

    @Override
    public void deleteUser(String username) {
    }

    @Override
    public void setCreationDate(String username, Date creationDate) throws UserNotFoundException {
    }

    @Override
    public void setEmail(String username, String email) throws UserNotFoundException {
    }

    @Override
    public void setModificationDate(String username, Date modificationDate) throws UserNotFoundException {
    }

    @Override
    public void setName(String username, String name) throws UserNotFoundException {
    }

    @Override
    public boolean isReadOnly() {
        return false;
    }

    @Override
    public boolean isNameRequired() {
        return false;
    }

    @Override
    public boolean isEmailRequired() {
        return false;
    }

}
