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

import com.ipvs.app.ServiceUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;

public class TestApplet extends Applet implements ActionListener, ItemListener, 
ServiceMessageHandlerI {
 
  private static final long serialVersionUID = 1L;
  TextField user;
  TextField password;
  TextField domain;
  TextField resource;
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

  public Panel addComponent(String label, Component c) {
    Panel p = new Panel();
    p.setLayout(new FlowLayout(FlowLayout.LEFT));
    Label l = new Label(label);
    p.add(l);
    p.add(c);
    return p;
  }
  public Panel addComponent(Component c) {
	    Panel p = new Panel();
	    p.setLayout(new FlowLayout(FlowLayout.LEFT));
	    p.add(c);
	    return p;
	  }
  public void init() {
    setLayout(new BorderLayout());

    user = new TextField("miyertest");
    password = new TextField("miyertest");
    server = new TextField("192.168.1.209");
    port = new TextField("5222");
    domain = new TextField("192.168.1.209");
    resource = new TextField("applet");
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
    request.add("------------------------Select Request --------------------------------------------------------");
    message = new TextArea("message, 2, 40");
    send = new Button("Send Request");
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
    topPanel.add(addComponent("Resource", resource));
    topPanel.add(addComponent("Cliendata", clientdata));
    add(topPanel, "North");

    Panel leftPanel = new Panel();
    leftPanel.setLayout(new BorderLayout());

    Panel topLeftPanel = new Panel();
    topLeftPanel.setLayout(new BorderLayout());
    Panel p = addComponent("Service", service);
    p.add(send);
    topLeftPanel.add(p, "North");
    Panel pp = addComponent(request);
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
    String resource = this.resource.getText();
    String server = this.server.getText();
    String port = this.port.getText();
    int loglevel = this.logLevel.getSelectedIndex();
    String request = "<Request>" 
        + "<header>"
        + "<servicename>" + "xmpp" + "</servicename>"
        + "<requestname>" + "LoginRequest" + "</requestname>" 
        + "<loglevel>" + String.valueOf(loglevel) + "</loglevel>"
        + "<clientdata>ClientData</clientdata>" 
        + "</header>"
        + "<data>"
        + "<LoginRequestData>"
        + "<user>" + userJID + "</user>"
        + "<password>" + password + "</password>" 
        + "<domain>" + domain + "</domain>" 
        + "<resource>" + resource + "</resource>"
        + "<server>" + server + "</server>" 
        + "<port>" + port + "</port>"
        + "</LoginRequestData>"
        + "</data>"
        + "</Request>";
    String response = null;
    response = ServiceUtils.getServiceClient(this).login(request);
    result.setText(response);
    String requestXML = "<Request>" 
        + "<header>"
        + "<servicename>" + "xmpp" + "</servicename>"
        + "<requestname>" + "SetServiceDomainRequest" + "</requestname>" 
        + "<loglevel>" + String.valueOf(loglevel) + "</loglevel>"
        + "<clientdata>ClientData</clientdata>" 
        + "<clientdata>ClientData</clientdata>"
        + "</header>"
        + "<data>"
        + "<SetServiceDomainRequestData>"
        + "<domain>" + userJID + "</domain>"
        + "</SetServiceDomainRequestData>"
        + "</data>"
        + "</Request>";
    response = ServiceUtils.getServiceClient(this).setServiceDomain("*", requestXML);
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

  public void handleServiceMessage(String userJID, String message) {
    try {
      result.append(message);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }


}