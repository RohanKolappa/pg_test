package com.barco.device.vng.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.ConnectException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;

import org.apache.log4j.Logger;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;
import com.barco.utils.mr.XMPPTransaction;

public class CommandLineUtils {
    static boolean dummy = false;
    public static final int SSPORT = 10001;
    public static final int SSTIMEOUT = 10000;
    public static String executeCLICommand(String[] cmdArray) throws Exception {
	if (dummy) {
	    return XMLUtils.loadXMLResource("/com/barco/device/vng/test/" + cmdArray[0] + ".xml").asXML();
	}
	Runtime runtime = Runtime.getRuntime();

	Process proc = runtime.exec(cmdArray);
	InputStream inputstream = proc.getInputStream();
	InputStreamReader inputstreamreader = new InputStreamReader(inputstream);
	BufferedReader bufferedreader = new BufferedReader(inputstreamreader);

	StringBuffer sb = new StringBuffer();
	sb.append(cmdArray[0]);
	sb.append("\n");

	String line = null;

	while ((line = bufferedreader.readLine()) != null) {
	    sb.append(line.trim());
	    sb.append("\n");
	}

	try {
	    if (proc.waitFor() != 0) {
		System.err.println("exit value = " + proc.exitValue() + "\n Command is=" + cmdArray[0]);
			throw new ServiceException(MediaRoomServiceExceptionCodes.EXECUTION_OF_COMMANDS_FIALED, "Execution of command failed");
	    }
	} catch (InterruptedException e) {
	    System.err.println(e);
	} finally {
	    bufferedreader.close();
	}

		return sb.toString();
    }
    
    public static int executeSystemCommand(String[] systemCmd) throws Exception {
    	StringBuffer sb = new StringBuffer();
    	return executeSystemCommand(systemCmd, sb);
    }
    
    public static int executeSystemCommand(String[] systemCmd, StringBuffer sb) throws Exception {
        Runtime runtime = Runtime.getRuntime();
        Process proc = runtime.exec(systemCmd);
        try {
        	int procResult = proc.waitFor();             
            if (procResult != 0) {
            	InputStream inputstream = proc.getErrorStream();
                InputStreamReader inputstreamreader = new InputStreamReader(inputstream);
                BufferedReader bufferedreader = new BufferedReader(inputstreamreader);

               // StringBuffer sb = new StringBuffer();
             
                String line = null;

                while ((line = bufferedreader.readLine()) != null) {
                    sb.append(line.trim());
                    sb.append("\n");
                }
                
                System.out.println("result of system command :"+sb.toString());
                System.err.println("exit value = " + proc.exitValue() + "\n Command is=" + systemCmd[0]);
            }
            return procResult;
        } catch (InterruptedException e) {
        	e.printStackTrace();
            System.err.println(e);
        }
        return -1;
    }
    public static String executeStaticSSCommand(String cmd,DeviceDBI deviceDB,Logger log) throws Exception {
	XMPPTransaction.startBlock("executeSSCommand");

	if (XMPPTransaction.isDEBUG()) {
	    System.out.println("<SSCommand><![CDATA[");
	    System.out.println(cmd);
	    System.out.println("]]></SSCommand>");
	}

	// Added retry logic if we fail to connect
	String response = null;
	int maxRetries = 10;
	int count = 0;
	while (response == null) {
	    count++;
	    try {
		response = executeCommand(cmd,deviceDB);
	    } catch (ConnectException ce) {
		log.info("CommandLineUtils:executeCommand failed retry#=" + count);
		if (count > maxRetries)
		    throw ce;
		Thread.sleep(1000);
	    }
	}

	if (cmd.indexOf("type=" + MediaRoomServiceConst.DVI) != -1) {
	    log.info("CommandLineUtils:executeSSCommand DVITest returning dummy response for cmd=" + cmd);
	    response = "200 OK";
	}

	XMPPTransaction.endBlock("executeSSCommand");

	return response.toString();
    }

	/**
	 * @return opens a socket on port 10001, writes the cmd, reads the answer
	 */
public static String executeCommand(String cmd,DeviceDBI deviceDB) throws Exception {
	String deviceIP = deviceDB.getIPAddress();
	SocketAddress sockaddr = new InetSocketAddress(deviceIP, SSPORT);
    Socket client = new Socket();
    BufferedWriter writer = null;
    BufferedReader reader = null;
    String line = null;
    StringBuffer response = new StringBuffer();

    // Make a connection to the streaming server
    // XMPPTransaction.startBlock("executeSSCommand.openSocket");
    try {
        client.connect(sockaddr, SSTIMEOUT);
    } catch (IOException e) {
        //log.addInfo(e.toString());
    	client.close();
       throw e;
    }

    // XMPPTransaction.endBlock("executeSSCommand.openSocket");
    try {
        // Send the command to the streaming server
        // XMPPTransaction.startBlock("executeSSCommand.writeCommand");
        if (client.isConnected()) {
            writer = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));
            writer.write(cmd);
            writer.flush();
        }

        // XMPPTransaction.endBlock("executeSSCommand.writeCommand");

        //Wait for the response
        if (client.isConnected()) {
            // XMPPTransaction.startBlock("executeSSCommand.readResponse");
            reader = new BufferedReader(new InputStreamReader(client.getInputStream()));

            // TBD : Look for a response delimiter, assuming \n is the delimiter
            while (true) {
                line = reader.readLine();

                if (line == null) {
                    break;
                }

                response.append(line);

                break;
            }

            // XMPPTransaction.endBlock("executeSSCommand.readResponse");
        }
    } catch (Exception exp) {
        exp.printStackTrace();
        throw new Exception(exp.toString());
    } finally {
        if (writer != null) {
            writer.close();
        }

        if (reader != null) {
            reader.close();
        }
    }

    // Close the connection to the streaming server
    client.close();
    
    return response.toString();
	
}
public static int executeProcessInThread(String[] cmd, StringBuffer buf,Logger log) throws Exception {
    //MRLog.println("executeProcessInBackground start");
    ExecHelper execHelper = new ExecHelper(cmd, buf);
    new Thread(execHelper).start();
    Thread.sleep(200);//wait for any system level errors in script execution
    int exitVal = execHelper.exitVal;
    if (exitVal != 0) {
	log.info("Opps.. Script exited with non zero value ....!!!");
        Utils.printArray(cmd);
        log.info("Script output: " + buf.toString());
    }
    //MRLog.println("executeProcessInBackground end");
    return exitVal;
}

public static class ExecHelper implements Runnable {
    int exitVal = 0;
    StringBuffer output = new StringBuffer();
    String[] cmd;
    public ExecHelper(String[] cmd, StringBuffer buf) {
        this.output = buf;
        this.cmd = cmd;
    }

    public void run() {
        try {
            exitVal = Utils.executeProcess(cmd, output);
        } catch (Exception ex) {
            exitVal=1;
            output.append("Error during process execution. " + ex.getMessage());
            ex.printStackTrace();
        }
    }
}


}
