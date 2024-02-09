package com.ipvs.proxy.app;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.InterruptedIOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.sql.Timestamp;
import java.util.Date;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;
import java.util.Timer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.proxy.impl.ProxyClientUtils;
import com.ipvs.proxy.impl.ProxyConstants;


public class ProxyServer implements Runnable {
    private static final int DEFAULTPORT = 4360;
    Hashtable<String, String> pending = null;
    private MRLog log;
    Thread proxyThread = null;
    private int port;
    private ServerSocket server;
    private volatile Socket client;
    private BufferedReader reader;
    private BufferedWriter writer;
    private StringBuffer proxyBuffer;
    private volatile Timestamp lastHeartBeatFromClientTS;
    private static int seq;
    private static String ack="0";
    private ProxyServerHeartBeat ts;
	static Hashtable<String, Integer> responseNameHash = null;
    //Changes for Test Script - Start
    private static String prompt = "ProxyServerReady>>";
    private static PrintStream stdout;
	private static boolean doSocket;
	private static int proxyPort;
	private static String cachedAuthorizeStreamRequest = null;

	//Changes for Test Script - End
    
    public ProxyServer() {
        super();
        log = MRLog.getInstance(MRLog.CONSOLE);
        log.setLogLevel(MRLog.INFO);
        proxyBuffer = new StringBuffer();
        pending = new Hashtable<String, String>();
    }

    public void start() {
        if (proxyThread == null) {
            proxyThread = new Thread(this, "ProxyInput");
            proxyThread.start();
        }
    }

    public synchronized void initialize(StringTokenizer st) {
        if (st.hasMoreTokens()) {
            this.port = Integer.parseInt(st.nextToken());
        } else {
            this.port = DEFAULTPORT;
        }

        if (server == null) {
            try {
                server = new ServerSocket(this.port);
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    public synchronized void send(StringTokenizer st) {
    	
        if (client != null) {
            if (client.isConnected()) {
                String xmlText = ProxyClientUtils.generate(st, log);
                if(xmlText == null) {
                	stdout.println("<Response>Invalid Request</Response>");
                	stdout.println(prompt);
                	return;
                }
                log.addInfo(xmlText);

                
                ProxyClientUtils.socketWrite(writer, xmlText, log);
                 
            } else {
            	log.addInfo("Send : No Connection");
                stdout.println("<Response>No Connection</Response>");
                stdout.println(prompt);
                return;
            }
        } else {
            log.addInfo("Send : No Connection");
            stdout.println("<Response>No Connection</Response>");
            stdout.println(prompt);
            return;
        }
    }

    /**
     * @param args
     */
    public boolean processUserInput(BufferedReader stdin) {
    	
        boolean result = false;
        String input = null;
        String response = null;

        try {
            input = stdin.readLine();
        } catch (IOException e) {
            log.addInfo("ProcessUserInput :" + e.toString());
        }

        if (input != null) {
            StringTokenizer st = new StringTokenizer(input);
            response = testfunction(st);
            if(response == null) {
            	stdout.println("<Response>No Connection</Response>");
            	stdout.println(prompt);
            }
            log.addInfo("ProcessUserInput :" + response);
            result = true;
        }

        return result;
    }

    /**
     * @param args
     */
    private String testfunction(StringTokenizer st) {
        String fname = null;

        boolean isConnection = true;
        
        if (st.hasMoreTokens()) {
            fname = st.nextToken();
        } else {
            return fname;

        }


        if (fname.compareToIgnoreCase(ProxyConstants.QUIT) == 0) {
            System.exit(0);
        }
        else if((client == null || !client.isConnected()) && !(fname.compareToIgnoreCase(ProxyConstants.BIND) == 0) && !(fname.compareToIgnoreCase(ProxyConstants.STATUS) == 0)) {
        	stdout.println("<Response>No Connection</Response>");
        	stdout.println(prompt);
        	isConnection = false;
        	
        }

        if (fname.compareToIgnoreCase(ProxyConstants.STATUS) == 0) {
        	if(client == null || !client.isConnected()) {
        		stdout.println("<Response>No Connection</Response>");
        	}
        	else  {
        		stdout.println("<Response>Connected</Response>");
        	}
        	stdout.println(prompt);
        		
         }

        
        else if (fname.compareToIgnoreCase(ProxyConstants.VERIFY) == 0) {
            ProxyServer.verify(st, log);
        }

        else if (fname.compareToIgnoreCase(ProxyConstants.BIND) == 0) {
            initialize(st);
        }

        else if (fname.compareToIgnoreCase(ProxyConstants.GENERATE) == 0) {
            ProxyClientUtils.generate(st, log);
        }

        else if (fname.compareToIgnoreCase(ProxyConstants.SEND) == 0 ) {
        	if(isConnection) {
        		send(st);
        	}
        }
        else if (fname.compareToIgnoreCase("waitforauthrequest") == 0 ) {
        	while(cachedAuthorizeStreamRequest==null) {
        		
        	}
        	stdout.println(cachedAuthorizeStreamRequest);
        	cachedAuthorizeStreamRequest = null;
        	stdout.println(prompt);
        }

        else {
        	if(isConnection) {
        		stdout.println("<Response>Invalid Request</Response>");
        		stdout.println(prompt);
        	}
        }
        return fname;
    }

    public synchronized void accept() {
        try {
            this.client = server.accept();
            	
        } catch (IOException e) {
            log.addInfo(e.toString());
            this.client = null;

            return;

            // TODO Auto-generated catch block
        }

        log.addInfo("Got a Connection");

        if (this.client.isConnected()) {
        	if(ProxyClientUtils.getHeartbeatFrequency() > 0) {
            	ts = new ProxyServerHeartBeat(this, log);
            	Timer timer  = new Timer();
            	timer.scheduleAtFixedRate(ts, new Date(), ProxyClientUtils.getHeartbeatFrequency());
            	lastHeartBeatFromClientTS = new Timestamp(System.currentTimeMillis());
            }
            try {
                client.setSoTimeout(100);
            } catch (SocketException e) {
                log.addInfo(e.toString());

                return;
            }
        }

        try {
            reader = new BufferedReader(new InputStreamReader(client.getInputStream()));
        } catch (IOException e) {
            log.addInfo(e.toString());
            return;
        }

        try {
            writer = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));
        } catch (IOException e) {
            log.addInfo(e.toString());

            return;
        }

        proxyBuffer.setLength(0);
    }

    private synchronized boolean processProxyClientInput() {
        String line = null;
        boolean result = true;

        if (server == null) {
            return false;
        }

        if (client == null || (ts != null && ts.getClient() == null)) {
        	client = null;
        	lastHeartBeatFromClientTS = null;
            accept();
        }

        if (client.isBound()) {
        	
            try {
            	//reader = new BufferedReader(new InputStreamReader(client.getInputStream())); 
                line = reader.readLine();
                
            } catch (InterruptedIOException e1) {
                result = false;
            } catch (IOException e) {
                // Reconnect
                log.addInfo("processProxyInput : Reconnecting" + e.toString());
                client = null;
                result = false;
            }
            

            if ((result) && (line == null)) {
                client = null;
                result = false;
            }
        } else {
            client = null;
            result = false;
        }

        if (result) {
            proxyBuffer.append(line);

            Element el = null;

            try {
                el = MRXMLUtils.getRootElement(MRXMLUtils.getDocument(proxyBuffer.toString()));
            } catch (Exception e) {
            }

            if (el != null) {
                //log.addInfo("processProxyClientInput :" + el.getName());
                processProxyMessage(proxyBuffer.toString());
                proxyBuffer.setLength(0);
            }
        }

        // TODO Auto-generated method stub
        return result;
    }

    private void processProxyMessage(String message) {
        //log.addInfo("processProxyMessage : Got an XML verifying with XSD");

    	
    	
        try {
            MRXMLUtils.validateXML(message, ProxyConstants.XSDFILE);
           	if(ProxyServer.getResponseNameHash().containsKey(ProxyClientUtils.getRequestName(message))) {
           		stdout.println(message);
           		stdout.println(prompt);
           	}

        } catch (Exception e) {
            log.addInfo("processProxyMessage :" + e.toString());

            return;
        }
        ProxyServer.printDetails(message, log);
        String requestID = ProxyClientUtils.getRequestId(message);
        String requestName = ProxyClientUtils.getRequestName(message);

        if (requestName.compareToIgnoreCase(ProxyConstants.HEARTBEAT) == 0) {
        	String ack = ProxyClientUtils.getAck(message);
        	ProxyServer.setAck(ack);
        	lastHeartBeatFromClientTS = new Timestamp(System.currentTimeMillis());
        }
        
        if (requestName.compareToIgnoreCase(ProxyConstants.AUTHREQUEST) == 0) {
            pending.put(requestID, message);
            cachedAuthorizeStreamRequest = message;
        }

        if (requestName.compareToIgnoreCase(ProxyConstants.AUTHNOTIFY) == 0) {
            pending.put(requestID, message);
        }
        
        if (requestName.compareToIgnoreCase(ProxyConstants.LISTRESPONSE) == 0) {
            pending.put(requestID, message);
        }
        /*******************************/
        pending.put(requestID, message);
        /******************************/
    }

    /**
     * @param args
     */
    public static void main(String[] args) {
        // TODO Auto-generated method stub
        boolean doSleep = false;
        ProxyServer s = new ProxyServer();

        
        //Modification for Test Script - start
        
        if (args.length > 1 ) {
            doSocket = true;
              try{
                proxyPort = Integer.parseInt(args[1]);
              } catch(Exception ex){
                proxyPort = 5678;
              }
        }
        BufferedReader stdin = null;
        ServerSocket serverSocket = null;
        Socket clientSocket = null;
        InputStream  is = null;
        OutputStream  os = null;
        if (doSocket) {
            try {
              // Create server socket
              serverSocket = new ServerSocket(proxyPort, 5);
              System.out.println("Running TCP server interface on port " + proxyPort + "...");

              // Wait for first connection
              clientSocket = serverSocket.accept();

              // Setup the input output streams
              is = clientSocket.getInputStream();
              os = clientSocket.getOutputStream();
              stdin = new BufferedReader(new InputStreamReader(is));
              stdout = new PrintStream(os);
            }
            catch (IOException ioe) {
              System.out.println("Error creating socket: " + ioe);
              System.exit(1);
            } 
          }
          else {
            stdin = new BufferedReader(new InputStreamReader(System.in));
            stdout = System.out;
          }
          //stdout.println(prompt);
          
          //Modification for Test Script -- End
        
        s.log.addInfo("Started proxyServer:");
        s.Usage();
        s.start();
  
        if (args.length > 0) {
            StringTokenizer st = new StringTokenizer("Bind " + args[0]);
            s.testfunction(st);
        }

        while (true) {
            doSleep = true;

            if (s.processUserInput(stdin)) {
                doSleep = false;
            }

            if (doSleep) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    s.log.addInfo("main :" + e.toString());
                }
            }
        }
    }

    private void Usage() {
    	
    	
        stdout.println("proxyClient [<port>]");
        stdout.println("Commands");
        stdout.println("	bind  <port>");
        stdout.println(
            "	send authorizestreamresponse <userid> <username> <remoteflag yes|no> <requestID> <action Allow|Deny>");
        stdout.println(
        "	send getstreamlistrequest ");
        stdout.println("	send authorizationupdaterequest <destagentJID eg: roomuser-default@10.1.5.185/callcenter>");
        stdout.println(
        "	send startrecordingrequest <userid> <username> <id> <timeoffset>  <patient> <procedure type> <physician> <notes>");
        stdout.println(
        "	send updaterecordingrequest <userid> <username> <id> <timeoffset>  <patient> <procedure type> <physician> <notes>");
        stdout.println(
        "	send stoprecordingrequest <userid> <username> <id> ");
        stdout.println(
        "	send pauserecordingrequest <userid> <username> <id> ");
        stdout.println(
        "	send resumerecordingrequest <userid> <username> <id> ");
        stdout.println(
        "	send recordingstatusrequest <userid> <username> <id> ");
        stdout.println(
        "	send mediatimestamprequest <userid> <username> <id> ");
        stdout.println(
        "	send createbookmarkrequest <userid> <username> <agentname> <id> <timestamp> <notes> ");
        stdout.println(
    	"   waitforauthrequest");
        stdout.println(
    	"   quit ");
        stdout.println(
    	"   status ");        
    	stdout.println(prompt);
    }

    @Override
    public void run() {
        boolean doSleep = false;

        while (true) {
            doSleep = true;

            if (processProxyClientInput()) {
                doSleep = false;
            }

            if (doSleep) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    log.addInfo("main :" + e.toString());
                }
            }
        }
    }

	public Timestamp getLastHeartBeatFromClientTS() {
		return lastHeartBeatFromClientTS;
	}

	public void setLastHeartBeatFromClientTS(
			Timestamp lastHeartBeatFromClientTS) {
		this.lastHeartBeatFromClientTS = lastHeartBeatFromClientTS;
	}

	public static int getSeq() {
		return seq;
	}

	public static void setSeq(int seq) {
		ProxyServer.seq = seq;
	}

	public static String getAck() {
		return ack;
	}

	public static void setAck(String ack) {
		ProxyServer.ack = ack;
	}

	public  MRLog getLog() {
		return log;
	}

	public void setLog(MRLog log) {
		this.log = log;
	}

	public Socket getClient() {
		return client;
	}

	public void setClient(Socket client) {
		this.client = client;
	}

	//????
	public static void verify(StringTokenizer st, MRLog log) {
	    String requestFile = null;
	
	    if (st.hasMoreTokens()) {
	        requestFile = st.nextToken();
	    }
	
	    if (requestFile != null) {
	        String xmlFilePath = null;
	
	        if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHREQUEST) == 0) {
	            xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamrequest.xml";
	        }
	
	        if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHNOTIFY) == 0) {
	            xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamnotification.xml";
	        }
	
	        if (requestFile.compareToIgnoreCase(ProxyConstants.LISTRESPONSE) == 0) {
	            xmlFilePath = ProxyConstants.XMLFILEPATH + "getstreamlistresponse.xml";
	        }
	
	        // These routines requested by the Odyssey Workstation
	        if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHRESPONSE) == 0) {
	            xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamresponse.xml";
	        }
	
	        if (requestFile.compareToIgnoreCase(ProxyConstants.LISTREQUEST) == 0) {
	            xmlFilePath = ProxyConstants.XMLFILEPATH + "getstreamlistrequest.xml";
	        }
	
	        if (xmlFilePath != null) {
	            Document xmldoc = null;
	
	            try {
	                xmldoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	            } catch (Exception e) {
	                log.addInfo("TestFunction :" + e.toString());
	            }
	
	            if (xmldoc != null) {
	                String xmlText = null;
	
	                try {
	                    xmlText = MRXMLUtils.documentToString(xmldoc);
	                } catch (Exception e) {
	                    log.addInfo("TestFunction :" + e.toString());
	                }
	
	                if (xmlText != null) {
	                    try {
	                        MRXMLUtils.validateXML(xmlText, ProxyConstants.XSDFILE);
	                    } catch (Exception e) {
	                        log.addInfo("TestFunction :" + e.toString());
	                    }
	                }
	            }
	        } else {
	            log.addInfo("TestFunction : Invalid file " + requestFile);
	        }
	    }
	}

	public static void displayMediaTimestampResponse(Document parentDoc, MRLog log) {
	    Element parentEL = parentDoc.getRootElement();
	    String requestID = parentEL.element("header").elementText("requestID");
	    String user = parentEL.element("data").element("requestorInfo").elementText("JID");
	    String nickName = parentEL.element("data").element("requestorInfo").elementText("nickName");
	    String id = parentEL.element("data").elementText("ID");
	    String timestamp = parentEL.element("data").elementText("Timestamp");
	
	    log.addInfo("Request ID : " + requestID);
	    log.addInfo("JID : " + user);
	    log.addInfo("Nick Name : " + nickName);
	    log.addInfo("ID : " + id);
	    log.addInfo("Timestamp : " + timestamp);
	}

	public static void displayBookmarkResponse(Document parentDoc, MRLog log) {
	    Element parentEL = parentDoc.getRootElement();
	    String requestID = parentEL.element("header").elementText("requestID");
	    String user = parentEL.element("data").element("requestorInfo").elementText("JID");
	    String nickName = parentEL.element("data").element("requestorInfo").elementText("nickName");
	    String id = parentEL.element("data").element("bookmarkInfo").elementText("ID");
	    String timestamp = parentEL.element("data").element("bookmarkInfo").elementText("Timestamp");
	    String notes = parentEL.element("data").element("bookmarkInfo").elementText("Notes");
	
	    log.addInfo("Request ID : " + requestID);
	    log.addInfo("JID : " + user);
	    log.addInfo("Nick Name : " + nickName);
	    log.addInfo("ID : " + id);
	    log.addInfo("Timestamp : " + timestamp);
	    log.addInfo("Notes : " + notes);
	}

	public static void displayAuthorizationUpdateResponse(Document parentDoc, MRLog log) {
	    Element parentEL = parentDoc.getRootElement();
	
	    String user = parentEL.element("data").element("requestorInfo").elementText("JID");
	    String nickName = parentEL.element("data").element("requestorInfo").elementText("nickName");
	    String sourceAgentName = parentEL.element("data").element("StreamData").element("sourceInfo").elementText("agentName");
	    String sourceJID = parentEL.element("data").element("StreamData").element("sourceInfo").elementText("JID");
	    String sourceNickName = parentEL.element("data").element("StreamData").element("sourceInfo").elementText("nickName");
	    String destAgentName = parentEL.element("data").element("StreamData").element("destInfo").elementText("agentName");
	    String destJID = parentEL.element("data").element("StreamData").element("destInfo").elementText("JID");
	    String destNickName = parentEL.element("data").element("StreamData").element("destInfo").elementText("nickName");
	    String connect = parentEL.element("data").element("StreamData").element("profileInfo").elementText("connect");
	    String remote = parentEL.element("data").element("StreamData").element("profileInfo").elementText("remote");
	
	    log.addInfo("JID : " + user);
	    log.addInfo("Nick Name : " + nickName);
	    log.addInfo("Source Info");
	    log.addInfo("Agent Name : " + sourceAgentName);
	    log.addInfo("JID : " + sourceJID);
	    log.addInfo("Nickname : " + sourceNickName);
	    log.addInfo("Destination Info");
	    log.addInfo("Agent Name : " + destAgentName);
	    log.addInfo("JID : " + destJID);
	    log.addInfo("Nickname : " + destNickName);
	    log.addInfo("Profile Info");
	    log.addInfo("Connect : " + connect);
	    log.addInfo("Remote : " + remote);
	}

	public static String printDetails(String message, MRLog log) {
	    String result = null;
	    Document msgdoc = null;
	    Element el = null;
	    String msgName = null;
	
	    String userJID = null;
	    String dstJID = null;
	    String nickName = null;
	    String dstName = null;
	    String connect = null;
	    String remote = null;
	    String requestID = null;
	
	    try {
	        msgdoc = MRXMLUtils.getDocument(message);
	        el = MRXMLUtils.getRootElement(msgdoc);
	        msgName = el.getName();
	    } catch (Exception e) {
	        log.addInfo("Generate :" + e.toString());
	
	        return null;
	    }
	
	    if ((msgName.compareToIgnoreCase(ProxyConstants.AUTHNOTIFY) == 0)) {
	        displayNotifyResponse(msgdoc, log);
	
	        return null;
	    } else if ((msgName.compareToIgnoreCase(ProxyConstants.LISTRESPONSE) == 0)) {
	        displayListResponse(msgdoc, log);
	
	        return null;
	    } else if ((msgName.compareToIgnoreCase(ProxyConstants.STARTRECORDINGRESPONSE) == 0) || (msgName.compareToIgnoreCase(ProxyConstants.STOPRECORDINGRESPONSE) == 0) ||
	            (msgName.compareToIgnoreCase(ProxyConstants.PAUSERECORDINGRESPONSE) == 0) || (msgName.compareToIgnoreCase(ProxyConstants.RESUMERECORDINGRESPONSE) == 0) ||
	            (msgName.compareToIgnoreCase(ProxyConstants.UPDATERECORDINGRESPONSE) == 0) || (msgName.compareToIgnoreCase(ProxyConstants.RECORDINGSTATUSRESPONSE) == 0)) {
	        displayRecordingResponse(msgdoc, log, msgName);
	
	        return null;
	    } else if (msgName.compareToIgnoreCase(ProxyConstants.RECORDINGNOTIFICATION) == 0) {
	        displayRecordingNotification(msgdoc, log);
	
	        return null;
	    } else if (msgName.compareToIgnoreCase(ProxyConstants.MEDIATIMESTAMPRESPONSE) == 0) {
	        ProxyServer.displayMediaTimestampResponse(msgdoc, log);
	
	        return null;
	    } else if ((msgName.compareToIgnoreCase(ProxyConstants.CREATEBOOKMARKRESPONSE) == 0)) {
	        ProxyServer.displayBookmarkResponse(msgdoc, log);
	
	        return null;
	    } else if ((msgName.compareToIgnoreCase(ProxyConstants.AUTHORIZATIONUPDATERESPONSE) == 0)) {
	        ProxyServer.displayAuthorizationUpdateResponse(msgdoc, log);
	
	        return null;
	    }
	
	    try {
	        if ((msgName.compareToIgnoreCase(ProxyConstants.HEARTBEAT) == 0) || (msgName.compareToIgnoreCase(ProxyConstants.RECORDINGNOTIFICATION) == 0)) {
	            userJID = MRXMLUtils.getValueXpath(el, "//requestorInfo/JID", "unknown");
	            nickName = MRXMLUtils.getValueXpath(el, "//requestorInfo/nickName", "unknown");
	        }
	
	        dstJID = MRXMLUtils.getValueXpath(el, "//destInfo/JID", "unknown");
	        dstName = MRXMLUtils.getValueXpath(el, "//destInfo/nickName", "unknown");
	        connect = MRXMLUtils.getValueXpath(el, "//profileInfo/connect", "unknown");
	        remote = MRXMLUtils.getValueXpath(el, "//profileInfo/remote", "unknown");
	        requestID = MRXMLUtils.getValueXpath(el, "//requestID", "unknown");
	    } catch (Exception e) {
	        e.printStackTrace();
	    }
	
	    if (!(msgName.compareToIgnoreCase(ProxyConstants.HEARTBEAT) == 0)) {
	        log.addInfo("RequestId : " + requestID);
	    }
	
	    if (!((msgName.compareToIgnoreCase(ProxyConstants.HEARTBEAT) == 0) || (msgName.compareToIgnoreCase(ProxyConstants.RECORDINGNOTIFICATION) == 0))) {
	        log.addInfo("UserJId : " + userJID);
	        log.addInfo("UserNickName : " + nickName);
	    }
	
	    if (!((msgName.compareToIgnoreCase(ProxyConstants.RECORDINGNOTIFICATION) == 0) || (msgName.compareToIgnoreCase(ProxyConstants.HEARTBEAT) == 0))) {
	        log.addInfo("DestinationUserJId : " + dstJID);
	        log.addInfo("DestinationUserName : " + dstName);
	        log.addInfo("Connect : " + connect);
	        log.addInfo("Remote Access : " + remote);
	    }
	
	    return result;
	}

	public static void displayListResponse(Document parentEL, MRLog log) {
	    List<?> streamList = null;
	
	    try {
	        //System.out.println("parentEL--------->"+MRXMLUtils.documentToStringOmitDeclaration(parentEL));
	        Element streamListEL = MRXMLUtils.getElementXpath(parentEL, "//StreamInfoList");
	
	        //System.out.println("streamListEL--------->"+MRXMLUtils.elementToString(streamListEL));
	        if (streamListEL != null) {
	            streamList = streamListEL.elements();
	        }
	    } catch (Exception e) {
	        e.printStackTrace();
	    }
	
	    if ((streamList != null) && (streamList.size() != 0)) {
	        Iterator<?> objIT = streamList.iterator();
	        int count = 1;
	
	        while (objIT.hasNext()) {
	            Element streamEL = (Element) objIT.next();
	
	            try {
	                log.addInfo("-------Stream " + count++ + " Details--------");
	                log.addInfo("SourceAgentName : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/sourceInfo/agentName", "unknown"));
	                log.addInfo("SourceAgentJID : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/sourceInfo/JID", "unknown"));
	                log.addInfo("SourceAgentNickName : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/sourceInfo/nickName", "unknown"));
	                log.addInfo("DestinationAgentName : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/destInfo/agentName", "unknown"));
	                log.addInfo("DestinationAgentJID : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/destInfo/JID", "unknown"));
	                log.addInfo("DestinationAgentNickName : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/destInfo/nickName", "unknown"));
	                log.addInfo("Connect : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/profileInfo/connect", "unknown"));
	                log.addInfo("RemoteConnection : " + MRXMLUtils.getValueXpath(streamEL, "StreamData/profileInfo/remote", "unknown"));
	                log.addInfo("State : " + MRXMLUtils.getValueXpath(streamEL, "StreamStatus/state", "unknown"));
	                log.addInfo("StartTime : " + MRXMLUtils.getValueXpath(streamEL, "StreamStatus/startTime", "unknown"));
	            } catch (Exception e) {
	                // TODO Auto-generated catch block
	                e.printStackTrace();
	            }
	        }
	    } else {
	        log.addInfo("NO STREAMS FOUND  IN DATA BASE");
	    }
	}

	public static void displayNotifyResponse(Document messageDoc, MRLog log) {
	    try {
	        log.addInfo("RequestID : " + MRXMLUtils.getValueXpath(messageDoc, "//requestID", "unknown"));
	        log.addInfo("SourceAgentName : " + MRXMLUtils.getValueXpath(messageDoc, "//sourceInfo/agentName", "unknown"));
	        log.addInfo("SourceAgentJID : " + MRXMLUtils.getValueXpath(messageDoc, "//sourceInfo/JID", "unknown"));
	        log.addInfo("SourceAgentNickName : " + MRXMLUtils.getValueXpath(messageDoc, "//sourceInfo/nickName", "unknown"));
	        log.addInfo("DestinationAgentName : " + MRXMLUtils.getValueXpath(messageDoc, "//destInfo/agentName", "unknown"));
	        log.addInfo("DestinationAgentJID : " + MRXMLUtils.getValueXpath(messageDoc, "//destInfo/JID", "unknown"));
	        log.addInfo("DestinationAgentNickName : " + MRXMLUtils.getValueXpath(messageDoc, "//destInfo/nickName", "unknown"));
	        log.addInfo("Connect : " + MRXMLUtils.getValueXpath(messageDoc, "//profileInfo/connect", "unknown"));
	        log.addInfo("RemoteConnection : " + MRXMLUtils.getValueXpath(messageDoc, "//profileInfo/remote", "unknown"));
	        log.addInfo("State : " + MRXMLUtils.getValueXpath(messageDoc, "//StreamStatus/state", "unknown"));
	        log.addInfo("StartTime : " + MRXMLUtils.getValueXpath(messageDoc, "//StreamStatus/startTime", "unknown"));
	    } catch (Exception e) {
	        e.printStackTrace();
	    }
	}

	public static void displayRecordingNotification(Document parentDoc, MRLog log) {
	    Element parentEL = parentDoc.getRootElement();
	    String requestID = parentEL.element("header").elementText("requestID");
	
	    //String timeOffset = parentEL.element("data").element("recordStatus").elementText("TimeOffset");
	    String startTime = parentEL.element("data").element("recordStatus").elementText("StartTime");
	    String endTime = parentEL.element("data").element("recordStatus").elementText("EndTime");
	    String date = parentEL.element("data").element("recordStatus").elementText("Date");
	
	    //String patient = parentEL.element("data").element("recordStatus").elementText("Patient");
	    //String procedureType = parentEL.element("data").element("recordStatus").elementText("ProcedureType");
	    //String physician = parentEL.element("data").element("recordStatus").elementText("Physician");
	    //String notes = parentEL.element("data").element("recordStatus").elementText("Notes");
	    String recordingStatus = parentEL.element("data").element("recordStatus").elementText("Status");
	    String diskSpaceUsed = parentEL.element("data").element("recordStatus").elementText("DiskSpaceUsed");
	    String totalDiskSpaceAvailable = parentEL.element("data").element("recordStatus").elementText("TotalDiskSpaceAvailable");
	    String errorCode = parentEL.element("data").element("recordStatus").elementText("ErrorCode");
	    String errorDescription = parentEL.element("data").element("recordStatus").elementText("ErrorDescription");
	
	    log.addInfo("RequestID : " + requestID);
	    log.addInfo("Start Time : " + startTime);
	    log.addInfo("End Time : " + endTime);
	    log.addInfo("Date : " + date);
	    //log.addInfo("Patient : " + patient);
	    //log.addInfo("Procedure Type : " + procedureType);
	    //log.addInfo("Physician : " + physician);
	    //log.addInfo("Notes : " + notes);
	    log.addInfo("Recording Status : " + recordingStatus);
	    log.addInfo("Disk Space Used : " + diskSpaceUsed);
	    log.addInfo("Disk Space Available : " + totalDiskSpaceAvailable);
	    log.addInfo("Error Code : " + errorCode);
	    log.addInfo("Error Description : " + errorDescription);
	}

	public static void displayRecordingResponse(Document parentDoc, MRLog log, String msgName) {
	    Element parentEL = parentDoc.getRootElement();
	    String requestID = parentEL.element("header").elementText("requestID");
	    String user = parentEL.element("data").element("requestorInfo").elementText("JID");
	    String nickName = parentEL.element("data").element("requestorInfo").elementText("nickName");
	    String agentName = parentEL.element("data").element("requestorInfo").elementText("agentName");
	    String id = parentEL.element("data").element("recordStatus").elementText("ID");
	    String startTime = parentEL.element("data").element("recordStatus").elementText("StartTime");
	    String endTime = parentEL.element("data").element("recordStatus").elementText("EndTime");
	    String date = parentEL.element("data").element("recordStatus").elementText("Date");
	    String recordingStatus = parentEL.element("data").element("recordStatus").elementText("Status");
	    String diskSpaceUsed = parentEL.element("data").element("recordStatus").elementText("DiskSpaceUsed");
	    String totalDiskSpaceAvailable = parentEL.element("data").element("recordStatus").elementText("TotalDiskSpaceAvailable");
	    String errorCode = parentEL.element("data").element("recordStatus").elementText("ErrorCode");
	    String errorDescription = parentEL.element("data").element("recordStatus").elementText("ErrorDescription");
	
	    log.addInfo("Request ID  : " + requestID);
	    log.addInfo("JID : " + user);
	    log.addInfo("Nick Name : " + nickName);
	    log.addInfo("Agent Name : " + agentName);
	    log.addInfo("ID : " + id);
	    log.addInfo("Start Time : " + startTime);
	    log.addInfo("End Time : " + endTime);
	    log.addInfo("Date : " + date);
	
	    if (msgName.equalsIgnoreCase(ProxyConstants.RECORDINGSTATUSRESPONSE)) {
	        String timeOffset = parentEL.element("data").element("recordInfo").elementText("TimeOffset");
	        String patient = parentEL.element("data").element("recordInfo").elementText("Patient");
	        String procedureType = parentEL.element("data").element("recordInfo").elementText("ProcedureType");
	        String physician = parentEL.element("data").element("recordInfo").elementText("Physician");
	        String notes = parentEL.element("data").element("recordInfo").elementText("Notes");
	
	        log.addInfo("Time Offset : " + timeOffset);
	        log.addInfo("Patient : " + patient);
	        log.addInfo("Procedure Type : " + procedureType);
	        log.addInfo("Physician : " + physician);
	        log.addInfo("Notes : " + notes);
	    }
	
	    log.addInfo("Recording Status : " + recordingStatus);
	    log.addInfo("Disk Space Used : " + diskSpaceUsed);
	    log.addInfo("Disk Space Available : " + totalDiskSpaceAvailable);
	    log.addInfo("Error Code : " + errorCode);
	    log.addInfo("Error Description : " + errorDescription);
	}

	public static String generateMediaTimestampRequest(StringTokenizer st, String requestType, MRLog log) {
	    String xmlFilePath = ProxyConstants.XMLFILEPATH + "mediatimestamprequest.xml";
	
	    try {
	        Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	        Element el = MRXMLUtils.getRootElement(msgdoc);
	        String user = null;
	        String nickname = null;
	        String id = null;
	        String timeOffset = null;
	
	        if (st.hasMoreTokens()) {
	            user = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            nickname = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            id = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            timeOffset = st.nextToken();
	            MRXMLUtils.setValueXpath(el, "//mediainfo/TimeOffset", timeOffset);
	        }
	
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
	
	        if ((id != null) && !id.trim().equalsIgnoreCase("")) {
	            MRXMLUtils.setValueXpath(el, "//ID", id);
	        }
	
	        String result = MRXMLUtils.elementToString(el) + "\n";
	
	        return result;
	    } catch (Exception e) {
	        log.addInfo("Generate :" + e.toString());
	        e.printStackTrace();
	
	        return null;
	    }
	}

	public static String generateBookMarkRequest(StringTokenizer st, String requestType, MRLog log) {
	    String xmlFilePath = null;
	
	    if (requestType.compareToIgnoreCase(ProxyConstants.CREATEBOOKMARKREQUEST) == 0) {
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "createbookmarkrequest.xml";
	    }
	
	    try {
	        Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	        Element el = MRXMLUtils.getRootElement(msgdoc);
	        String user = null;
	        String nickname = null;
	        String id = null;
	
	        String timestamp = null;
	        String notes = null;
	
	        if (st.hasMoreTokens()) {
	            user = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            nickname = st.nextToken();
	        }
	
	        /*if (st.hasMoreTokens()) {
	                agentName = st.nextToken();
	        }*/
	        if (st.hasMoreTokens()) {
	            id = st.nextToken();
	            MRXMLUtils.setValueXpath(el, "//bookmarkInfo/ID", id);
	        }
	
	        /*if (st.hasMoreTokens()) {
	                mediaID = st.nextToken();
	                MRXMLUtils.setValueXpath(el, "//bookmarkInfo/mediaID", id);
	        }*/
	        if (st.hasMoreTokens()) {
	            timestamp = st.nextToken();
	            MRXMLUtils.setValueXpath(el, "//bookmarkInfo/Timestamp", timestamp);
	        }
	
	        if (st.hasMoreTokens()) {
	            notes = st.nextToken();
	            MRXMLUtils.setValueXpath(el, "//bookmarkInfo/Notes", notes);
	        }
	
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
	
	        //MRXMLUtils.setValueXpath(el, "//recordData/agentName", agentName);
	        String result = MRXMLUtils.elementToString(el) + "\n";
	
	        return result;
	    } catch (Exception e) {
	        log.addInfo("Generate :" + e.toString());
	        e.printStackTrace();
	
	        return null;
	    }
	}

	public static String generateStopPauseResumeRecordingRequest(StringTokenizer st, String requestType, MRLog log) {
	    String xmlFilePath = null;
	
	    if (requestType.compareToIgnoreCase(ProxyConstants.STOPRECORDINGREQUEST) == 0) {
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "stoprecordingrequest.xml";
	    } else if (requestType.compareToIgnoreCase(ProxyConstants.PAUSERECORDINGREQUEST) == 0) {
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "pauserecordingrequest.xml";
	    } else if (requestType.compareToIgnoreCase(ProxyConstants.RESUMERECORDINGREQUEST) == 0) {
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "resumerecordingrequest.xml";
	    } else if (requestType.compareToIgnoreCase(ProxyConstants.RECORDINGSTATUSREQUEST) == 0) {
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "recordingstatusrequest.xml";
	    }
	
	    try {
	        Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	        Element el = MRXMLUtils.getRootElement(msgdoc);
	        String user = null;
	        String nickname = null;
	        String id = null;
	
	        if (st.hasMoreTokens()) {
	            user = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            nickname = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            id = st.nextToken();
	        }
	
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
	
	        if ((id != null) && !id.trim().equalsIgnoreCase("")) {
	            MRXMLUtils.setValueXpath(el, "//recordID/ID", id);
	        }
	
	        String result = MRXMLUtils.elementToString(el) + "\n";
	
	        return result;
	    } catch (Exception e) {
	        log.addInfo("Generate :" + e.toString());
	        e.printStackTrace();
	
	        return null;
	    }
	}

	public static String generateAuthorizationUpdateRequest(StringTokenizer st, String requestType, MRLog log) {
	    String xmlFilePath = null;
	
	    if (requestType.compareToIgnoreCase(ProxyConstants.AUTHORIZATIONUPDATEREQUEST) == 0) {
	        //xmlFilePath = "/com/ipvs/xsd/xml/startrecordingrequest.xml";
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizationupdaterequest.xml";
	    }
	
	    try {
	        Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	        Element el = MRXMLUtils.getRootElement(msgdoc);
	        String destAgentJID = null;
	
	        if (st.hasMoreTokens()) {
	            destAgentJID = st.nextToken();
	        }
	
	        if (destAgentJID == null) {
	            destAgentJID = "preeta@10.1.5.185/callcenter";
	        }
	
	        MRXMLUtils.setValueXpath(el, "//destInfo/JID", destAgentJID);
	
	        String result = MRXMLUtils.elementToString(el) + "\n";
	
	        return result;
	    } catch (Exception e) {
	        log.addInfo("Generate :" + e.toString());
	        e.printStackTrace();
	
	        return null;
	    }
	}

	public static String generateStartUpdateRecordingRequest(StringTokenizer st, String requestType, MRLog log) {
	    String xmlFilePath = null;
	
	    if (requestType.compareToIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST) == 0) {
	        //xmlFilePath = "/com/ipvs/xsd/xml/startrecordingrequest.xml";
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "startrecordingrequest.xml";
	    } else if (requestType.compareToIgnoreCase(ProxyConstants.UPDATERECORDINGREQUEST) == 0) {
	        xmlFilePath = ProxyConstants.XMLFILEPATH + "updaterecordingrequest.xml";
	    }
	
	    /*****************************************/
	    try {
	        Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	        Element el = MRXMLUtils.getRootElement(msgdoc);
	        String user = null;
	        String nickname = null;
	        String id = null;
	        String timeOffset = null;
	        String patient = null;
	        String procedureType = null;
	        String physician = null;
	        String notes = null;
	
	        if (st.hasMoreTokens()) {
	            user = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            nickname = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            id = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            timeOffset = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            patient = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            procedureType = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            physician = st.nextToken();
	        }
	
	        if (st.hasMoreTokens()) {
	            notes = st.nextToken();
	        }
	
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
	        MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
	
	        if ((id != null) && !id.trim().equalsIgnoreCase("")) {
	            MRXMLUtils.setValueXpath(el, "//recordInfo/ID", id);
	        }
	
	        if ((timeOffset != null) && !timeOffset.trim().equalsIgnoreCase("")) {
	            MRXMLUtils.setValueXpath(el, "//recordInfo/TimeOffset", timeOffset);
	        }
	
	        /*MRXMLUtils.setValueXpath(el, "//recordInfo/StartTime", startTime);
	        MRXMLUtils.setValueXpath(el, "//recordInfo/EndTime", endTime);
	        MRXMLUtils.setValueXpath(el, "//recordInfo/Date", date);*/
	        MRXMLUtils.setValueXpath(el, "//recordInfo/Patient", patient);
	        MRXMLUtils.setValueXpath(el, "//recordInfo/ProcedureType", procedureType);
	        MRXMLUtils.setValueXpath(el, "//recordInfo/Physician", physician);
	        MRXMLUtils.setValueXpath(el, "//recordInfo/Notes", notes);
	
	        String result = MRXMLUtils.elementToString(el) + "\n";
	
	        return result;
	    } catch (Exception e) {
	        log.addInfo("Generate :" + e.toString());
	        e.printStackTrace();
	
	        return null;
	    }
	}

	private static Element setValuesWithOutStatus(Element parentEL, StringTokenizer st, MRLog log) {
	    String srcUser = null;
	    String srcUserJID = null;
	    String srcUserNickName = null;
	    String destUser = null;
	    String destUserJID = null;
	    String destUserNickName = null;
	    String remote = null;
	    String connection = null;
	
	    if (st.hasMoreTokens()) {
	        srcUser = st.nextToken();
	    }
	
	    if (st.hasMoreTokens()) {
	        srcUserJID = st.nextToken();
	    }
	
	    if (st.hasMoreTokens()) {
	        srcUserNickName = st.nextToken();
	    }
	
	    if (st.hasMoreTokens()) {
	        destUser = st.nextToken();
	    }
	
	    if (st.hasMoreTokens()) {
	        destUserJID = st.nextToken();
	    }
	
	    if (st.hasMoreTokens()) {
	        destUserNickName = st.nextToken();
	    }
	
	    if (st.hasMoreTokens()) {
	        connection = st.nextToken();
	    }
	
	    if (st.hasMoreTokens()) {
	        remote = st.nextToken();
	    }
	
	    try {
	        MRXMLUtils.setValueXpath(parentEL, "requestorInfo/agentName", destUser);
	        MRXMLUtils.setValueXpath(parentEL, "requestorInfo/JID", destUserJID);
	        MRXMLUtils.setValueXpath(parentEL, "requestorInfo/nickName", destUserNickName);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/sourceInfo/agentName", srcUser);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/sourceInfo/JID", srcUserJID);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/sourceInfo/nickName", srcUserNickName);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/destInfo/agentName", destUser);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/destInfo/JID", destUserJID);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/destInfo/nickName", destUserNickName);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/profileInfo/connect", connection);
	        MRXMLUtils.setValueXpath(parentEL, "streamData/profileInfo/remote", remote);
	    } catch (Exception e) {
	        e.printStackTrace();
	    }
	
	    return parentEL;
	}

	public static String generateAuthRequest(StringTokenizer message, MRLog log) {
	    String xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamrequest.xml";
	    Document msgdoc = null;
	
	    Element streamEL = null;
	    String requestID = null;
	
	    try {
	        if (message.hasMoreTokens()) {
	            requestID = message.nextToken();
	        }
	
	        msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	        streamEL = MRXMLUtils.getElementXpath(msgdoc, "//data");
	    } catch (Exception e) {
	        log.addInfo("Generate :" + e.toString());
	
	        return null;
	    }
	
	    setValuesWithOutStatus(streamEL, message, log);
	
	    String result = null;
	
	    if (streamEL != null) {
	        try {
	            MRXMLUtils.setValueXpath(msgdoc, "//requestID", requestID);
	            result = MRXMLUtils.documentToStringOmitDeclaration(msgdoc) + "\n";
	        } catch (Exception e) {
	            // TODO Auto-generated catch block
	            e.printStackTrace();
	        }
	    }
	
	    return result;
	}

	public static Hashtable<String, Integer> getResponseNameHash() {
	    if (responseNameHash == null) {
	        initialize();
	    }
	
	    Hashtable<String, Integer> responseNameHashTemp = new Hashtable<String, Integer>();
	    responseNameHashTemp.putAll(responseNameHash);
	
	    return responseNameHashTemp;
	}

	public static synchronized void initialize() {
	    if (responseNameHash == null) {
	        responseNameHash = new Hashtable<String, Integer>();
	        responseNameHash.put(ProxyConstants.STARTRECORDINGRESPONSE, ProxyConstants.CASE_STARTRECORDINGREQUEST);
	        responseNameHash.put(ProxyConstants.STOPRECORDINGRESPONSE, ProxyConstants.CASE_STOPRECORDINGREQUEST);
	        responseNameHash.put(ProxyConstants.UPDATERECORDINGRESPONSE, ProxyConstants.CASE_UPDATERECORDINGREQUEST);
	        responseNameHash.put(ProxyConstants.RESUMERECORDINGRESPONSE, ProxyConstants.CASE_RESUMERECORDINGREQUEST);
	        responseNameHash.put(ProxyConstants.PAUSERECORDINGRESPONSE, ProxyConstants.CASE_PAUSERECORDINGREQUEST);
	        responseNameHash.put(ProxyConstants.CREATEBOOKMARKRESPONSE, ProxyConstants.CASE_CREATEBOOKMARKREQUEST);
	        responseNameHash.put(ProxyConstants.RECORDINGSTATUSRESPONSE, ProxyConstants.CASE_RECORDINGSTATUSREQUEST);
	        responseNameHash.put(ProxyConstants.MEDIATIMESTAMPRESPONSE, ProxyConstants.CASE_MEDIATIMESTAMPREQUEST);
	        responseNameHash.put(ProxyConstants.LISTRESPONSE, ProxyConstants.CASE_LISTREQUEST);
	        responseNameHash.put(ProxyConstants.AUTHORIZATIONUPDATERESPONSE, ProxyConstants.CASE_AUTHORIZATIONUPDATEREQUEST);
	    }
	}

	
}
