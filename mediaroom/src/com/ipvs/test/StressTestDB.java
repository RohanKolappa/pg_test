package com.ipvs.test;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import java.util.Random;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgent;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class StressTestDB {
	private static final String DELETE = "DELETE";
	private static final String ADD = "ADD";
	private static final String UPDATE = "UPDATE";
	private static final String READ = "READ";
	private static final String REPLACE_COUNTER = "${CN}";

	private Properties props = null;

	private int totalThreads = 1;
	private int threadsLoopCount = 1;
	private boolean repeatLoopNonStop = false;
	private int repeatLoopNonStopSleep = 0;

	private boolean addEnable = false;
	private boolean deleteEnable = false;
	private boolean readEnable = false;
	private boolean updateEnable = false;

	private String deleteXmlFile;
	private String addXmlFile;
	private String readXmlFile;
	private String updateXmlFile;
	private boolean deleteXmlReplaceCounter;
	private boolean addXmlFileReplaceCounter;
	private boolean readXmlFileReplaceCounter;
	private boolean updateXmlFileReplaceCounter;

	private boolean waitforResult;
	private String loginXML="";
	private String setDomainXML="";

	private Hashtable<String, String> xmls = new Hashtable<String, String>();
	private ServiceTestUtil mServiceTestUtil = null;

	private static String propsFile = "/com/ipvs/test/xml/StressTestDB.properties";
	public static void main(String[] args) {
		try {
			String propsPath = propsFile;
			String loadType = "file";//todo
			if (args.length > 1) {
				//loadType = args[0].trim();
				propsPath = args[0].trim();
			}
			StressTestDB test = new StressTestDB();
			test.initProperties(loadType, propsPath);
			test.process();
			
			
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		System.exit(0);
		
	}

	public void process() {
		try {
			mServiceTestUtil = new ServiceTestUtil();
			//Login
			//setDomain
			if (login()) {
				List<DBExecuteThread> list = new ArrayList<DBExecuteThread>();
				for (int i = 1; i < totalThreads ; i++) {
					list.add(new DBExecuteThread(i));
				}
				for (Iterator<DBExecuteThread> iterator = list.iterator(); iterator.hasNext();) {
					DBExecuteThread executeThread = iterator.next();
					executeThread.start(); 
				}
					 
				while(!list.isEmpty()){
					Thread.sleep(1000);
					List<DBExecuteThread> list2 = new ArrayList<DBExecuteThread>();
					for (Iterator<DBExecuteThread> iterator = list.iterator(); iterator.hasNext();) {
						DBExecuteThread executeThread = iterator.next();
						if(executeThread.done) 
							list2.add(executeThread);
					}
					while(!list2.isEmpty()){
						DBExecuteThread executeThread = list2.remove(0);
						list.remove(executeThread);
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		//Logout
		logout() ;
		
	}

	public boolean login() {
		 MRClientAgent.setLegacyResponse(true);
		if(!mServiceTestUtil.doLogin_File(loginXML)){
			System.out.println("FAILED Login: " + mServiceTestUtil.getLoginResponse()  );
 			return false;
		}
		if(!mServiceTestUtil.doSetServiceDomain_File( setDomainXML )){
			System.out.println("FAILED SetServiceDomain: " + mServiceTestUtil.getLoginResponse()  );
			return false;
		}
		return true;
	}
	public boolean logout() {
		return mServiceTestUtil.doLogout();
	}
	
	////////////////////////DBExecuteThread  ////////////////////////////////////////////////////////////////////////
	////////////////////////DBExecuteThread  ////////////////////////////////////////////////////////////////////////
	////////////////////////DBExecuteThread  ////////////////////////////////////////////////////////////////////////
	class DBExecuteThread extends Thread {
		int threadNumber = 0;
		boolean done=false;
		public DBExecuteThread(int threadNumber) {
			this.threadNumber = threadNumber;
		}
		public void run() {
			try {
				processLoop(threadNumber);
				//System.out.println("DONE  Thread_" + threadNumber);
			} catch (Exception ex) {
				ex.printStackTrace();
			}
			done=true;
		}
		
	}
	//////////////////////////////////////////
	//boolean isDebug=false;
	public void processLoop(int threadNumber) {
		int doWhilecounter = 0;
		//long t0 = System.currentTimeMillis();
		//long tLoop =  System.currentTimeMillis();
		//System.out.println("PROCESS THREAD_" + threadNumber + " START - time: diff: " + (System.currentTimeMillis() - t0) + " - " + System.currentTimeMillis());
		try {
			do {
				doWhilecounter++;
				//System.out.println("\n\n\nSTART_REPEATE_LOOP :Thread_" + threadNumber +" no-" + doWhilecounter);
				//tLoop = System.currentTimeMillis();
				String name = "R" + doWhilecounter + "T" + threadNumber;
				//System.out.println( "\n START "+ name + "  time: diff: " + (System.currentTimeMillis() - tLoop) + " - " + System.currentTimeMillis());
				try {
					//todo: sequence of run 
					if (addEnable) {
						String xml = getXML(ADD);
						boolean replaceCounter = addXmlFileReplaceCounter;
						doRunDB(ADD, name, xml, replaceCounter);
					}
					if (updateEnable) {
						String xml = getXML(UPDATE);
						boolean replaceCounter = updateXmlFileReplaceCounter;
						doRunDB(UPDATE, name, xml, replaceCounter);
					}
					if (readEnable) {
						String xml = getXML(READ);
						boolean replaceCounter = readXmlFileReplaceCounter;
						doRunDB(READ, name, xml, replaceCounter);
					}
					if (deleteEnable) {
						String xml = getXML(DELETE);
						boolean replaceCounter = deleteXmlReplaceCounter;
						doRunDB(DELETE, name, xml, replaceCounter);
					}
					if (repeatLoopNonStopSleep > 0)
						Thread.sleep(repeatLoopNonStopSleep);
				} catch (Exception e) {
					e.printStackTrace();
				}
				//if(isDebug){
				//	System.out.println( "\n END "+ name + "  time: diff: " + (System.currentTimeMillis() - tLoop) + " - " + System.currentTimeMillis());
				//	System.out.println("\n\n\nLOOP end " + doWhilecounter +":T_" + threadNumber);
				//}
			} while (repeatLoopNonStop);

			
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			//System.out.println("PROCESS THREAD_" + threadNumber + " END - time: diff: " + (System.currentTimeMillis() - t0) + " - " + System.currentTimeMillis());
		}
	}
	public void doRunDB(String ID, String doWhilecounter, String xml, boolean replaceCounter) {
		//long t0 = System.currentTimeMillis();
		//long tLoop = 0;
		//System.out.println( doWhilecounter + "_" + ID + " START - time: diff: " + (System.currentTimeMillis() - t0) + " - " + System.currentTimeMillis());
		try {
			//tLoop = System.currentTimeMillis();
			Document doc = null;
			if (!replaceCounter) {
				doc = MRXMLUtils.stringToDocument(xml);
			}
			for (int i = 1; i < threadsLoopCount; i++) {
				String name = doWhilecounter+ "L" + i + "" + ID ;
				//System.out.println(name + " START time: diff: " + (System.currentTimeMillis() - tLoop) + " - " + System.currentTimeMillis());
				try {
					if (replaceCounter) {
						String xmlLoop = xml;
						xmlLoop = xml.replace(REPLACE_COUNTER, doWhilecounter+"L"+i + "");
						doc = MRXMLUtils.stringToDocument(xmlLoop);
					}
					Element clientdata = MRXMLUtils.getElementXpath(doc, "//header/clientdata");
					if(clientdata !=null){
						clientdata.setText( getNextClientData(name));
					}
					try {
						ServiceTestUtil.RespXML resp = mServiceTestUtil.doSend(MRXMLUtils.documentToString(doc), " GetRequest ", waitforResult);
						if (resp.got_resp != 1) {
							
						}
					} catch (Exception ex) {
						ex.printStackTrace();
					}
				} catch (Exception e) {
				}
				//System.out.println("DONE: "+name + " timeSpent: " + (System.currentTimeMillis() - tLoop) + " ms waitforResult="+waitforResult+" time- " + System.currentTimeMillis());
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		} finally {
			//System.out.println( doWhilecounter + "_" + ID + " END - time: diff: " + (System.currentTimeMillis() - t0) + " - " + System.currentTimeMillis());
		}
	}	
	public static final Random wheel = new Random();
	private String getNextClientData(String seed){
		return  seed +"_"+ System.currentTimeMillis() +"_"+ (Integer.toHexString(wheel.nextInt()));
	}
	//////////////////////////////////// used in DBExecuteThread ////////////////////////////////////

	////////////////////////DBExecuteThread  ////////////////////////////////////////////////////////////////////////
	////////////////////////DBExecuteThread  ////////////////////////////////////////////////////////////////////////
	////////////////////////DBExecuteThread  ////////////////////////////////////////////////////////////////////////

	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	public String getXML(String key) {
		if (xmls.containsKey(key))
			return xmls.get(key);
		return "";
	}

	public void initProperties(String loadType, String propFile) {
		try {
			/*
				############login ################
				loginXML=/com/ipvs/test/xml/StressTestDB_login.xml
				setDomainXML=/com/ipvs/test/xml/StressTestDB_setServiceDomain.xml
				
				############ wait response - synchronised sendXML ##############
				wait.for.result.synchronized=true
				
				################### thread and connections ################
				threads.count=30 threads.loop.count.per.thread=200
				threads.repeat.loop.nonstop=false
				threads.repeat.loop.nonstop.sleep.before.next.loop=0
				
				################ enable test ################# true false
				read.test.enable=true
				add.test.enable=false
				delete.test.enable=false
				update.test.enable=false
				
				################ test xml #################
				read.test.xml=/com/ipvs/test/xml/StressTestDB_read.xml
				add.test.xml=/com/ipvs/test/xml/StressTestDB_add.xml
				delete.test.xml=/com/ipvs/test/xml/StressTestDB_delete.xml
				update.test.xml=/com/ipvs/test/xml/StressTestDB_udpate.xml
				
				################ test xml replace counter ${N}#################
				read.test.xml.replace.counter=false
				add.test.xml.replace.counter=false
				delete.test.xml.replace.counter=false
				update.test.xml.replace.counter=false
 			 */
			props= new Properties();
			try {
				props = Utils.loadProperties(propFile);
			} catch (Exception ex) {
				try {
					props = Utils.loadPropertiesResource(props, propFile, this.getClass());
				} catch (Exception ex2) {
					ex.printStackTrace();
					ex2.printStackTrace();
				}
			}

			loginXML = props.getProperty("loginXML", "").trim();
			setDomainXML = props.getProperty("setDomainXML", "").trim();
			waitforResult = Utils.getBooleanValue(props.getProperty("wait.for.result.synchronized", "false").trim(), false);

			totalThreads = Utils.getIntValue(props.getProperty("threads.count", "1").trim(), 1)+1;
			threadsLoopCount = Utils.getIntValue(props.getProperty("threads.loop.count.per.thread", "1").trim(), 1)+1;
			repeatLoopNonStop = Utils.getBooleanValue(props.getProperty("threads.repeat.loop.nonstop", "false").trim(), false);
			repeatLoopNonStopSleep = Utils.getIntValue(props.getProperty("threads.repeat.loop.nonstop.sleep.before.next.loop", "0").trim(), 0);

			// ################## deletexmltest ##########
			deleteEnable = Utils.getBooleanValue(props.getProperty("delete.test.enable", "false").trim(), false);
			addEnable = Utils.getBooleanValue(props.getProperty("add.test.enable", "false").trim(), false);
			readEnable = Utils.getBooleanValue(props.getProperty("read.test.enable", "false").trim(), false);
			updateEnable = Utils.getBooleanValue(props.getProperty("update.test.enable", "false").trim(), false);

			deleteXmlFile = props.getProperty("delete.test.xml", "").trim();
			addXmlFile = props.getProperty("add.test.xml", "").trim();
			readXmlFile = props.getProperty("read.test.xml", "").trim();
			updateXmlFile = props.getProperty("update.test.xml", "").trim();

			deleteXmlReplaceCounter = Utils.getBooleanValue(props.getProperty("delete.test.enable.replace.counter", "false").trim(), false);
			addXmlFileReplaceCounter = Utils.getBooleanValue(props.getProperty("add.test.enable.replace.counter", "false").trim(), false);
			readXmlFileReplaceCounter = Utils.getBooleanValue(props.getProperty("read.test.enable.replace.counter", "false").trim(), false);
			updateXmlFileReplaceCounter = Utils.getBooleanValue(props.getProperty("update.test.enable.replace.counter", "false").trim(), false);

			if (!deleteXmlFile.equals("")) {
				String xml = readXML(deleteXmlFile);
				xmls.put(DELETE, xml);
			}
			if (!addXmlFile.equals("")) {
				String xml = readXML(addXmlFile);
				xmls.put(ADD, xml);
			}
			if (!readXmlFile.equals("")) {
				String xml = readXML(readXmlFile);
				xmls.put(READ, xml);
			}
			if (!updateXmlFile.equals("")) {
				String xml = readXML(updateXmlFile);
				xmls.put(UPDATE, xml);
			}

		} catch (Exception ex) {

		}
	}

	private static String readXML(String propFile) throws Exception {
		try {
			return readFile(new FileInputStream(new File(propFile)));
		} catch (Exception ex) {
			try {
				return readFile(StressTestDB.class.getResourceAsStream(propFile));
			} catch (Exception ex2) {
				return "";
			}
		}
	}

	private static String readFile(InputStream fis) throws Exception {
		BufferedReader br=null;
		StringBuffer buf = new StringBuffer();
		try {
			br   = new BufferedReader(new InputStreamReader(fis));
			StringBuilder sb = new StringBuilder();
			String line = null;
			while ((line = br.readLine()) != null) {
				sb.append(line + "\n");
			}			
		} catch (Exception ex) {
			throw new Exception("readFile error ." + " " + ex.getMessage());
		} finally {
			// dispose all the resources after using them.
			try {
				br.close();
			} catch (Exception e) {
			}
			try {
				fis.close();
			} catch (Exception e) {
			}
		}
		return buf.toString();
	}
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////

}
