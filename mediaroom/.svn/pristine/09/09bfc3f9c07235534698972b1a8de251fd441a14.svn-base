package com.ipvs.device.impl;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.InterruptedIOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.DeviceApp;


public class SSNotificationListener implements Runnable {
    private static volatile boolean resetNotification;
    private DeviceApp app = null;
    private Socket notificationClient = null;
    BufferedReader reader = null;
    private StringTokenizer st = null;
    private StringBuffer notificationBuffer = null;
    private MRLog log = null;

    public SSNotificationListener(MRLog log, DeviceApp deviceApp) {
        this.log = log;
        this.app = deviceApp;
        notificationClient = null;
        notificationBuffer = null;
    }

    public static boolean isResetNotification() {
        return resetNotification;
    }

    public static void setResetNotification(boolean resetNotif) {
        resetNotification = resetNotif;
    }

    public void run() {
        // Cleanup of code required Shanker : 07/15
        // What if the DeviceApp were to terminate this thread
        //    how do you handle it
        // What if the streamingserver disconnects do you flush out the buffers
        // processNotification should actually run indefintely and only
        // terminate when the reset Notification flag is set
        while (!isResetNotification()) {
            try {
                if (notificationBuffer == null) {
                    notificationBuffer = new StringBuffer();
                }

                if (notificationClient == null) {
                    try {
                        connect();
                    } catch (Exception e) {
                    }
                } else if (!notificationClient.isConnected()) {
                    try {
                        Thread.sleep(500);
                        connect();
                        st = null;
                    } catch (Exception e) {
                    }
                }

                Element el = null;

                if ((st != null) && st.hasMoreTokens()) {
                    while (st.hasMoreTokens()) {
                        String temp = st.nextToken().trim();

                        if ((temp != null) && temp.contains("/SSNotificationStream")) {
                            temp = null;
                            connect();
                        }

                        if ((temp != null) && !temp.contains("SSNotificationStream")) {
                            notificationBuffer.append(temp);
                            notificationBuffer.append(">");
                        }

                        try {
                            el = MRXMLUtils.getRootElement(MRXMLUtils.getDocument(notificationBuffer.toString()));
                        } catch (Exception e) {
                        }

                        if (el != null) {
                            try {
                                // System.out.println(notificationBuffer.toString());
                                sendMessageToAgent();

                                // srcAgent.processStreamingServerNotification(notificationBuffer.toString());
                            } catch (Exception e) {
                                e.printStackTrace();
                            }

                            notificationBuffer.setLength(0);
                        }
                    }
                }

                String line = null;
                @SuppressWarnings("unused")
                boolean result = true;

                if (notificationClient == null) {
                    result = false;
                } else if (notificationClient.isConnected()) {
                    try {
                        line = reader.readLine();

                        if (line == null) {
                            connect();
                        }

                        //System.out.println(line);
                    } catch (InterruptedIOException e1) {
                        result = false;
                    } catch (IOException e) {
                        log.addInfo("processNotification : Reconnecting" + e.toString());
                        result = false;
                        connect();
                    } catch (Exception e) {
                        connect();
                    }
                }

                if (line != null) {
                    st = new StringTokenizer(line, ">");
                }

                while ((st != null) && st.hasMoreTokens()) {
                    String temp = st.nextToken();

                    if ((temp != null) && temp.contains("/SSNotificationStream")) {
                        temp = null;
                        connect();
                    }

                    if ((temp != null) && !temp.contains("SSNotificationStream")) {
                        notificationBuffer.append(temp);
                        notificationBuffer.append(">");
                    }

                    try {
                        el = MRXMLUtils.getRootElement(MRXMLUtils.getDocument(notificationBuffer.toString()));
                    } catch (Exception e) {
                    }

                    if (el != null) {
                        try {
                            //System.out.println(notificationBuffer.toString());
                            sendMessageToAgent();

                            //srcAgent.processStreamingServerNotification(notificationBuffer.toString());
                        } catch (Exception e) {
                            e.printStackTrace();
                        }

                        notificationBuffer.setLength(0);
                    }
                }
            } catch (Exception e) {
            }
        }

        //Issues in build due to the below code -- To be fixed
        if (notificationClient != null) {
            try {
                notificationClient.close();
                notificationClient = null;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void connect() {
        SocketAddress sockaddr = new InetSocketAddress("localhost", 10002);

        if ((notificationClient != null) && this.notificationClient.isConnected()) {
            try {
                this.notificationClient.close();
            } catch (IOException e1) {
                log.addInfo(e1.toString());
            }
        }

        this.notificationClient = new Socket();

        try {
            notificationClient.connect(sockaddr, 1000);
        } catch (IOException e) {
            log.addInfo(e.toString());

            return;
        }

        try {
            if (notificationClient.isConnected()) {
                reader = new BufferedReader(new InputStreamReader(notificationClient.getInputStream()));
            }
        } catch (IOException e) {
            return;
        }

        try {
            notificationClient.setSoTimeout(100);
        } catch (SocketException e) {
            log.addInfo(e.toString());

            return;
        }

        st = null;
    }

    private void sendMessageToAgent() throws MRException, Exception {
        String userJID = app.getUserJID();        
        if (userJID == null) {
            return;
        }
        
        Document notifcationDoc = MRXMLUtils.stringToDocument(notificationBuffer.toString());
        String eventSource = MRXMLUtils.getValueXpath(notifcationDoc, "//EventType", "");
        String type = MRXMLUtils.getValueXpath(notifcationDoc, "//Type", "");
        XMPPI xmpp = app.getServiceClient().getXMPP(userJID);
        MRDB db = app.getServiceClient().getMRClientAgent(userJID).getDb();
        DeviceAppUtils.handleNotification((Element)notifcationDoc.getRootElement().detach(), DeviceConst.SS_NOTIFICATION, eventSource, type, userJID, log, xmpp, db);
    }
}
