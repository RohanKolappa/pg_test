package com.ipvs.test;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPEvent;
import com.ipvs.common.XMPPEventHandlerI;
import com.ipvs.common.XMPPEventHandlerThread;
import com.ipvs.common.XMPPI;
import com.ipvs.xmpp.client.smack.IPVSSessionSmackImpl;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public class DBTest implements XMPPEventHandlerI {


  public static void testAddNode(String rootNID, XMPPI xmpp) throws Exception {
//    // Add the parent node
//    BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
//    String parentNID = null;
//    System.out.println("Adding Parent Node:" + parentNID);
//    // String NID = xmpp.PubSub_addNodeWithNodeID(rootNID, parentNID);
//    parentNID = xmpp.PubSub_addNode(rootNID);
//    xmpp.PubSub_setData(parentNID, "<data><child>1</child></data>");
//    System.out.println("Added Parent Node hit enter :" + parentNID);
//    stdin.readLine();
//    
//    try {
//      String data = xmpp.PubSub_getData(parentNID);
//      System.out.println("Parent Data:" + data);
//    }
//    catch(Exception e) {
//      System.out.println("Failed to verify parent node");
//    }
//  
//    String cParentNID = parentNID;
//    String childNID = parentNID + ".Child";
//    try {
//      System.out.println("Adding Child Node:" + childNID);
//      String NID = xmpp.PubSub_addNodeWithNodeID(cParentNID, childNID);
//      xmpp.PubSub_setData(childNID, "childdata");
//      System.out.println("Added Child Node hit enter :" + NID);
//      stdin.readLine();
//    }
//    catch(Exception e) {
//      System.out.println("Child node already present:" + childNID);
//    }
//    
//    // Check the child data
//    try {
//      String data = xmpp.PubSub_getData(childNID);
//      System.out.println("Child Data:" + data);
//    }
//    catch(Exception e) {
//      System.out.println("Failed to verify child node");
//    }
//        
//    // Delete the child node
//    System.out.println("Deleting child Node:" + childNID);
//    xmpp.PubSub_deleteNode(childNID);
//    System.out.println("Deleted child Node:" + childNID);
//  
//    // Check the child data
//    try {
//      String data = xmpp.PubSub_getData(childNID);
//      System.out.println("Child Data:" + data);
//    }
//    catch(Exception e) {
//      System.out.println("Verified child node deleted");
//    }
//    
//    // Delete the parent node
//    System.out.println("Deleting Parent Node:" + parentNID);
//    xmpp.PubSub_deleteNode(parentNID);
//    System.out.println("Deleted Parent Node:" + parentNID);
//    
//    // Check the parent data
//    try {
//      String data = xmpp.PubSub_getData(parentNID);
//      System.out.println("Parent Data:" + data);
//    }
//    catch(Exception e) {
//      System.out.println("Verified parent node deleted");
//    }
  }
  
  public static void deleteNode(XMPPI xmpp, String nodeid) {
//    try {
//      xmpp.PubSub_deleteNode(nodeid);
//    }
//    catch(Exception e) {
//      // ignore
//    }
  }

  public static void testAddElement(XMPPI xmpp, String rootNID) throws Exception {
    /*MRLog log =  MRLog.getInstance(MRLog.CONSOLE);
    MRDB db = new MRDB(xmpp, MRLog.getInstance(MRLog.CONSOLE) );
    // TBD initDB
    MRRequest request = MRRequest.createRequest("userJID");

    Document doc = MRXMLUtils.stringToDocument(
        "<MediaRoom NIDScheme='AUTO' Parse='true'>" +
          "<systemtracklist NIDScheme='PARENTNID.test' Parse='true'></systemtracklist>" +
          "<tracklist nodeid='PARENT.ELEMENT' Parse='true'></tracklist>" +
          "<data><name>TestRoom</name></data>" + 
          "<status Parse='true'><name>TestRoom</name></status>" + 
        "</MediaRoom>");

    //String NID = MRXMLDBUtils.addElement(request, log, db, rootNID, doc.getRootElement());
    String NID = MRXMLDBUtils.add(request, log, db, "mediaroomlist", "MediaRoom", rootNID, null, null, doc.getRootElement());
       System.out.println("Added Element:" + NID);
    
    Document rdoc = MRXMLDBUtils.readElement(request, log, db, NID);
    System.out.println(MRXMLUtils.documentToString(rdoc));
   */
    /*
    String selectFilter = "";
    String whereFilter = "";
    String childlistNID = NID + "." + "childlist";
    List<String> cdata = db.getChildrenData(request, childlistNID, selectFilter, whereFilter);
    Iterator<String> i = cdata.iterator();
    System.out.println("Printing children data");
    while(i.hasNext()) {
      System.out.println(i.next());
    }
    System.out.println("Done Printing children data");
    */
   /* 
    MRXMLDBUtils.deleteElement(request, log, db, NID);
    System.out.println("Deleted Element:" + NID);
    */
  }

  public static void testAddObject(XMPPI xmpp, String rootNID) throws Exception {
    /*MRDB db = new MRDB(xmpp,  MRLog.getInstance(MRLog.CONSOLE) );
    // TBD initDB
    MRRequest request = MRRequest.createRequest("userJID");
    MRLog log = MRLog.getInstance(MRLog.CONSOLE);
    String NID = MRXMLDBUtils.addElement(request, log, db, rootNID, 
    		MRXMLUtils.stringToElement("<dummy/>"));
    
    System.out.println("Added Object:" + NID);
    MRXMLDBUtils.deleteElement(request, log, db, NID);
    System.out.println("Deleted Object:" + NID);
    */
  }

  public static void nodeTest(String serverip, int port, String username, String password) 
  throws Exception {
//
//    System.out.println("nodeTest");
//    String rootNID = "root";    
//
//	  XMPPI xmpp = new TestXMPPImpl();
//    // XMPPI xmpp = new IPVSSession();
//	  xmpp.start();
//	  String resource = "applet";
//	  xmpp.login(serverip, port, username, password, resource);
//    System.out.println("Logged in");
//
//    try {
//      System.out.println("Deleting all children");
//      try {
//    	  //findbugs: absolete in new db
//         //xmpp.PubSub_DeleteChildren(null);
//      }
//      catch(Exception e) {
//        e.printStackTrace();
//      }
//      System.out.println("Adding root node=" + rootNID);
//      xmpp.PubSub_addNodeWithNodeID(null, rootNID);
//      String NID = rootNID + ".1";
//      System.out.println("Adding node=" + NID);
//      xmpp.PubSub_addNodeWithNodeID(rootNID, NID);
//      String data = "<room><name>red</name><status>1</status></room>";
//      data = URLEncoder.encode(data, "UTF-8");
//
//      System.out.println("Setting nodedate node=" + NID + " data=" + data);
//      xmpp.PubSub_setData(NID, data);
//      String d = xmpp.PubSub_getData(NID);
//      d = URLDecoder.decode(d, "UTF-8");
//      System.out.println("Reading nodedata node=" + NID + " data=" + d);
//      
//      //  MRDB db = new MRDB("test", xmpp, new MRLog());
//      // xmpp.PubSub_addNodeWithNodeID("test", "test.ServiceRoot.RoomRoot");
//      // DBTest.testAddNode(rootNID, xmpp);
//      // DBTest.testAddObject(xmpp, rootNID);
//      DBTest.testAddElement(xmpp, rootNID);
//    }
//    catch(Exception e) {
//      e.printStackTrace();
//    }
//    
//    xmpp.logout();
//    xmpp.stop();
//    
//    System.out.println("Logged out");
  }

  public void xmppTest(String serverip, int port, 
      String username, String password) 
  throws Exception {

    System.out.println("xmppTest user=" + username);
    XMPPI xmpp = new IPVSSessionSmackImpl();
    xmpp.start();
    String resource = "applet";
    xmpp.login(serverip, port, username, password, resource);
    System.out.println("Logged in");

    String userJID = xmpp.getXMPPUserJid();
    XMPPEventHandlerThread eventHandlerThread = new XMPPEventHandlerThread(
            userJID, xmpp, this,MRLog.getInstance(MRLog.CONSOLE));
    eventHandlerThread.startListening();
      
    BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
    String line = "";
    String roomNID = "test";
    String deviceJID = "deviceuser-miyertest" + "@" + serverip;

/*
    XMPPConnection conn = (XMPPConnection)xmpp.getConnection();
    MultiUserChat.addInvitationListener(conn, new InvitationListener() {
	public void invitationReceived(XMPPConnection conn, String room, String invite, String reason, String password, Message message) {
		System.out.println("Invitation received");
	}
    });
*/
    while (!"quit".equals(line)) {
      System.out.println(">");
      line = stdin.readLine();
      if("create".equals(line)) {
        xmpp.addRoom(roomNID, userJID);
      }
      else if("delete".equals(line)) {
        xmpp.deleteRoom(roomNID);
      }
      else if("check".equals(line)) {
        System.out.println("isAvailable=" + xmpp.isAvailable(roomNID, deviceJID));
      }
      else if("invite".equals(line)) {
        xmpp.sendInvite(roomNID, deviceJID);
/*
	MultiUserChat muc = new MultiUserChat(conn, "test@conference.localhost");
	muc.invite(deviceJID, "noreason");
*/
      }
      else if("send".equals(line)) {
        xmpp.sendMessage(deviceJID, "TestMessage",
        		IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);
/*
	MultiUserChat muc = new MultiUserChat(conn, "test@conference.localhost");
	Message m = muc.createMessage();
	m.setTo("test@conference.localhost/deviceuser-miyertest");
	m.setType(Message.Type.groupchat);
	m.setBody("TestMessage");
	// muc.sendMessage("hello test");
        muc.sendMessage(m);
*/
      }
      else if("join".equals(line)) {
        xmpp.joinRoom(roomNID, username + "@" + serverip);
      }
    }
    eventHandlerThread.stopListening();
    
    xmpp.logout();
    xmpp.stop();
    
    System.out.println("Logged out");
  }

  public void handleXMPPEvent(String userJID, XMPPEvent event) {
    System.out.println("Event=" + event.getMessage());    
  }

  public static void main(String[] args) {
    if(args.length < 2) {
      System.out.println("Usage: com.ipvs.DBTest <server> <user>");
      System.exit(1);
    }
    try {
      String server = args[0];
      String user = args[1];
      DBTest dbtest = new DBTest();
      dbtest.xmppTest(server, 5222, user, user);
    }
    catch(Exception e) {
      StackTraceElement[] st = e.getStackTrace();
      for(int i = 0; i < st.length; i++) {
        System.out.println(st[i].toString());
        if(i > 5)
          break;
      }
      System.out.println(e.toString());
    }
    System.out.println("Done");
  }


}

