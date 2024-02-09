package com.ipvs.test;

import java.applet.Applet;
import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Choice;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Label;
import java.awt.Panel;
import java.awt.TextArea;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.ServiceUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.FileUtils;

public class PolicyAdminTest extends Applet implements ActionListener, ItemListener, 
ServiceMessageHandlerI {
 
	private static final long serialVersionUID = -448895861601134470L;
  TextField user;
  TextField password;
  TextField domain;
  TextField resourceID;
  TextField clientdata;
  TextField server;
  TextField port;
  Button loginButton;
  Button OpenFireloginButton;
  Choice service;
  Choice request;
  TextArea message;
  TextField requestNIDField;
  TextArea result;
  Choice logLevel;
  Button send;
  Label status;
  String lastResponse;
  static String prompt = "ready>";
  static PrintStream stdout;

  boolean isApplet = true;
  
  public Panel addComponent(String label, Component c) {
    Panel p = new Panel();
    p.setLayout(new FlowLayout(FlowLayout.LEFT));
    Label l = new Label(label);
    p.add(l);
    p.add(c);
    return p;
  }

  public void init() {
    setLayout(new BorderLayout());

    user = new TextField("miyer@ipvs.com/resource");
    password = new TextField("miyer123");
    server = new TextField("www.ipvs.com");
    port = new TextField("5222");
    domain = new TextField("ipvs.com");
    resourceID = new TextField("applet");
    clientdata = new TextField("clientdata");
    loginButton = new Button("login");
    OpenFireloginButton = new Button("OpenFirelogin");
    status = new Label("Not logged in");
    service = new Choice();
    service.add("Select Service");
    service.add("mediaroomservice");
    service.add("policyadminservice");
    service.add("assetadminservice");
    logLevel = new Choice();
    logLevel.add("ERROR");
    logLevel.add("INFO");
    logLevel.add("DEBUG");
    request = new Choice();
    request.add("Select Request");
    message = new TextArea("message, 2, 40");
    send = new Button("send");
    requestNIDField = new TextField("requestNID");
    result = new TextArea("result", 2, 40);
    loginButton.addActionListener(this);
    OpenFireloginButton.addActionListener(this);
    send.addActionListener(this);
    service.addItemListener(this);
    request.addItemListener(this);

    Panel topPanel = new Panel();
    topPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
    topPanel.add(loginButton);
    topPanel.add(OpenFireloginButton);
    topPanel.add(status);
    topPanel.add(addComponent("User", user));
    topPanel.add(addComponent("Password", password));
    topPanel.add(addComponent("Server", server));
    topPanel.add(addComponent("Port", port));
    topPanel.add(addComponent("LogLevel", logLevel));
    topPanel.add(addComponent("Domain", domain));
    topPanel.add(addComponent("ResourceID", resourceID));
    topPanel.add(addComponent("Cliendata", clientdata));
    add(topPanel, "North");

    Panel leftPanel = new Panel();
    leftPanel.setLayout(new BorderLayout());

    Panel topLeftPanel = new Panel();
    topLeftPanel.setLayout(new BorderLayout());
    Panel p = addComponent("Service", service);
    topLeftPanel.add(p, "North");
    Panel pp = addComponent("Request", request);
    pp.add(send);
    topLeftPanel.add(pp, "South");

    leftPanel.add(topLeftPanel, "North");
    leftPanel.add(message, "Center");
    add(leftPanel, "West");

    Panel rightPanel = new Panel();
    rightPanel.setLayout(new BorderLayout());
    rightPanel.add(requestNIDField, "North");

    rightPanel.add(result, "Center");
    add(rightPanel, "Center");
  }

  public void actionPerformed(ActionEvent e) {
    if (e.getSource() == send) {
      doSend();
    }
    if (e.getSource() == loginButton) {
      ServiceUtils.setDummyXMPPImpl(true);
      doLogin();
      this.status.setText("Logged in");
    }
    if (e.getSource() == OpenFireloginButton) {
      doLogin();
      this.status.setText("Logged in");
    }

  }

  public void doLogin() {
    String userJID = user.getText();
    String password = this.password.getText();
    String domain = this.domain.getText();
    String resourceID = this.resourceID.getText();
    String server = this.server.getText();
    String port = this.port.getText();
    int loglevel = this.logLevel.getSelectedIndex();
    String request = "<LoginRequest>" + "<user>" + userJID + "</user>"
        + "<password>" + password + "</password>" + "<domain>" + domain
        + "</domain>" + "<resourceID>" + resourceID + "</resourceID>"
        + "<server>" + server + "</server>" + "<port>" + port + "</port>"
        + "<loglevel>" + String.valueOf(loglevel) + "</loglevel>"
        + "<destNID>DestNID</destNID>" + "<resourceGroup>ResourceGroup</resourceGroup>" + "</LoginRequest>";
    String response = null;
    response = ServiceUtils.getServiceClient(this).login(request);
    result.setText(response);
  }

  public void doSend() {

    String message = this.message.getText();
    String userJID = this.user.getText();
    userJID = "*";
    try {
      ServiceClient client = ServiceUtils.getServiceClient(this);
      String responseNID = client.sendRequest(userJID, message);
      requestNIDField.setText(responseNID);
    } catch (Exception e) {
      requestNIDField.setText("Exception:" + e.toString());
      e.printStackTrace();
    }
  }

  public void itemStateChanged(ItemEvent e) {
    if (e.getSource() == service) {
      request.removeAll();
      request.add("Select Request");
      String[] s = ServiceUtils.getRequestList(service.getSelectedItem());
      if (s != null)
        for (int i = 0; i < s.length; i++)
          request.add(s[i]);
    }
    if (e.getSource() == request) {
      String xml = ServiceUtils.getRequestXML(service.getSelectedItem(), request
          .getSelectedItem());
      message.setText(xml);
    }
    // TODO Auto-generated method stub

  }

  public synchronized void handleCmdlineMessage(String userJID, String message)
      throws Exception {
    stdout.println(message);
    stdout.println(prompt);
    Document doc = MRXMLUtils.getDocument(message);
    if ("Response".equals(doc.getRootElement().getName()))
      this.setLastResponse(message); // Only if this is a response message
  }

  public void handleAppletMessage(String userJID, String message)
      throws Exception {
    // result.setText(message);
    result.append(message);
  }

  public void handleServiceMessage(String userJID, String message) {
    try {
      if (isApplet)
        handleAppletMessage(userJID, message);
      else
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
      if (prompt!= null && prompt.equals(line.trim()))
        break;
      request.append(line);
    }
    return request.toString();
  }

  private boolean isDebug=false;
  public void handleRequestFiles(String[] args) throws Exception {
    // ServiceUtils.setDummyXMPPImpl(true);
    String loginRequest = FileUtils.readFile(new File(args[0]));

    String loginResponse = ServiceUtils.getServiceClient(this).login(loginRequest);

    System.out.println(loginResponse);

    setLastResponse(loginResponse);
    
    String setServiceDomainRequest = FileUtils.readFile(new File(args[1]));

    String setServiceDomainResponse = 
      ServiceUtils.getServiceClient(this).setServiceDomain("*", 
          setServiceDomainRequest);
    System.out.println(setServiceDomainResponse);
    setLastResponse(setServiceDomainResponse);

    for (int i = 2; i < args.length; i++) {
      if (args[i].startsWith("#"))
        continue;
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
      Element data = doc.getRootElement().element("data");
      Iterator<?> children = data.elements().iterator();
      while (children.hasNext()) {
        Element child = (Element) children.next();
        // find the corresponding elements in the last response and set the
        // values
        if ("$".equals(MRXMLUtils.getValue(child))) {
          String name = child.getName();
          // System.out.println("Found $:" + name);
          try {
            //String value = rdoc.getRootElement().element("data")
            //    .element(name).getValue();
            String value = MRXMLUtils.getValue(MRXMLUtils.getChild(rdoc.getRootElement(),"data"), name,"");
            // System.out.println("Found value:" + value);
            child.setText(value);
          } catch (Exception ee) {
            // Not found
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
    String p = stdin.readLine();
    String result = ServiceUtils.getServiceClient(this).logout("*");
    System.out.println(result);
    System.out.println("hit return to exit");
    p = stdin.readLine();
    if(isDebug)
    	System.out.println(p);
    System.out.println("Done");
  }

  public static void main(String[] args) {
	  TestAppRequestHandler applet = new TestAppRequestHandler();
    //applet.isApplet = false;
    boolean doSocket= false;
    int port = 5678;
    
    // grab command line arguments
    if (args.length > 0 && args[0].equals("socket")) {
      doSocket = true;
      if (args.length > 1 ) {
        try{
          port = Integer.parseInt(args[1]);
        } catch(Exception ex){
          port = 5678;
        }
      }     
    }
    
    if (!doSocket && args.length > 1) {
      try {
        applet.handleRequestFiles(args);
      } catch (Exception e) {
        System.out.println("<Error>" + e.toString() + "</Error>");
      }
      System.exit(1);
    }


    BufferedReader stdin = null;
    ServerSocket serverSocket = null;
    Socket clientSocket = null;
    InputStream  is = null;
    OutputStream  os = null;
    if (doSocket) {
      try {
        // Create server socket
        serverSocket = new ServerSocket(port, 5);
        System.out.println("Running TCP server interface on port " + port + "...");

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

    while (true) {
      stdout.println(prompt);
      try {
        String line = stdin.readLine();
        if (line == null) {
          // eof on input (e.g. socket closed)
          clientSocket.close();
          break;
        }
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
          String status = ServiceUtils.getServiceClient(applet).login(request);
          stdout.println(status);
        } else if (line.startsWith("login")) {
          String request = applet.readXML(stdin);
          //StringTokenizer st = new StringTokenizer(line);
          if (request.trim().equals("")) {
            stdout.println("Usage: login\n<loginRequest>");
            continue;
          }
          String status = ServiceUtils.getServiceClient(applet).login(request);
          stdout.println(status);
        } else if (line.startsWith("logout")) {
          StringTokenizer st = new StringTokenizer(line);
          if (st.countTokens() < 2) {
            stdout.println("Usage: logout <userJID>");
            continue;
          }
          st.nextToken();
          String userJID = st.nextToken();
          String response = ServiceUtils.getServiceClient(applet).logout(userJID);
          stdout.println(response);
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
          String requestNID = ServiceUtils.getServiceClient(applet).sendRequest(userJID, request);
          stdout.println(requestNID);
        } else if (line.startsWith("sendRequest")) {
          StringTokenizer st = new StringTokenizer(line);
          if (st.countTokens() < 2) {
            stdout.println("Usage: sendRequest <userJID>\n<request>");
            continue;
          }
          st.nextToken();
          String userJID = st.nextToken();
          String request = applet.readXML(stdin);
          // System.out.println("sendRequest " + request);
          String requestNID = ServiceUtils.getServiceClient(applet).sendRequest(userJID, request);
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
      } catch (Exception e) {
        stdout.println("<Error>" + e.toString() + "</Error>");
      }
    }
    System.out.println("Done");
    System.exit(0);
  }

  public synchronized String getLastResponse() {
    return lastResponse;
  }

  public synchronized void setLastResponse(String lastResponse) {
    this.lastResponse = lastResponse;
  }
}