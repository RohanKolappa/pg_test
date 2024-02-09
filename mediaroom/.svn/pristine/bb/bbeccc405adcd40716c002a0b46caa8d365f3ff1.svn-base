package com.ipvs.device.impl;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.device.DeviceAppI;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerImpl;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;


public class DeviceStreamingServerImpl extends StreamingServerImpl {
	
    public static final String SSHOST = "127.0.0.1";
    public static final int SSPORT = 10001;
    public static final int SSTIMEOUT = 10000;
    public static final int RESERVED_SSINSTANCEID=1000;
    DeviceAppI deviceApp;

    public DeviceStreamingServerImpl(String userJID, MRLog log, MRDB db,XMPPF xf, DeviceAppI deviceApp) {
        super(userJID, log, db, xf);
        this.deviceApp = deviceApp;
    }

    
    public String getIPAddress() throws Exception {
    	return DeviceAgentUtils.getInstance().getIPAddress();
    }
    

    public String executeSSCommand(String cmd) throws Exception {
        com.ipvs.common.XMPPTransaction.startBlock("executeSSCommand");

        if (com.ipvs.common.XMPPTransaction.isDEBUG()) {
            System.out.println("<SSCommand><![CDATA[");
            System.out.println(cmd);
            System.out.println("]]></SSCommand>");
        }

        String response  =  DeviceStreamingServerImpl.executeCommand(cmd);
        
        if(cmd.indexOf("type=" + MediaRoomServiceConst.DVI) != -1) {
    	    System.out.println("DeviceStreamingServerImpl:executeSSCommand DVITest returning dummy response for cmd=" + cmd);
        	response =  "200 OK";
        }
        

        com.ipvs.common.XMPPTransaction.endBlock("executeSSCommand");

        return response.toString();
    }
    public static String executeCommand(String cmd) throws MRException,Exception {
    	SocketAddress sockaddr = new InetSocketAddress(SSHOST, SSPORT);
        Socket client = new Socket();
        BufferedWriter writer = null;
        BufferedReader reader = null;
        String line = null;
        StringBuffer response = new StringBuffer();

        // Make a connection to the streaming server
        // com.ipvs.common.XMPPTransaction.startBlock("executeSSCommand.openSocket");
        try {
            client.connect(sockaddr, SSTIMEOUT);
        } catch (IOException e) {
            //log.addInfo(e.toString());
           client.close();
           throw e;
        }

        // com.ipvs.common.XMPPTransaction.endBlock("executeSSCommand.openSocket");
        try {
            // Send the command to the streaming server
            // com.ipvs.common.XMPPTransaction.startBlock("executeSSCommand.writeCommand");
            if (client.isConnected()) {
                writer = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));
                writer.write(cmd);
                writer.flush();
            }

            // com.ipvs.common.XMPPTransaction.endBlock("executeSSCommand.writeCommand");

            //Wait for the response
            if (client.isConnected()) {
                // com.ipvs.common.XMPPTransaction.startBlock("executeSSCommand.readResponse");
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

                // com.ipvs.common.XMPPTransaction.endBlock("executeSSCommand.readResponse");
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
   

    
    protected String setupEncoderPort(Element inputElement, Document streamDataDoc) {
        String srcPortElementID = MRXMLUtils.getAttributeValue(inputElement,"sourcePortElementID");         
	    return this.deviceApp.setTXConnectionParameters(srcPortElementID, streamDataDoc);
    }
    
    
    protected void teardownEncoderPort(Element inputElement) {
    	//Placeholder for tearing down the encoder port
    }

	@Override
    protected void setupDecoderPort(Element outputElement) {
        String destPortElementID = MRXMLUtils.getAttributeValue(outputElement,"destPortElementID");         
        try {
            teardownInternalDecoder(destPortElementID);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
	@Override
	protected void teardownDecoderPort(Element outputElement) {
        String destPortElementID = MRXMLUtils.getAttributeValue(outputElement,"destPortElementID");
        try {
            setupInternalDecoder(destPortElementID);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

    protected String executeStatus(String statusCmd) throws Exception {
        log.addInfo("StreamingServer status command is ->" + statusCmd);

        String statusResult = this.executeSSCommand(statusCmd);

        if ((statusResult == null) || statusResult.contains("201 OK")) {
            throw new MRException(MediaRoomServiceExceptionCodes.GETINSTANCESTATUSFAILED, "Get STREAMING SERVER INSTANCE STATUS FAILED cmd=" + statusCmd + " result=" + statusResult);
        }

        return statusResult;
    }


	@Override
	public void setupInternalInstances(Document doc) throws Exception {
		List<?> l = null;
        Element el = null;
        Element port=null;
    	Document deviceDoc = DeviceAgentUtils.getInstance().getDeviceXML();
    	if (doc == null) {
    		l=MRXMLUtils.getListXpath(deviceDoc, "//XP200PortConfig/PortID[../Info[PortIOType='StreamDst']");
    	}
    	else {
    		l=MRXMLUtils.getListXpath(doc, "//XP200PortConfig/PortID[../Info[PortIOType='StreamDst']");
    	}
        if ((l != null) && !l.isEmpty()) {
            for (int i = 0; i < l.size(); i++) {
                el = (Element) l.get(i);
                if (doc != null) {
                	port = MRXMLUtils.getElementXpath(deviceDoc, "//XP200PortConfig/Info/PortIOType[../../PortID='" + el.getStringValue()+ "']");
                	if (port.getStringValue().compareTo("StreamDst") != 0) {
                		setupInternalDecoder(el.getStringValue());
                	}
                }
                else {
            		setupInternalDecoder(el.getStringValue());
                }
			}
        }
        else {
        	System.out.println("List is empty");
        }
	}


	private void setupInternalDecoder(String portId) throws Exception {
		// TODO Auto-generated method stub
        int nSSInstanceID=0;
        int nSSQueueID=0;
        nSSInstanceID = DeviceStreamingServerImpl.RESERVED_SSINSTANCEID + 2*Integer.parseInt(portId);
        nSSQueueID= nSSInstanceID +1;
        executeSSCommand (
        		"action=add id=" + nSSInstanceID + 
        		" operation=output dsttype=V2DHardWarePort ioport=" +
        		portId);
        executeSSCommand(
        		"action=start id=" + nSSInstanceID + 
        		" operation=output dsttype=V2DHardWarePort queueid=" + nSSQueueID);
	}
	private void teardownInternalDecoder(String portId) throws Exception {
		// TODO Auto-generated method stub
        int nSSInstanceID=0;
        int nSSQueueID=0;
        nSSInstanceID = DeviceStreamingServerImpl.RESERVED_SSINSTANCEID + 2*Integer.parseInt(portId);
        nSSQueueID= nSSInstanceID +1;
        executeSSCommand (
        		"action=stop id=" + nSSInstanceID + 
        		" operation=output dsttype=V2DHardWarePort ioport=" +
        		portId);
        executeSSCommand(
        		"action=delete id=" + nSSInstanceID + 
        		" operation=output dsttype=V2DHardWarePort queueid=" + nSSQueueID);
	}


	@Override
	public int getMaxEncoderInstances(String streamType) {
		return DeviceAgentUtils.getInstance().getMaxEncoderInstances(streamType);
	}


	@Override
	protected void simulateRecordProgressStoppedNotfication(Element outputElement,String ssInstanceNID) throws MRException,Exception {
		Element streamElement = outputElement.getParent();
		String errorCode = MRXMLUtils.getAttributeValue(streamElement, SessionServiceConst.ERROR_CODE,"");			
		double timeScale = Utils.getDoubleValue(MRXMLUtils.getAttributeValueXpath(streamElement, "./MediaTimeline/Last","TimeScale", "1"),1);
		
		//if recording is  in paused state then send progress stopped notification to the streaming server
    	//We are simulating the progress stopped notification because streaming server not sending us notification on delete instance
    	if(timeScale == 0 || !errorCode.isEmpty()) {
    		log.addInfo("DeviceStreamingServerImpl: Simulate Recording stopped message:="+ssInstanceNID + " timeScale:="+timeScale +" errorCode:="+errorCode,MRLog.INFO,MRLog.NOTIFY);    	    
    		sendProgressStoppedSSEventToSourceStreamAgent(outputElement, ssInstanceNID);
    	}		
	}
}
