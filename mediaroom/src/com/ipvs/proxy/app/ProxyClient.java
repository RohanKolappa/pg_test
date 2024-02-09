package com.ipvs.proxy.app;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.InterruptedIOException;
import java.io.OutputStreamWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.StringTokenizer;
import org.dom4j.Document;
import org.dom4j.Element;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.proxy.impl.ProxyClientUtils;
import com.ipvs.proxy.impl.ProxyConstants;




public class ProxyClient {
    private static int seq;
    private static String ack = "0";
    private MRLog log;
    private String serverip;
    private int port;
    private Socket client;
    private BufferedReader reader;
    private BufferedWriter writer;
    private StringBuffer proxyBuffer;
    ProxyClientListenerI proxyClientListener;
    private volatile long lastHeartBeatFromServerTS;
    private volatile long heartBeatPendingTS;
    
    private boolean heartBeatFromServerPending;
    private ProxyClientHeartBeat ts;
    private volatile IPVSTimer timer = null;
    private boolean restart = false;
    int heartbeatFrequency = 1000;
    int heartbeatTimeout = 4;
    int readTimeout = 100;
    
    public ProxyClient() {
        super();
        log = MRLog.getInstance(MRLog.CONSOLE);
        proxyBuffer = new StringBuffer();
    }

    public ProxyClient(ProxyClientListenerI proxyClientListener, String serverip, int port, int heartbeatFrequency , MRLog log) {
        super();
        this.log = log;
        proxyBuffer = new StringBuffer();
        this.proxyClientListener = proxyClientListener;
        this.serverip= serverip;
        this.port = port;
        this.heartbeatFrequency = heartbeatFrequency;
       
    }

 
    public void disconnect() {
    	try {
    		client.close();
    		timer.cancel();
    		lastHeartBeatFromServerTS = 0;
    	}catch(Exception e) {
    		e.printStackTrace();
    	}
    }
    
    public synchronized void connect() {
    	
    	
    	try {
    		
    		if(client == null) {
    			this.client = new Socket();
    		}
    		else if(client.isConnected()) {
    			client.close();
    		}
    		
    		while(true) {
    			try {
    				this.client = new Socket();
    				SocketAddress sockaddr = new InetSocketAddress(this.serverip, this.port);
    				client.connect(sockaddr);
    			
    				if(client.isConnected()) {
    					client.setSoTimeout(readTimeout);
    					reader = new BufferedReader(new InputStreamReader(client.getInputStream()));
    					writer = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));
    					proxyBuffer.setLength(0);
    					break;
    				}
    			} catch(Exception e) {
    				e.printStackTrace();
    			}
    		}
    	}catch(Exception e) {
    		e.printStackTrace();
    	}
    	
    }
    
    public boolean startHeartBeat() {
        try {
            if (client.isConnected()) {
                try {
                    if (heartbeatFrequency > 0) {
                        ts = new ProxyClientHeartBeat(this, log);
                        if(timer!= null) {
                        	timer.cancel();
                        }
                        timer = new IPVSTimer();
                        lastHeartBeatFromServerTS = System.nanoTime();
                        timer.schedule(ts,heartbeatFrequency);
                    }
                } catch (Exception e) {
                }
            }
        } catch(Exception e){
        	e.printStackTrace();
        	return false;
        }
        return true;
    }
    
    public synchronized void send(StringTokenizer st) {
        if (client != null) {
            if (client.isConnected()) {
                String xmlText = ProxyClientUtils.generate(st, log) + "\n";
                if (xmlText != null) {
                    ProxyClientUtils.socketWrite(writer, xmlText, log);
                } else {
                    log.addInfo("Send : Invalid request", MRLog.INFO,MRLog.NOTIFY);
                }
            } else {
                log.addInfo("Send : No Connection", MRLog.INFO,MRLog.NOTIFY);
            }
        } else {
            log.addInfo("Send : No Connection", MRLog.INFO,MRLog.NOTIFY);
        }
    }


    public void sendResponse(StringTokenizer st) {
        String fname = null;

        if (st.hasMoreTokens()) {
            fname = st.nextToken();

            if (fname != null) {
                fname = fname.trim();
            }
        } else {
            return;
        }       
/*        if (fname.compareToIgnoreCase(ProxyConstants.CONNECT) == 0) {
            //initialize(st);
        }

        if (fname.compareToIgnoreCase(ProxyConstants.GENERATE) == 0) {
            ProxyClientUtils.generate(st, log);
        }
*/
        if (fname.compareToIgnoreCase(ProxyConstants.SEND) == 0) {
            send(st);
        }

        
    }
   
    public synchronized void sendHeartBeatToServer(String heartBeatMessage) {
    	ProxyClientUtils.socketWrite(writer, heartBeatMessage, log);
    	//if(getHeartBeatPendingTS()==null) {
    	if(getHeartBeatPendingTS() == 0) {
    		setHeartBeatPendingTS(System.nanoTime());
    	}	
    	//}	
    	setHeartBeatFromServerPending(true);
    }
    
    private synchronized String getNextProxyRequest() {
        Element el = null;
        boolean result = true;
        while(true) {
        String line = null;
        try {
        	
            line = reader.readLine();
            if(line ==null) {
            	result = false;
            }

        } catch (InterruptedIOException e1) {
            result = false;
        } catch (IOException e) {
            // Reconnect
            log.addInfo("processProxyInput : Reconnecting" + e.toString(), MRLog.INFO,MRLog.NOTIFY);
            result = false;
        }
        
        if (result) {
        
            StringTokenizer st = new StringTokenizer(line, ">");
            while (st.hasMoreTokens()) {
                proxyBuffer.append(st.nextToken());
                proxyBuffer.append(">");
                String proxyBufferLocal = null;
                try {
                	if (proxyBuffer.toString().indexOf("<") != -1) {
                        proxyBufferLocal = proxyBuffer.toString().substring(proxyBuffer.toString().indexOf("<"));
                    }
                    el = MRXMLUtils.getRootElement(MRXMLUtils.getDocument(proxyBufferLocal));
                } catch (Exception e) {
                    //e.printStackTrace();
                }

                if (el != null) {
                    try {
                        proxyBuffer.setLength(0);
                        return proxyBufferLocal;

                    } catch (Exception e) {
                        e.printStackTrace();
                    }

                    proxyBuffer.setLength(0);
                }
            }
        }
        else {
        	break;
        }

        }
        // TODO Auto-generated method stub
		return null;
        
    }


    public void run() {
    	while(true) {
    		boolean isLoginSuccessful = false;
    		while(!isLoginSuccessful) { //Check if login is successful. 
    			isLoginSuccessful = proxyClientListener.loginProxyUser();
    		} 	
    		connect();
			startHeartBeat();
       		while(!isRestart()) {
       			String request = this.getNextProxyRequest();
       			try {
       				//check for app restart, SetServiceDomain Failed
       				if(isRestart()) { // App has sent a restart
       					break;
       				}
       				//check for missed heartbeat
       				if(request == null){
       		   			long cusTS = System.nanoTime();
       		   			long diff = (cusTS - lastHeartBeatFromServerTS)/1000000;
       		   			long diffSinceLastHeartBeatSent = 0;
       		   			if(getHeartBeatPendingTS()!=0) {
       		   				diffSinceLastHeartBeatSent = (cusTS - getHeartBeatPendingTS())/1000000;
       		   			}	
       		   			if(diff > heartbeatFrequency * getHeartbeatTimeout() && isHeartBeatFromServerPending() && diffSinceLastHeartBeatSent > heartbeatFrequency * getHeartbeatTimeout()) { // missed heartbeat
       		   				this.setHeartBeatFromServerPending(false);
       		   				this.setHeartBeatPendingTS(0);
       		   				System.out.println("Did not receive heartbeat from Server");
       		   				log.addInfo("Did not receive heartbeat from server", MRLog.INFO,MRLog.NOTIFY);
       		   				break;
       		   			}
       		   		}
       				else {
       					processProxyRequest(request);
       				}	
       				Thread.yield();
       	    	}catch(Exception e) {
       	    		e.printStackTrace();

       	    	}
    		}
       		disconnect();
       		proxyClientListener.handleProxyDisconnected();
    	}	
       		
    }     

    public void processProxyRequest(String request) throws Exception {
   			Document requestDoc = MRXMLUtils.stringToDocument(request);
   			//System.out.println(request);
   			if(requestDoc.getRootElement().getName().equalsIgnoreCase(ProxyConstants.HEARTBEAT)) {
   				String ack = ProxyClientUtils.getAck(request);
   				ProxyClient.setAck(ack);
   				lastHeartBeatFromServerTS = System.nanoTime();
   				setHeartBeatFromServerPending(false);
   				this.setHeartBeatPendingTS(0);
   				//System.out.println(lastHeartBeatFromServerTS);
   			} else if(requestDoc.getRootElement().getName().equalsIgnoreCase(ProxyConstants.AUTHRESPONSE)) {
   				proxyClientListener.handleXAuthResponse(requestDoc);
   				lastHeartBeatFromServerTS = System.nanoTime();
   			} else {
   				proxyClientListener.handleProxyClientRequest(requestDoc);
   				lastHeartBeatFromServerTS = System.nanoTime();
   			}
    }
    
    public static int getSeq() {
        return seq;
    }

    public static void setSeq(int seq) {
        ProxyClient.seq = seq;
    }

    public static String getAck() {
        return ack;
    }

    public static void setAck(String ack) {
        ProxyClient.ack = ack;
    }

    public long getLastHeartBeatFromServerTS() {
        return lastHeartBeatFromServerTS;
    }

    public void setLastHeartBeatFromServerTS(long lastHeartBeatFromServerTS) {
        this.lastHeartBeatFromServerTS = lastHeartBeatFromServerTS;
    }

    public synchronized Socket getClient() {
        return client;
    }

    public synchronized void setClient(Socket client) {
        this.client = client;
    }

	public synchronized boolean isRestart() {
		return restart;
	}

	public synchronized void setRestart(boolean restart) {
		this.restart = restart;
	}

	public int getHeartbeatTimeout() {
		return heartbeatTimeout;
	}

	public void setHeartbeatTimeout(int heartbeatTimeout) {
		this.heartbeatTimeout = heartbeatTimeout;
	}

	public synchronized boolean isHeartBeatFromServerPending() {
		return heartBeatFromServerPending;
	}

	public synchronized void setHeartBeatFromServerPending(boolean heartBeatFromServerPending) {
		this.heartBeatFromServerPending = heartBeatFromServerPending;
	}

	public synchronized long getHeartBeatPendingTS() {
		return heartBeatPendingTS;
	}

	public synchronized void setHeartBeatPendingTS(long heartBeatPendingTS) {
		this.heartBeatPendingTS = heartBeatPendingTS;
	}


}
