package com.barco.device.vng.Media.streamingserver;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.ServiceException;
import com.barco.device.vng.utils.VNGConst;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;


public abstract class SSAPI {
	
	
	protected abstract String getStreamType();
	protected abstract String getInputPortType(String inputType) throws ServiceException;
    protected abstract String getNetworkInputString(String ssInstanceID, Document streamDataDoc,
    		NetworkResource networkResource, String streamURL, Logger log) throws Exception ;
	protected abstract void releaseNetworkInputResources(NetworkResource networkResource, 
			String ssInstanceID, Logger log) throws Exception;
    protected abstract String getOutputPortType(String outputType) throws Exception;
    protected abstract String getNetworkOutputString(String ssInstanceID, Document streamDataDoc,
    		NetworkResource networkResource, String IPAddress, String inputType, String inputInstanceID,
    		Logger log, StringBuffer streamURLReturn) throws Exception;
	protected abstract void releaseNetworkOutputResources(NetworkResource networkResource, 
			String ssInstanceID, Document streamDataDoc, Logger log) throws Exception;
	public abstract boolean isValidSourceIP(String ipaddress);
	public abstract boolean isValidDestIP(String ipaddress);
	
	public String getPublishedDestIPAddress(Document streamDataDoc)
			throws Exception {
		return XMLUtils.getValueXpath(streamDataDoc, "//publishedDestIP","");
	}


	
	public String getPublishedSourceIPAddress(Document streamDataDoc)
			throws Exception {
		return XMLUtils.getValueXpath(streamDataDoc, "//publishedSourceIP","");
	}


	
	public String getResolvedDestIPAddress(Document streamDataDoc)
			throws Exception {
		return XMLUtils.getValueXpath(streamDataDoc, "//resolvedDestIP","");
	}

	protected String getAuthData(Document streamDataDoc) throws Exception {
		String mediaDestNID = XMLUtils.getValueXpath(streamDataDoc, "//mediaDestNID", ""); 
		String destStreamURL = XMLUtils.getValueXpath(streamDataDoc, "//destStreamURL", "");
		// System.out.println("SSAPI:getAuthData mediaDestNID=" + mediaDestNID + " destStreamURL=" + destStreamURL);
	    if(!"".equals(destStreamURL)) // Is UnManaged 
	    	return "";
    	return mediaDestNID;
	}


	public String getResolvedSourceIPAddress(Document streamDataDoc)
			throws Exception {
		return XMLUtils.getValueXpath(streamDataDoc, "//resolvedSourceIP","");
	}
    public void stopSSInputCmd(Element inputElement, StringBuffer startCmd) throws Exception {
        String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
        String ssInstanceID =XMLUtils.getAttributeValue( inputElement,"ssInstanceID");
        startCmd.append("action=stop ");
        startCmd.append("id=" + ssInstanceID + " ");
        startCmd.append("queueid=" + ssInstanceID + " ");
        startCmd.append("operation=input ");
        startCmd.append("srctype=" + this.getInputPortType(inputType) + " ");
    }	
    
    public void startSSInputCmd(Element instanceElement, Element streamElement, StringBuffer startCmd)
	    throws Exception {
	    Element inputEL = instanceElement.element("Input");
	    String inputType = XMLUtils.getAttributeValue(inputEL,"inputType");
	    String ssInstanceID = XMLUtils.getAttributeValue(inputEL,"ssInstanceID");
	    startCmd.append("action=start ");
	    startCmd.append("id=" + ssInstanceID + " ");
	    startCmd.append("queueid=" + ssInstanceID + " ");
	    startCmd.append("operation=input ");	
        startCmd.append("srctype=" + this.getInputPortType(inputType) + " ");	
	}
    
    public void refreshSSInputCmd(Element inputElement, Document streamDataDoc, 
    		StringBuffer updateCmd) throws Exception {
	    String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
	    String ssInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
	    updateCmd.append("action=update ");
	    updateCmd.append("id=" + ssInstanceID + " ");
	    updateCmd.append("operation=input ");
	    updateCmd.append("streamtype=" + this.getStreamType() + " ");
	    updateCmd.append("queueid=" + ssInstanceID + " ");
        updateCmd.append("srctype=" + this.getInputPortType(inputType) + " ");
	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
	        updateCmd.append("fullscreen_refresh=1 ");
	    }	
	}
    
    public void updateSSOutputCmd(Element outputElement, Document streamDataDoc, 
    		StringBuffer updateCmd) throws Exception {
	    String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
	    String ssInstanceID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
	    updateCmd.append("action=update ");
	    updateCmd.append("id=" + ssInstanceID + " ");
	    updateCmd.append("operation=output ");
	    updateCmd.append("streamtype=" + this.getStreamType() + " ");
	    updateCmd.append("queueid=" + ssInstanceID + " ");
        updateCmd.append("dsttype=" + this.getOutputPortType(outputType) + " ");
	    if (MediaRoomServiceConst.OUTPUT_IMAGE_WRITER.equalsIgnoreCase(outputType)) {
	    	String publishURL = XMLUtils.getAttributeValue(outputElement,"publishURL");
	    	String ioPort = XMLUtils.getAttributeValue(outputElement,"destPortElementID");
	        updateCmd.append("publishURL=\"" + publishURL + "\"" + " ");
	        updateCmd.append("ioport=" + ioPort + " ");
	    }	
	    else if (MediaRoomServiceConst.OUTPUT_DECODER.equalsIgnoreCase(outputType)) {
		String enablekbm = XMLUtils.getValueXpath(streamDataDoc, ".//EnableKbm", "false");
		int enable = enablekbm.equals("true") ? 1:0;
		updateCmd.append("enablekbm=" +enable);
	    }
	}
    
    public void createSSInputCmd(Element inputElement, 
    		Document streamDataDoc, Document mediaSourceDataDoc,
    		StringBuffer addCmd, String ioport)     throws Exception {

    	String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
        String ssInstanceID =XMLUtils.getAttributeValue( inputElement,"ssInstanceID");
        addCmd.append("action=add ");
        addCmd.append("id=" + ssInstanceID + " ");
        addCmd.append("operation=input ");
        addCmd.append("streamtype=" + this.getStreamType() + " ");
        addCmd.append("srctype=" + this.getInputPortType(inputType) + " ");
        addCmd.append("ioport=" + ioport + " ");

        if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            // String uuid = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileUUID", "");
            String uuid = XMLUtils.getAttributeValue(inputElement,"mediaUUID");
            addCmd.append("srcname=" + uuid + " ");
            long startTimeCode = Long.parseLong(XMLUtils.getAttributeValueWithDefault(inputElement, "startTimecode", "0"));
            long offset = Long.parseLong(XMLUtils.getAttributeValueWithDefault(inputElement, "offset", "0"));
            long tc = 0;
            long syncTC = Long.parseLong(XMLUtils.getAttributeValueWithDefault(inputElement, "syncTC", "0"));
            if (syncTC > 0) {
                addCmd.append(" tc=" + syncTC + " sync=true");
            }
            else if (startTimeCode > 0) {
                tc = startTimeCode + offset;
                addCmd.append(" tc=" + tc);
            } else {
                addCmd.append(" offset=" + offset);
            }
            String dirPath = XMLUtils.getAttributeValue(inputElement,"dirPath");
            addCmd.append(" srcdir=\"" + dirPath + "\" ");  
            
            //add time line 
            String  timeScale = XMLUtils.getAttributeValueWithDefault(inputElement, "timescale", "1.0");
            addCmd.append(" speed=\"" + timeScale + "\" "); 
            String ffTracks = XMLUtils.getValueXpath(streamDataDoc, "//V2DRecorderConfig/FFTracks", "");
            String trickPlay = XMLUtils.getValueXpath(streamDataDoc, "//trickPlay", VNGConst.FALSE);
            if((ffTracks.isEmpty() || trickPlay.equalsIgnoreCase(VNGConst.FALSE)) && (!timeScale.equals("1.0"))) {
            	addCmd.append(" dummyplayback=1 ");
            }
        }

    }
    
    public void updateSSNetworkInputCmd(Element inputElement, Document streamDataDoc, String action, 
    		NetworkResource networkResource, Logger log, StringBuffer addCmd, StringBuffer startCmd)
        throws Exception {
        String streamURL = XMLUtils.getAttributeValue(inputElement,"streamURL");
        String ssInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
        String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");

        addCmd.append("action=add ");
        addCmd.append("id=" + ssInstanceID + " ");
        addCmd.append("operation=input ");
        addCmd.append("streamtype=" + this.getStreamType() + " ");
        addCmd.append("srctype=" + this.getInputPortType(inputType) + " ");
        
        
        String authdata = this.getAuthData(streamDataDoc);
        if(!"".equals(authdata)) {
        	String authscheme = MediaRoomServiceConst.TOKEN;
            addCmd.append("authscheme="+ authscheme+ " ");
            addCmd.append("authdata="+ authdata+ " ");
        }
        
        addCmd.append(this.getNetworkInputString(ssInstanceID, streamDataDoc, networkResource, streamURL, log));

        startCmd.append("action=start ");
        startCmd.append("id=" + ssInstanceID + " ");
        startCmd.append("queueid=" + ssInstanceID + " ");
        startCmd.append("operation=input ");
        startCmd.append("srctype=" + this.getInputPortType(inputType) + " ");

    }
    
    public void deleteSSInputCmd(Element inputElement, NetworkResource networkResource, 
    		Logger log, StringBuffer deleteCmd) throws Exception {
        String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
        String ssInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
        deleteCmd.append("action=delete ");
        deleteCmd.append("id=" + ssInstanceID + " ");
        deleteCmd.append("operation=input ");
        deleteCmd.append("streamtype=" + this.getStreamType() + " ");
        deleteCmd.append("srctype=" + this.getInputPortType(inputType) + " ");

	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType) ||
	    	 MediaRoomServiceConst.INPUT_STREAM.equals(inputType) ) {
	    	this.releaseNetworkInputResources(networkResource, ssInstanceID, log);
		}
    }
    
    public void stopSSOutputAtCmd(Element instanceElement, Element streamElement, long offset, 
    		StringBuffer stopCmd) throws Exception {
	
	    Element inputElement = instanceElement.element("Input");
    	Element outputElement = streamElement.element("Output");
	    String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
	    String ssInstanceID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
	    String inputInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
	
	    stopCmd.append("action=stop ");
	    stopCmd.append("id=" + ssInstanceID + " ");
	    stopCmd.append("queueid=" + inputInstanceID + " ");
	    stopCmd.append("operation=output ");
        stopCmd.append("dsttype=" + this.getOutputPortType(outputType) + " ");
        
        /*//TBR : once streaming server notifications implemented update endTC for File Outputs
        Document streamDataDoc = MRXMLUtils.stringToDocument(db.getObjectData(MRRequest.createRequest(userJID), streamNID));
        String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID", "");
        updateDestFileEndTC(destNID, System.currentTimeMillis());*/
        
	    long tc = 0;
	    stopCmd.append("tc=" + tc + " ");
	    stopCmd.append("offset=" + offset + " ");
	
	}

    public void stopSSOutputAtSyncCmd(List<Element> streamElementList, long offset, 
    		StringBuffer stopCmd) throws Exception {
    	// System.out.println("SSAPI:stopSSOutputAtSyncCmd streamElementList count=" + streamElementList.size());
    	StringBuffer idlist = new StringBuffer();
    	StringBuffer qlist = new StringBuffer();
    	String outputType = "";
        for (Iterator<Element> i = streamElementList.iterator(); i.hasNext();) {
        	Element streamElement = i.next();
        	Element instanceElement = streamElement.getParent().getParent();
        	Element inputElement = instanceElement.element("Input");
        	Element outputElement = streamElement.element("Output");
        	if(idlist.length() > 0)
        		idlist.append(",");
        	idlist.append(XMLUtils.getAttributeValue(outputElement,"ssInstanceID"));
        	if(qlist.length() > 0)
        		qlist.append(",");
        	qlist.append(XMLUtils.getAttributeValue(inputElement,"ssInstanceID"));
    	    outputType = XMLUtils.getAttributeValue(outputElement,"outputType"); // Not used
	    }
		
        long now = System.currentTimeMillis();
	    stopCmd.append("action=syncstop ");
	    stopCmd.append("id=" + now + " ");
	    stopCmd.append("idlist=" + idlist.toString() + " ");
	    stopCmd.append("qlist=" + qlist.toString() + " ");
	    stopCmd.append("operation=output ");
        stopCmd.append("dsttype=" + this.getOutputPortType(outputType) + " ");
                
	    long tc = 0;
	    stopCmd.append("tc=" + tc + " ");
	
	}
    
    public void startSSInputAtSyncCmd(Hashtable<String,Element> instanceElementList, StringBuffer startCmd) throws Exception {
    	// System.out.println("SSAPI:stopSSOutputAtSyncCmd streamElementList count=" + streamElementList.size());
    	StringBuffer idlist = new StringBuffer();
    	StringBuffer qlist = new StringBuffer();
    	Enumeration<String> e = instanceElementList.keys();
    	String inputType = "";
    	while(e.hasMoreElements()) {
    		String instanceNID = e.nextElement();
    		Element instanceElement = instanceElementList.get(instanceNID);
    	    Element inputEL = instanceElement.element("Input");
    	    inputType = XMLUtils.getAttributeValue(inputEL,"inputType");
	    	String id = XMLUtils.getAttributeValue(inputEL,"ssInstanceID");
        	if(idlist.length() > 0)
        		idlist.append(",");
        	idlist.append(id);
        	if(qlist.length() > 0)
        		qlist.append(",");
        	qlist.append(id);
    	}    	
        long now = System.currentTimeMillis();
        startCmd.append("action=syncstart ");
        startCmd.append("id=" + now + " ");
        startCmd.append("idlist=" + idlist.toString() + " ");
        startCmd.append("qlist=" + qlist.toString() + " ");
        startCmd.append("operation=input ");
        startCmd.append("srctype=" + this.getInputPortType(inputType) + " ");
    }

    public void startSSOutputCmd(Element instanceElement, Element streamElement, 
    		Element outputElement, StringBuffer startCmd) throws Exception {
        Element inputElement = instanceElement.element("Input");
        String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
        String ssInstanceID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
        String inputInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");

        startCmd.append("action=start ");
        startCmd.append("id=" + ssInstanceID + " ");
        startCmd.append("queueid=" + inputInstanceID + " ");
        startCmd.append("operation=output ");
        startCmd.append("dsttype=" + this.getOutputPortType(outputType) + " ");

        /* //TBR : once streaming server notifcations implemented update startTC for File Outputs
        Document streamDataDoc = MRXMLUtils.stringToDocument(db.getObjectData(MRRequest.createRequest(userJID), streamNID));
        String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID", "");
        updateDestFileStartTC(destNID, System.currentTimeMillis());*/


    }
    
    public void createSSOutputCmd(Element outputElement, Element inputElement, Document streamDataDoc,
    		NetworkResource networkResource, String IPAddress,  Logger log,
    		StringBuffer addCmd, StringBuffer streamURLReturn) throws Exception {
	log.debug("createSSOutputCmd: Ipaddress: "+IPAddress);
        String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
        String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
        String inputInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
        String ssInstanceID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
        String destPortElementID = XMLUtils.getAttributeValue(outputElement,"destPortElementID");
        addCmd.append("action=add ");
        addCmd.append("id=" + ssInstanceID + " ");
        addCmd.append("operation=output ");
        addCmd.append("streamtype=" + this.getStreamType() + " ");
        addCmd.append("dsttype=" + this.getOutputPortType(outputType) + " ");
        addCmd.append("ioport=" + destPortElementID + " ");

        if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || 
        		MediaRoomServiceConst.OUTPUT_PVR.equals(outputType)) {
        	String destUUID =  XMLUtils.getAttributeValue(outputElement,"destFileUUID");  
            addCmd.append("dstname=" + destUUID + " ");
          
            if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType)) {
	            String ffTracks = XMLUtils.getValueXpath(streamDataDoc, "//V2DRecorderConfig/FFTracks", "");
	            if(!ffTracks.isEmpty())
	            	ffTracks = "1";
	            else 
	            	ffTracks = "0";
	            addCmd.append("speedtracks=" + ffTracks + " ");
	            
	            boolean primaryThumbnail = Utils.getBooleanValue(XMLUtils.getValueXpath(streamDataDoc, "//ThumbnailConfig/Enable", "false"), false);
	            if (!primaryThumbnail)
	            	addCmd.append(" primarythumbnail=0 ");
	            else
	            	addCmd.append(" primarythumbnail=1 ");
	           
	        }
            
            if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType)) {
                addCmd.append("trimlength=0 ");
            } else {
                String trimLength = XMLUtils.getValue(streamDataDoc.getRootElement(), "trimLength", MediaRoomServiceConst.DEFAULT_TRIMLENGTH);
                addCmd.append("trimlength=" + trimLength + " ");
            }
            String dirPath = XMLUtils.getAttributeValue(outputElement,"dirPath");
            addCmd.append("dstdir=\"" + dirPath + "\" ");            
        } else if (MediaRoomServiceConst.OUTPUT_STREAM.equals(outputType)) {
            String authdata = this.getAuthData(streamDataDoc);
            if(!"".equals(authdata)) {
            	String authscheme = MediaRoomServiceConst.TOKEN;
                addCmd.append("authscheme="+ authscheme+ " ");
                addCmd.append("authdata="+ authdata+ " ");
            }
        	String networkOutputString = this.getNetworkOutputString(ssInstanceID, streamDataDoc, 
        			networkResource, IPAddress, inputType, inputInstanceID, log, streamURLReturn);
        	addCmd.append(networkOutputString);

        } else if (MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) {
        	String enablekbm = XMLUtils.getValueXpath(streamDataDoc, "//EnableKbm", "false");
        	
        	if(streamDataDoc == null) { // Rx output - Use the kbm value from the inputElement
        		enablekbm = XMLUtils.getAttributeValue(inputElement, "EnableKbm");
        	}

        	if (enablekbm.equalsIgnoreCase(MediaRoomServiceConst.TRUE)) {
        		addCmd.append("enablekbm=1 ");
            }
            else {
            	addCmd.append("enablekbm=0 ");
            }
        }
        else if(MediaRoomServiceConst.OUTPUT_IMAGE_WRITER.equalsIgnoreCase(outputType)){
           //cli command to add imagewriter port parameters 
            String dirPath = XMLUtils.getAttributeValue(outputElement,"dirPath");
            String publishURL = XMLUtils.getAttributeValue(outputElement,"publishURL");;            
            addCmd.append("dstdir=\"" + dirPath + "\" ");
            addCmd.append("publishURL=\"" + publishURL + "\" ");
            
        }
    }
    public void deleteSSOutputCmd(Element outputElement, Document streamDataDoc, 
    		NetworkResource networkResource, Logger log, StringBuffer deleteCmd) throws Exception {
        String outputType =XMLUtils.getAttributeValue( outputElement,"outputType");
        String ssInstanceID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
        deleteCmd.append("action=delete ");
        deleteCmd.append("id=" + ssInstanceID + " ");
        deleteCmd.append("operation=output ");
        deleteCmd.append("streamtype=" + this.getStreamType() + " ");
        deleteCmd.append("dsttype=" + this.getOutputPortType(outputType) + " ");

	    if (MediaRoomServiceConst.OUTPUT_STREAM.equals(outputType)) {
	    	this.releaseNetworkOutputResources(networkResource, ssInstanceID, streamDataDoc, log);
		}	    
    }
    
    public void getCurrentTCCmd(Element outputElement, StringBuffer statusCmd) throws ServiceException, Exception {
        String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
        String ssInstanceID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
        statusCmd.append("action=status ");
        statusCmd.append("id=" + ssInstanceID + " ");
        statusCmd.append("operation=output ");
        statusCmd.append("streamtype=" + this.getStreamType() + " ");
        statusCmd.append("dsttype=" + this.getOutputPortType(outputType) + " ");
    }
    
    public void getInputInstanceStatusCmd(Element inputElement, StringBuffer statusCmd) throws Exception {
        String inputType =XMLUtils.getAttributeValue(inputElement,"inputType");
        String ssInstanceID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
        //start command
        statusCmd.append("action=status ");
        statusCmd.append("id=" + ssInstanceID + " ");
        statusCmd.append("queueid=" + ssInstanceID + " ");
        statusCmd.append("operation=input ");
        statusCmd.append("srctype=" + this.getInputPortType(inputType) + " ");
    }
    
	
	protected String getMcastTTL() { // TBD
		return "1";
	}

	public void getTCAtOffsetCmd(String mediaUUID,String srcDirPath, long offset, StringBuffer statusCmd) throws Exception {
        statusCmd.append("action=query ");
        statusCmd.append("command=GetTimeCodeAtOffset ");
        statusCmd.append("srctype=DMSMediaPort ");
        statusCmd.append("srcname=" + mediaUUID + " ");
        statusCmd.append("srcdir=\"" + srcDirPath + "\" ");  
        statusCmd.append("offset=" + offset);
	}
	
	public void getOffsetAtTCCmd(String mediaUUID, long tc,String srcDirPath, StringBuffer statusCmd) throws Exception {
        statusCmd.append("action=query ");
        statusCmd.append("command=GetOffsetAtTimecode ");
        statusCmd.append("srctype=DMSMediaPort ");
        statusCmd.append("srcname=" + mediaUUID + " ");
        statusCmd.append("srcdir=\"" + srcDirPath + "\" ");  
        statusCmd.append("tc=" + tc);
	}


}
