package com.barco.device.vng.Media.streamingserver;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.device.vng.DeviceAdmin.DeviceAdminEngineObserverI;
import com.barco.device.vng.Media.StreamingServerI;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.device.vng.utils.SDPUtils;
import com.barco.device.vng.utils.VNGConst;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;


public abstract class StreamingServerImpl implements StreamingServerI {
	    
	protected Logger log = null;
	protected List<DeviceAdminEngineObserverI> deviceServerObserverList = null;
	protected DeviceDBI  deviceDB= null;

	private NetworkResource networkResource;
    protected static final String DMS_RECORD_PORT_PROGRESS_STOP_SSEVENT_XML = "/com/ipvs/mediaroomservice/streamingserver/DMSRecordPortProgressStoppedSSEvent.xml";

    public abstract String getIPAddress() throws Exception;
    protected abstract String setupEncoderPort(Element inputElement, Document streamDataDoc);
    protected abstract void setupDecoderPort(Element outputElement);
    protected abstract void teardownDecoderPort(Element outputElement);
    protected abstract void teardownEncoderPort(Element inputElement);
    protected abstract String executeStatus(String statusCmd) throws Exception;

    //private static boolean isInitialized = false;


   /* public static void setInitialized(boolean isInitialized) {
		StreamingServerImpl.isInitialized = isInitialized;
	}*/
    
	public StreamingServerImpl (Logger log, DeviceDBI deviceDB){
		this.log =log;	
		this.deviceDB = deviceDB;
        this.networkResource = new NetworkResource(log);
       /* try {        	
			this.init();
		} catch (ServiceException e) {			
			e.printStackTrace();
		} catch (Exception e) {			
			e.printStackTrace();
		}*/
    }

    protected String executeSSCommand(String cmd) throws Exception {
    	 return CommandLineUtils.executeStaticSSCommand(cmd, deviceDB, log);
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
        throw new ServiceException(MediaRoomServiceExceptionCodes.STREAMTYPENOTFOUND, 
        		"<streamType>" + streamType + "</streamType>");
    }

	public SSAPI getSSAPI(Element element) throws Exception  {
		if(element == null)
			return new V2DSSAPI();
		String streamType = XMLUtils.getAttributeValue(element,"streamType");
		SSAPI ssAPI = getSSAPIFromStreamType(streamType);
        return ssAPI;
	}

    
    private void executeStop(String stopCmd) throws Exception {
        //perf-logs this.log.info("DeviceSourceStreamAgent::executeStop - start" , MRLog.OK, MRLog.NOTIFY);
        log.info("StreamingServer stop command is ->" + stopCmd);

        String stopResult = executeSSCommand(stopCmd);

        if ((stopResult == null) || stopResult.startsWith("201 OK")) {
            log.info("StreamingServer error cmd=" + stopCmd);
            throw new ServiceException(MediaRoomServiceExceptionCodes.STOPSSINSTANCEIDFAILED, "STOP STREAMING SERVER INSTANCE  FAILED: RESULT = " + stopResult + " cmd=" + stopCmd);
        }

        //perf-logs this.log.info("DeviceSourceStreamAgent::executeStop - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeStart(String startCmd) throws Exception {
        //perf-logs this.log.info("DeviceSourceStreamAgent::executeStart - start" , MRLog.OK, MRLog.NOTIFY);
        log.info("StreamingServer start command is ->" + startCmd);

        String startResult = executeSSCommand(startCmd);

        if ((startResult == null) || startResult.startsWith("201 OK")) {
            log.info("StreamingServer error cmd=" + startCmd +" startResult:="+startResult);
            throw new ServiceException(MediaRoomServiceExceptionCodes.STARTSSINSTANCEIDFAILED, "START STREAMING SERVER INSTANCE  FAILED : RESULT = " + startResult + " cmd=" + startCmd);
        }

        //perf-logs this.log.info("DeviceSourceStreamAgent::executeStart - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeUpdate(String updateCmd) throws Exception {
        //perf-logs this.log.info("DeviceSourceStreamAgent::executeUpdate - start" , MRLog.OK, MRLog.NOTIFY);
        log.info("StreamingServer updateCmd command is ->" + updateCmd);

        String updateResult = executeSSCommand(updateCmd);

        if ((updateResult == null) || updateResult.startsWith("201 OK")) {
            log.info("StreamingServer error cmd=" + updateCmd +" updateResult:="+updateResult);
            throw new ServiceException(MediaRoomServiceExceptionCodes.UPDATESSINSTANCEIDFAILED, "UPDATE STREAMING SERVER INSTANCE  FAILED: RESULT = " + updateResult + " cmd=" + updateCmd);
        }

        //perf-logs this.log.info("DeviceSourceStreamAgent::executeUpdate - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeCreate(String addCmd) throws Exception {
        //perf-logs this.log.info("DeviceSourceStreamAgent::executeCreate - start" , MRLog.OK, MRLog.NOTIFY);
        log.info("StreamingServer add command is ->" + addCmd);

        String createResult = executeSSCommand(addCmd);

        if ((createResult == null) || createResult.startsWith("201 OK")) {
            log.info("StreamingServer error cmd=" + addCmd +" createResult:="+createResult);
            throw new ServiceException(MediaRoomServiceExceptionCodes.ADDSSINSTANCEIDFAILED, "ADD STREAMING SERVER INSTANCE  FAILED : RESULT = " + createResult + " cmd=" + addCmd);
        }

        //perf-logs this.log.info("DeviceSourceStreamAgent::executeCreate - end" , MRLog.OK, MRLog.NOTIFY);
    }

    private void executeDelete(String delCmd) throws Exception {
        //perf-logs this.log.info("DeviceSourceStreamAgent::executeDelete - start" , MRLog.OK, MRLog.NOTIFY);
        log.info("StreamingServer delete command is ->" + delCmd);

        String deleteResult = executeSSCommand(delCmd);

        if ((deleteResult == null) || deleteResult.startsWith("201 OK")) {
            log.info("StreamingServer error cmd=" + delCmd +" deleteResult:="+deleteResult);
            throw new ServiceException(MediaRoomServiceExceptionCodes.DELETESSINSTANCEIDFAILED, "DELETE STREAMING SERVER INSTANCE  FAILED : RESULT = " + deleteResult + " cmd=" + delCmd);
        }

        //perf-logs this.log.info("DeviceSourceStreamAgent::executeDelete - end" , MRLog.OK, MRLog.NOTIFY);
    }

    public void stopSSInput(Element inputElement) throws Exception {
        StringBuffer cmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(inputElement);
    	ssAPI.stopSSInputCmd(inputElement, cmd);
        executeStop(cmd.toString());
        //rest start time
        XMLUtils.addAttribute(inputElement,"startTime", String.valueOf(0));
        // log.info("DeviceSourceStreamAgent:stopSSInput:=" + this.deviceDB.getDeviceResourceNID());
    }

    public void startSSInput(Element instanceElement, Element streamElement)
    throws Exception {
        StringBuffer cmd = new StringBuffer();
	    Element inputEL = instanceElement.element("Input");
	    String startTime = XMLUtils.getAttributeValue(inputEL,"startTime");
	    if(!"0".equals(startTime))
	    	return;
    	SSAPI ssAPI = this.getSSAPI(inputEL);
    	ssAPI.startSSInputCmd(instanceElement, streamElement, cmd);
	    String inputType = XMLUtils.getAttributeValue(inputEL,"inputType");
	 
        if (!MediaRoomServiceConst.INPUT_RELAY.equals(inputType) && !MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
	        executeStart(cmd.toString());
	    }
        XMLUtils.addAttribute(inputEL,"startTime", String.valueOf(System.currentTimeMillis()));
	    // log.info("DeviceSourceStreamAgent:startSSInput:=" + this.deviceDB.getDeviceResourceNID());
    }

    
    public void refreshSSInput(Element inputElement, Document streamDataDoc)
    throws Exception {
    	StringBuffer cmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(inputElement);
    	ssAPI.refreshSSInputCmd(inputElement, streamDataDoc, cmd);
        executeUpdate(cmd.toString());
    }
    
    public void updateSSOutput(Element outputElement, Document streamDataDoc)
    	    throws Exception {
    	    	StringBuffer cmd = new StringBuffer();
    	    	SSAPI ssAPI = this.getSSAPI(outputElement);
    	    	ssAPI.updateSSOutputCmd(outputElement, streamDataDoc, cmd);
    	    	executeUpdate(cmd.toString());
    }

    public void createSSInput(Element inputElement, Document streamDataDoc, Document mediaSourceDataDoc)
        throws Exception {
        StringBuffer cmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(inputElement);
        String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
        String ioport = "";
        if (MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)) {
            //perf-logs this.log.info(this.deviceDB.getDeviceResourceNID() + ": DeviceSourceStreamAgent:add Encoder ssInstanceID=" + ssInstanceID);
        	ioport = this.setupEncoderPort(inputElement, streamDataDoc);
        }
    	ssAPI.createSSInputCmd(inputElement, streamDataDoc, mediaSourceDataDoc, cmd, ioport);
        if (!MediaRoomServiceConst.INPUT_RELAY.equals(inputType) && !MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            executeCreate(cmd.toString());
        }
    }
    
	@Override
	public void updateSSInput(Element inputElement, Document streamDataDoc) throws Exception {
		//we have to update 
		String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
        if (MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)) {
            this.setupEncoderPort(inputElement, streamDataDoc);
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
	
	    //perf-logs this.log.info("Checking inputInstanceStarted" + MRXMLUtils.elementToString(inputElement), MRLog.OK, MRLog.NOTIFY);
	    isInstanceAdded = this.isInputInstanceStarted(inputElement);
	
	    if (isInstanceAdded) {
	        deleteSSInput(inputElement);
	    }

        String streamURL = XMLUtils.getAttributeValue(inputElement,"streamURL");

        // Update the addCmd to include the streamURL
        if(this.isExternal()) {
        	addCmd.append(" streamURL='" + streamURL + "'");
        	String windowId = XMLUtils.getValueXpath(streamDataDoc, "//"+VNGConst.WINDOWID, "");
        	addCmd.append(" windowId='"+windowId+"'");
        	String canvasId = XMLUtils.getValueXpath(streamDataDoc, "//"+VNGConst.CANVASID, "");
        	addCmd.append(" canvasId='"+canvasId+"'");
        	
        	String destPortElementID = XMLUtils.getValueXpath(streamDataDoc, "//destPortElementID", "");
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

        String ssInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
        this.log.info(this.deviceDB.getDeviceResourceNID() + ": DeviceSourceStreamAgent:updateSSRelayInput ssInstanceID=" + ssInstanceID + " with URL=" + streamURL);
    }
    

    public void deleteSSInput(Element inputElement) throws Exception {
        StringBuffer deleteCmd = new StringBuffer();
        SSAPI ssAPI = this.getSSAPI(inputElement);
        ssAPI.deleteSSInputCmd(inputElement, this.getNetworkResource(), this.log, deleteCmd);
        executeDelete(deleteCmd.toString());

        String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
        if (MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)) {
            String ssInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
            this.log.info(this.deviceDB.getDeviceResourceNID() + ": DeviceSourceStreamAgent:delete Encoder ssInstanceID=" + ssInstanceID);
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
        log.info("DeviceSourceStreamAgent:stopSSOutput:=" + this.deviceDB.getDeviceResourceNID());
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
    	    XMLUtils.addAttribute(inputEL,"startTime", String.valueOf(System.currentTimeMillis()));
    	}
	    log.info("DeviceSourceStreamAgent:startSSInputAtSync:=" + this.deviceDB.getDeviceResourceNID());
    }


    public void stopSSOutputAtSync(List<Element> streamElementList, long offset)
	    throws Exception {
    	// System.out.println("StreamingServerImpl:stopSSOutputAtSync streamElementList count=" + streamElementList.size());
	    StringBuffer stopCmd = new StringBuffer();
	    SSAPI ssAPI = this.getSSAPI(null);
	    ssAPI.stopSSOutputAtSyncCmd(streamElementList, offset, stopCmd);
	    executeStop(stopCmd.toString());
	    log.info("DeviceSourceStreamAgent:stopSSOutputAtSync:=" + this.deviceDB.getDeviceResourceNID());
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

        String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
        if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || 
        		MediaRoomServiceConst.OUTPUT_PVR.equals(outputType) ) {
            if (isInputInstanceStarted(inputElement)) {
            	// Refresh works only for V2D
        		String streamType = XMLUtils.getAttributeValue(inputElement,"streamType");
        		if(MediaRoomServiceConst.V2D.equals(streamType)) // Refresh implemented only for V2D
        			refreshSSInput(inputElement, null); 
        		else 
        	        log.info("StreamingServerImpl:startSSOutput:=" + this.deviceDB.getDeviceResourceNID() + " skipping refresh command for non V2D streams");
            }
        }

        // log.info("DeviceSourceStreamAgent:startSSOutput:=" + this.deviceDB.getDeviceResourceNID());
    }
    
    // We keep the sdp file in the mediaroom directory and not in the devicevng directory
    // This is to prevent accidental access to the jar files in the devicevng directory
    protected String getMediaDocDir() throws Exception {
    	String classpath = System.getProperties().getProperty("java.class.path", null);
    	StringTokenizer st = new StringTokenizer(classpath, File.pathSeparator);
    	String jarfile = st.nextToken();
    	File file = new File(jarfile);
    	String fullFilename = file.getAbsolutePath();
    	int index = fullFilename.lastIndexOf(File.separator);
    	String dir = fullFilename.substring(0, index);
    	dir = dir.replace("devicevng", "mediaroom");
    	// System.out.println("StreamingServerImpl::getHTTPDocDir jarfile=" + jarfile + " dir=" + dir);
    	return dir;
    }
    
    protected String getMediaURL(String protocol, String destIP, String sdpFilename, String sdpFilepath) throws Exception {
    	if(MediaRoomServiceConst.PROTOCOL_HTTP.equals(protocol))
    		return  protocol + "://" + destIP + "/mediaroom/" + sdpFilename;
    	else 
    		return protocol + "://" + sdpFilepath;
    }

    private String setupMediaURL(String protocol, String filename, String sdp, String resolvedIP, Logger log) throws Exception {
    	sdp = SDPUtils.fixsdp(sdp);
    	String sdpFilepath = getMediaDocDir() + File.separator + filename;
    	FileWriter fstream = new FileWriter(sdpFilepath);
    	BufferedWriter out = new BufferedWriter(fstream);
    	out.write(sdp);
    	out.close();
    	String mediaURL = getMediaURL(protocol, resolvedIP, filename, sdpFilepath);
    	this.log.info("StreamingServerImpl::setupMediaURL=" + mediaURL + " filename=" + sdpFilepath);
    	return mediaURL;
    }
    
    
    private void setupHLSMediaLink(Document mediaSourceDataDoc,String fileName) throws Exception {
    	
    	String lookbackFileUUID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileUUID", "");
    	if(lookbackFileUUID.isEmpty()) {
    		return;
    	}
    	
    	String dirPath = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileParentDirPath", "");        
    	
    	String[] linkCommand = {"ln", "-s" , dirPath+"/blob/"+lookbackFileUUID+"/hls", getMediaDocDir()+"/"+fileName };
    	CommandLineUtils.executeSystemCommand(linkCommand);
    	log.info(" Created HLS directory link for fileUUID:="+lookbackFileUUID +" to "+getMediaDocDir()+"/"+fileName );
    	   	
    }
    
    private void teardownMediaURL(String filename) throws Exception {
    	filename = getMediaDocDir() + File.separator + filename;
    	File file = new File(filename);
    	file.delete();
    	this.log.info("StreamingServerImpl::teardownMediaURL filename=" + filename);
    }
    
    
   private void teardownHLSMediaLink(String fileName) throws Exception {
       	String[] linkCommand = {"rm", getMediaDocDir()+"/"+fileName };
    	CommandLineUtils.executeSystemCommand(linkCommand);
    	log.info("StreamingServer: Delete HLS directory link "+getMediaDocDir()+"/"+fileName );
    	   	
    }


    public void createSSOutput(Element outputElement, Element inputElement, Document streamDataDoc, Document mediaSourceDataDoc)
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
        
        String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
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

        String ssInstanceNID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
        boolean enableHLS = Utils.getBooleanValue(XMLUtils.getValueXpath(streamDataDoc, "//HLSConfig/Enable", "false"), false);
        
        // Check if we need to setup sdp file for RTP
        if((MediaRoomServiceConst.OUTPUT_PVR.equalsIgnoreCase(outputType) || MediaRoomServiceConst.OUTPUT_RECORD.equalsIgnoreCase(outputType)) && MediaRoomServiceConst.RTP.equals(ssAPI.getStreamType())) {
        	String sdpContent = null;
            String destIP = ssAPI.getResolvedDestIPAddress(streamDataDoc);
         	if(destIP == null || "".equalsIgnoreCase(destIP)) {
     			destIP = XMLUtils.getValueXpath(streamDataDoc, "//destIP", "");
     		}
        	
    		String sdptemplate = XMLUtils.getValueXpath(streamDataDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE, "");
    		sdpContent = SDPUtils.generateSDPFromSDPTemplate(sdptemplate, destIP, Integer.parseInt(MediaRoomServiceConst.DEFAULT_DEST_UDP_START_PORT));
    		if(sdpContent != null) {
    			String sdpStreamURL = this.setupMediaURL(MediaRoomServiceConst.PROTOCOL_FILE, ssInstanceNID + ".sdp", sdpContent, destIP, log);
       			if (sdpStreamURL != null && !sdpStreamURL.isEmpty()) {
       				addCmd.append("streamURL=\"" + sdpStreamURL + "\" ");
       			}
    		}
    		
    		if(enableHLS) {
    			addCmd.append(" enableHLS=1 ");
    		}	
    		else {
	        	addCmd.append(" enableHLS=0 ");
	        }
        }
        
        // Check if we need to setup an HTTP URL
        String streamURL = url.toString();
        String streamURLType = null;
        if(streamDataDoc!=null) {
        	streamURLType = XMLUtils.getValueXpath(streamDataDoc, "//streamURLType", "");
        }	
        if (MediaRoomServiceConst.HTTP.equals(streamURLType)) {
        		streamURL = this.setupMediaURL(MediaRoomServiceConst.PROTOCOL_HTTP, ssInstanceNID + ".sdp", streamURL, ssAPI.getResolvedSourceIPAddress(streamDataDoc), log);
        		if(enableHLS)
        			setupHLSMediaLink(mediaSourceDataDoc, ssInstanceNID);
  				 
        }        
        executeCreate(addCmd.toString());
        /*
        if (MediaRoomServiceConst.OUTPUT_STREAM.equals(outputType)) {
            MRXMLUtils.addAttribute(outputElement,"streamURL", streamURL);
        }
        */
        if("".equals(streamURL))
        	streamURL = "none";
        XMLUtils.addAttribute(outputElement,"streamURL", streamURL);
        
    }
    

    public void deleteSSOutput(Element outputElement, Document streamDataDoc)
        throws Exception {
        StringBuffer deleteCmd = new StringBuffer();
        String outputType = XMLUtils.getAttributeValueWithDefault(outputElement,"outputType","");
    	String ssInstanceNID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
    	SSAPI ssAPI = this.getSSAPI(outputElement);
        
       	ssAPI.deleteSSOutputCmd(outputElement, streamDataDoc, this.getNetworkResource(), log, deleteCmd);
       	executeDelete(deleteCmd.toString()); // TBD confirm that it is okay to release Mcast resources before deleteCmd
        
    	// Check if we need to teardown sdp file for RTP
        if((MediaRoomServiceConst.OUTPUT_PVR.equalsIgnoreCase(outputType) || MediaRoomServiceConst.OUTPUT_RECORD.equalsIgnoreCase(outputType)) && MediaRoomServiceConst.RTP.equals(ssAPI.getStreamType())) {
        	this.teardownMediaURL(ssInstanceNID + ".sdp");
        }
        String streamURLType = null;
        if(streamDataDoc!=null) {
        	streamURLType = XMLUtils.getValueXpath(streamDataDoc, "//streamURLType", "");
        }	
        if (MediaRoomServiceConst.HTTP.equals(streamURLType)) {
        		this.teardownMediaURL(ssInstanceNID + ".sdp");
        		boolean enableHLS = Utils.getBooleanValue(XMLUtils.getValueXpath(streamDataDoc, "//HLSConfig/Enable", "false"), false);
        		if(enableHLS)
        			this.teardownHLSMediaLink(ssInstanceNID);                
        }
        if(MediaRoomServiceConst.OUTPUT_DECODER.equalsIgnoreCase(outputType)) {
            this.teardownDecoderPort(outputElement);
        }
    }  
   

    public void updateSSPlaybackInput(Element instanceElement, Element streamElement, long offset)
    throws Exception {
    	System.out.println("TBD: DeviceSourceStreamAgent:updateSSPlaybackInput:=" + this.deviceDB.getDeviceResourceNID());
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

    public long getCurrentTC(Element outputElement) throws ServiceException, Exception {
    	
        StringBuffer statusCmd = new StringBuffer();

        SSAPI ssAPI = this.getSSAPI(outputElement);
        ssAPI.getCurrentTCCmd(outputElement, statusCmd);

        String result = executeStatus(statusCmd.toString());
        return this.parseStatusResultForValue(result, "t");
    }

    public long getOffsetAtTC(long timecode,String mediaUUID,String srcDirPath) throws ServiceException, Exception {
    	
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

    public boolean isInputInstanceStarted(Element inputElement) throws Exception {
        try {
            getInputInstanceStatus(inputElement);
        } catch (ServiceException exp) {
            //Exception means instance not existed then return false
            return false;
        } catch (Exception exp) {
            //Exception means instance not existed then return false
            return false;
        }

        return true;
    }

    public void init() throws ServiceException, Exception { 
    	log.info("Reset StreamingServer");
    	StringBuffer resetCmd = new StringBuffer();
    	resetCmd.append("action=reset");
        try {
            String commandResult = executeSSCommand(resetCmd.toString());
            if ((commandResult == null) || commandResult.startsWith("201 OK")) {
                log.info("StreamingServer error cmd=" + resetCmd);
                throw new ServiceException(MediaRoomServiceExceptionCodes.EXECUTESTREAMINGSERVERCMDFAILED, "EXECUTE STREAMING SERVER COMMAND FAILED: RESULT = " + commandResult + " cmd=" + resetCmd);
            }
        } catch (Exception exp) {
            exp.printStackTrace();
        }
    	this.setupInternalInstances(null);
    }    


    @Override
    public void processStreamingServerNotification(String notifyMessage) throws ServiceException, Exception {
    }
    
   

    public long getTCAtOffset(String mediaUUID, long offset,String rootDir) throws Exception {
    	
    	StringBuffer statusCmd = new StringBuffer();
    	SSAPI ssAPI = this.getSSAPI(null);
    	ssAPI.getTCAtOffsetCmd(mediaUUID,rootDir, offset, statusCmd);
        String statusResult = executeSSCommand(statusCmd.toString());
    	// System.out.println("StreamingServerImpl:getTCAtOffset cmd=" + statusCmd.toString() + " result=" + statusResult);
        return this.parseStatusResultForValue(statusResult, "t");

    }
    
    public static final void main(String[] args) {
    	String dir = "/home/onair/modules/devicevng/public";
    	dir = dir.replace("devicevng", "mediaroom");
    	System.out.println("Dir=" + dir);
    }
}
