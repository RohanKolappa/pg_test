package com.ipvs.device.impl;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.ArrayList;
import java.util.Enumeration;

import org.dom4j.Document;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;


public class DeviceAgentUtils {
    private static DeviceAgentUtils deviceAgentUtils = null;
    Document deviceXML = null;

    public static DeviceAgentUtils getInstance() {
        if (deviceAgentUtils == null) {
            deviceAgentUtils = new DeviceAgentUtils();
        }
        return deviceAgentUtils;
    }

    public void setDeviceXML(Document deviceXML) {
        this.deviceXML = deviceXML;
    }

    public Document getDeviceXML() {
        return deviceXML;
    }


    
    public void setTXConnectionParameters(String srcPortElementID, Document streamDoc) {
    	
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
        System.out.println("***setTXConnectionParameters: StreamDoc:"+streamDoc.asXML());
        try { 												
            minRefresh = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Refresh/Min", "1");
            maxRefresh = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Refresh/Max", "15");
            minSlice = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Slice/Min", "0");
            maxSlice = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Slice/Max", "15");
            blockThreshold = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/BlockThreshold", "5");
            colorSampling = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/ColorSampling", "4:2:2");
           /* if(colorSampling.equalsIgnoreCase(MediaRoomServiceConst.TRUE)) {
            	colorSampling = MediaRoomServiceConst.YES;
            } else {
            	colorSampling = MediaRoomServiceConst.NO;
            } */        
            frameRateDivider = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/FrameRateDivider", "1");
            
            bandwidth = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/Bandwidth", "20M");
            highCompression = MRXMLUtils.getValueXpath(streamDoc, "//Compression/High", "7");
            lowCompression = MRXMLUtils.getValueXpath(streamDoc, "//Compression/Low", "1");
            burstRatio = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/BurstRatio", "1.00");
            burstDuration = MRXMLUtils.getValueXpath(streamDoc, "//V2DEncoderConnectionParameterConfig/BurstDuration", "50");
            avOption = MRXMLUtils.getValueXpath(streamDoc, "//V2DStreamConfig/AVOption", "Video");
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
         
        
        MRLog.getInstance(MRLog.CONSOLE).addInfo("Set Connection Parameters " + cmdArray,MRLog.INFO,MRLog.NOTIFY);
   
        try {
            MediaRoomServiceUtils.executeCLICommand( cmdArray.toArray(new String[cmdArray.size()]));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    
    public String getIPAddress() throws Exception {
        String address = null;

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
                address = add.getHostAddress();
            }
        }
        return address;
    }

    public int getMaxEncoderInstances(String streamType) {
        try {
        	String maxEncoderInstances = MRXMLUtils.getValueXpath(this.deviceXML, "//V2DStreamsEncodeConfig/MaxStreamInstanceCount", "1");
        	return Integer.parseInt(maxEncoderInstances);
        }
        catch(Exception e) {
        	return 0;
        }

    }
}
