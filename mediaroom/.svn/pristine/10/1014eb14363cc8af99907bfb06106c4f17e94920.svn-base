package com.ipvs.test;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPEvent;
import com.ipvs.common.XMPPEventHandlerI;
import com.ipvs.common.XMPPEventHandlerThread;
import com.ipvs.common.XMPPI;
import com.ipvs.xmpp.client.smack.IPVSSessionSmackImpl;

public class MUCTest implements XMPPEventHandlerI {

  public void handleXMPPEvent(String userJID, XMPPEvent event) {
    System.out.println("Event=" + event.getMessage());    
  }

  public static void main(String[] args) {
    if(args.length < 3) {
      System.out.println("Usage: com.ipvs.MUCTest <server> <user> <roomNID>");
      System.exit(1);
    }
    String server = args[0];
    String user = args[1];
    String roomNID = args[2];

    try {
      XMPPI xmpp = new IPVSSessionSmackImpl();
      xmpp.start();
      String resource = "applet";
      xmpp.login(server, 5222, user, user, resource);
      MRLog log = MRLog.getInstance(MRLog.CONSOLE);
      String userJID = xmpp.getXMPPUserJid();
      MUCTest test = new MUCTest();
      XMPPEventHandlerThread xmppEventHandlerThread = new XMPPEventHandlerThread(userJID, xmpp, test, log);
      xmppEventHandlerThread.startListening();
      xmpp.joinRoom(roomNID, user);
      System.out.println("hit return to logout");
      BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
      stdin.readLine();

      xmpp.leaveRoom(roomNID, user);
      xmpp.logout();
      xmpp.stop();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
    System.out.println("Done");
  }


}

