package com.barco.device.vng.Media.streamingserver;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.device.vng.utils.XMLStreamReaderThread;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class DeviceStreamingServerImpl extends StreamingServerImpl{
	
    
    public static final int SSEVENTPORT = 10002;
   
    public static final int RESERVED_SSINSTANCEID=1000;
	public static final String ERROR_CODE = "errorCode";

	ServiceMessageHandlerI messageHandler = null;
	XMLStreamReaderThread xmlStreamReader;

    public DeviceStreamingServerImpl(Logger log, DeviceDBI deviceDB, ServiceMessageHandlerI messageHandler) {
        super(log, deviceDB);
       
        this.messageHandler = messageHandler;  
        
    }
    
	@Override
	public int getMaxEncoderInstances(String streamType) {
        try {
        	String maxEncoderInstances = XMLUtils.getValueXpath(this.deviceDB.getDeviceConfig(), "//V2DStreamsEncodeConfig/MaxStreamInstanceCount", "1");
        	return Integer.parseInt(maxEncoderInstances);
        }
        catch(Exception e) {
        	return 0;
        }
	}

	@Override
	public void setupInternalInstances(Document doc) throws Exception {
		List<?> l = null;
        Element el = null;
        Element port=null;
        
    	this.log.info("DeviceStreamingServerImpl:setupInternalInstances getDeviceConfig begin ...");
    	Document deviceDoc = this.deviceDB.getDeviceConfig();
    	this.log.info("DeviceStreamingServerImpl:setupInternalInstances getDeviceConfig end");
    	if (doc == null) {
    		l = XMLUtils.getListOfNodesXpath(deviceDoc, "//XP200PortConfig/PortID[../Info[PortIOType='StreamDst']");
    	}
    	else {
    		l = XMLUtils.getListOfNodesXpath(doc, "//XP200PortConfig/PortID[../Info[PortIOType='StreamDst']");
    	}
        if ((l != null) && !l.isEmpty()) {
            for (int i = 0; i < l.size(); i++) {
                el = (Element) l.get(i);
                if (doc != null) {
                	port = XMLUtils.getElementXpath(deviceDoc, "//XP200PortConfig/Info/PortIOType[../../PortID='" + el.getStringValue()+ "']");
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
        	this.log.info("DeviceStreamingServerImpl:setupInternalInstances List is empty");
        }
	}


	private void setupInternalDecoder(String portId) throws Exception {
		// TODO Auto-generated method stub
		/* TBD MIYE
        int nSSInstanceID=0;
        int nSSQueueID=0;
        nSSInstanceID = RESERVED_SSINSTANCEID + 2*Integer.parseInt(portId);
        nSSQueueID= nSSInstanceID +1;
        executeSSCommand (
        		"action=add id=" + nSSInstanceID + 
        		" operation=output dsttype=V2DHardWarePort ioport=" +
        		portId);
        executeSSCommand(
        		"action=start id=" + nSSInstanceID + 
        		" operation=output dsttype=V2DHardWarePort queueid=" + nSSQueueID);
        */
	}

	
	@Override
    public String getIPAddress() throws Exception {
		Document deviceStatusDoc = this.deviceDB.getDeviceStatus();
		String ipAddressXpath  = "//Parameters[Parameter[@name='Status']='True' and Parameter[@name='Primary']='True']/Parameter[@name='IPAddress']";
		String ipAddress = XMLUtils.getValueXpath(deviceStatusDoc, ipAddressXpath, "");  
		if(ipAddress.isEmpty()) {
			throw new ServiceException(ServiceExceptionCodes.RESOURCENOTAVAILABLE, "Device IP Address Not Found");
		}
		return ipAddress;
	}
	
    public String getIPAddressOld() throws Exception {
        String address = null;
		this.log.debug("DeviceStreamingServerImpl:getIPAddress begin");	    	

        for (Enumeration<NetworkInterface> cards = NetworkInterface.getNetworkInterfaces(); cards.hasMoreElements();) {
            NetworkInterface card = cards.nextElement();

            if (card.isLoopback()) {
                continue;
            }

            if (!card.isUp()) {
                continue;
            }

            if (card.isVirtual()) {
                continue;
            }

            for (Enumeration<InetAddress> addresses = card.getInetAddresses(); addresses.hasMoreElements();) {
                InetAddress add = addresses.nextElement();
        		this.log.debug("DeviceStreamingServerImpl:getHostAddress begin");	    	
                address = add.getHostAddress();
        		this.log.debug("DeviceStreamingServerImpl:getHostAddress end");	    	
            }
        }
		this.log.debug("DeviceStreamingServerImpl:getIPAddress end");	    	
        return address;
    }


	
	@Override
    protected String setupEncoderPort(Element inputElement, Document streamDoc) {
        String srcPortElementID = XMLUtils.getAttributeValue(inputElement,"sourcePortElementID");         
    	
        String minRefresh = null;
        String maxRefresh = null;
        String minSlice =null;
        String maxSlice = null;
        String blockThreshold  = null;
        String colorSampling =null;
        String frameRateDivider = null;
        String bandwidth = null;
        String lowCompression = null;
        String highCompression = null;
        String burstRatio = null;
        String burstDuration = null;
        String avOption = null;
        
        try { 												
            minRefresh = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Refresh/Min", "1");
            maxRefresh = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Refresh/Max", "15");
            minSlice = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Slice/Min", "0");
            maxSlice = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Slice/Max", "15");
            blockThreshold = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/BlockThreshold", "5");
            colorSampling = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/ColorSampling", "4:2:2");
           /* if(colorSampling.equalsIgnoreCase(MediaRoomServiceConst.TRUE)) {
            	colorSampling = MediaRoomServiceConst.YES;
            } else {
            	colorSampling = MediaRoomServiceConst.NO;
            } */        
            frameRateDivider = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/FrameRateDivider", "1");
            
            bandwidth = XMLUtils.getValueXpath(streamDoc, "//Bandwidth", V2DSSAPI.DefaultBandwidth);
            highCompression = XMLUtils.getValueXpath(streamDoc, "//Compression/High", "7");
            lowCompression = XMLUtils.getValueXpath(streamDoc, "//Compression/Low", "1");
            burstRatio = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/BurstRatio", "1.00");
            burstDuration = XMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/BurstDuration", "50");
            avOption = XMLUtils.getValueXpath(streamDoc, "//V2DStreamConfig/AVOption", "Video");
        } catch (Exception e1) {
            e1.printStackTrace();
        }

        ArrayList<String> cmdArray = new ArrayList<String>();

        cmdArray.add("set_tx_connection_parm");      
        cmdArray.add("-min_refresh");
        cmdArray.add(minRefresh);
        cmdArray.add("-max_refresh");
        cmdArray.add(maxRefresh);
        cmdArray.add("-min_slice");
        cmdArray.add(minSlice);
        cmdArray.add("-max_slice");
        cmdArray.add(maxSlice);
        cmdArray.add("-block_threshold");
        cmdArray.add(blockThreshold);
        cmdArray.add("-color_sampling");
        cmdArray.add(colorSampling);    
        cmdArray.add("-framerate_divider");
        cmdArray.add(frameRateDivider);
        cmdArray.add("-bandwidth");
        cmdArray.add("" + (Long.parseLong(bandwidth) * 1024));
        cmdArray.add("-low_compression");
        cmdArray.add(lowCompression);
        cmdArray.add("-high_compression");
        cmdArray.add(highCompression);
        cmdArray.add("-burst_ratio");
        cmdArray.add(burstRatio);
        cmdArray.add("-burst_duration");
        cmdArray.add(burstDuration);
        cmdArray.add("-av_option");
        cmdArray.add(avOption);
        cmdArray.add("-channelNumber");
        cmdArray.add(srcPortElementID);
         
        
    	this.log.info("VNGDeviceStreamingServerImpl:setupEncoderPortSet Connection Parameters " + cmdArray);
   
        try {
            executeCLICommand( cmdArray.toArray(new String[cmdArray.size()]));
        } catch (Exception e) {
            e.printStackTrace();
        }
    	return srcPortElementID;

    }
	
	// TBD MIYE Check with Padmaja if this is already defiend someplace
    public String executeCLICommand(String[] cmdArray) throws Exception {
        Runtime runtime = Runtime.getRuntime();

        //Testing on windows
        /* String[] cmdarr = {"cmd", "/c" ,"type Z:\\work\\mediaroom\\etc\\" + cmdArray[0] +".out"};
          Process proc = runtime.exec(cmdarr);*/
        cmdArray[0] = "/V2O/bin/" + cmdArray[0];

        Process proc = runtime.exec(cmdArray);

        //put a BufferedReader on the ls output
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
            	throw new ServiceException(com.barco.utils.mr.MediaRoomServiceExceptionCodes.EXECUTION_OF_COMMANDS_FIALED,"Execution of command failed" );
            }
        } catch (InterruptedException e) {
            System.err.println(e);
        } finally {
            bufferedreader.close();
        }

        return sb.toString();
    }
    

  
	@Override
    protected void setupDecoderPort(Element outputElement) {
        String destPortElementID = XMLUtils.getAttributeValue(outputElement,"destPortElementID");         
        try {
            teardownInternalDecoder(destPortElementID);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
	private void teardownInternalDecoder(String portId) throws Exception {
		// TODO Auto-generated method stub
        int nSSInstanceID=0;
        int nSSQueueID=0;
        nSSInstanceID = RESERVED_SSINSTANCEID + 2*Integer.parseInt(portId);
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
	protected void teardownDecoderPort(Element outputElement) {
        String destPortElementID = XMLUtils.getAttributeValue(outputElement,"destPortElementID");
        try {
            setupInternalDecoder(destPortElementID);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Override
    protected void teardownEncoderPort(Element inputElement) {
    	//Placeholder for tearing down the encoder port
    }

//	@Override
//	protected void simulateRecordProgressStoppedNotfication(Element outputElement,String ssInstanceNID) throws Exception {
//		Element streamElement = outputElement.getParent();
//		String errorCode = streamElement.attributeValue(ERROR_CODE,"");
//		Element mediaTimelineLastElement = XMLUtils.getElementXpathFromElement(streamElement, "./MediaTimeline/Last");
//		String timeScaleStr = mediaTimelineLastElement.attributeValue("TimeScale");
//		double timeScale = 0;
//		try {
// 			timeScale = Double.parseDouble(timeScaleStr);
//		}
//		catch(Exception e) {
//	   		log.info("VNGDeviceStreamingServerImpl:simulateRecordProgressStoppedNotfication invalid MediaTimeline.Last.timeScale=" + timeScaleStr);			
//		}
//		
//		//if recording is  in paused state then send progress stopped notification to the streaming server
//    	//We are simulating the progress stopped notification because streaming server not sending us notification on delete instance
//    	if(timeScale == 0 || !errorCode.isEmpty()) {
//    		log.info("VNGDeviceStreamingServerImpl: Simulate Recording stopped message:="+ssInstanceNID + " timeScale:="+timeScale +" errorCode:="+errorCode);    	    
//    		sendProgressStoppedSSEventToSourceStreamAgent(outputElement, ssInstanceNID);
//    	}		
//	}


	@Override
    protected String executeStatus(String statusCmd) throws Exception {
        String statusResult = executeSSCommand(statusCmd);

        if ((statusResult == null) || statusResult.contains("201 OK")) {
            throw new ServiceException(MediaRoomServiceExceptionCodes.GETINSTANCESTATUSFAILED, "VNGDeviceStreamingServerImpl:executeStatus Get STREAMING SERVER INSTANCE STATUS FAILED cmd=" + statusCmd + " result=" + statusResult);
        }

        return statusResult;
    }

}
