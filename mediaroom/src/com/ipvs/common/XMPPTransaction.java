package com.ipvs.common;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Stack;

class BlockTrace {
	String name;
	long startTime;
	//StringBuffer trace;
	BlockTrace(String name) {
		this.name = name;
		this.startTime = System.currentTimeMillis();
		//this.trace = new StringBuffer();
	}
}

public class XMPPTransaction {

	private Object response;
	private Exception exception;
	private boolean receivedResponse;
	private String id;
	public static final long WAIT_TIME = 40*1000; 
	long startTime = 0;
	String client = null;
	public String clientdata = null;
	private static boolean DEBUG = false; //Findbugs suppress this warning
	private static Stack<BlockTrace> BlockList = new Stack<BlockTrace>();
	private long timeout=WAIT_TIME;
	//added to debug
	private long time_started =0;
	private static long threadID = 0;

    public static boolean isDEBUG() {
    	if(XMPPTransaction.threadID != 0) {
    		if(Thread.currentThread().getId() != XMPPTransaction.threadID)
    			return false;
    	}
		return XMPPTransaction.DEBUG;
	}

	public static void setDEBUG(boolean debug) {
		XMPPTransaction.setDEBUG(debug, 0);
	}

	public static void setDEBUG(boolean debug, long threadID) {
		System.out.println("XMPPTransaction:setDEBUG Setting debug=" + debug + " for threadID=" + threadID);
		if(XMPPTransaction.DEBUG && debug) {
			System.out.println("XMPPTransaction:setDEBUG trying to set debug for threadID=" + threadID + 
					", debug already set for threadID=" + XMPPTransaction.threadID);
			return;
		}
		XMPPTransaction.threadID = threadID;
		XMPPTransaction.DEBUG = debug;
	}

	public XMPPTransaction(String id, String client, String clientdata) {
 		this(id, client, clientdata, WAIT_TIME);
	}
	public XMPPTransaction(String id, String client, String clientdata, long timeout) {
        response = null;
        exception = null;
        receivedResponse = false;
        //added to debug
        time_started=System.currentTimeMillis();
        //this.id = new String(id);
        this.id = id;
        this.client = client;
        this.clientdata = clientdata;
        this.timeout=timeout;
    }

	public synchronized void waitForResponse() throws Exception {
		this.waitForResponse(timeout);
	}
	public synchronized void waitForResponse(long timeout) throws Exception {
		this.startTrace();
		if (this.receivedResponse) {
			this.endTrace();
			return;
		}
		// System.out.print("W");
		wait(timeout);
		// System.out.print("D");
		//added to debug
		if(response==null || System.currentTimeMillis()- time_started > timeout)
		    MRLog.println(id + " XMPPTransaction waitForResponse: "  + ( System.currentTimeMillis()- time_started) + " milisec.  is response null? " +(response==null) );
		this.endTrace();
	}

	public synchronized void setResponse(Object response) {
		this.receivedResponse = true;
		this.response = response;
		notify();

	}

	public synchronized void setException(Exception exception) {
		this.exception = exception;
		setResponse(null);
	}

	public synchronized Object getResponse() throws Exception {
		if (exception != null) {
			StringWriter sw = new StringWriter();
			exception.printStackTrace(new PrintWriter(sw));
	        //added to debug
	        if(System.currentTimeMillis()- time_started > timeout)
	            MRLog.println(id + " XMPPTransaction exception "   + sw.toString());
			throw new MRException(MRExceptionCodes.SYSTEMERROR, sw.toString());
		}
        //added to debug
        if(response==null || System.currentTimeMillis()- time_started > timeout)
            MRLog.println(id + " XMPPTransaction getResponse: "  + (System.currentTimeMillis()- time_started ) +" milisec. is response null? " +(response==null)  );
		return response;
	}

	public synchronized String getId() {
		return id;
	}

	public synchronized void setId(String transaction_id) {
		id = transaction_id;
	}

	private void startTrace() {
		if(!XMPPTransaction.isDEBUG())
			return;
		this.startTime = System.currentTimeMillis();
		String tag = "t." + client + "." + clientdata;
		System.out.println("<" + tag + "/>");

	}
	
	private void endTrace() {
		if(!XMPPTransaction.isDEBUG())
			return;
		long timeSpent = System.currentTimeMillis() - this.startTime;
		String tag = "t." + String.valueOf(timeSpent) + "." + client + "." + clientdata;
		System.out.println("<" + tag + ">");
		System.out.println( "<stacktrace><![CDATA[");
		Exception e = new Exception();
		StackTraceElement[] st = e.getStackTrace();
		for(int i = 0; i < st.length; i++) {
			String func = st[i].toString();
			if(i > 15)
				break;
			System.out.println("\t" + func);
		}
		System.out.println( "]]></stacktrace>");
		System.out.println("</" + tag + ">");
	}
	
	public static void startBlock(String blockName) {
		XMPPTransaction.startBlock(blockName, false);
	}
	
	public static void startBlock(String blockName, boolean debug) {
		if(!XMPPTransaction.isDEBUG() && !debug)
			return;
		XMPPTransaction.BlockList.push(new BlockTrace(blockName));
		long now = System.currentTimeMillis();
		System.out.println("<" + blockName + " time='" + now + "'>");
	}
	
	public static void endBlock(String blockName) {
		XMPPTransaction.endBlock(blockName, false);
	}
	
	public static void endBlock(String blockName, boolean debug) {
		if(!XMPPTransaction.isDEBUG() && !debug)
			return;
		BlockTrace bt = XMPPTransaction.BlockList.pop();
		if(!bt.name.equals(blockName)) { 
			Exception e = new Exception();
			e.printStackTrace();
			System.out.println("Mismatched blockName=" + blockName + " expecting endBlock for blockName=" + bt.name);
		}
		long now = System.currentTimeMillis();
		// System.out.println("</" + blockName + " time='" + now + "'>");
		System.out.println("</" + blockName + ">");
		long timeSpent = now - bt.startTime;
		System.out.println("<T." + String.valueOf(timeSpent) + "." + blockName + "/>");
	}
}
