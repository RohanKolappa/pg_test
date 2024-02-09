package com.ipvs.xmpp.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.StringTokenizer;

import com.ipvs.common.XMPPI;

import com.ipvs.xmpp.client.smack.IPVSSessionSmackImpl;
 
public class IPVSSessionTest {
	public static void main(String[] args) {
		IPVSSessionTest test = new IPVSSessionTest();
		test.start(args); 
	}
	
	public static final String ADDNODE = "AddNode";
	public static final String ADDNODEWITHNODEID = "AddNodeWithNodeId";
	public static final String DELETENODE = "DeleteNode";
	public static final String GETDATA = "getData";
	public static final String SETDATA = "setData";
	public static final String GETCHILDREN = "getChildren";
	public static final String DELETECHILDREN = "DeleteChildren";
	public static final String NODEEXISTS = "nodeExists";
	public static final String SUBSCRIBENODE = "SubscribeNode";
	public static final String UNSUBSCRIBENODE = "UnsubscribeNode";
	public static final String LISTSUBSCRIPTIONS = "ListSubscriptions";
	public static final String CUSTOMPUBSUBSET = "Custompubsubset";
	public static final String CUSTOMPUBSUBGET = "Custompubsubget";
	public static final String SENDMESSAGE = "SendMessage";
	public static final String SUBSCRIBEPRESENCE = "SubscribePresence";
	public static final String GETSERVERTIMEOFFSET = "GetServerTimeOffset";	
	public static final String GETFILE = "handleSetup";	
	public static final String GETFILESTATUS = "handleGetFileStatus";	
	public static final String SENDFILE = "handleStart";	
	public static final String SENDFILESTATUS = "handleSendFileStatus";	
	public static final String ABORTGETFILE = "handleTeardown";	
	public static final String STOPSENDFILE = "handleStop";	
	String server = null;
	String user = null;
	String resource = null;
	String password = null;
	String userJID = null;
    //IPVSSession xmpp = new IPVSSession();
	int port = 0;
	XMPPI xmpp = new IPVSSessionSmackImpl();
	/*XMPPI xmpp = null;
	
	
	private static XMPPI startXMPPInstance() throws Exception {
        Class<?> c = Class.forName(IPVSConst.getInstance().getXmppClientClass());
        XMPPI xmpp = (XMPPI) c.newInstance();
        xmpp.start(); 
        //todo:
        XMPPI xmpp = new SmackXmppClient();
        xmpp.start();
        return xmpp;
   }	*/
	public void start(String[] args) {
 		if (args.length > 4) {
			try {
				server = args[0];
			} catch (NumberFormatException e) {
				System.err.println("Argument must be an integer");
				System.exit(1);
			}

			try {
				port = Integer.parseInt(args[1]);
			} catch (NumberFormatException e) {
				System.err.println("Argument must be an integer");
				System.exit(1);
			}

			try {
				user = args[2];
			} catch (NumberFormatException e) {
				System.err.println("Argument must be an integer");
				System.exit(1);
			}

			try {
				password = args[3];
			} catch (NumberFormatException e) {
				System.err.println("Argument must be an integer");
				System.exit(1);
			}

			try {
				resource = args[4];
			} catch (NumberFormatException e) {
				System.err.println("Argument must be an integer");
				System.exit(1);
			}
		} else {
			server = "10.1.5.220";
			user = "test";
			password = "test123";
			resource = "IPVSAgent";
			port = 5222;
		}
 		try{
 		    xmpp = new IPVSSessionSmackImpl();//startXMPPInstance();
 		}catch(Exception ex){
 		    ex.printStackTrace();
 		}

		try {
			xmpp.login(server, port, user, password, resource);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		BufferedReader stdin = new BufferedReader(new InputStreamReader(
				System.in));

		while (true) {
			if (!processUserInput(stdin)) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}

	public boolean processUserInput(BufferedReader stdin) {
		boolean result = false;
		String input = null;
		// @SuppressWarnings("unused")
		String response = null;

		try {
			if (System.in.available() > 0) {
				input = stdin.readLine();
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		if (input != null) {
			StringTokenizer st = new StringTokenizer(input);
			response = testfunction(st);
			System.out.println(response);
			result = true;
		}

		return result;
	}

	public String testfunction(StringTokenizer st) {
		String response = "OK";
		String commandName = null; 
		String fileName = "";
		if (st.hasMoreTokens())	commandName = st.nextToken();
		if (commandName == null) return "Error";
		if (commandName.compareToIgnoreCase (SENDFILE)== 0) {
			String toJID = st.nextToken();
			fileName = st.nextToken();
			System.out.println("Sending File " + fileName + " To " + toJID);
			handleStart(toJID, fileName);
			System.out.println("Sent File " + fileName + " To " + toJID);
		} else if (commandName.compareToIgnoreCase (GETFILE)== 0) {
			String fromJID = st.nextToken();
			String destFileName;
			fileName = st.nextToken();
			destFileName = st.nextToken();
			System.out.println("Getting File " + fileName + " From " + fromJID);
			handleSetup(fromJID, fileName, destFileName);
		} else if (commandName.compareToIgnoreCase (ABORTGETFILE)== 0) {
			String fromJID = st.nextToken();
			String destFileName;
			fileName = st.nextToken();
			destFileName = st.nextToken();
			System.out.println("Stop getting File " + fileName + " From " + fromJID);
			handleTeardown(fromJID, fileName, destFileName);
		} else if (commandName.compareToIgnoreCase (STOPSENDFILE)== 0) {
			String toJID = st.nextToken();
			fileName = st.nextToken();
			System.out.println("Stop sending File " + fileName + " To " + toJID);
			handleStop(toJID, fileName);
		} else if (commandName.compareToIgnoreCase (GETFILESTATUS)== 0) {
			String fromJID = st.nextToken();
			String destFileName;
			fileName = st.nextToken();
			destFileName = st.nextToken();
			System.out.println("Getting File Status" + fileName + " From " + fromJID);
			handleGetFileStatus(fromJID, fileName, destFileName);
		} else if (commandName.compareToIgnoreCase (SENDFILESTATUS)== 0) {
			String toJID = st.nextToken();
			fileName = st.nextToken();
			System.out.println("Send File Status" + fileName + " To " + toJID);
			handleSendFileStatus(toJID, fileName);
		}
		return response;
	}

    private void handleStop(String toJID, String fileName) {
		// TODO Auto-generated method stub
		xmpp.sendFileStop (toJID, fileName);
	}

	private void handleTeardown(String fromJID, String srcFileName,
			String destFileName) {
		// TODO Auto-generated method stub
		xmpp.getFileStop (fromJID, srcFileName, destFileName);
	}

	private void handleSetup(String fromJID, String srcFileName,
			String destFileName) {
		// Also send a message to the "fromJID"
		xmpp.getFile (fromJID, srcFileName, destFileName, "Default");
	}
	private void handleGetFileStatus(String toJID, String srcFileName, String destFileName) {
		xmpp.getFileStatus (toJID, srcFileName, destFileName);
	}

	private void handleStart(String toJID, String fileName) {
		xmpp.sendFile (toJID, fileName, "Default");
	}
	private void handleSendFileStatus(String toJID, String fileName) {
		xmpp.sendFileStatus (toJID, fileName);
	}

	public void login(String server, int port, String userJID, String password, String resource) throws Exception {
         xmpp.login(server, port, userJID, password, resource);
        
    }

    public void logout() throws Exception {
        xmpp.logout();
    }
}
