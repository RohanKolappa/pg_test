package com.ipvs.mediaroomservice.streamingserver;

import java.net.InetAddress;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;

public class NetworkResource {
    String mcastTTL;
    private MCastIPAddressReservation mCastIPAddressReservation = new  MCastIPAddressReservation();
    private UDPReservation rtpUdpReservation = new  UDPReservation();
    private UDPReservation mpegUdpReservation = new  UDPReservation();
    private UDPReservation udpReservation = new  UDPReservation(); 
    private V2DReservation v2dTCPReservation = new  V2DReservation();
    private V2DReservation v2dUDPReservation = new  V2DReservation();
    
    public static String intToIPAddress(int ip) {
        StringBuffer sb = new StringBuffer(15);

        for (int shift = 24; shift > 0; shift -= 8) {
            // process 3 bytes, from high order byte down.
            sb.append(Integer.toString((ip >>> shift) & 0xff));
            sb.append('.');
        }

        sb.append(Integer.toString(ip & 0xff));

        return sb.toString();
    }

    public static int ipAddresstoInt(InetAddress inetAddress) {
        long longIpAddress = 0;
        byte[] bytes = inetAddress.getAddress();

        for (int i = 0; i < bytes.length; i++) {
            if (bytes[i] < 0) {
                longIpAddress += ((256 + bytes[i]) * Math.pow(256, 4 - i - 1));
            } else {
                longIpAddress += (bytes[i] * Math.pow(256, 4 - i - 1));
            }
        }

        int intIpAddress = (int) longIpAddress;

        return intIpAddress;
    }
    
    public void initRTPUDPPorts(int RTPUDPPortBegin, int RTPUDPPortEnd, int RTPUDPPortIncr) {
    	rtpUdpReservation.init(String.valueOf(RTPUDPPortBegin), String.valueOf(RTPUDPPortEnd), String.valueOf(RTPUDPPortIncr)); 
    }
    
    public void initMPEGUDPPorts(int MPEGUDPPortBegin, int MPEGUDPPortEnd, int MPEGUDPPortIncr) {
    	mpegUdpReservation.init(String.valueOf(MPEGUDPPortBegin), String.valueOf(MPEGUDPPortEnd), String.valueOf(MPEGUDPPortIncr)); 
    }
    
    public void initUDPPorts(int UDPPortBegin, int UDPPortEnd, int UDPPortIncr) {
    	udpReservation.init(String.valueOf(UDPPortBegin), String.valueOf(UDPPortEnd), String.valueOf(UDPPortIncr)); 
    }
    
    public void initV2DUDPPorts(int V2DUDPPortBegin, int V2DUDPPortEnd, int V2DUDPPortIncr) {
    	v2dUDPReservation.init(String.valueOf(V2DUDPPortBegin), String.valueOf(V2DUDPPortEnd), String.valueOf(V2DUDPPortIncr)); 
    }
    
    public void initV2DTCPPorts(String fromPortStr, String toPortStr) {
        try {
        		this.v2dTCPReservation.init(fromPortStr, toPortStr, String.valueOf(1));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public void initMcastIPAddresses(String fromIpAddress, String toIpAddress, String mcastTTL) {
        try {
        	this.mCastIPAddressReservation.init(fromIpAddress, toIpAddress, String.valueOf(1));
        } catch (Exception e) {
            e.printStackTrace();
        }
        this.mcastTTL = mcastTTL;
    }
    
    public String getMcastTTL() {
    	return this.mcastTTL;
    }
    
    // Return the next available IP address from queue for given input instance.
    public String getAvailableMcastIPAddress(String outputInstanceID, String inputInstanceID)
        throws MRException, Exception {
    	String ipAddress = this.mCastIPAddressReservation.reserve(outputInstanceID, inputInstanceID);
        MRLog.getInstance(MRLog.CONSOLE).addInfo(" Available Multicast IP Address = "+ipAddress +" InputInstance = "+inputInstanceID +" outputInstance = "+outputInstanceID,MRLog.DEBUG,MRLog.NOTIFY);
        return ipAddress;
    }

    //Release IP address from inputMcastAddress if there is no clients are connected. 
    public void releaseMcastIPAddress(String outputInstanceID) throws Exception {
    	this.mCastIPAddressReservation.release(outputInstanceID);
    }
    
    public boolean isReservedTCPPort(int tcpPort) throws Exception {
    	return this.v2dTCPReservation.inRange(tcpPort);
    }

    public String getAvailableTCPPort(String instanceId) throws Exception {
    	return this.v2dTCPReservation.reserve(instanceId);
    }
    
    public String getAvailableUDPPort(String instanceId) throws Exception {
    	return this.v2dUDPReservation.reserve(instanceId);
    }
    
    public void releaseTCPPort(String instanceId, MRLog log) throws Exception {
    	this.v2dTCPReservation.release(instanceId);
    }
    
    public void releaseUDPPort(String instanceId, MRLog log) throws Exception {
    	this.v2dUDPReservation.release(instanceId);
    }

	public String getNextUDPStartPort(String streamType) throws Exception {
		if((MediaRoomServiceConst.RTP).equalsIgnoreCase(streamType)) {
			return this.rtpUdpReservation.reserve();
		} else if (MediaRoomServiceConst.MPEGTS.equalsIgnoreCase(streamType)) {
			return this.mpegUdpReservation.reserve();
		} else if (MediaRoomServiceConst.UDP.equalsIgnoreCase(streamType)) {
			return this.udpReservation.reserve();
		} else {
			return "";
		}
	}
	
	public void releaseUDPStartPort(String startPort, String streamType) {
		if(MediaRoomServiceConst.RTP.equalsIgnoreCase(streamType)) {
			this.rtpUdpReservation.release(startPort);
		} else if (MediaRoomServiceConst.MPEGTS.equalsIgnoreCase(streamType)) {
			this.mpegUdpReservation.release(startPort);
		} else if (MediaRoomServiceConst.UDP.equalsIgnoreCase(streamType)) {
			this.udpReservation.release(startPort);
		} 
	}
	

	
	public void updateNetworkResource(Document deviceXML)  {
		try {
			Element ipAddressEL = MRXMLUtils.getElementXpath(deviceXML, "//MulticastIpAddressRange");
			if(ipAddressEL != null) {
				String fromIpAddress = ipAddressEL.elementText("From");
				String toIpAddress = ipAddressEL.elementText("To");
				String mcastTTL = MRXMLUtils.getValueXpath(deviceXML, "//DeviceConfig//MulticastIpAddressRange/MulticastTTL", "");
				this.initMcastIPAddresses(fromIpAddress, toIpAddress, mcastTTL);
			}	


			Element portEL = MRXMLUtils.getElementXpath(deviceXML, "//TxCtrlPortRange");
		// 	TBD Both need to be handled at the same time not one instead of the other
			if (portEL != null) {
				String fromPortStr = portEL.elementText("From");
				String toPortStr = portEL.elementText("To");
				this.initV2DTCPPorts(fromPortStr, toPortStr);
			}
		
			//System.out.println("DeviceSourceStreamAgent:createNetworkResource RxMediaPortRange=" + MRXMLUtils.elementToString(portEL));

		// TBD Read these values from the deviceXML file
			portEL = MRXMLUtils.getElementXpath(deviceXML, "//V2DStreamsRxConfig/RxMediaPortRange"); 
			initUDPPorts(this,portEL, MediaRoomServiceConst.V2D, 1);
			int UDPPortIncr = 4;
			portEL = MRXMLUtils.getElementXpath(deviceXML, "//MPEGTSStreamsRxConfig/RxMediaPortRange");
			initUDPPorts(this,portEL, MediaRoomServiceConst.MPEGTS, UDPPortIncr);
			portEL = MRXMLUtils.getElementXpath(deviceXML, "//UDPStreamsRxConfig/RxMediaPortRange");
			initUDPPorts(this,portEL, MediaRoomServiceConst.UDP, UDPPortIncr);
			portEL = MRXMLUtils.getElementXpath(deviceXML, "//RTPStreamsRxConfig/RxMediaPortRange");
			initUDPPorts(this,portEL, MediaRoomServiceConst.RTP, UDPPortIncr);
	   } catch(Exception e) {
		   e.printStackTrace();
	   }
		
	}
	
	
	private void initUDPPorts(NetworkResource networkResource, Element portEL, String portType, int portIncrement) {
		try {
			if (portEL != null) {
				String fromPortStr = portEL.elementText("From");
				String toPortStr = portEL.elementText("To");
				if(MediaRoomServiceConst.V2D.equalsIgnoreCase(portType)) {
					networkResource.initV2DUDPPorts(Integer.valueOf(fromPortStr), Integer.valueOf(toPortStr), portIncrement);
				} else if(MediaRoomServiceConst.UDP.equalsIgnoreCase(portType)) {
					networkResource.initUDPPorts(Integer.valueOf(fromPortStr), Integer.valueOf(toPortStr), portIncrement);
				} else if(MediaRoomServiceConst.MPEGTS.equalsIgnoreCase(portType)) {
					networkResource.initMPEGUDPPorts(Integer.valueOf(fromPortStr), Integer.valueOf(toPortStr), portIncrement);
				} else if(MediaRoomServiceConst.RTP.equalsIgnoreCase(portType)) {
					networkResource.initRTPUDPPorts(Integer.valueOf(fromPortStr), Integer.valueOf(toPortStr), portIncrement);
				}			
			}
		}catch(Exception e) {
			e.printStackTrace();
		}
	}

	
	
	
}
