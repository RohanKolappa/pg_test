package com.ipvs.test;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.ServiceUtils;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRException;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.ServiceAgentObserverI;
import com.ipvs.common.service.XAuthServerAgent;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.DummySourceStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.xmpp.client.IPVSSessionTest;

public class TestAppRequestHandler implements ServiceMessageHandlerI, ServiceAgentObserverI {

	String lastResponse;
	static String prompt = "ready>";
	PrintStream stdout;

	//boolean isApplet = true;

	public synchronized void handleCmdlineMessage(String userJID, String message) throws Exception {
		try {
			Document doc = MRXMLUtils.getDocument(message);
			String rootName = doc.getRootElement().getName();
			if ("Response".equals(rootName))
				this.setLastResponse(message); // Only if this is a response message
			else if ("Notification".equals(rootName)) {
				Element type = MRXMLUtils.getElementXpath(doc, "//notificationtype");
				if (!"AgentMessage".equals(MRXMLUtils.getValue(type)))
					return;
			}
			stdout.println(MRXMLUtils.documentToString(doc));
		} catch (Exception e) {
			System.out.println("Invalid message:" + message);
		}
		stdout.println(prompt);
	}

	public void handleServiceMessage(String userJID, String message) {
		try {
			// validateResponse(message);
			handleCmdlineMessage(userJID, message);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public String readXML(BufferedReader stdin) throws Exception {
		StringBuffer request = new StringBuffer();
		String line;
		while (true) {
			line = stdin.readLine();
			if (prompt!=null && prompt.equals(line.trim()))
				break;
			request.append(line);
		}
		return request.toString();
	}

	public void addDefaultAgents(String loginResponse) {
		ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), new DestStreamAgent());
		ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.SOURCESTREAMAGENT, new DummySourceStreamAgent(this));
		XAuthServerAgent xAuthServerAgent = new XAuthServerAgent();
		// xAuthServerAgent.addAgentObserver(this.getClass().getName(), this);
		ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), xAuthServerAgent);
		// ServiceUtils.getServiceClient(this).addServiceAgent(
		// ServiceUtils.getUserJID(loginResponse), new XMPPAdminServerAgent());
	}

	/*private void validateResponse(String xml) throws Exception {
		Document doc = MRXMLUtils.stringToDocument(xml);
		if (!"Response".equals(doc.getRootElement().getName()))
			return;

		String serviceName = (MRXMLUtils.getElementXpath(doc, "//servicename")).getValue();
		if (serviceName.equals("xmpp"))
			return;
		// if(serviceName.equals("assetadminservice"))
		// return;
		Integer state = Integer.parseInt((MRXMLUtils.getElementXpath(doc, "//header/state")).getValue());
		if (state.intValue() == 0)
			return;
		String requestName = (MRXMLUtils.getElementXpath(doc, "//requestname")).getValue();
		String responseDataName = requestName.replace("Request", "Response") + "Data";
		String responseDataXML = MRXMLUtils.elementToString(MRXMLUtils.getElementXpath(doc, "//" + responseDataName));

		String xsdPath = "/com/ipvs/xsd/mediaroom." + serviceName + ".xsd";
		URL url = TestApp.class.getResource(xsdPath);
		try {
			MRXMLUtils.getDocument(responseDataXML, url.toString());
			System.out.println(responseDataName + ".... IS VALID");
		} catch (Exception e) {
			System.out.println(responseDataName + ".... IS NOT VALID\n" + e.toString());
			System.out.println(xml);
			// e.printStackTrace();
		}
	}*/

	public void validateMediaRoom(Document rdoc) throws Exception {
		Element mediaRoom = MRXMLUtils.getElementXpath(rdoc, "//MediaRoom");
		if (mediaRoom != null) {
			System.out.println("Checking MediaRoom XML");

			String xsdURL = "com/ipvs/xsd/mediaroom.mediaroomservice.datamodel.xsd";
			String xml = MRXMLUtils.elementToString(mediaRoom);
			StringTokenizer st = new StringTokenizer(xml, "\n");
			int ii = 1;
			while (st.hasMoreTokens()) {
				System.out.println(String.valueOf(ii) + ":" + st.nextToken());
				ii++;
			}
			MRXMLUtils.getDocument(xml, xsdURL);
		}
	}

	public void handleRequestFiles(String[] args) throws Exception {
		if (stdout == null)
			stdout = System.out;
		ServiceUtils.setDummyXMPPImpl(true);
		String loginRequest = FileUtils.readFile(new File(args[0]));

		String loginResponse = ServiceUtils.getServiceClient(this).login(loginRequest);
		// Set the StreamAgent
		addDefaultAgents(loginResponse);
		System.out.println(loginResponse);
		setLastResponse(loginResponse);

		String setServiceDomainRequest = FileUtils.readFile(new File(args[1]));

		String setServiceDomainResponse = ServiceUtils.getServiceClient(this).setServiceDomain("*", setServiceDomainRequest);
		System.out.println(setServiceDomainResponse);
		setLastResponse(setServiceDomainResponse);

		for (int i = 2; i < args.length; i++) {
			if (args[i].startsWith("#")) {
				if (args[i].startsWith("#sleep=")) {
					System.out.println("Started sleeping");
					int st = Integer.parseInt(args[i].substring("#sleep=".length()));
					Thread.sleep(st * 1000);
					System.out.println("Done sleeping");
				}
				continue;
			}
			// Ensure that the prev request has finished
			String response = null;
			while (true) {
				response = this.getLastResponse();
				if (response != null)
					break;
				Thread.sleep(1000);
			}
			this.setLastResponse(null);

			// System.out.println("Last Response:" + response);
			String request = FileUtils.readFile(new File(args[i]));
			// Move in values from lastResponse into new Request
			// find the element in the request which have "$"
			Document doc = MRXMLUtils.stringToDocument(request);
			Document rdoc = MRXMLUtils.stringToDocument(response);

			String requestname = MRXMLUtils.getValueXpath(doc, "//requestname","");
			Element rqdata = MRXMLUtils.getElementXpath(doc, "//" + requestname + "Data");

			// validateMediaRoom(rdoc);
			if (rqdata != null) {
				Iterator<?> children = rqdata.elements().iterator();
				while (children.hasNext()) {
					Element child = (Element) children.next();
					// find the corresponding elements in the last response and set the
					// values
					if ("$".equals(MRXMLUtils.getValue(child))) {
						String name = child.getName();
						// System.out.println("Found $:" + name);
						try {
							Element el = MRXMLUtils.getElementXpath(rdoc, "//" + name);
							child.setText(MRXMLUtils.getValue(el));
						} catch (Exception ee) {
							ee.printStackTrace();
						}
					}
				}
			}
			String requestXML = MRXMLUtils.documentToString(doc);
			// System.out.println("RequestXML :" + requestXML);
			String requestNIDXML = ServiceUtils.getServiceClient(this).sendRequest("*", requestXML);
			System.out.println("RequestNID:" + requestNIDXML);
		}
		// Get the last response
		BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
		System.out.println("hit return to logout");
		stdin.readLine();
		String result = ServiceUtils.getServiceClient(this).logout("*");
		System.out.println(result);
		System.out.println("hit return to exit");
		stdin.readLine();
		System.out.println("Done");
	}
	 public  void process(boolean doSocket , BufferedReader stdin, PrintStream stdout, boolean dummydatabase, Socket clientSocket ){
		 	TestAppRequestHandler app=this;
		 	this.stdout =stdout;
		 	stdout.println(prompt);
		    
		    
		 	while (true) {
		      try {
		        //added by pravin to see if any activity going on or not
		        String line = stdin.readLine();
		        //System.out.println();
		        if (line == null) {
		          // eof on input (e.g. socket closed)
		          break;
		        }
		        //if(dummydatabase)
		        	ServiceUtils.setDummyXMPPImpl(dummydatabase);
		        synchronized(app) {  
		          if ("quit".equals(line.trim()))
		            break;
		          else if (line.startsWith("prompt")) {
		            StringTokenizer st = new StringTokenizer(line);
		            st.nextToken();
		            prompt = st.nextToken();
		          } else if (line.startsWith("database")) {
		            StringTokenizer st = new StringTokenizer(line);
		            st.nextToken();
		            String database = st.nextToken();
		            if (database.trim().equals("dummy")) {
		              ServiceUtils.setDummyXMPPImpl(true);
		            }
		            stdout.println(" ");
		          } else if (line.startsWith("setServiceDomain")) {
		            StringTokenizer st = new StringTokenizer(line);
		            if (st.countTokens() < 2) {
		              stdout.println("Usage: setServiceDomain <userJID>\n<setServiceDomainRequest>");
		              continue;
		            }
		            st.nextToken();
		            String userJID = st.nextToken();
		            String request = app.readXML(stdin);
		            // System.out.println("sendRequest " + request);
		            String status = ServiceUtils.getServiceClient(app).setServiceDomain(userJID, request);
		            stdout.println(status);
		          } else if (line.startsWith("loginFile")) {
		            StringTokenizer st = new StringTokenizer(line);
		            String request = null;
		            if (st.countTokens() < 2) {
		              stdout.println("Usage: loginFile <loginRequestXMLFile>");
		              continue;
		            }
		            st.nextToken();
		            request = FileUtils.readFile(new File(st.nextToken()));
		            if (request.trim().equals("")) {
		              stdout.println("Usage: login\n<loginRequest>");
		              continue;
		            }
		            String status = ServiceUtils.getServiceClient(app).login(request);

		            // Set the StreamAgent
		            app.addDefaultAgents(status);          
		            stdout.println(status);
		          } else if (line.startsWith("login")) {
		            String request = app.readXML(stdin);

		            if (request.trim().equals("")) {
		              stdout.println("Usage: login\n<loginRequest>");
		              continue;
		            }
		            String status = ServiceUtils.getServiceClient(app).login(request);
		            // Set the StreamAgent
		            app.addDefaultAgents(status);          
		            stdout.println(status);
		          } else if (line.startsWith("logout")) {
		            StringTokenizer st = new StringTokenizer(line);
		            if (st.countTokens() < 2) {
		              stdout.println("Usage: logout <userJID>");
		              continue;
		            }
		            st.nextToken();
		            String userJID = st.nextToken();
		            String response = ServiceUtils.getServiceClient(app).logout(userJID);
		            stdout.println(response);
		          } else if (line.startsWith("testsmack")) {
		            StringTokenizer st = new StringTokenizer(line);
		            if (st.countTokens() < 2) {
		              stdout.println("Usage: testsmack <testfunction> <args> ...");
		              continue;
		            }
		            st.nextToken();
		            String server="192.168.1.209";
		            int port = 5222;
		            String userJID="si_1";
		            String password= "siyer";
		            String resource="v2dtx";
		            IPVSSessionTest xmpp = new IPVSSessionTest();
		            try {
		              xmpp.login (server, port, userJID, password, resource);
		            } catch (Exception e) {
		              e.printStackTrace();
		            }
		            String response = xmpp.testfunction(st);

		            stdout.println(response);
		            xmpp.logout();
		          } else if (line.startsWith("sendRequestFile")) {
		            StringTokenizer st = new StringTokenizer(line);
		            if (st.countTokens() < 3) {
		              stdout
		              .println("Usage: sendRequestFile <userJID>\n<requestFile>");
		              continue;
		            }
		            st.nextToken();
		            String userJID = st.nextToken();
		            String request = FileUtils.readFile(new File(st.nextToken()));
		            // System.out.println("sendRequest " + request);
		            String requestNID = ServiceUtils.getServiceClient(app).sendRequest(userJID, request);
		            stdout.println(requestNID);
		          } else if (line.startsWith("sendRequest")) {
		            StringTokenizer st = new StringTokenizer(line);
		            if (st.countTokens() < 2) {
		              stdout.println("Usage: sendRequest <userJID>\n<request>");
		              continue;
		            }
		            st.nextToken();
		            String userJID = st.nextToken();
		            String request = app.readXML(stdin);
		            //System.out.println("sendRequest Length=" + request.length());
		            String requestNID = ServiceUtils.getServiceClient(app).sendRequest(userJID, request);
		            //System.out.println("sendRequest " + requestNID);
		            stdout.println(requestNID);
		          } else if (line.startsWith("getServices")) {
		            stdout
		            .println("mediaroomservice\nassetadminservice\npolicyadminservice");
		          } else if (line.startsWith("getRequests")) {
		            StringTokenizer st = new StringTokenizer(line);
		            if (st.countTokens() < 2) {
		              stdout.println("Usage: getRequests <serviceName>");
		              continue;
		            }
		            st.nextToken();
		            String serviceName = st.nextToken();
		            String[] requests = ServiceUtils.getRequestList(serviceName);
		            for (int i = 0; i < requests.length; i++)
		              stdout.println(requests[i]);
		          } else if (line.startsWith("getRequestXML")) {
		            StringTokenizer st = new StringTokenizer(line);
		            if (st.countTokens() < 3) {
		              stdout
		              .println("Usage: getRequestXML <serviceName> <requestName>");
		              continue;
		            }
		            st.nextToken();
		            String serviceName = st.nextToken();
		            String requestName = st.nextToken();
		            stdout.println(ServiceUtils.getRequestXML(serviceName, requestName));
		          } else {
		            stdout.println("CommandList:\n" + "login\n" + "LoginFile\n"
		                + "logout\n" + "sendRequest\n" + "getServices\n"
		                + "getRequests\n" + "getRequestXML\n" + "quit");
		          }
		          // print the next prompt before releasing lock
		          stdout.println(prompt);
		        }
		      }
		      catch(java.net.SocketException e) {
		        e.printStackTrace();
		        System.out.println("Exiting");
		        break;
		      } catch (Exception e) {
		        stdout.println("<Error>" + e.toString() + "</Error>");
		        break;
		      }
		    }
		    if (doSocket) {
		      try {
		         
		      }      
		      catch(Exception e) {
		        
		      }
		    }
	 }

	public synchronized String getLastResponse() {
		return lastResponse;
	}

	public synchronized void setLastResponse(String lastResponse) {
		this.lastResponse = lastResponse;
	}

	public void handleMessage(String message) throws MRException {
		System.out.println("TestApp::handleMessage message=" + message);
		// throw new MRException(MRExceptionCodes.XAUTH_FAILED, "NO");
	}

}

/*
 * public static void main(String[] args) { TestApp app = new TestApp(); boolean doSocket= false; int port = 5678; // grab command
 * line arguments if (args.length > 0 && args[0].equals("socket")) { doSocket = true; if (args.length > 1 ) { try{ port =
 * Integer.parseInt(args[1]); } catch(Exception ex){ port = 5678; } } }
 * 
 * if (!doSocket && args.length > 1) { try { app.handleRequestFiles(args); } catch (Exception e) { e.printStackTrace();
 * System.out.println("<Error>" + e.toString() + "</Error>"); } System.exit(1); }
 * 
 * BufferedReader stdin = null; ServerSocket serverSocket = null; Socket clientSocket = null; InputStream is = null; OutputStream
 * os = null; if (doSocket) { try { // Create server socket serverSocket = new ServerSocket(port, 5); System.out.println("Running
 * TCP server interface on port " + port + "..."); // Wait for first connection clientSocket = serverSocket.accept(); // Setup the
 * input output streams is = clientSocket.getInputStream(); os = clientSocket.getOutputStream(); stdin = new BufferedReader(new
 * InputStreamReader(is)); stdout = new PrintStream(os); } catch (IOException ioe) { System.out.println("Error creating socket: " +
 * ioe); System.exit(1); } } else { stdin = new BufferedReader(new InputStreamReader(System.in)); stdout = System.out; }
 * stdout.println(prompt);
 * 
 * while (true) { try { //added by pravin to see if any activity going on or not String line = stdin.readLine();
 * System.out.println(); if (line == null) { // eof on input (e.g. socket closed) clientSocket.close(); break; } synchronized(app) {
 * if ("quit".equals(line.trim())) break; else if (line.startsWith("prompt")) { StringTokenizer st = new StringTokenizer(line);
 * st.nextToken(); prompt = st.nextToken(); } else if (line.startsWith("database")) { StringTokenizer st = new
 * StringTokenizer(line); st.nextToken(); String database = st.nextToken(); if (database.trim().equals("dummy")) {
 * ServiceUtils.setDummyXMPPImpl(true); } stdout.println(" "); } else if (line.startsWith("setServiceDomain")) { StringTokenizer
 * st = new StringTokenizer(line); if (st.countTokens() < 2) { stdout.println("Usage: setServiceDomain <userJID>\n<setServiceDomainRequest>");
 * continue; } st.nextToken(); String userJID = st.nextToken(); String request = app.readXML(stdin); //
 * System.out.println("sendRequest " + request); String status = ServiceUtils.getServiceClient(app).setServiceDomain(userJID,
 * request); stdout.println(status); } else if (line.startsWith("loginFile")) { StringTokenizer st = new StringTokenizer(line);
 * String request = null; if (st.countTokens() < 2) { stdout.println("Usage: loginFile <loginRequestXMLFile>"); continue; }
 * st.nextToken(); request = TestUtils.readFile(new File(st.nextToken())); if (request.trim().equals("")) { stdout.println("Usage:
 * login\n<loginRequest>"); continue; } String status = ServiceUtils.getServiceClient(app).login(request); // Set the StreamAgent
 * app.addDefaultAgents(status); stdout.println(status); } else if (line.startsWith("login")) { String request =
 * app.readXML(stdin);
 * 
 * if (request.trim().equals("")) { stdout.println("Usage: login\n<loginRequest>"); continue; } String status =
 * ServiceUtils.getServiceClient(app).login(request); // Set the StreamAgent app.addDefaultAgents(status); stdout.println(status); }
 * else if (line.startsWith("logout")) { StringTokenizer st = new StringTokenizer(line); if (st.countTokens() < 2) {
 * stdout.println("Usage: logout <userJID>"); continue; } st.nextToken(); String userJID = st.nextToken(); String response =
 * ServiceUtils.getServiceClient(app).logout(userJID); stdout.println(response); } else if (line.startsWith("testsmack")) {
 * StringTokenizer st = new StringTokenizer(line); if (st.countTokens() < 2) { stdout.println("Usage: testsmack <testfunction>
 * <args> ..."); continue; } st.nextToken(); String server="192.168.1.209"; port = 5222; String userJID="si_1"; String password=
 * "siyer"; String resource="v2dtx"; IPVSSession xmpp = new IPVSSession(); try { xmpp.login (server, port, userJID, password,
 * resource); } catch (Exception e) { e.printStackTrace(); } String response = xmpp.testfunction(st);
 * 
 * stdout.println(response); xmpp.logout(); } else if (line.startsWith("sendRequestFile")) { StringTokenizer st = new
 * StringTokenizer(line); if (st.countTokens() < 3) { stdout .println("Usage: sendRequestFile <userJID>\n<requestFile>");
 * continue; } st.nextToken(); String userJID = st.nextToken(); String request = TestUtils.readFile(new File(st.nextToken())); //
 * System.out.println("sendRequest " + request); String requestNID = ServiceUtils.getServiceClient(app).sendRequest(userJID,
 * request); stdout.println(requestNID); } else if (line.startsWith("sendRequest")) { StringTokenizer st = new
 * StringTokenizer(line); if (st.countTokens() < 2) { stdout.println("Usage: sendRequest <userJID>\n<request>"); continue; }
 * st.nextToken(); String userJID = st.nextToken(); String request = app.readXML(stdin); //System.out.println("sendRequest
 * Length=" + request.length()); String requestNID = ServiceUtils.getServiceClient(app).sendRequest(userJID, request);
 * //System.out.println("sendRequest " + requestNID); stdout.println(requestNID); } else if (line.startsWith("getServices")) {
 * stdout .println("mediaroomservice\nassetadminservice\npolicyadminservice"); } else if (line.startsWith("getRequests")) {
 * StringTokenizer st = new StringTokenizer(line); if (st.countTokens() < 2) { stdout.println("Usage: getRequests <serviceName>");
 * continue; } st.nextToken(); String serviceName = st.nextToken(); String[] requests = ServiceUtils.getRequestList(serviceName);
 * for (int i = 0; i < requests.length; i++) stdout.println(requests[i]); } else if (line.startsWith("getRequestXML")) {
 * StringTokenizer st = new StringTokenizer(line); if (st.countTokens() < 3) { stdout .println("Usage: getRequestXML <serviceName>
 * <requestName>"); continue; } st.nextToken(); String serviceName = st.nextToken(); String requestName = st.nextToken();
 * stdout.println(ServiceUtils.getRequestXML(serviceName, requestName)); } else { stdout.println("CommandList:\n" + "login\n" +
 * "LoginFile\n" + "logout\n" + "sendRequest\n" + "getServices\n" + "getRequests\n" + "getRequestXML\n" + "quit"); } // print the
 * next prompt before releasing lock stdout.println(prompt); } } catch(java.net.SocketException e) { e.printStackTrace();
 * System.out.println("Exiting"); break; } catch (Exception e) { stdout.println("<Error>" + e.toString() + "</Error>"); break; } }
 * if (doSocket) { try { clientSocket.close(); serverSocket.close(); } catch(Exception e) { } } System.out.println("Done");
 * System.exit(0); }
 */