package com.ipvs.mediaroomservice.streamingserver;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.mediaroomservice.impl.SourceStreamAgent;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public abstract class StreamingServerImpl implements StreamingServerI {
	
    protected String userJID = null;
    protected MRLog log = null;
    protected MRDB db = null;
    protected XMPPF xf = null;
    private NetworkResource networkResource;
   protected static final String DMS_RECORD_PORT_PROGRESS_STOP_SSEVENT_XML = "/com/ipvs/mediaroomservice/streamingserver/DMSRecordPortProgressStoppedSSEvent.xml";

    public abstract String getIPAddress() throws Exception;
    protected abstract String executeSSCommand(String cmd) throws Exception;
    protected abstract String setupEncoderPort(Element inputElement, Document streamDataDoc);
    protected abstract void setupDecoderPort(Element outputElement);
    protected abstract void teardownDecoderPort(Element outputElement);
    protected abstract void teardownEncoderPort(Element inputElement);
    protected abstract void simulateRecordProgressStoppedNotfication(Element outputElement,String ssInstanceNID) throws MRException,Exception ;
    //private static boolean isInitialized = false;


   /* public static void setInitialized(boolean isInitialized) {
		StreamingServerImpl.isInitialized = isInitialized;
	}*/
    
	public StreamingServerImpl(String userJID, MRLog log, MRDB db,XMPPF xf) {
        this.userJID = userJID;
        this.log = log;
        this.db = db;
        this.xf = xf;
        this.networkResource = new NetworkResource();
        try {        	
			this.initStreamingServer();
		} catch (MRException e) {			
			e.printStackTrace();
		} catch (Exception e) {			
			e.printStackTrace();
		}
    }

    
    public NetworkResource getNetworkResource() {
    	return this.networkResource;
    }
    
   /* public void setNetworkResource(NetworkResource networkResource) {
    	this.networkResource = networkResource;
    }*/
    
    public static SSAPI getSSAPIFromStreamType (String streamType) throws Exception {
    	if(MediaRoomServiceConst.V2D.equals(streamType))
			return new V2DSSAPI();
		if(MediaRoomServiceConst.RTP.equals(streamType))
			return new RTPSSAPI();
		if(MediaRoomServiceConst.UDP.equals(streamType))
			return new UDPSSAPI();
		if(MediaRoomServiceConst.MPEGTS.equals(streamType))
			return new MPEGTSSSAPI();
		if(MediaRoomServiceConst.DVI.equals(streamType))
			return new DVISSAPI();
		if(MediaRoomServiceConst.RTP_RAW.equals(streamType))
			return new RTPRAWSSAPI();
		if (MediaRoomServiceConst.RTP_PCM.equals(streamType))
			return new RTPPCMSSAPI();
		if( MediaRoomServiceConst.RFB.equals(streamType)) 
			return new RFBSSAPI();
        throw new MRException(MediaRoomServiceExceptionCodes.STREAMTYPENOTFOUND, 
        		"<streamType>" + streamType + "</streamType>");
    }

	public SSAPI getSSAPI(Element element) throws Exception  {
		if(element == null)
			return new V2DSSAPI();
		String streamType = MRXMLUtils.getAttributeValue(element,"streamType");
		SSAPI ssAPI = getSSAPIFromStreamType(streamType);
        return ssAPI;
	}

    
    private void executeStop(String stopCmd) throws Exception {
        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeStop - start" , MRLog.OK, MRLog.NOTIFY);
        log.addInfo("StreamingServer stop command is ->" + stopCmd);

        String stopResult = this.executeSSCommand(stopCmd);

        if ((stopResult == null) || stopResult.startsWith("201 OK")) {
            log.addInfo("StreamingServer error cmd=" + stopCmd);
            throw new MRException(MediaRoomServiceExceptionCodes.STOPSSINSTANCEIDFAILED, "STOP STREAMING SERVER INSTANCE  FAILED: RESULT = " + stopResult + " cmd=" + stopCmd);
        }

        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeStop - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeStart(String startCmd) throws Exception {
        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeStart - start" , MRLog.OK, MRLog.NOTIFY);
        log.addInfo("StreamingServer start command is ->" + startCmd);

        String startResult = this.executeSSCommand(startCmd);

        if ((startResult == null) || startResult.startsWith("201 OK")) {
            log.addInfo("StreamingServer error cmd=" + startCmd);
            throw new MRException(MediaRoomServiceExceptionCodes.STARTSSINSTANCEIDFAILED, "START STREAMING SERVER INSTANCE  FAILED : RESULT = " + startResult + " cmd=" + startCmd);
        }

        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeStart - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeUpdate(String updateCmd) throws Exception {
        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeUpdate - start" , MRLog.OK, MRLog.NOTIFY);
        log.addInfo("StreamingServer updateCmd command is ->" + updateCmd);

        String updateResult = this.executeSSCommand(updateCmd);

        if ((updateResult == null) || updateResult.startsWith("201 OK")) {
            log.addInfo("StreamingServer error cmd=" + updateCmd);
            throw new MRException(MediaRoomServiceExceptionCodes.UPDATESSINSTANCEIDFAILED, "UPDATE STREAMING SERVER INSTANCE  FAILED: RESULT = " + updateResult + " cmd=" + updateCmd);
        }

        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeUpdate - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeCreate(String addCmd) throws Exception {
        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeCreate - start" , MRLog.OK, MRLog.NOTIFY);
        log.addInfo("StreamingServer add command is ->" + addCmd);

        String createResult = this.executeSSCommand(addCmd);

        if ((createResult == null) || createResult.startsWith("201 OK")) {
            log.addInfo("StreamingServer error cmd=" + addCmd);
            throw new MRException(MediaRoomServiceExceptionCodes.ADDSSINSTANCEIDFAILED, "ADD STREAMING SERVER INSTANCE  FAILED : RESULT = " + createResult + " cmd=" + addCmd);
        }

        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeCreate - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeDelete(String delCmd) throws Exception {
        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeDelete - start" , MRLog.OK, MRLog.NOTIFY);
        log.addInfo("StreamingServer delete command is ->" + delCmd);

        String deleteResult = this.executeSSCommand(delCmd);

        if ((deleteResult == null) || deleteResult.startsWith("201 OK")) {
            log.addInfo("StreamingServer error cmd=" + delCmd);
            throw new MRException(MediaRoomServiceExceptionCodes.DELETESSINSTANCEIDFAILED, "DELETE STREAMING SERVER INSTANCE  FAILED : RESULT = " + deleteResult + " cmd=" + delCmd);
        }

        //perf-logs this.log.addInfo("DeviceSourceStreamAgent::executeDelete - end" , MRLog.OK, MRLog.NOTIFY);
    }

    public void stopSSInput(Element inputElement, Element streamElement) throws Exception {
        StringBuffer cmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(inputElement);
    	ssAPI.stopSSInputCmd(inputElement, streamElement, cmd);
        executeStop(cmd.toString());
        //rest start time
        MRXMLUtils.addAttribute(inputElement,"startTime", String.valueOf(0));
        log.addInfo("DeviceSourceStreamAgent:stopSSInput:=" + this.userJID);
    }

    public void startSSInput(Element instanceElement, Element streamElement)
    throws Exception {
        StringBuffer cmd = new StringBuffer();
	    Element inputEL = instanceElement.element("Input");
	    String startTime = MRXMLUtils.getAttributeValue(inputEL,"startTime");
	    if(!"0".equals(startTime))
	    	return;
    	SSAPI ssAPI = this.getSSAPI(inputEL);
    	ssAPI.startSSInputCmd(instanceElement, streamElement, cmd);
	    String inputType = MRXMLUtils.getAttributeValue(inputEL,"inputType");
	 
        if (!MediaRoomServiceConst.INPUT_RELAY.equals(inputType) && !MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
	        executeStart(cmd.toString());
	    }
        MRXMLUtils.addAttribute(inputEL,"startTime", String.valueOf(this.xf.xmpp(userJID).getCurrentTimeMillis()));
	    log.addInfo("DeviceSourceStreamAgent:startSSInput:=" + this.userJID);
    }

    
    public void refreshSSInput(Element inputElement, Document streamDataDoc)
    throws Exception {
    	StringBuffer cmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(inputElement);
    	ssAPI.refreshSSInputCmd(inputElement, streamDataDoc, cmd);
        executeUpdate(cmd.toString());
    }

    public void createSSInput(Element inputElement, Document streamDataDoc, Document mediaSourceDataDoc)
        throws Exception {
        StringBuffer cmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(inputElement);
        String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
        String ioport = "";
        if (MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)) {
            //perf-logs this.log.addInfo(this.userJID + ": DeviceSourceStreamAgent:add Encoder ssInstanceID=" + ssInstanceID);
        	ioport = this.setupEncoderPort(inputElement, streamDataDoc);
        }
    	ssAPI.createSSInputCmd(inputElement, streamDataDoc, mediaSourceDataDoc, cmd, ioport);
        if (!MediaRoomServiceConst.INPUT_RELAY.equals(inputType) && !MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            executeCreate(cmd.toString());
        }
    }
    
    public boolean isExternal() {
    	return false;
    }

    public void updateSSNetworkInput(Element inputElement, Document streamDataDoc, String action)
        throws Exception {
        StringBuffer addCmd = new StringBuffer();
        StringBuffer startCmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(inputElement);
    	ssAPI.updateSSNetworkInputCmd(inputElement, streamDataDoc, action, this.getNetworkResource(), log, addCmd, startCmd);

	    boolean isInstanceAdded = false;
	
	    //perf-logs this.log.addInfo("Checking inputInstanceStarted" + MRXMLUtils.elementToString(inputElement), MRLog.OK, MRLog.NOTIFY);
	    isInstanceAdded = this.isInputInstanceStarted(inputElement);
	
	    if (isInstanceAdded) {
	        deleteSSInput(inputElement);
	    }

        String streamURL = MRXMLUtils.getAttributeValue(inputElement,"streamURL");

        // Update the addCmd to include the streamURL
        if(this.isExternal()) {
        	addCmd.append(" streamURL='" + streamURL + "'");
        	String windowId = MRXMLUtils.getValueXpath(streamDataDoc, "//"+SessionServiceConst.WINDOWID, "");
        	addCmd.append(" windowId='"+windowId+"'");
        	String canvasId = MRXMLUtils.getValueXpath(streamDataDoc, "//"+SessionServiceConst.CANVASID, "");
        	addCmd.append(" canvasId='"+canvasId+"'");
        	
        	String destPortElementID = MRXMLUtils.getValueXpath(streamDataDoc, "//destPortElementID", "");
        	String regex = "\\\\Q(.*)\\\\E\\.\\\\Q(.*)\\\\E";
    		Matcher matcher = Pattern.compile(regex).matcher(destPortElementID);
    		String sessionId = "";
    		String connId = "" ;
    		if(matcher.find()) {
    			sessionId = matcher.group(1);
    			connId = matcher.group(2);
    		}
        	addCmd.append(" sessionId='"+sessionId+"'");
			addCmd.append(" connId='"+connId+"'");
        }
        
        executeCreate(addCmd.toString());
        executeStart(startCmd.toString());

        String ssInstanceID = MRXMLUtils.getAttributeValue(inputElement,"ssInstanceID");
        this.log.addInfo(this.userJID + ": DeviceSourceStreamAgent:updateSSRelayInput ssInstanceID=" + ssInstanceID + " with URL=" + streamURL, MRLog.OK, MRLog.NOTIFY);
    }
    

    public void deleteSSInput(Element inputElement) throws Exception {
        if (!this.isInputInstanceStarted(inputElement)) {
            return;
        }
        StringBuffer deleteCmd = new StringBuffer();
        SSAPI ssAPI = this.getSSAPI(inputElement);
        ssAPI.deleteSSInputCmd(inputElement, this.getNetworkResource(), this.log, deleteCmd);
        executeDelete(deleteCmd.toString());

        String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
        if (MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)) {
            String ssInstanceID = MRXMLUtils.getAttributeValue(inputElement,"ssInstanceID");
            this.log.addInfo(this.userJID + ": DeviceSourceStreamAgent:delete Encoder ssInstanceID=" + ssInstanceID);
            this.teardownEncoderPort(inputElement);
        }
    }    

    public void stopSSOutputAt(Element instanceElement, Element streamElement, long offset)
        throws Exception {
        StringBuffer stopCmd = new StringBuffer();
	    Element inputElement = instanceElement.element("Input");
        SSAPI ssAPI = this.getSSAPI(inputElement);
        ssAPI.stopSSOutputAtCmd(instanceElement, streamElement, offset, stopCmd);
        executeStop(stopCmd.toString());
        log.addInfo("DeviceSourceStreamAgent:stopSSOutput:=" + this.userJID);
    }

    public void startSSInputAtSync(Hashtable<String, Element> instanceElementList)
    throws Exception {
	    StringBuffer startCmd = new StringBuffer();
	    SSAPI ssAPI = this.getSSAPI(null);
	    ssAPI.startSSInputAtSyncCmd(instanceElementList, startCmd);
	    executeStart(startCmd.toString());
    	Enumeration<String> e = instanceElementList.keys();
    	while(e.hasMoreElements()) {
    		String instanceNID = e.nextElement();
    		Element instanceElement = instanceElementList.get(instanceNID);
    	    Element inputEL = instanceElement.element("Input");
    	    MRXMLUtils.addAttribute(inputEL,"startTime", String.valueOf(this.xf.xmpp(userJID).getCurrentTimeMillis()));
    	}
	    log.addInfo("DeviceSourceStreamAgent:startSSInputAtSync:=" + this.userJID);
    }


    public void stopSSOutputAtSync(List<Element> streamElementList, long offset)
	    throws Exception {
    	// System.out.println("StreamingServerImpl:stopSSOutputAtSync streamElementList count=" + streamElementList.size());
	    StringBuffer stopCmd = new StringBuffer();
	    SSAPI ssAPI = this.getSSAPI(null);
	    ssAPI.stopSSOutputAtSyncCmd(streamElementList, offset, stopCmd);
	    executeStop(stopCmd.toString());
	    log.addInfo("DeviceSourceStreamAgent:stopSSOutputAtSync:=" + this.userJID);
	}

    public void startSSOutput(Element instanceElement, Element streamElement)
        throws Exception {
    	/*
        List<Element> mediaSourceList = instanceElement.elements("MediaSource");
        Element outputElement = null;
        String streamNID = MRXMLUtils.getAttributeValue(streamElement, "NID", "");

        if ((mediaSourceList != null) && (mediaSourceList.size() != 0)) {
            List<Element> streamList = null;
            String nid = null;

            for (Iterator<Element> objIT = mediaSourceList.iterator(); objIT.hasNext();) {
            	streamList = objIT.next().elements("Stream");
            	int i=0;
            	for(i=0;streamList!=null && i< streamList.size();i++) {
            		nid = MRXMLUtils.getAttributeValue(streamList.get(i), "NID", "");
            		if ((nid != null) && nid.equals(streamNID)) {
            			outputElement = streamList.get(i).element("Output");
            			break;
            		}
            	}
            	if(i<streamList.size()) {
            		break;
            	}
            }
        }

        if (outputElement == null) {
            return;
        }
        */
    	Element outputElement = streamElement.element("Output");
        StringBuffer startCmd = new StringBuffer();
        Element inputElement = instanceElement.element("Input");
        SSAPI ssAPI = this.getSSAPI(inputElement);
        ssAPI.startSSOutputCmd(instanceElement, streamElement, outputElement, startCmd);
        executeStart(startCmd.toString());

        String outputType = MRXMLUtils.getAttributeValue(outputElement,"outputType");
        if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || 
        		MediaRoomServiceConst.OUTPUT_PVR.equals(outputType) ) {
            if (isInputInstanceStarted(inputElement)) {
            	// Refresh works only for V2D
        		String streamType = MRXMLUtils.getAttributeValue(inputElement,"streamType");
        		if(MediaRoomServiceConst.V2D.equals(streamType)) // Refresh implemented only for V2D
        			refreshSSInput(inputElement, null); 
        		else 
        	        log.addInfo("StreamingServerImpl:startSSOutput:=" + this.userJID + " skipping refresh command for non V2D streams");
            }
        }

        log.addInfo("DeviceSourceStreamAgent:startSSOutput:=" + this.userJID);
    }
    
    protected String getMediaDocDir() throws Exception {
    	String classpath = System.getProperties().getProperty("java.class.path", null);
    	StringTokenizer st = new StringTokenizer(classpath, File.pathSeparator);
    	String jarfile = st.nextToken();
    	File file = new File(jarfile);
    	String fullFilename = file.getAbsolutePath();
    	int index = fullFilename.lastIndexOf(File.separator);
    	String dir = fullFilename.substring(0, index);
    	// System.out.println("StreamingServerImpl::getHTTPDocDir jarfile=" + jarfile + " dir=" + dir);
    	return dir;
    }
    
    protected String getMediaURL(String protocol, String destIP, String sdpFilename, String sdpFilepath) throws Exception {
    	if(MediaRoomServiceConst.PROTOCOL_HTTP.equals(protocol))
    		return  protocol + "://" + destIP + "/mediaroom/" + sdpFilename;
    	else 
    		return protocol + "://" + sdpFilepath;
    }

    private String setupMediaURL(String protocol, String filename, String sdp, String resolvedIP, MRLog log) throws Exception {
    	sdp = SDPUtils.fixsdp(sdp);
    	String sdpFilepath = getMediaDocDir() + File.separator + filename;
    	FileWriter fstream = new FileWriter(sdpFilepath);
    	BufferedWriter out = new BufferedWriter(fstream);
    	out.write(sdp);
    	out.close();
    	String mediaURL = getMediaURL(protocol, resolvedIP, filename, sdpFilepath);
    	this.log.addInfo("StreamingServerImpl::setupMediaURL=" + mediaURL + " filename=" + sdpFilepath,
    			MRLog.OK, MRLog.NOTIFY);
    	return mediaURL;
    }
    
    private void teardownMediaURL(String filename) throws Exception {
    	filename = getMediaDocDir() + File.separator + filename;
    	File file = new File(filename);
    	file.delete();
    	this.log.addInfo("StreamingServerImpl::teardownMediaURL filename=" + filename,
    			MRLog.OK, MRLog.NOTIFY);
    }


    public void createSSOutput(Element outputElement, Element inputElement, Document streamDataDoc)
        throws Exception {
        StringBuffer addCmd = new StringBuffer();
        StringBuffer url = new StringBuffer();    	
        
        // Get the network interface address
        String IPAddress = null;
        try {
            IPAddress = this.getIPAddress();
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        String outputType = MRXMLUtils.getAttributeValue(outputElement,"outputType");
        /*String destNID = null;
        if(streamDataDoc!=null) {
        	destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID", "");
        }	*/
        
        /* Done as part of the updateStreamProfile
        if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || MediaRoomServiceConst.OUTPUT_PVR.equals(outputType) ) {        	
        	String status = "<Status>" + MediaStoreServiceConst.BUSY + "</Status>";
        	MediaStoreServiceUtils.sendInternalRequestToMediaStoreAgent(MediaStoreServiceConst.UPDATE_MEDIA_STATUS_REQUEST, destNID, MRXMLUtils.stringToDocument(status),userJID, xmpp, log, db);
        }
        */

        
        SSAPI ssAPI = this.getSSAPI(inputElement);
        ssAPI.createSSOutputCmd(outputElement, inputElement, streamDataDoc,  
        		this.getNetworkResource(), IPAddress,  log, addCmd, url);
        if(MediaRoomServiceConst.OUTPUT_DECODER.equalsIgnoreCase(outputType)) {
        	this.setupDecoderPort(outputElement);
        }

        String ssInstanceNID = MRXMLUtils.getAttributeValue(outputElement,"ssInstanceID");
        
        // Check if we need to setup sdp file for RTP
        if(MediaRoomServiceConst.OUTPUT_RECORD.equalsIgnoreCase(outputType) && MediaRoomServiceConst.RTP.equals(ssAPI.getStreamType())) {
        	String sdpContent = null;
            String destIP = ssAPI.getResolvedDestIPAddress(streamDataDoc);
         	if(destIP == null || "".equalsIgnoreCase(destIP)) {
     			destIP = MRXMLUtils.getValueXpath(streamDataDoc, "//destIP", "");
     		}
        	
    		String sdptemplate = MRXMLUtils.getValueXpath(streamDataDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE, "");
    		sdpContent = SDPUtils.generateSDPFromSDPTemplate(sdptemplate, destIP, Integer.parseInt(MediaRoomServiceConst.DEFAULT_DEST_UDP_START_PORT));
    		if(sdpContent != null) {
    			String sdpStreamURL = this.setupMediaURL(MediaRoomServiceConst.PROTOCOL_FILE, ssInstanceNID + ".sdp", sdpContent, destIP, log);
       			if (sdpStreamURL != null && !sdpStreamURL.isEmpty()) {
       				addCmd.append("streamURL=\"" + sdpStreamURL + "\" ");
       			}
    		}
        }
        
        // Check if we need to setup an HTTP URL
        String streamURL = url.toString();
        String streamURLType = null;
        if(streamDataDoc!=null) {
        	streamURLType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamURLType", "");
        }	
        if (MediaRoomServiceConst.HTTP.equals(streamURLType)) {
        		streamURL = this.setupMediaURL(MediaRoomServiceConst.PROTOCOL_HTTP, ssInstanceNID + ".sdp", streamURL, ssAPI.getResolvedSourceIPAddress(streamDataDoc), log);
        }
        
        executeCreate(addCmd.toString());
        /*
        if (MediaRoomServiceConst.OUTPUT_STREAM.equals(outputType)) {
            MRXMLUtils.addAttribute(outputElement,"streamURL", streamURL);
        }
        */
        if("".equals(streamURL))
        	streamURL = "none";
        MRXMLUtils.addAttribute(outputElement,"streamURL", streamURL);
        
    }
    

    public void deleteSSOutput(Element outputElement, Document streamDataDoc)
        throws Exception {
        StringBuffer deleteCmd = new StringBuffer();
        String outputType = MRXMLUtils.getAttributeValue(outputElement,"outputType","");
    	String ssInstanceNID = MRXMLUtils.getAttributeValue(outputElement,"ssInstanceID");
    	SSAPI ssAPI = this.getSSAPI(outputElement);
        
    	// postpone the deletion of record port until we receive Progress stopped notification from streamng server.
    	if(!outputType.equals(MediaRoomServiceConst.OUTPUT_RECORD)) {
        	ssAPI.deleteSSOutputCmd(outputElement, streamDataDoc, this.getNetworkResource(), log, deleteCmd);
        	executeDelete(deleteCmd.toString()); // TBD confirm that it is okay to release Mcast resources before deleteCmd
        } else {        	
        	this.simulateRecordProgressStoppedNotfication(outputElement, ssInstanceNID);
        }
        
    	// Check if we need to teardown sdp file for RTP
        if(MediaRoomServiceConst.OUTPUT_RECORD.equalsIgnoreCase(outputType) && MediaRoomServiceConst.RTP.equals(ssAPI.getStreamType())) {
        	this.teardownMediaURL(ssInstanceNID + ".sdp");
        }
        String streamURLType = null;
        if(streamDataDoc!=null) {
        	streamURLType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamURLType", "");
        }	
        if (MediaRoomServiceConst.HTTP.equals(streamURLType)) {
        		this.teardownMediaURL(ssInstanceNID + ".sdp");
        }
        if(MediaRoomServiceConst.OUTPUT_DECODER.equalsIgnoreCase(outputType)) {
            this.teardownDecoderPort(outputElement);
        }
    }
    
    protected void sendProgressStoppedSSEventToSourceStreamAgent(Element outputElement,String ssInstanceNID) throws MRException,Exception {
    	Document ssEventDoc = MRXMLUtils.loadXMLResource(DMS_RECORD_PORT_PROGRESS_STOP_SSEVENT_XML, log);
		MRXMLUtils.setValueXpath(ssEventDoc, "//Id", ssInstanceNID);
		MRXMLUtils.setValueXpath(ssEventDoc, "//MediaDirectory", MRXMLUtils.getAttributeValue(outputElement, "dirPath",""));
		MRXMLUtils.setValueXpath(ssEventDoc, "//MediaName", MRXMLUtils.getAttributeValue(outputElement, "destFileUUID",""));
		MRXMLUtils.setValueXpath(ssEventDoc, "//BytesWritten", MRXMLUtils.getAttributeValue(outputElement, "size",""));
		MRXMLUtils.setValueXpath(ssEventDoc, "//PlayLength", MRXMLUtils.getAttributeValue(outputElement, "playLength",""));
	    String info = "StreamingServiceImpl :=" + userJID + " > DMSRecorderPort Progress Stopped SSEvent to " + ServiceAgentUtils.getAgentShortName(SourceStreamAgent.class.getName());
        ServiceAgentUtils.sendAgentEvent(xf.xmpp(userJID), log, null, "", userJID, MRConst.MEDIA_SERVICE, MRLog.OK, userJID, xf.xmpp(userJID).getCurrentTimeMillis(), MediaRoomServiceConst.SSEVENT, ssEventDoc.getRootElement(), "", info);
    }
    

    public void updateSSPlaybackInput(Element instanceElement, Element streamElement, long offset)
    throws Exception {
    	System.out.println("TBD: DeviceSourceStreamAgent:updateSSPlaybackInput:=" + this.userJID);
    }
    
    public long parseStatusResultForValue(String result, String name) throws Exception {
        String valueStr = null;
        long value = 0;

        StringTokenizer resultST = new StringTokenizer(result, " ");
        String token = null;

        while (resultST.hasMoreTokens()) {
            token = resultST.nextToken();

            if (token.contains(name + ":")) {
                int index = token.indexOf(":");
                valueStr = token.substring(index + 1, token.length());

                if ((valueStr != null) && (valueStr.trim().length() != 0)) {
                    value = Long.parseLong(valueStr);
                }

                break;
            }
        }

        return value;    	
    }

    public long getCurrentTC(Element outputElement) throws MRException, Exception {
    	
        StringBuffer statusCmd = new StringBuffer();

        SSAPI ssAPI = this.getSSAPI(outputElement);
        ssAPI.getCurrentTCCmd(outputElement, statusCmd);

        String result = executeStatus(statusCmd.toString());
        return this.parseStatusResultForValue(result, "t");
    }

    public long getOffsetAtTC(long timecode,String mediaUUID,String srcDirPath) throws MRException, Exception {
    	
        StringBuffer statusCmd = new StringBuffer();

        SSAPI ssAPI = this.getSSAPI(null);
        ssAPI.getOffsetAtTCCmd(mediaUUID, timecode, srcDirPath, statusCmd);
        String result = executeStatus(statusCmd.toString());
    	// System.out.println("StreamingServerImpl:getOffsetAtTC cmd=" + statusCmd.toString() + " result=" + result);
        return this.parseStatusResultForValue(result, "t");
    }

    public void getInputInstanceStatus(Element inputElement) throws Exception {
        StringBuffer statusCmd = new StringBuffer();
        SSAPI ssAPI = this.getSSAPI(inputElement);
        ssAPI.getInputInstanceStatusCmd(inputElement, statusCmd);
        executeStatus(statusCmd.toString());
    }

    protected String executeStatus(String statusCmd) throws Exception {
    	return "t:" + String.valueOf(System.currentTimeMillis());
    }

    public boolean isInputInstanceStarted(Element inputElement) throws Exception {
        try {
            getInputInstanceStatus(inputElement);
        } catch (MRException exp) {
            //Exception means instance not existed then return false
            return false;
        } catch (Exception exp) {
            //Exception means instance not existed then return false
            return false;
        }

        return true;
    }

    public void initStreamingServer() throws MRException, Exception {   
    	this.setupInternalInstances(null);
    }   
   
    //This port will start the streaming server process to clean the deleted media
    public void addDeleterPort(String deleterPortInstanceID,String trashDirPath) throws MRException,Exception {
    	StringBuffer addDeleterPortCmd = new StringBuffer();
    	addDeleterPortCmd.append("action=add ");
    	addDeleterPortCmd.append("id="+deleterPortInstanceID +" ");
    	addDeleterPortCmd.append("dsttype="+MediaRoomServiceConst.DELETER_PORT + " ");
    	addDeleterPortCmd.append("deletedir=\"" + trashDirPath+ "\" ");    	
        log.addInfo("Add Deleter Port :="+deleterPortInstanceID,MRLog.INFO,MRLog.NOTIFY);
    
        String commandResult = this.executeSSCommand(addDeleterPortCmd.toString());
        if ((commandResult == null) || commandResult.startsWith("201 OK")) {
            throw new MRException(MediaRoomServiceExceptionCodes.EXECUTESTREAMINGSERVERCMDFAILED, "EXECUTE STREAMING SERVER COMMAND FAILED: RESULT = " + commandResult);
        }       
    }
    
    
    //This port will Delete deleter port from streaming server
    public void deleteDeleterPort(String deleterPortInstanceID) throws MRException,Exception {
    	StringBuffer addDeleterPortCmd = new StringBuffer();
    	addDeleterPortCmd.append("action=delete ");
    	addDeleterPortCmd.append("id="+deleterPortInstanceID +" ");
    	addDeleterPortCmd.append("dsttype="+MediaRoomServiceConst.DELETER_PORT + " ");    	    	
        log.addInfo("Delete Deleter Port:="+deleterPortInstanceID,MRLog.INFO,MRLog.NOTIFY);
    
        String commandResult = this.executeSSCommand(addDeleterPortCmd.toString());
        if ((commandResult == null) || commandResult.startsWith("201 OK")) {
            throw new MRException(MediaRoomServiceExceptionCodes.EXECUTESTREAMINGSERVERCMDFAILED, "EXECUTE STREAMING SERVER COMMAND FAILED: RESULT = " + commandResult);
        }       
    }
    
    public void deleteRecordPort(String recordPortId) throws MRException,Exception {
    	StringBuffer deleteRecordPortCmd = new StringBuffer();
    	deleteRecordPortCmd.append("action=delete ");
    	deleteRecordPortCmd.append("id="+recordPortId +" ");
    	deleteRecordPortCmd.append("operation=output ");
    	deleteRecordPortCmd.append("dsttype="+ MediaRoomServiceConst.DMS_RECORDER_PORT + " ");    	    	
        log.addInfo("Delete DMSRecordPort:="+recordPortId,MRLog.INFO,MRLog.NOTIFY);
        this.executeDelete(deleteRecordPortCmd.toString());
    }

    @Override
    public void processStreamingServerNotification(String notifyMessage) throws MRException, Exception {
    }
    
   

    public long getTCAtOffset(String mediaUUID, long offset,String rootDir) throws Exception {
    	
    	StringBuffer statusCmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(null);
    	ssAPI.getTCAtOffsetCmd(mediaUUID,rootDir, offset, statusCmd);
        String statusResult = this.executeSSCommand(statusCmd.toString());
    	// System.out.println("StreamingServerImpl:getTCAtOffset cmd=" + statusCmd.toString() + " result=" + statusResult);
        return this.parseStatusResultForValue(statusResult, "t");

    }
}
