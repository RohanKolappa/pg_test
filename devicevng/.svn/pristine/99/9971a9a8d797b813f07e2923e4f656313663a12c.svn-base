package com.barco.device.vng.Media.streamingserver;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;
import org.dom4j.Document;

import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class V2DSSAPI extends  SSAPI {
	
    public static final String V2D_XML =  "/com/barco/device/vng/Media/V2D.xml";
    public static final String DefaultBandwidth = String.valueOf(10 * 1024);

	protected String getStreamType() {
		return MediaRoomServiceConst.V2D;
	}
	
	public static Document parseV2DURL(String V2DURL, Logger log) throws Exception {
		
        Document doc = XMLUtils.loadXMLResource(V2DSSAPI.V2D_XML);

		String regexp = "v2d://(\\S+)/port=(\\d+)";
		Matcher m = Pattern.compile(regexp).matcher(V2DURL);
		m.find();
		
		XMLUtils.getElementXpath(doc, "//callbackIP").setText(m.group(1));
		XMLUtils.getElementXpath(doc, "//callbackPort").setText(m.group(2));
		
		return doc;
	}

	protected String getInputPortType(String inputType) throws ServiceException {
        if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            return MediaRoomServiceConst.DMS_MEDIA_PORT; // TBD Was also returning V2DMediaPort
        } else if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
            return MediaRoomServiceConst.V2D_NETWORK_PORT;
        } else if (MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            return MediaRoomServiceConst.V2D_NETWORK_PORT;
        } else if (MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)) {
            return MediaRoomServiceConst.V2D_ENCODER_PORT;
        } else {
            throw new ServiceException(MediaRoomServiceExceptionCodes.INPUTTYPENOTFOUND, XMLUtils.generateErrorXML("inputType", inputType));
        }		
	}


    protected String getNetworkInputString(String ssInstanceID, Document streamDataDoc,
    		NetworkResource networkResource, String streamURL, Logger log) throws Exception {
    	
    	StringBuffer addCmd = new StringBuffer();
        String ismulticast = null;
        String avoption = null;

        //get from the port pool
        String videoPort = networkResource.getAvailableUDPPort(ssInstanceID);
        String audioPort = networkResource.getAvailableUDPPort(ssInstanceID);
        String bandwidth = null;
        long lbandwidth = 10 * 1024 * 1024;

        try {
            ismulticast = XMLUtils.getValueXpath(streamDataDoc, "//IsMulticast", "false");
            avoption = XMLUtils.getValueXpath(streamDataDoc, "//AVOption", "Video");
            bandwidth = XMLUtils.getValueXpath(streamDataDoc, "//Bandwidth", V2DSSAPI.DefaultBandwidth);
            
  
            if ((bandwidth != null) && (bandwidth.trim().length() != 0)) {
                lbandwidth = Long.parseLong(bandwidth) * 1024;
            }
            
        } catch (Exception e1) {
            e1.printStackTrace();
        }

        String ipaddress = "";
        String port = "";

        if ((streamURL != null) && (streamURL.trim().length() != 0)) {
        	try {
        		
        		
        		String regexp = "bandwidth\\s*=\\s*[0-9]+";
        		Matcher m = Pattern.compile(regexp).matcher(streamURL);
        		if(m.find()) {
        			lbandwidth = Long.getLong(m.group(0).replace("bandwidth=", ""),lbandwidth);
        		} 
        		
        		regexp = "avoption=((?i)[A-Z]+)";
        		m = Pattern.compile(regexp).matcher(streamURL);
        		if(m.find()) {
        			avoption =m.group(0).replace("avoption=", "");
        		}
        		
        		
        		regexp = "ismulticast=((?i)[A-Z]+)";
        		m = Pattern.compile(regexp).matcher(streamURL);
        		if(m.find()) {
        			ismulticast =m.group(0).replace("ismulticast=", "");        		
        		}
        		
				Document doc = V2DSSAPI.parseV2DURL(streamURL, log);
				//callbackIP from the URL would eventually be the publishIP
		        //Currently it is the resolvedIP
		        //When it changes to publishIP this code will have to get the resolvedIP using the NATTable
				ipaddress = XMLUtils.getValueXpath(doc, "//callbackIP", "");
				port = XMLUtils.getValueXpath(doc, "//callbackPort", "");
        	}
        	catch(Exception e) {
        		log.info("V2DSSAPI:getNetworkInputString bad streamURL=" + streamURL);
        		throw e;
        	}
        }

        //TBD:Get the values from profile xml or device xml?

        addCmd.append("srcip=" + ipaddress + " ");
        addCmd.append("srcport=" + port + " ");

     

        addCmd.append("srcbitrate=" + lbandwidth + " ");

        if (ismulticast.equals("true")) {
            addCmd.append("srcmulticast=1 ");
        } else {
            addCmd.append("srcmulticast=0 ");
        }

        if (avoption.equals("Video")) {
            addCmd.append("avoption=1 ");
        } else if (avoption.equals("Audio")) {
            addCmd.append("avoption=2 ");
        } else if (avoption.equals("Both")) {
            addCmd.append("avoption=3 ");
        } else {
            addCmd.append("avoption=1 ");
        }

        //TBD: Check the even and odd
        addCmd.append("videoport=" + videoPort + " ");
        addCmd.append("audioport=" + audioPort + " ");
        
        return addCmd.toString();
    }
    
	protected void releaseNetworkInputResources(NetworkResource networkResource, 
			String ssInstanceID, Logger log) throws Exception {
		networkResource.releaseUDPPort(ssInstanceID, log);
	}
    
	protected String getOutputPortType(String outputType) {
	    if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || 
		    MediaRoomServiceConst.OUTPUT_PVR.equals(outputType)) {
		return MediaRoomServiceConst.DMS_RECORDER_PORT;	
	    } else if(MediaRoomServiceConst.OUTPUT_DECODER.equalsIgnoreCase(outputType)) {
		return MediaRoomServiceConst.V2D_HARDWARE_DECODER_PORT;
	    }
	    else if(MediaRoomServiceConst.OUTPUT_IMAGE_WRITER.equalsIgnoreCase(outputType)) {
		return MediaRoomServiceConst.IMAGE_WRITER_PORT;
	    }
	    else {
		return MediaRoomServiceConst.V2D_DECODER_PORT;
	    }
	}

    protected String getNetworkOutputString(String ssInstanceID, Document streamDataDoc,
    		NetworkResource networkResource, String IPAddress, String inputType, String inputInstanceID,
    		Logger log, StringBuffer streamURLReturn) throws Exception {
    	
    	StringBuffer addCmd = new StringBuffer();
        String bandwidth = null;
        String tcpPort = "";
        String isMulticast = "false";
        String enablekbm = "false";
        String avoption = "Video";
        String mediatype = "v2d";
        String publishedDestIP = "";

        try {
        	if(streamDataDoc!=null) {
	            enablekbm = XMLUtils.getValueXpath(streamDataDoc, "//EnableKbm", "false");
	            bandwidth = XMLUtils.getValueXpath(streamDataDoc, "//Bandwidth",V2DSSAPI.DefaultBandwidth);
	            isMulticast = XMLUtils.getValueXpath(streamDataDoc, "//IsMulticast", "false");
	            avoption = XMLUtils.getValueXpath(streamDataDoc, "//AVOption", "Video");
	            tcpPort = XMLUtils.getValueXpath(streamDataDoc, "//callbackPort", "");
	            //we send the resolved IP address since the URL is being passed directly to the player
	            IPAddress = this.getResolvedSourceIPAddress(streamDataDoc); 
	            publishedDestIP = this.getPublishedDestIPAddress(streamDataDoc);
        	}
        } catch (Exception e1) {
            e1.printStackTrace();
        }
        long lbandwidth = 10 * 1024 * 1024;

        if ((bandwidth != null) && (bandwidth.trim().length() != 0)) {
            lbandwidth = Long.parseLong(bandwidth) * 1024;
        }

        if("".equals(tcpPort))
        	tcpPort = networkResource.getAvailableTCPPort(ssInstanceID);
        /*else { // Unmanaged Rx port, check to see if overlaps with configuredPool
        	int tcpPortNumber = Integer.parseInt(tcpPort);
        	if(networkResource.isReservedTCPPort(tcpPortNumber)) {
                throw new MRException(MediaRoomServiceExceptionCodes.OVERLAPPING_UNAMANAGED_V2D_DEST_PORT, MRXMLUtils.generateErrorXML("tcpPort", tcpPort));
        	}
        }*/

        streamURLReturn.append(mediatype).append("://");
        streamURLReturn.append(IPAddress).append("/");
        streamURLReturn.append("port=").append(tcpPort);
        streamURLReturn.append("?bandwidth=").append(lbandwidth);
        streamURLReturn.append("?avoption=").append(avoption);
        streamURLReturn.append("?ismulticast=").append(isMulticast);
        streamURLReturn.append("?enablekbm=").append(enablekbm);
        
        String authdata = this.getAuthData(streamDataDoc);
        if(!"".equals(authdata)) {
        	String authscheme = MediaRoomServiceConst.TOKEN;
	        streamURLReturn.append("?authscheme=").append(authscheme);
	        streamURLReturn.append("?authdata=").append(authdata);
        }

        addCmd.append("dstip="+ publishedDestIP+ " ");
        addCmd.append("dstport=" + tcpPort + " ");
        addCmd.append("maxbitrate=" + lbandwidth + " ");

        if (isMulticast.equalsIgnoreCase(MediaRoomServiceConst.TRUE)) {
        	//if multicast ipaddress is signaled use signaled one otherwise use local configuration
        	String mcastIP = XMLUtils.getValueXpath(streamDataDoc, "//multicastIP", "");
        	if(mcastIP.isEmpty()) {
        		mcastIP = networkResource.getAvailableMcastIPAddress(ssInstanceID, inputInstanceID);
        	}
            addCmd.append("mcastaddr=" + mcastIP + " ");
            addCmd.append("mcastttl=" + networkResource.getMcastTTL() + " ");
        }

        if (inputType.equals(MediaRoomServiceConst.INPUT_ENCODER)) {
            addCmd.append("v2dtx=1 ");
            addCmd.append("policycheck=0 ");
        }
        addCmd.append("listener=1");
        return addCmd.toString();
    }
    
	protected void releaseNetworkOutputResources(NetworkResource networkResource, 
			String ssInstanceID, Document streamDataDoc, Logger log) throws Exception {
        String isMulticast = XMLUtils.getValueXpath(streamDataDoc, "//IsMulticast", "");
        if(MediaRoomServiceConst.TRUE.equalsIgnoreCase(isMulticast)) {
        	String mcastIP = XMLUtils.getValueXpath(streamDataDoc, "//multicastIP", "");
        	if(mcastIP.isEmpty()) {
        		networkResource.releaseMcastIPAddress(ssInstanceID);
        	}
        }
        networkResource.releaseTCPPort(ssInstanceID, log);
	}
	
	public static void main(String args[]) throws Exception {
		Document doc = V2DSSAPI.parseV2DURL("v2d://10.1.5.180/port=6060?bandwidth=1024", null);
		System.out.println(XMLUtils.documentToString(doc));
	}

	@Override
	public boolean isValidDestIP(String ipaddress) {
		return true;
	}

	@Override
	public boolean isValidSourceIP(String ipaddress) {
		if (ipaddress == null || "".equalsIgnoreCase(ipaddress)) {
			return false;
			
		}
		return true;
	}

	
}
