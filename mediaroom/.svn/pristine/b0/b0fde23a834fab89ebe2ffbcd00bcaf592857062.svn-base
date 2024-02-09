/**
 * $Revision:$
 * $Date:$
 *
 * Copyright 2003-2007 Jive Software.
 *
 * All rights reserved. Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.ipvs.xmpp.client.smack.ft;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.QName;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.filter.PacketIDFilter;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.util.Cache;
import org.jivesoftware.smackx.packet.Bytestream;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.xmpp.client.smack.IPVSServiceDiscoveryManager;

/**
 *
 */
public class Socks5TransferNegotiatorManager implements FileTransferNegotiatorManager {

    private static final long BLACKLIST_LIFETIME = 60 * 1000 * 120;
    // locks the proxies during their initialization process
    private final Object proxyLock = new Object();

    private static ProxyProcess proxyProcess;

    // locks on the proxy process during its initiatilization process
    private final Object processLock = new Object();

    private final Cache<String, Integer> addressBlacklist
            = new Cache<String, Integer>(100, BLACKLIST_LIFETIME);

    private XMPPI connection;

    private List<String> proxies;

    private List<Bytestream.StreamHost> streamHosts;

    public Socks5TransferNegotiatorManager(XMPPI connection) {
        this.connection = connection;
    }

    public StreamNegotiator createNegotiator() {
        return new Socks5TransferNegotiator(this, connection);
    }

    public void incrementConnectionFailures(String address) {
        Integer count = addressBlacklist.get(address);
        if (count == null) {
            count = 1;
        }
        else {
            count += 1;
        }
        addressBlacklist.put(address, count);
    }

    public int getConnectionFailures(String address) {
        Integer count = addressBlacklist.get(address);
        return count != null ? count : 0;
    }

    public ProxyProcess addTransfer() throws IOException {
        synchronized (processLock) {
            if (proxyProcess == null) {
                proxyProcess = new ProxyProcess(new ServerSocket(7777));
                proxyProcess.start();
            }
        }
        proxyProcess.addTransfer();
        return proxyProcess;
    }

    public void removeTransfer() {
        if (proxyProcess == null) {
            return;
        }
        proxyProcess.removeTransfer();
    }

    public Collection<Bytestream.StreamHost> getStreamHosts() {
        synchronized (proxyLock) {
            if (proxies == null) {
                initProxies();
            }
        }
        return Collections.unmodifiableCollection(streamHosts);
    }

    /**
     * Checks the service discovery item returned from a server component to verify if it is
     * a File Transfer proxy or not.
     *
     * @param manager the service discovery manager which will be used to query the component
     * @param item    the discovered item on the server relating
     * @return returns the JID of the proxy if it is a proxy or null if the item is not a proxy.
     */
    private String checkIsProxy(IPVSServiceDiscoveryManager manager, Element item) {
        Document doc =null;
    	try {
            String itemID = item.attributeValue("jid");
            doc = manager.discoverInfo(itemID);
            if(doc !=null && doc.hasContent()){
            	/*
            	    <iq xmlns="" type="result" id="hz2si-74" from="proxy.localhost" to="destrelay1@localhost/destrelay1">
            	      <query xmlns="http://jabber.org/protocol/disco#info">
            	        <identity category="proxy" name="SOCKS5 Bytestreams Service" type="bytestreams" />
            	        <feature var="http://jabber.org/protocol/bytestreams" />
            	        <feature var="http://jabber.org/protocol/disco#info" />
            	      </query>
            	    </iq>
            	*/
            	Element root = doc.getRootElement();
    	        Element query = root.element(new QName("query", Namespace.get("http://jabber.org/protocol/disco#info")));
    	        if (query == null) {
    	            throw new Exception("No response or invalid response from server.");
    	        }            	
    	        Element identity = query.element(new QName("identity", Namespace.get("http://jabber.org/protocol/disco#info")));
    	        if (identity != null) {
    	        	String category = MRXMLUtils.getAttributeValue(identity,"category","");
    	        	String type =  MRXMLUtils.getAttributeValue(identity,"type","");
    	        	if(category.equals("proxy") && type.equals("bytestreams")){
    	        		String from =  MRXMLUtils.getAttributeValue(root,"from","");
    	        		return from;
    	        	}
    	        }            	
            }
        }
        catch (Exception e) {
            return null;
        }
        

        /*DiscoverInfo info;
        try {
            info = manager.discoverInfo(item.getEntityID());
        }
        catch (XMPPException e) {
            return null;
        }
        Iterator itx = info.getIdentities();
        while (itx.hasNext()) {
            DiscoverInfo.Identity identity = (DiscoverInfo.Identity) itx.next();
            if ("proxy".equalsIgnoreCase(identity.getCategory())
                    && "bytestreams".equalsIgnoreCase(
                    identity.getType())) {
                return info.getFrom();
            }
        }*/
        return null;
    }  
    @SuppressWarnings("unchecked")
	private void initProxies() {
        proxies = new ArrayList<String>();
        IPVSServiceDiscoveryManager discoManager = connection.getIPVSServiceDiscoveryManager();
        try {
	        Document doc = discoManager.discoverItems(connection.getServiceName());
	        Element root = doc.getRootElement();
	        Element query = root.element(new QName("query", Namespace.get("http://jabber.org/protocol/disco#items")));
	        if (query == null) {
	            throw new Exception("No response or invalid response from server.");
	        }
			/*
			 <iq xmlns="" type="result" id="wO4D5-101" from="localhost" to="destrelay1@localhost/destrelay1">
			    <query xmlns="http://jabber.org/protocol/disco#items">
			      <item jid="proxy.localhost" name="Socks 5 Bytestreams Proxy" />
			      <item jid="conference.localhost" name="Public Chatrooms" />
			      <item jid="pubsub.localhost" name="Publish-Subscribe service" />
			    </query>
			  </iq> 	        
			 */
	        List<Element> list = (List<Element>) query.elements(new QName("item", Namespace.get("http://jabber.org/protocol/disco#items")));
	        for (Element item : list) {
	            String itemID = item.attributeValue("jid");
	            if(itemID.startsWith("proxy.")){
	            	MRLog.println("itemID " + itemID);
	            }
	            String proxy = checkIsProxy(discoManager, item);
                if (proxy != null) {
                    proxies.add(proxy);
                }
	        }
        }
        catch (Exception e) {
            return;
        }
        /*ServiceDiscoveryManager manager = null;//ServiceDiscoveryManager.getInstanceFor(connection);
        try {
            DiscoverItems discoItems = manager.discoverItems(connection.getServiceName());
            Iterator it = discoItems.getItems();
            while (it.hasNext()) {
                DiscoverItems.Item item = (DiscoverItems.Item) it.next();
                String proxy = checkIsProxy(manager, item);
                if (proxy != null) {
                    proxies.add(proxy);
                }
            }
        }
        catch (XMPPException e) {
            return;
        }*/
        if (proxies.size() > 0) {
            initStreamHosts();
        }
    }

    /**
     * Loads streamhost address and ports from the proxies on the local server.
     */
	private void initStreamHosts() {
        List<Bytestream.StreamHost> streamHosts = new ArrayList<Bytestream.StreamHost>();
        Iterator<String> it = proxies.iterator();
        IQ query;
        PacketCollector collector;
        Bytestream response;
        while (it.hasNext()) {
            String jid = it.next().toString();
            query = new IQ() {
                public String getChildElementXML() {
                    return "<query xmlns=\"http://jabber.org/protocol/bytestreams\"/>";
                }
            };
            query.setType(IQ.Type.GET);
            query.setTo(jid);

            collector = connection.createPacketCollector(new PacketIDFilter(
                    query.getPacketID()));
            try {
				connection.sendPacket(query);
			} catch (Exception e) {
				e.printStackTrace();
			}
            response = (Bytestream) collector.nextResult(OutgoingFileTransfer.getResponseTimeout());
            if (response != null) {
                streamHosts.addAll(response.getStreamHosts());
            }
            collector.cancel();
        }
        this.streamHosts = streamHosts;
    }

    public void cleanup() {
        synchronized (processLock) {
            if (proxyProcess != null) {
                proxyProcess.stop();
                proxyProcess = null;
            }
        }
    }

    class ProxyProcess implements Runnable {

        private final ServerSocket listeningSocket;

        private final Map<String, Socket> connectionMap = new HashMap<String, Socket>();
        
        private boolean done = false;
        private final int MAX_COUNT = 5;

        private Thread thread;
        private int transfers;

        public void run() {
            try {
                try {
                    listeningSocket.setSoTimeout(10000);
                }
                catch (SocketException e) {
                    // There was a TCP error, lets print the stack trace
                    e.printStackTrace();
                    return;
                }
                while (!done) {
                    Socket conn = null;
                    synchronized (ProxyProcess.this) {
                        while (transfers <= 0 && !done) {
                            transfers = -1;
                            try {
                                ProxyProcess.this.wait();
                            }
                            catch (InterruptedException e) {
                                /* Do nothing */
                            }
                        }
                    }
                    if (done) {
                        break;
                    }
                    try {
                        synchronized (listeningSocket) {
                            conn = listeningSocket.accept();
                        }
                        if (conn == null) {
                            continue;
                        }
                        String digest = establishSocks5UploadConnection(conn);                        
                        synchronized (connectionMap) {                                   	
                            connectionMap.put(digest, conn);                                                                                   
                            connectionMap.notify();					                            
                        }
                    }
                    catch (SocketTimeoutException e) {
                        /* Do Nothing */
                    }
                    catch (IOException e) {
                        /* Do Nothing */
                    }
                    catch (XMPPException e) {
                        e.printStackTrace();
                        if (conn != null) {
                            try {
                                conn.close();
                            }
                            catch (IOException e1) {
                                /* Do Nothing */
                            }
                        }
                    }
                }
            }
            finally {
                try {
                    listeningSocket.close();
                }
                catch (IOException e) {
                    /* Do Nothing */
                }
            }
        }

        /**
         * Negotiates the Socks 5 bytestream when the local computer is acting as
         * the proxy.
         *
         * @param connection the socket connection with the peer.
         * @return the SHA-1 digest that is used to uniquely identify the file
         *         transfer.
         * @throws XMPPException
         * @throws IOException
         */
        private String establishSocks5UploadConnection(Socket connection) throws XMPPException, IOException {
            OutputStream out = new DataOutputStream(connection.getOutputStream());
            InputStream in = new DataInputStream(connection.getInputStream());

            // first byte is version should be 5
            int b = in.read();
            if (b != 5) {
                throw new XMPPException("Only SOCKS5 supported");
            }

            // second byte number of authentication methods supported
            b = in.read();
            int[] auth = new int[b];
            for (int i = 0; i < b; i++) {
                auth[i] = in.read();
            }

            int authMethod = -1;
            for (int anAuth : auth) {
                authMethod = (anAuth == 0 ? 0 : -1); // only auth method
                // 0, no
                // authentication,
                // supported
                if (authMethod == 0) {
                    break;
                }
            }
            if (authMethod != 0) {
                throw new XMPPException("Authentication method not supported");
            }
            byte[] cmd = new byte[2];
            cmd[0] = (byte) 0x05;
            cmd[1] = (byte) 0x00;
            out.write(cmd);

            String responseDigest = Socks5TransferNegotiator.createIncomingSocks5Message(in);
            cmd = Socks5TransferNegotiator.createOutgoingSocks5Message(0, responseDigest);

            if (!connection.isConnected()) {
                throw new XMPPException("Socket closed by remote user");
            }
            out.write(cmd);
            return responseDigest;
        }


        public void start() {
            thread.start();
        }

        public void stop() {
            done = true;
            synchronized (this) {
                this.notify();
            }
            synchronized (listeningSocket) {
                listeningSocket.notify();
            }
        }

        public int getPort() {
            return listeningSocket.getLocalPort();
        }

        ProxyProcess(ServerSocket listeningSocket) {
            thread = new Thread(this, "File Transfer Connection Listener");
            this.listeningSocket = listeningSocket;
        }

        public Socket getSocket(String digest) {
            synchronized (connectionMap) {            	
            	int counter = 0;         
                while(connectionMap.get(digest) == null && counter <= MAX_COUNT) {                	                       	
                	try {                                			
    					connectionMap.wait(10000);	// 10 secs
    				} catch (InterruptedException e) {						
    					e.printStackTrace();
    				}    					
    				counter++;                	
                }                            
                return connectionMap.get(digest);
            }
        }

        public void addTransfer() {
            synchronized (this) {
                if (transfers == -1) {
                    transfers = 1;
                    this.notify();
                }
                else {
                    transfers++;
                }
            }
        }

        public void removeTransfer() {
            synchronized (this) {
                transfers--;
            }
        }
    }
}
