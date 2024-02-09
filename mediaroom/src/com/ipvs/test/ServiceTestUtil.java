package com.ipvs.test;

import java.io.File;
import java.util.Hashtable;

import org.dom4j.Document;

import com.ipvs.app.ServiceUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.NIDSchemeMDC;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class ServiceTestUtil implements ServiceMessageHandlerI {
	public int timeout = 20 * 1000;
	public int timeoutResp = 3 * 60 * 1000;
	public int cleanupRespXMLLoop = 1 * 60 * 1000;
	public boolean stopping = false;
	 
	private Hashtable<String, RespXML> resplist = new Hashtable<String, RespXML>();

	private CleanupRespXML cleanupRespXMLThread = null;
	private String cl_tag_s = "<clientdata>";
	private String st_tag_s = "<state>";
	private String rq_tag_s = "<requestNID>";
	private String cl_tag_e = "<";
	private String response = "";
	private  boolean bThreadStarted=false;
	
	private String userID="*";
	private String userJID="";
	
	public ServiceTestUtil() {
		this(20 * 1000, 3 * 60 * 1000, 1 * 60 * 1000);
	}

	public ServiceTestUtil(int timeout, int timeoutResp, int cleanupRespXMLLoop) {
		try {
			this.timeout = timeout;
			this.timeoutResp = timeoutResp;
			this.cleanupRespXMLLoop = cleanupRespXMLLoop;
			startThread();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	public synchronized void  startThread() {
		try {
			if(bThreadStarted)
				return ;
			cleanupRespXMLThread = new CleanupRespXML();
			cleanupRespXMLThread.parent = this;
			cleanupRespXMLThread.start();
			bThreadStarted=true;
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	public Hashtable<String, RespXML> getResplist() {
		Hashtable<String, RespXML> list = new Hashtable<String, RespXML>();
		list.putAll(resplist);
		return list;
	}

	public RespXML doSend(String message, String messageID) {
		return doSend(message, messageID, true);
	}
	public String getParentListNID(String objectType, String nodeID) throws Exception{
		if(nodeID !=null && !nodeID.trim().equals("")){
			NIDSchemeMDC nid = new NIDSchemeMDC();
			nid.parse(nodeID);
			return nid.getCollectionNodeID();
		}
		ServiceClient client = ServiceUtils.getServiceClient(this);
		try {
			return client.getMRClientAgent(userJID).getDb().getServiceDomainRootNID(null, objectType);
		} catch (MRException e) {
			e.printStackTrace();
		}
		return null;
	 
	}
	public RespXML doSend(String message, String messageID,
			boolean waitForResult) {
		
		RespXML resp = new RespXML();
		try {
			ServiceClient client = ServiceUtils.getServiceClient(this);
			
			String response = client.sendRequest(userJID, message);
			// String response = client.sendRequestSync(user2 , message);
			Document doc = MRXMLUtils.stringToDocument(response);
			String requestNID = MRXMLUtils.getValueXpath(doc, "//requestNID","").trim();
			//System.out.println(MRXMLUtils.documentToString(doc));
			resp.key = requestNID;
			addToResList(resp);
			// client.waitForResult(requestNID);
			int counter = 0;
			if (waitForResult) {
				//System.out.println("START wait requestNID " + requestNID +" " + response);
				while (resp.got_resp != 1) {
//					/System.out.println(counter+" start requestNID " + requestNID);
					if (counter > 40) {
						return resp;
					}
					counter++;
					synchronized (this) {
						wait(1 * 1000);
					}
					//System.out.println(counter+" wait requestNID " + requestNID);
				}
				//System.out.println("DONE requestNID " + requestNID);
			}
			if (resp.got_resp == 1) {
				removeFromResList(requestNID);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return resp;
	}

	public void handleServiceMessage(String userJID, String message) {
		try {
			//System.out.println("1 handleServiceMessage for:" + message);
			if (message == null || message.trim().equals("")) {
				return;
			}
			if (message.indexOf("<Notification>") != -1) {
				return;
			}
			String key = "";
			String clientdata= "";
			try {
				int i = message.indexOf(cl_tag_s);
				int k = message.indexOf(cl_tag_e, i + 1);
				clientdata = message.substring(i + cl_tag_s.length(), k);
			} catch (Exception ex) {
				ex.printStackTrace();
			}
			try {
				int i = message.indexOf(rq_tag_s);
				int k = message.indexOf(cl_tag_e, i + 1);
				key = message.substring(i + rq_tag_s.length(), k);
			} catch (Exception ex) {
				ex.printStackTrace();
			}
			int state = -1;
			try {
				int i = message.indexOf(st_tag_s);
				int k = message.indexOf(cl_tag_e, i + 1);
				String str = message.substring(i + st_tag_s.length(), k);
				state = Utils.getIntValue(str, -1);
			} catch (Exception ex) {
				ex.printStackTrace();
			}
			try {
				synchronized (this) {
					addToResList(key, message, state, 1);
					//System.out.println("1 handleServiceMessage for:" + message);
					//notifyAll(); //Findbugs
					notify();
					//System.out.println("2 handleServiceMessage for:" +clientdata + " " + key);
				}
			} catch (Exception ex) {
			}
			long time = -1;
			if(clientdata.indexOf("_")!=-1){
				String[] arr = clientdata.split("_");
				if(arr.length>2){
					time = Utils.getLongValue(arr[1] , -1);
					if(time !=-1)
						System.out.println("\nDONE: "+ arr[0] + " timespent:"+(System.currentTimeMillis() - time)+ " time:"   +System.currentTimeMillis() + " " + key);
				}
			if(time ==-1 )
				System.out.println("\nDONE: "+System.currentTimeMillis() + " got response for:" + clientdata + " " + key);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	public void addToResList(RespXML resp) {
		//System.out.println("sent for request:" + resp.key);
		synchronized (resplist) {
			resplist.put(resp.key, resp);
		}
	}

	public RespXML addToResList(String key, String value, int state,
			int got_resp) {
		//System.out.println("got response for:" + key);
		RespXML resp = getFromResList(key);
		if (resp == null)// / its not there in list
			return null;
		resp.xml = value;
		resp.key = key;
		resp.state = state;
		resp.got_resp = got_resp;
		synchronized (resplist) {
			resplist.put(key, resp);
		}
		return resp;
	}
	public void printRespXML(RespXML b ){
		System.out.println(" " + b.key +"" +b.state +"" +b.xml+ " " + ( System.currentTimeMillis() ));
	}
	public RespXML getFromResList(String key) {
		synchronized (resplist) {
			return resplist.get(key);
		}
	}

	public RespXML removeFromResList(String key) {
		synchronized (resplist) {
			return resplist.remove(key);
		}
	}


	static class RespXML {
		String key = "";
		String xml = "";
		int state = -1;
		int got_resp = -1;
		long createTime = System.currentTimeMillis();
	}

	static class CleanupRespXML extends Thread {
		ServiceTestUtil parent = null;

		public void run() {
				while (!parent.stopping) {
					try {
						Thread.sleep(parent.cleanupRespXMLLoop);
						try {
							Object[] keys = null;
							synchronized (parent.resplist) {
								keys = parent.resplist.keySet().toArray();
							}
							for (int i = 0; i < keys.length; i++) {
								try {
									String key = (String) keys[i];
									RespXML resp = parent.getFromResList(key);
									if ((resp.createTime - System
											.currentTimeMillis()) > parent.timeoutResp) {
										parent.removeFromResList(key);
									}
								} catch (Exception ex) {
								}
							}
						} catch (Exception ex) {
						}
					} catch (InterruptedException ex) {
						ex.printStackTrace();
					}
				}
		}
	}

	// //////////////////////// login /////////////////////////

	public String getLoginResponse() {
		return response;
	}

	public boolean doLogout() {
		try {
			ServiceClient client = ServiceUtils.getServiceClient(this);
			response = client.logout(client.getUserJID(userJID));
			return isRequestFailed(response);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	public boolean doLogin_File(String xml) {
		String request = getXML(xml);
		if (request != null) {
			try {
				//Document doc = MRXMLUtils.stringToDocument(request);
				//user2 = MRXMLUtils.getValueXpath(doc, "//user","");
				response = ServiceUtils.getServiceClient(this).login(request);
				Document doc = MRXMLUtils.stringToDocument(request);
				userID = MRXMLUtils.getValueXpath(doc, "//user", "");
				doc = MRXMLUtils.stringToDocument(response);
				userJID = MRXMLUtils.getValueXpath(doc, "//userJID", "");
				
				
				return isRequestFailed(response);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return false;
	}

	public boolean doSetServiceDomain_File(String xml) {
		String request = getXML(xml);
		if (request != null) {
			try {
				response = ServiceUtils.getServiceClient(this)
						.setServiceDomain(userJID, request);
				return isRequestFailed(response);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return false;
	}

	private boolean isRequestFailed(String xml) {
		if (xml != null && !xml.equals("")) {
			try {
				Document doc = MRXMLUtils.stringToDocument(xml);
				String error = MRXMLUtils.getValueXpath(doc,
						"//Data/Exception", "").trim();
				int state = Utils.intValue(MRXMLUtils.getValueXpath(doc,
						"//Header/state", "-1").trim(), -1);
				if (error.length() > 0 || (state == -1 || state < 1))
					return false;
				return true;

			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return false;
	}

	// //////////////////////// login /////////////////////////

	public static void addSystemOut(int level, int code, String data,
			String systemOutAndLog) {
		try {
			// log.addSystemOut(level, code,data , systemOutAndLog);
			// if( level <= MRLog.ERROR)
			// logger.error(data);
			// logger2.log(Level.toLevel(level), data);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	private String getXML(String xml) {
		// String xml="/src/com/ipvs/test/xml/LoginRequest.xml";
		try {
			File file = new File(xml);
			if(file.exists()) {
				Document doc = MRXMLUtils.loadXMLFile(xml, MRLog.getInstance(MRLog.CONSOLE));
				return MRXMLUtils.documentToString(doc);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}		
		try {
			Document doc = MRXMLUtils.loadXMLResource(xml, MRLog.getInstance(MRLog.CONSOLE));
			return MRXMLUtils.documentToString(doc);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

}
