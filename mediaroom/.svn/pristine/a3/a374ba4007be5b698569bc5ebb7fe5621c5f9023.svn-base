package com.ipvs.assetadminservice.datamodel;

import java.io.StringReader;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.IpvsDBUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;
import com.ipvs.utils.ValidationUtils;

public class AMValidatorPlugin {
	  /*<CustomValidation methodName="validateTagName" defaultValidatorClass="com.ipvs.assetadminservice.datamodel.AMValidatorPlugin">
			 <validation validationID="ServerName" methodName="validateIPAddress" action="Add"
			   validatorClass="com.ipvs.assetadminservice.datamodel.AMValidatorPlugin" 
			   xpath="//OperationData/V2D-Rx-Device/DeviceStatus/ServiceElementsConfig/StreamingServiceConfig/InstanceList/V2DDecoderInstance/V2DDecoderInstanceConfig/V2DDecoderInstanceConnectionEntryConfig/IPAddress" >
			    <!-- any config parameters to be passed to validate -->
			    <mustNotContain>
				<specialChar>true<specialChar> <!-- no specialChar-->
				<digit>true<digit> <!-- no specialChar-->
				</mustNotContain>
				<maxLength>15</maxLength>
		 	</validation>
		 </CustomValidation> 
	  */
	  public Boolean validateTagName( String action, String serviceName,  String dbObjectName,  Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
//		  if(elReq !=null){
//			  //System.out.println(action + " AMValidatorPlugin Called validateTagName req for " + elReq.getName() + ": " );
//		  }else if(elReq !=null){
//			  //System.out.println(action + " AMValidatorPlugin Called validateTagName cust req for " + elCustomValidate.getName() + ": " );
//		  }	  	
		  
		  return Boolean.valueOf(true);
	  }	 
	
	  public Boolean validateUserName( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		  return Boolean.valueOf(true);
	  }	 
	  public Boolean validatePassword( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
  		  return Boolean.valueOf(true);
	  }
	  public Boolean validateEmail( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		  return Boolean.valueOf(true);
	  }	  
	  public Boolean validateGroupName( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
  		  return Boolean.valueOf(true);
	  }	  
	  public Boolean validateLocation( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
  		  return Boolean.valueOf(true);
	  }	  
	  public Boolean validateDestGroup( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
  		  return Boolean.valueOf(true);
	  }
	  public Boolean validateConnectionParameterConfig( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean connectionParameterMode = false;
    	    String bandwidth = MRXMLUtils.getValueXpath(elReq, "Bandwidth", name);
    	    String maxBandWidth = maxBandwidthValue(bandwidth);
		    boolean bandwidthMode = false;
			long band = new Long(maxBandWidth).longValue();
			if (band >= 0 && band <= 524288000) {
				bandwidthMode = true;
			}
			connectionParameterMode = bandwidthMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
		    }
			String burstRatio = MRXMLUtils.getValueXpath(elReq, "BurstRatio",name);
			boolean burstRatioMode = false;
			double ratio = new Double(burstRatio).doubleValue();
			if (ratio >= 1 && ratio <= 100) {
				burstRatioMode = true;
			}
			connectionParameterMode = burstRatioMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String burstDuration = MRXMLUtils.getValueXpath(elReq, "BurstDuration",name);
			boolean burstDurationMode = false;
			double duration = new Double(burstDuration).doubleValue();
			if (duration >= 0 && duration <= 1000) {
				burstDurationMode = true;
			}
			connectionParameterMode = burstDurationMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String highCompression = MRXMLUtils.getValueXpath(elReq, "Compression/High", name);
			boolean highCompressionMode = false;
			int high = new Integer(highCompression).intValue();
			if (high >= 0 && high <= 10) {
				highCompressionMode = true;
			}
			connectionParameterMode = highCompressionMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String lowCompression = MRXMLUtils.getValueXpath(elReq, "Compression/Low", name);
			boolean lowCompressionMode = false;
			int low = new Integer(lowCompression).intValue();
			if (low >= 0 && low <= 10) {
				lowCompressionMode = true;
			}
			connectionParameterMode = lowCompressionMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String minRefresh = MRXMLUtils.getValueXpath(elReq, "Refresh/Min",name);
			boolean minRefreshMode = false;
			int min = new Integer(minRefresh).intValue();
			if (min >= 0 && min <= 255) {
				minRefreshMode = true;
			}
			connectionParameterMode = minRefreshMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String maxRefresh = MRXMLUtils.getValueXpath(elReq, "Refresh/Max", name);
			boolean maxRefreshMode = false;
			int max = new Integer(maxRefresh).intValue();
			if (max >= 0 && max <= 255) {
				maxRefreshMode = true;
			}
			connectionParameterMode = maxRefreshMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String minSlice = MRXMLUtils.getValueXpath(elReq, "Slice/Min",name);
			boolean minSliceMode = false;
			int minimum = new Integer(minSlice).intValue();
			if (minimum >= 0 && minimum <= 255) {
				minSliceMode = true;
			}
			connectionParameterMode = minSliceMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String maxSlice = MRXMLUtils.getValueXpath(elReq, "Slice/Max", name);
			boolean maxSliceMode = false;
			int maximum = new Integer(maxSlice).intValue();
			if (maximum >= 0 && maximum <= 255) {
				maxSliceMode = true;
			}
			connectionParameterMode = maxSliceMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
			String blockThreshold = MRXMLUtils.getValueXpath(elReq, "BlockThreshold", name);
			boolean blockThresholdMode = false;
			int block = new Integer(blockThreshold).intValue();
			if (block >= 1 && block <= 32) {
				blockThresholdMode = true;
			}
			connectionParameterMode = blockThresholdMode;
			if(!connectionParameterMode){
				return connectionParameterMode;
			}
		    return connectionParameterMode;
	  }
	  public Boolean validateRxConnectionParameterConfig( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    Element parentElement = MRXMLUtils.getParentElement(elReq);
		    boolean connectionParameterMode = false;
		    List <?> attrList =  MRXMLUtils.getList(parentElement, name);
		    if(attrList.size() > 0){
				for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
					Element connectionParameterEL = (Element) i.next();
					String destination = MRXMLUtils.getValueXpath(connectionParameterEL, "IPAddress",name);
					boolean ipaddressMode = verifyIP(destination);
					connectionParameterMode = ipaddressMode;
					if(!connectionParameterMode){
						break;
					}
					String bandwidth = MRXMLUtils.getValueXpath(connectionParameterEL, "Bandwidth", name);
					String maxBandWidth = maxBandwidthValue(bandwidth);
					boolean bandwidthMode = false;
					long band = new Long(maxBandWidth).longValue();
					if (band >= 0 && band <= 524288000) {
						bandwidthMode = true;
					}
					connectionParameterMode = bandwidthMode;
					if(!connectionParameterMode){
						break;
					}
					String port = MRXMLUtils.getValueXpath(connectionParameterEL, "Port", name);
					boolean portMode = false;
					int portno = new Integer(port).intValue();
					if (portno >= 1025 && portno <= 65500) {
						portMode = true;
					}
					connectionParameterMode = portMode;
					if(!connectionParameterMode){
						break;
					}
				}
		    }
		    return connectionParameterMode;
	  }
	  public Boolean validateVideoandIOParameters( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean videoandIOParametersMode = false;
			String samplingPhase = MRXMLUtils.getValueXpath(elReq, "SamplingPhase",name);
			boolean samplingPhaseMode = false;
			int sampling = new Integer(samplingPhase).intValue();
			if (sampling >= -16 && sampling <= 15) {
				samplingPhaseMode = true;
			}
			videoandIOParametersMode = samplingPhaseMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
			String coarseNoise = MRXMLUtils.getValueXpath(elReq, "NoiseElimination/Coarse",name);
			boolean coarseNoiseMode = false;
			int coarse = new Integer(coarseNoise).intValue();
			if (coarse >= 1 && coarse <= 256) {
				coarseNoiseMode = true;
			}
			videoandIOParametersMode = coarseNoiseMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
			String fineNoise = MRXMLUtils.getValueXpath(elReq, "NoiseElimination/Fine",name);
			boolean fineNoiseMode = false;
			int fine = new Integer(fineNoise).intValue();
			if (fine >= 1 && fine <= 2048) {
				fineNoiseMode = true;
			}
			videoandIOParametersMode = fineNoiseMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
			String horizontalPosition = MRXMLUtils.getValueXpath(elReq, "Position/Horizontal",name);
			boolean horizontalPositionMode = false;
			int horizontal = new Integer(horizontalPosition).intValue();
			if (horizontal >= -135 && horizontal <= 375) {
				horizontalPositionMode = true;
			}
			videoandIOParametersMode = horizontalPositionMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
			String verticalPosition = MRXMLUtils.getValueXpath(elReq, "Position/Vertical",name);
			boolean verticalPositionMode = false;
			int vertical = new Integer(verticalPosition).intValue();
			if (vertical >= -25 && vertical <= 50) {
				verticalPositionMode = true;
			}
			videoandIOParametersMode = verticalPositionMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
			String pumpCurr = MRXMLUtils.getValueXpath(elReq, "PumpCurr",name);
			boolean pumpCurrMode = false;
			int pump = new Integer(pumpCurr).intValue();
			if (pump >= 50 && pump <= 1500) {
				pumpCurrMode = true;
			}
			videoandIOParametersMode = pumpCurrMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
		    return videoandIOParametersMode;
	  }
	  public Boolean validateRxVideoandIOParameters( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean videoandIOParametersMode = false;
			String pixelAdjust = MRXMLUtils.getValueXpath(elReq, "PixelAdjust",name);
			boolean pixelAdjustMode = false;
			int pixel = new Integer(pixelAdjust).intValue();
			if (pixel >= -25 && pixel <= 25) {
				pixelAdjustMode = true;
			}
			videoandIOParametersMode = pixelAdjustMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
			String phaseAdjust = MRXMLUtils.getValueXpath(elReq, "PhaseAdjust",name);
			boolean phaseAdjustMode = false;
			int phase = new Integer(phaseAdjust).intValue();
			if (phase >= 0 && phase <= 32) {
				phaseAdjustMode = true;
			}
			videoandIOParametersMode = phaseAdjustMode;
			if(!videoandIOParametersMode){
				return videoandIOParametersMode;
			}
		    return videoandIOParametersMode;
	  }
	  public Boolean validateVideoandIOSerialConfig( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean videoandIOSerialConfigMode = false;
			String serialPort = MRXMLUtils.getValueXpath(elReq, "SerialPort",name);
			boolean serialPortMode = false;
			int serial = new Integer(serialPort).intValue();
			if (serial >= 0 && serial <= 1) {
				serialPortMode = true;
			}
			videoandIOSerialConfigMode = serialPortMode;
			if(!videoandIOSerialConfigMode){
				return videoandIOSerialConfigMode;
			}
			String dataBits = MRXMLUtils.getValueXpath(elReq, "DataBits",name);
			boolean dataBitsMode = false;
			int data = new Integer(dataBits).intValue();
			if (data >= 5 && data <= 8) {
				dataBitsMode = true;
			}
			videoandIOSerialConfigMode = dataBitsMode;
			if(!videoandIOSerialConfigMode){
				return videoandIOSerialConfigMode;
			}
			String stopBits = MRXMLUtils.getValueXpath(elReq, "StopBits",name);
			boolean stopBitsMode = false;
			int stop = new Integer(stopBits).intValue();
			if (stop >= 1 && stop <= 2) {
				stopBitsMode = true;
			}
			videoandIOSerialConfigMode = stopBitsMode;
			if(!videoandIOSerialConfigMode){
				return videoandIOSerialConfigMode;
			}
			String maximumWaitTime = MRXMLUtils.getValueXpath(elReq, "MaximumWaitTime",name);
			boolean maximumWaitTimeMode = false;
			int maximum = new Integer(maximumWaitTime).intValue();
			if (maximum >= 0 && maximum <= 200) {
				maximumWaitTimeMode = true;
			}
			videoandIOSerialConfigMode = maximumWaitTimeMode;
			if(!videoandIOSerialConfigMode){
				return videoandIOSerialConfigMode;
			}
		    return videoandIOSerialConfigMode;
	  }
	  public Boolean validateConnectionAuthorization( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    Element parentElement = MRXMLUtils.getParentElement(elReq);
		    boolean connectionAuthorizationMode = false;
		    List <?> attrList =  MRXMLUtils.getList(parentElement, name);
		    if(attrList.size() > 0){
				for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
					Element connectionAuthorizationEL = (Element) i.next();
					String network = MRXMLUtils.getValueXpath(connectionAuthorizationEL, "Network",name);
					boolean networkMode = verifyIP(network);
					connectionAuthorizationMode = networkMode;
					if(!connectionAuthorizationMode){
						break;
					}
					String netmask = MRXMLUtils.getValueXpath(connectionAuthorizationEL, "Netmask",name);
					boolean netmaskMode = verifyNetMask(netmask);
					connectionAuthorizationMode = netmaskMode;
					if(!connectionAuthorizationMode){
						break;
					}
					String bandwidth = MRXMLUtils.getValueXpath(connectionAuthorizationEL, "MaxBandwidth", name);
					String maxBandWidth = maxBandwidthValue(bandwidth);
					boolean bandwidthMode = false;
					long band = new Long(maxBandWidth).longValue();
					if (band >= 0 && band <= 524288000) {
						bandwidthMode = true;
					}
					connectionAuthorizationMode = bandwidthMode;
					if(!connectionAuthorizationMode){
						break;
					}
					String burstRatio = MRXMLUtils.getValueXpath(connectionAuthorizationEL, "BurstRatio",name);
					boolean burstRatioMode = false;
					double ratio = new Double(burstRatio).doubleValue();
					if (ratio >= 1 && ratio <= 100) {
						burstRatioMode = true;
					}
					connectionAuthorizationMode = burstRatioMode;
					if(!connectionAuthorizationMode){
						break;
					}
					String burstDuration = MRXMLUtils.getValueXpath(connectionAuthorizationEL, "BurstDuration",name);
					boolean burstDurationMode = false;
					double duration = new Double(burstDuration).doubleValue();
					if (duration >= 0 && duration <= 1000) {
						burstDurationMode = true;
					}
					connectionAuthorizationMode = burstDurationMode;
					if(!connectionAuthorizationMode){
						break;
					}
					String highCompression = MRXMLUtils.getValueXpath(connectionAuthorizationEL, "Compression/High", name);
					boolean highCompressionMode = false;
					int high = new Integer(highCompression).intValue();
					if (high >= 0 && high <= 10) {
						highCompressionMode = true;
					}
					connectionAuthorizationMode = highCompressionMode;
					if(!connectionAuthorizationMode){
						break;
					}
					String lowCompression = MRXMLUtils.getValueXpath(connectionAuthorizationEL, "Compression/Low", name);
					boolean lowCompressionMode = false;
					int low = new Integer(lowCompression).intValue();
					if (low >= 0 && low <= 10) {
						lowCompressionMode = true;
					}
					connectionAuthorizationMode = lowCompressionMode;
					if(!connectionAuthorizationMode){
						break;
					}
				}
		    }
		    return connectionAuthorizationMode;
	  }
	  public Boolean validateSplashParameters( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean splashParametersMode = false;
			String bandwidth = MRXMLUtils.getValueXpath(elReq, "SplashBandwidth", name);
			String maxBandWidth = maxBandwidthValue(bandwidth);
			boolean bandwidthMode = false;
			long band = new Long(maxBandWidth).longValue();
			if (band >= 0 && band <= 524288000) {
				bandwidthMode = true;
			}
			splashParametersMode = bandwidthMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String horizontalResolution = MRXMLUtils.getValueXpath(elReq, "Resolution/Horizontal", name);
			boolean horizontalResolutionMode = false;
			int horizontal = new Integer(horizontalResolution).intValue();
			if (horizontal >= 512 && horizontal <= 3000) {
				horizontalResolutionMode = true;
			}
			splashParametersMode = horizontalResolutionMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String verticalResolution = MRXMLUtils.getValueXpath(elReq, "Resolution/Vertical", name);
			boolean verticalResolutionMode = false;
			int vertical = new Integer(verticalResolution).intValue();
			if (vertical >= 480 && vertical <= 2048) {
				verticalResolutionMode = true;
			}
			splashParametersMode = verticalResolutionMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String refreshRate = MRXMLUtils.getValueXpath(elReq, "RefreshRate", name);
			boolean refreshRateMode = false;
			double refresh = new Double(refreshRate).doubleValue();
			if (refresh >= 30 && refresh <= 250) {
				refreshRateMode = true;
			}
			splashParametersMode = refreshRateMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String pixelClockRate = MRXMLUtils.getValueXpath(elReq, "PixelClockRate", name);
			boolean pixelClockRateMode = false;
			double pixel = new Double(pixelClockRate).doubleValue();
			if (pixel >= 30 && pixel <= 200) {
				pixelClockRateMode = true;
			}
			splashParametersMode = pixelClockRateMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String horizontalFrontPorch = MRXMLUtils.getValueXpath(elReq, "FrontPorch/Horizontal", name);
			boolean horizontalFrontPorchMode = false;
			int horizontalFront = new Integer(horizontalFrontPorch).intValue();
			if (horizontalFront >= 0 && horizontalFront <= 1000) {
				horizontalFrontPorchMode = true;
			}
			splashParametersMode = horizontalFrontPorchMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String verticalFrontPorch = MRXMLUtils.getValueXpath(elReq, "FrontPorch/Vertical", name);
			boolean verticalFrontPorchMode = false;
			int verticalFront = new Integer(verticalFrontPorch).intValue();
			if (verticalFront >= 0 && verticalFront <= 1000) {
				verticalFrontPorchMode = true;
			}
			splashParametersMode = verticalFrontPorchMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String horizontalBackPorch = MRXMLUtils.getValueXpath(elReq, "BackPorch/Horizontal", name);
			boolean horizontalBackPorchMode = false;
			int horizontalBack = new Integer(horizontalBackPorch).intValue();
			if (horizontalBack >= 0 && horizontalBack <= 1000) {
				horizontalBackPorchMode = true;
			}
			splashParametersMode = horizontalBackPorchMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String verticalBackPorch = MRXMLUtils.getValueXpath(elReq, "BackPorch/Vertical", name);
			boolean verticalBackPorchMode = false;
			int verticalBack = new Integer(verticalBackPorch).intValue();
			if (verticalBack >= 0 && verticalBack <= 1000) {
				verticalBackPorchMode = true;
			}
			splashParametersMode = verticalBackPorchMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String horizontalSynchronization = MRXMLUtils.getValueXpath(elReq, "Sync/Horizontal", name);
			boolean horizontalSynchronizationMode = false;
			int horizontalSync = new Integer(horizontalSynchronization).intValue();
			if (horizontalSync >= 1 && horizontalSync <= 1000) {
				horizontalSynchronizationMode = true;
			}
			splashParametersMode = horizontalSynchronizationMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
			String verticalSynchronization = MRXMLUtils.getValueXpath(elReq, "Sync/Vertical", name);
			boolean verticalSynchronizationMode = false;
			int verticalSync = new Integer(verticalSynchronization).intValue();
			if (verticalSync >= 1 && verticalSync <= 1000) {
				verticalSynchronizationMode = true;
			}
			splashParametersMode = verticalSynchronizationMode;
			if(!splashParametersMode){
				return splashParametersMode;
			}
		    return splashParametersMode;
	  }
	  public Boolean validateNetworkParameters( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean networkParametersMode = false;
			String destination = MRXMLUtils.getValueXpath(elReq, "IPAddress",name);
			boolean ipaddressMode = verifyIP(destination);
			networkParametersMode = ipaddressMode;
			if(!networkParametersMode){
				return networkParametersMode;
			}
			String netmask = MRXMLUtils.getValueXpath(elReq, "Netmask",name);
			boolean netmaskMode = verifyNetMask(netmask);
			networkParametersMode = netmaskMode;
			if(!networkParametersMode){
				return networkParametersMode;
			}
		    return networkParametersMode;
	  }
	  public Boolean validateRouteEntry( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    /*<RouteTableConfig Dirty="true">
                <RouteEntry>
                  <RouteID>true</RouteID>
                  <Destination>Destination</Destination>
                  <Netmask>Netmask</Netmask>
                  <Gateway>Gateway</Gateway>
                </RouteEntry> 
                <RouteEntry>
                  <RouteID>true</RouteID>
                  <Destination>Destination</Destination>
                  <Netmask>Netmask</Netmask>
                  <Gateway>Gateway</Gateway>
                </RouteEntry>c
              </RouteTableConfig>	*/	  
		    boolean routeEntryMode = false;
		    Element parentElement = MRXMLUtils.getParentElement(elReq);
		    List <?> attrList =  MRXMLUtils.getList(parentElement, name);
		    if(attrList.size() > 0){
				for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
					Element routeEntryEL = (Element) i.next();
					String destination = MRXMLUtils.getValueXpath(routeEntryEL, "Destination",name);
					boolean ipaddressMode = verifyIP(destination);
					routeEntryMode = ipaddressMode;
					if(!routeEntryMode){
						break;
					}
					String netmask = MRXMLUtils.getValueXpath(routeEntryEL, "Netmask",name);
					boolean netmaskMode = verifyNetMask(netmask);
					routeEntryMode = netmaskMode;
					if(!routeEntryMode){
						break;
					}
					String gateway = MRXMLUtils.getValueXpath(routeEntryEL, "Gateway",name);
					boolean gatewayMode = verifyIP(gateway);
					routeEntryMode = gatewayMode;
					if(!routeEntryMode){
						break;
					}
				}
		    }
			/* //Multiable RouteEntrys
			  List <?> attrList =  MRXMLUtils.getList(elReq, name);
				for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
					Element DestinationEL = (Element) i.next();
					String destination = MRXMLUtils.getValueXpath(DestinationEL, "Destination",name);
				}
			//RouteTableConfig
			String Destination = MRXMLUtils.getValueXpath(elReq, "RouteEntry/Destination",name);
			//RouteEntry
			String Netmask = MRXMLUtils.getValue(elReq, "Netmask",name);*/
		    return routeEntryMode;
	  }
	  public Boolean validateSSHAdminServiceConfig( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean sSHAdminServiceConfigMode = false;
			String keyRegenerationInteval = MRXMLUtils.getValueXpath(elReq, "KeyRegenerationInteval",name);
			boolean keyRegenerationIntevalMode = false;
			long keyRegeneration = new Long(keyRegenerationInteval).longValue();
			if(keyRegeneration >= 0 && keyRegeneration <= 4294967295L){
				keyRegenerationIntevalMode = true; 
			}
			sSHAdminServiceConfigMode = keyRegenerationIntevalMode;
			if(!sSHAdminServiceConfigMode){
				return sSHAdminServiceConfigMode;
			}
			String loginGraceTime = MRXMLUtils.getValueXpath(elReq, "LoginGraceTime",name);
			boolean loginGraceMode = false;
			long loginGrace = new Long(loginGraceTime).longValue();
			if(loginGrace >= 0 && loginGrace <= 4294967295L){
				loginGraceMode = true; 
			}
			sSHAdminServiceConfigMode = loginGraceMode;
			if(!sSHAdminServiceConfigMode){
				return sSHAdminServiceConfigMode;
			}
		    return sSHAdminServiceConfigMode;
	  }
	  public Boolean validateVideoLookupTable( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    Element parentElement = MRXMLUtils.getParentElement(elReq);
		    boolean videoLookupTableMode = false;
		    List <?> attrList =  MRXMLUtils.getList(parentElement, name);
		    if(attrList.size() > 0){
				for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
					Element videoLookupTableEL = (Element) i.next();
					String horizontalResolution = MRXMLUtils.getValueXpath(videoLookupTableEL, "Resolution/Horizontal", name);
					boolean horizontalResolutionMode = false;
					int horizontal = new Integer(horizontalResolution).intValue();
					if (horizontal >= 512 && horizontal <= 3000) {
						horizontalResolutionMode = true;
					}
					videoLookupTableMode = horizontalResolutionMode;
					if(!videoLookupTableMode){
						break;
					}
					String verticalResolution = MRXMLUtils.getValueXpath(videoLookupTableEL, "Resolution/Vertical", name);
					boolean verticalResolutionMode = false;
					int vertical = new Integer(verticalResolution).intValue();
					if (vertical >= 480 && vertical <= 2048) {
						verticalResolutionMode = true;
					}
					videoLookupTableMode = verticalResolutionMode;
					if(!videoLookupTableMode){
						break;
					}
					String refreshRate = MRXMLUtils.getValueXpath(videoLookupTableEL, "RefreshRate", name);
					boolean refreshRateMode = false;
					double refresh = new Double(refreshRate).doubleValue();
					if (refresh >= 30 && refresh <= 250) {
						refreshRateMode = true;
					}
					videoLookupTableMode = refreshRateMode;
					if(!videoLookupTableMode){
						break;
					}
					String pixelClockRate = MRXMLUtils.getValueXpath(videoLookupTableEL, "PixelClockRate", name);
					boolean pixelClockRateMode = false;
					double pixel = new Double(pixelClockRate).doubleValue();
					if (pixel >= 30 && pixel <= 200) {
						pixelClockRateMode = true;
					}
					videoLookupTableMode = pixelClockRateMode;
					if(!videoLookupTableMode){
						break;
					}
					String horizontalFrontPorch = MRXMLUtils.getValueXpath(videoLookupTableEL, "FrontPorch/Horizontal", name);
					boolean horizontalFrontPorchMode = false;
					int horizontalFront = new Integer(horizontalFrontPorch).intValue();
					if (horizontalFront >= 0 && horizontalFront <= 1000) {
						horizontalFrontPorchMode = true;
					}
					videoLookupTableMode = horizontalFrontPorchMode;
					if(!videoLookupTableMode){
						break;
					}
					String verticalFrontPorch = MRXMLUtils.getValueXpath(videoLookupTableEL, "FrontPorch/Vertical", name);
					boolean verticalFrontPorchMode = false;
					int verticalFront = new Integer(verticalFrontPorch).intValue();
					if (verticalFront >= 0 && verticalFront <= 1000) {
						verticalFrontPorchMode = true;
					}
					videoLookupTableMode = verticalFrontPorchMode;
					if(!videoLookupTableMode){
						break;
					}
					String horizontalBackPorch = MRXMLUtils.getValueXpath(videoLookupTableEL, "BackPorch/Horizontal", name);
					boolean horizontalBackPorchMode = false;
					int horizontalBack = new Integer(horizontalBackPorch).intValue();
					if (horizontalBack >= 0 && horizontalBack <= 1000) {
						horizontalBackPorchMode = true;
					}
					videoLookupTableMode = horizontalBackPorchMode;
					if(!videoLookupTableMode){
						break;
					}
					String verticalBackPorch = MRXMLUtils.getValueXpath(videoLookupTableEL, "BackPorch/Vertical", name);
					boolean verticalBackPorchMode = false;
					int verticalBack = new Integer(verticalBackPorch).intValue();
					if (verticalBack >= 0 && verticalBack <= 1000) {
						verticalBackPorchMode = true;
					}
					videoLookupTableMode = verticalBackPorchMode;
					if(!videoLookupTableMode){
						break;
					}
					String horizontalSynchronization = MRXMLUtils.getValueXpath(videoLookupTableEL, "Sync/Horizontal", name);
					boolean horizontalSynchronizationMode = false;
					int horizontalSync = new Integer(horizontalSynchronization).intValue();
					if (horizontalSync >= 1 && horizontalSync <= 1000) {
						horizontalSynchronizationMode = true;
					}
					videoLookupTableMode = horizontalSynchronizationMode;
					if(!videoLookupTableMode){
						break;
					}
					String verticalSynchronization = MRXMLUtils.getValueXpath(videoLookupTableEL, "Sync/Vertical", name);
					boolean verticalSynchronizationMode = false;
					int verticalSync = new Integer(verticalSynchronization).intValue();
					if (verticalSync >= 1 && verticalSync <= 1000) {
						verticalSynchronizationMode = true;
					}
					videoLookupTableMode = verticalSynchronizationMode;
					if(!videoLookupTableMode){
						break;
					}
					String coarseNoise = MRXMLUtils.getValueXpath(videoLookupTableEL, "NoiseElimination/Coarse",name);
					boolean coarseNoiseMode = false;
					int coarse = new Integer(coarseNoise).intValue();
					if (coarse >= 1 && coarse <= 256) {
						coarseNoiseMode = true;
					}
					videoLookupTableMode = coarseNoiseMode;
					if(!videoLookupTableMode){
						break;
					}
					String fineNoise = MRXMLUtils.getValueXpath(videoLookupTableEL, "NoiseElimination/Fine",name);
					boolean fineNoiseMode = false;
					int fine = new Integer(fineNoise).intValue();
					if (fine >= 1 && fine <= 2048) {
						fineNoiseMode = true;
					}
					videoLookupTableMode = fineNoiseMode;
					if(!videoLookupTableMode){
						break;
					}
					String horizontalPosition = MRXMLUtils.getValueXpath(videoLookupTableEL, "Position/Horizontal",name);
					boolean horizontalPositionMode = false;
					int horizontalPos = new Integer(horizontalPosition).intValue();
					if (horizontalPos >= -135 && horizontalPos <= 375) {
						horizontalPositionMode = true;
					}
					videoLookupTableMode = horizontalPositionMode;
					if(!videoLookupTableMode){
						break;
					}
					String verticalPosition = MRXMLUtils.getValueXpath(videoLookupTableEL, "Position/Vertical",name);
					boolean verticalPositionMode = false;
					int verticalPos = new Integer(verticalPosition).intValue();
					if (verticalPos >= -25 && verticalPos <= 50) {
						verticalPositionMode = true;
					}
					videoLookupTableMode = verticalPositionMode;
					if(!videoLookupTableMode){
						break;
					}
					String verticalMargin = MRXMLUtils.getValueXpath(videoLookupTableEL, "VerticalMargin", name);
					boolean verticalMarginMode = false;
					int verticalMar = new Integer(verticalMargin).intValue();
					if (verticalMar >= 0 && verticalMar <= 2) {
						verticalMarginMode = true;
					}
					videoLookupTableMode = verticalMarginMode;
					if(!videoLookupTableMode){
						break;
					}
					String samplingPhase = MRXMLUtils.getValueXpath(videoLookupTableEL, "Phase",name);
					boolean samplingPhaseMode = false;
					int sampling = new Integer(samplingPhase).intValue();
					if (sampling >= -16 && sampling <= 15) {
						samplingPhaseMode = true;
					}
					videoLookupTableMode = samplingPhaseMode;
					if(!videoLookupTableMode){
						break;
					}
				}
		    }
		    return videoLookupTableMode;
	  }
	  public Boolean validateAutoPhaseAdvancedVideoConfig( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean advancedVideoConfigELMode = false;
			String coarseNoise = MRXMLUtils.getValueXpath(elReq, "NoiseOptimization/Coarse",name);
			boolean coarseNoiseMode = false;
			int coarse = new Integer(coarseNoise).intValue();
			if (coarse >= 1 && coarse <= 256) {
				coarseNoiseMode = true;
			}
			advancedVideoConfigELMode = coarseNoiseMode;
			if(!advancedVideoConfigELMode){
				return advancedVideoConfigELMode;
			}
			String fineNoise = MRXMLUtils.getValueXpath(elReq, "NoiseOptimization/Fine",name);
			boolean fineNoiseMode = false;
			int fine = new Integer(fineNoise).intValue();
			if (fine >= 1 && fine <= 2048) {
				fineNoiseMode = true;
			}
			advancedVideoConfigELMode = fineNoiseMode;
			if(!advancedVideoConfigELMode){
				return advancedVideoConfigELMode;
			}
		    return advancedVideoConfigELMode;
	  }
	  public Boolean validateV2DStreamInstanceConfig( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		    String name = elReq.getName();
		    boolean v2DStreamInstanceConfigMode = false;
			String destination = MRXMLUtils.getValueXpath(elReq, "TxIpAddress",name);
			boolean ipaddressMode = verifyIP(destination);
			v2DStreamInstanceConfigMode = ipaddressMode;
			if(!v2DStreamInstanceConfigMode){
				return v2DStreamInstanceConfigMode;
			}
			String multicastIpAddress = MRXMLUtils.getValueXpath(elReq, "MulticastIpAddress",name);
			boolean multicastIpAddressMode = verifyIP(multicastIpAddress);
			v2DStreamInstanceConfigMode = multicastIpAddressMode;
			if(!v2DStreamInstanceConfigMode){
				return v2DStreamInstanceConfigMode;
			}
			String rxAVIpAddress = MRXMLUtils.getValueXpath(elReq, "RxAVIpAddress", name);
			boolean rxAVIpAddressMode = verifyIP(rxAVIpAddress);
			v2DStreamInstanceConfigMode = rxAVIpAddressMode;
			if(!v2DStreamInstanceConfigMode){
				return v2DStreamInstanceConfigMode;
			}
			String port = MRXMLUtils.getValueXpath(elReq, "RxAVPort", name);
			boolean portMode = false;
			int portno = new Integer(port).intValue();
			if (portno >= 1025 && portno <= 65500) {
				portMode = true;
			}
			v2DStreamInstanceConfigMode = portMode;
			if(!v2DStreamInstanceConfigMode){
				return v2DStreamInstanceConfigMode;
			}
			String ctrlPort = MRXMLUtils.getValueXpath(elReq, "TxCtrlPort", name);
			boolean ctrlPortMode = false;
			int ctrlPortno = new Integer(ctrlPort).intValue();
			if (ctrlPortno >= 1025 && ctrlPortno <= 65500) {
				ctrlPortMode = true;
			}
			v2DStreamInstanceConfigMode = ctrlPortMode;
			if(!v2DStreamInstanceConfigMode){
				return v2DStreamInstanceConfigMode;
			}
		    return v2DStreamInstanceConfigMode;
	  }
	  public Boolean validateIPAddress( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		  String name = elReq.getName();
		  String ipaddress = MRXMLUtils.getValue2(elReq, name);
		  boolean ipaddressMode = verifyIP(ipaddress);
		  return ipaddressMode;
	  }
	  public Boolean validateNetMask( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		  String name = elReq.getName();
		  String netmask = MRXMLUtils.getValue2(elReq, name);
		  boolean netmaskMode = verifyNetMask(netmask);
		  return netmaskMode;
	  }
	  
      public Boolean validateMaxBandwidth( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			boolean bandwidthMode = false;
			String name = elReq.getName();
			String bandwidth = MRXMLUtils.getValue2(elReq, name);
			String maxBandWidth = maxBandwidthValue(bandwidth);
			long band = new Long(maxBandWidth).longValue();
			if (band >= 0 && band <= 524288000) {
				bandwidthMode = true;
			}
			return bandwidthMode;
	  }

 	  public Boolean validateBurstRatio( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {			
			String name = elReq.getName();
			String burstRatio = MRXMLUtils.getValue2(elReq, name);
			boolean burstRatioMode = false;
			double ratio = new Double(burstRatio).doubleValue();
			if (ratio >= 1 && ratio <= 100) {
				burstRatioMode = true;
			}
			return burstRatioMode;
	  }

 	  public Boolean validateBurstDuration( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {			
			String name = elReq.getName();
			String burstDuration = MRXMLUtils.getValue2(elReq, name);
			boolean burstDurationMode = false;
			double duration = new Double(burstDuration).doubleValue();
			if (duration >= 0 && duration <= 1000) {
				burstDurationMode = true;
			}
			return burstDurationMode;
      }

	  public Boolean validateHighCompression( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {			
			String name = elReq.getName();
			String highCompression = MRXMLUtils.getValue2(elReq, name);
			boolean highCompressionMode = false;
			int high = new Integer(highCompression).intValue();
			if (high >= 0 && high <= 10) {
				highCompressionMode = true;
			}
			return highCompressionMode;
	  }

	  public Boolean validateLowCompression( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String lowCompression = MRXMLUtils.getValue2(elReq, name);
			boolean lowCompressionMode = false;
			int low = new Integer(lowCompression).intValue();
			if (low >= 0 && low <= 10) {
				lowCompressionMode = true;
			}
			return lowCompressionMode;
	  }

	  public Boolean validateMinRefresh( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
    		String name = elReq.getName();
			String minRefresh = MRXMLUtils.getValue2(elReq, name);
			boolean minRefreshMode = false;
			int min = new Integer(minRefresh).intValue();
			if (min >= 0 && min <= 255) {
				minRefreshMode = true;
			}
			return minRefreshMode;
	  }

	  public Boolean validateMaxRefresh( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String maxRefresh = MRXMLUtils.getValue2(elReq, name);
			boolean maxRefreshMode = false;
			int max = new Integer(maxRefresh).intValue();
			if (max >= 0 && max <= 255) {
				maxRefreshMode = true;
			}
			return maxRefreshMode;
	  }

      public Boolean validateMinSlice( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String minSlice = MRXMLUtils.getValue2(elReq, name);
			boolean minSliceMode = false;
			int min = new Integer(minSlice).intValue();
			if (min >= 0 && min <= 255) {
				minSliceMode = true;
			}
			return minSliceMode;
	  }

	  public Boolean validateMaxSlice( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String maxSlice = MRXMLUtils.getValue2(elReq, name);
			boolean maxSliceMode = false;
			int max = new Integer(maxSlice).intValue();
			if (max >= 0 && max <= 255) {
				maxSliceMode = true;
			}
			return maxSliceMode;
	  }

      public Boolean validateBlockThreshold( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String blockThreshold = MRXMLUtils.getValue2(elReq, name);
			boolean blockThresholdMode = false;
			int block = new Integer(blockThreshold).intValue();
			if (block >= 1 && block <= 32) {
				blockThresholdMode = true;
			}
			return blockThresholdMode;
	  }

	  public Boolean validateKMIdleTime( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				 	   MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String kMIdleTime = MRXMLUtils.getValue2(elReq, name);
			boolean kMIdleTimeMode = false;
			int km = new Integer(kMIdleTime).intValue();
			if (km >= 3 && km <= 30) {
				kMIdleTimeMode = true;
			}
			return kMIdleTimeMode;
	  }

	  public Boolean validateSamplingPhase( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String samplingPhase = MRXMLUtils.getValue2(elReq, name);
			boolean samplingPhaseMode = false;
			int sampling = new Integer(samplingPhase).intValue();
			if (sampling >= -16 && sampling <= 15) {
				samplingPhaseMode = true;
			}
			return samplingPhaseMode;
	  }

	  public Boolean validateCoarseNoise( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String coarseNoise = MRXMLUtils.getValue2(elReq, name);
			boolean coarseNoiseMode = false;
			int coarse = new Integer(coarseNoise).intValue();
			if (coarse >= 1 && coarse <= 256) {
				coarseNoiseMode = true;
			}
			return coarseNoiseMode;
	  }

	  public Boolean validateFineNoise( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String fineNoise = MRXMLUtils.getValue2(elReq, name);
			boolean fineNoiseMode = false;
			int fine = new Integer(fineNoise).intValue();
			if (fine >= 1 && fine <= 2048) {
				fineNoiseMode = true;
			}
			return fineNoiseMode;
	  }

	  public Boolean validateHorizontalPosition( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String horizontalPosition = MRXMLUtils.getValue2(elReq, name);
			boolean horizontalPositionMode = false;
			int horizontal = new Integer(horizontalPosition).intValue();
			if (horizontal >= -135 && horizontal <= 375) {
				horizontalPositionMode = true;
			}
			return horizontalPositionMode;
	  }

	  public Boolean validateVerticalPosition( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String verticalPosition = MRXMLUtils.getValue2(elReq, name);
			boolean verticalPositionMode = false;
			int vertical = new Integer(verticalPosition).intValue();
			if (vertical >= -25 && vertical <= 50) {
				verticalPositionMode = true;
			}
			return verticalPositionMode;
	  }

	  public Boolean validatePumpCurr( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String pumpCurr = MRXMLUtils.getValue2(elReq, name);
			boolean pumpCurrMode = false;
			int pump = new Integer(pumpCurr).intValue();
			if (pump >= 50 && pump <= 1500) {
				pumpCurrMode = true;
			}
			return pumpCurrMode;
	  }

	  public Boolean validateSerialPort( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String serialPort = MRXMLUtils.getValue2(elReq, name);
			boolean serialPortMode = false;
			int serial = new Integer(serialPort).intValue();
			if (serial >= 0 && serial <= 1) {
				serialPortMode = true;
			}
			return serialPortMode;
	  }

	  public Boolean validateDataBits( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String dataBits = MRXMLUtils.getValue2(elReq, name);
			boolean dataBitsMode = false;
			int data = new Integer(dataBits).intValue();
			if (data >= 5 && data <= 8) {
				dataBitsMode = true;
			}
			return dataBitsMode;
	  }

      public Boolean validateStopBits( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
	    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String stopBits = MRXMLUtils.getValue2(elReq, name);
			boolean stopBitsMode = false;
			int stop = new Integer(stopBits).intValue();
			if (stop >= 1 && stop <= 2) {
				stopBitsMode = true;
			}
			return stopBitsMode;
	  }

	  public Boolean validateMaximumWaitTime( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String maximumWaitTime = MRXMLUtils.getValue2(elReq, name);
			boolean maximumWaitTimeMode = false;
			int maximum = new Integer(maximumWaitTime).intValue();
			if (maximum >= 0 && maximum <= 200) {
				maximumWaitTimeMode = true;
			}
			return maximumWaitTimeMode;
	  }

	  public Boolean validateHorizontalResolution( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String horizontalResolution = MRXMLUtils.getValue2(elReq, name);
			boolean horizontalResolutionMode = false;
			int horizontal = new Integer(horizontalResolution).intValue();
			if (horizontal >= 512 && horizontal <= 3000) {
				horizontalResolutionMode = true;
			}
			return horizontalResolutionMode;
	  }

	  public Boolean validateVerticalResolution( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String verticalResolution = MRXMLUtils.getValue2(elReq, name);
			boolean verticalResolutionMode = false;
			int vertical = new Integer(verticalResolution).intValue();
			if (vertical >= 480 && vertical <= 2048) {
				verticalResolutionMode = true;
			}
			return verticalResolutionMode;
	  }

	  public Boolean validateRefreshRate( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String refreshRate = MRXMLUtils.getValue2(elReq, name);
			boolean refreshRateMode = false;
			double refresh = new Double(refreshRate).doubleValue();
			if (refresh >= 30 && refresh <= 250) {
				refreshRateMode = true;
			}
			return refreshRateMode;
	  }

	  public Boolean validatePixelClockRate( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String pixelClockRate = MRXMLUtils.getValue2(elReq, name);
			boolean pixelClockRateMode = false;
			double pixel = new Double(pixelClockRate).doubleValue();
			if (pixel >= 30 && pixel <= 200) {
				pixelClockRateMode = true;
			}
			return pixelClockRateMode;
	  }

	  public Boolean validateHorizontalFrontPorch( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String horizontalFrontPorch = MRXMLUtils.getValue2(elReq, name);
			boolean horizontalFrontPorchMode = false;
			int horizontal = new Integer(horizontalFrontPorch).intValue();
			if (horizontal >= 0 && horizontal <= 1000) {
				horizontalFrontPorchMode = true;
			}
			return horizontalFrontPorchMode;
	  }

	  public Boolean validateVerticalFrontPorch( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String verticalFrontPorch = MRXMLUtils.getValue2(elReq, name);
			boolean verticalFrontPorchMode = false;
			int vertical = new Integer(verticalFrontPorch).intValue();
			if (vertical >= 0 && vertical <= 1000) {
				verticalFrontPorchMode = true;
			}
			return verticalFrontPorchMode;
	  }

	  public Boolean validateHorizontalBackPorch( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String horizontalBackPorch = MRXMLUtils.getValue2(elReq, name);
			boolean horizontalBackPorchMode = false;
			int horizontal = new Integer(horizontalBackPorch).intValue();
			if (horizontal >= 0 && horizontal <= 1000) {
				horizontalBackPorchMode = true;
			}
			return horizontalBackPorchMode;
	  }

	  public Boolean validateVerticalBackPorch( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String verticalBackPorch = MRXMLUtils.getValue2(elReq, name);
			boolean verticalFrontPorchMode = false;
			int vertical = new Integer(verticalBackPorch).intValue();
			if (vertical >= 0 && vertical <= 1000) {
				verticalFrontPorchMode = true;
			}
			return verticalFrontPorchMode;
	  }

	  public Boolean validateHorizontalSynchronization( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String horizontalSynchronization = MRXMLUtils.getValue2(elReq, name);
			boolean horizontalSynchronizationMode = false;
			int horizontal = new Integer(horizontalSynchronization).intValue();
			if (horizontal >= 1 && horizontal <= 1000) {
				horizontalSynchronizationMode = true;
			}
			return horizontalSynchronizationMode;
	  }

	  public Boolean validateVerticalSynchronization( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String verticalSynchronization = MRXMLUtils.getValue2(elReq, name);
			boolean verticalSynchronizationMode = false;
			int vertical = new Integer(verticalSynchronization).intValue();
			if (vertical >= 1 && vertical <= 1000) {
				verticalSynchronizationMode = true;
			}
			return verticalSynchronizationMode;
	  }

	  public Boolean validateKeyRegenerationInteval( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String keyRegenerationInteval = MRXMLUtils.getValue2(elReq, name);
			boolean keyRegenerationIntevalMode = false;
			long keyRegeneration = new Long(keyRegenerationInteval).longValue();
			if(keyRegeneration >= 0 && keyRegeneration <= 4294967295L){
				keyRegenerationIntevalMode = true; 
			}
			return keyRegenerationIntevalMode;
	  }

	  public Boolean validateLoginGraceTime( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String loginGraceTime = MRXMLUtils.getValue2(elReq, name);
			boolean loginGraceMode = false;
			long loginGrace = new Long(loginGraceTime).longValue();
			if(loginGrace >= 0 && loginGrace <= 4294967295L){
				loginGraceMode = true; 
			}
			return loginGraceMode;
	  }

	  public Boolean validateVerticalMargin( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String verticalMargin = MRXMLUtils.getValue2(elReq, name);
			boolean verticalMarginMode = false;
			int vertical = new Integer(verticalMargin).intValue();
			if (vertical >= 0 && vertical <= 2) {
				verticalMarginMode = true;
			}
			return verticalMarginMode;
	  }

	  public Boolean validatePort( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String port = MRXMLUtils.getValue2(elReq, name);
			boolean portMode = false;
			int portno = new Integer(port).intValue();
			if (portno >= 1025 && portno <= 65500) {
				portMode = true;
			}
			return portMode;
	  }

	  public Boolean validatePixelAdjust( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
    				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String pixelAdjust = MRXMLUtils.getValue2(elReq, name);
			boolean pixelAdjustMode = false;
			int pixel = new Integer(pixelAdjust).intValue();
			if (pixel >= -25 && pixel <= 25) {
				pixelAdjustMode = true;
			}
			return pixelAdjustMode;
	  }

	  public Boolean validatePhaseAdjust( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String phaseAdjust = MRXMLUtils.getValue2(elReq, name);
			boolean phaseAdjustMode = false;
			int phase = new Integer(phaseAdjust).intValue();
			if (phase >= 0 && phase <= 32) {
				phaseAdjustMode = true;
			}
			return phaseAdjustMode;
	  }

	  public Boolean validateNameLength( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
					MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
			String name = elReq.getName();
			String value = MRXMLUtils.getValue2(elReq, name);
			boolean nameMode = false;
			int length = value.length();
			if (length <= 255) {
				nameMode = true;
			}
			return nameMode;
	  }
	  
	  
	  public Boolean validateIntRange( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		  //String allowZero=MRXMLUtils.getValue(elCustomValidate,"allowZero", "").trim();
		  //String minValue=MRXMLUtils.getValue(elCustomValidate, "minValue", "").trim();
		  //String maxValue=MRXMLUtils.getValue(elCustomValidate, "maxValue", "").trim();
		  //try validating the elReq with custom rule here. 
		  
		  return Boolean.valueOf(true);
	  }
	  public Boolean validateIntRange2( String action, String serviceName,  String dbObjectName,   Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		  //String allowZero=MRXMLUtils.getValue(elCustomValidate, "allowZero", "").trim();
		  //String minValue=MRXMLUtils.getValue(elCustomValidate, "minValue", "").trim();
		  //String maxValue=MRXMLUtils.getValue(elCustomValidate, "maxValue", "").trim();
		  //String compareWithElXpath = MRXMLUtils.getValue(elCustomValidate, "shouldbeDifferntFrom", "").trim();
		  //try validating the elReq with custom rule here. 
		  //Element  compareWithEl = MRXMLUtils.getElementXpath(elReq.getDocument(), compareWithElXpath);
		  
		  return Boolean.valueOf(true);
	  }
	  
	  public boolean verifyIP(String ipAddress){
	        boolean ipAddressMode = false;
	        if(ipAddress.equals("0.0.0.0")){
	            ipAddressMode = true;
	            return ipAddressMode;
	        }
	        if(ipAddress.equals("255.255.255.255")){
	            ipAddressMode = false;
	            return ipAddressMode;
	        }
	        StringTokenizer st = new StringTokenizer(ipAddress.trim(), ".");
	        if(st.countTokens()==4){
	            try{
	                Integer firstOne = Integer.valueOf(st.nextToken());
	                int firstValue = firstOne.intValue();
	                if(firstValue < 0){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else if(firstValue > 255){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = true;
	                }
	                Integer secondOne = Integer.valueOf(st.nextToken());
	                int secondValue = secondOne.intValue();
	                if(secondValue < 0){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else if(secondValue > 255){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = true;
	                }
	                Integer thirdOne = Integer.valueOf(st.nextToken());
	                int thirdValue = thirdOne.intValue();
	                if(thirdValue < 0){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else if(thirdValue > 255){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = true;
	                }
	                Integer fourthOne = Integer.valueOf(st.nextToken());
	                int fourthValue = fourthOne.intValue();
	                if(fourthValue < 0){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else if(fourthValue > 255){
	                    ipAddressMode = false;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = true;
	                }
	            }catch(NumberFormatException e){
	                e.printStackTrace();
	                ipAddressMode = false;
	            }
	        }else{
	            ipAddressMode = false;
	        }
	        return ipAddressMode;
	    }
	  public boolean verifyNetMask(String ipAddress){
	    	boolean ipAddressMode = false;
	    	if(ipAddress.equals("0.0.0.0")){
	    		ipAddressMode = true;
	            return ipAddressMode;
	    	}
	    	StringTokenizer st = new StringTokenizer(ipAddress.trim(), ".");
	        if(st.countTokens()==4){
	            try{
	                Integer firstOne = Integer.valueOf(st.nextToken());
	                int firstValue = firstOne.intValue();
	                if(firstValue == 128 || firstValue == 192 || firstValue == 224 || firstValue == 240 || firstValue == 248 || firstValue == 252 || firstValue == 254 || firstValue == 255){
	                    ipAddressMode = true;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = false;
	                }
	                Integer secondOne = Integer.valueOf(st.nextToken());
	                int secondValue = secondOne.intValue();
	                if(secondValue == 0 || secondValue == 128 || secondValue == 192 || secondValue == 224 || secondValue == 240 || secondValue == 248 || secondValue == 252 || secondValue == 254 || secondValue == 255){
	                    ipAddressMode = true;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = false;
	                }
	                Integer thirdOne = Integer.valueOf(st.nextToken());
	                int thirdValue = thirdOne.intValue();
	                if(thirdValue == 0 || thirdValue == 128 || thirdValue == 192 || thirdValue == 224 || thirdValue == 240 || thirdValue == 248 || thirdValue == 252 || thirdValue == 254 || thirdValue == 255){
	                    ipAddressMode = true;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = false;
	                }
	                Integer fourthOne = Integer.valueOf(st.nextToken());
	                int fourthValue = fourthOne.intValue();
	                if(fourthValue == 0 || fourthValue == 128 || fourthValue == 192 || fourthValue == 224 || fourthValue == 240 || fourthValue == 248 || fourthValue == 252 || fourthValue == 254 || fourthValue == 255){
	                    ipAddressMode = true;
	                    return ipAddressMode;
	                }else{
	                    ipAddressMode = false;
	                }
	            }catch(NumberFormatException e){
	                e.printStackTrace();
	                ipAddressMode = false;
	            }
	        }else{
	            ipAddressMode = false;
	        }
	        return ipAddressMode;
	    }
	  public String maxBandwidthValue(String maxBandwidthValue){
	        maxBandwidthValue = maxBandwidthValue.toUpperCase().trim();
	        boolean maxBandwidth = maxBandwidthValue.endsWith("K");
	        if(maxBandwidth){
	            try{
	                StringTokenizer st = new StringTokenizer(maxBandwidthValue, "K");
	                double number= new Double(st.nextToken()).doubleValue();
	                double kbpsnumber = number*1024;
	                DecimalFormat form = new DecimalFormat();
	                form.setMaximumFractionDigits(0);
	                form.setGroupingUsed(false);
	                String maxBandWidth = form.format(kbpsnumber);
	                maxBandwidthValue = maxBandWidth;
	            }catch(Exception e){
	                e.printStackTrace();
	            }
	        }
	        boolean maxBandwidthVal = maxBandwidthValue.endsWith("M");
	        if(maxBandwidthVal){
	            try{
	                StringTokenizer st1 = new StringTokenizer(maxBandwidthValue, "M");
	                double number= new Double(st1.nextToken()).doubleValue();
	                double mbpsnumber = number*1024*1024;
	                DecimalFormat form = new DecimalFormat();
	                form.setMaximumFractionDigits(0);
	                form.setGroupingUsed(false);
	                String maxBandWidth = form.format(mbpsnumber);
	                maxBandwidthValue = maxBandWidth;
	            }catch(Exception e){
	                e.printStackTrace();
	            }
	        }
	        return maxBandwidthValue;
	    }
	  
	  public Boolean deleteDevice( String action,  String serviceName,  String dbObjectName,  Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		String name = elReq.getName();
		boolean verticalPositionMode = false;
		try { 
			if(name.trim().equals("DeleteRequest")){
				//get read xml 
				Element readXpathEL = MRXMLUtils.getElementXpath(elCustomValidate, "readDBXml_foreignKeyNID/getXmlFromXpath");
				String readXpath = MRXMLUtils.getValue2(readXpathEL, "");
				int iCopyChildsToDepth =  Utils.intValue(MRXMLUtils.getAttributeValue(readXpathEL, "iCopyChildsToDepth", "1"),1);
				MRConfig mrConfig = MRConfig.getInstance(serviceName, dbObjectName, log);
				Document docReadXml = null;
				docReadXml = mrConfig.getXmlRepresentation(log).getElementFromXpath( iCopyChildsToDepth, docReadXml, readXpath, log);
				if(docReadXml==null) {
					return true;
				}
				//read xml 
				Element newOperatioNData = (Element)docReadXml.getRootElement().clone();
				
				//create GetRequest
				Element actionEL_new = (Element)elReq.clone();
				actionEL_new.setName("GetRequest");
				
				//remove OperationData 
				Element oldOperatioNData = MRXMLUtils.getElement(actionEL_new, "OperationData");
				if(oldOperatioNData !=null)
					oldOperatioNData.detach();

				//add new OperationData 
				MRXMLUtils.addElement(actionEL_new, newOperatioNData);

				//call doGet
				Document resDoc = MRXMLUtils.getDocument();
				Element rootRes = MRXMLUtils.addElement(resDoc, "Handler");
				MRXMLUtils.addElement(rootRes, "ObjectType", dbObjectName);
				MRDBHelper amUtil = new MRDBHelper();
				amUtil.doInit(serviceName, dbObjectName, resDoc, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
				amUtil.doGet(actionEL_new, "GetResponse", dbObjectName, "");
				
				List<?> list = MRXMLUtils.getList(elCustomValidate,"foreignKeyNID");
				for (Iterator<?> i = list.iterator(); i.hasNext();) {
					Element foreignKeyNIDEL = (Element) i.next();
					String xpath = MRXMLUtils.getAttributeValue(foreignKeyNIDEL, "xpath", "");
					String doaction = MRXMLUtils.getAttributeValue(foreignKeyNIDEL, "action", "");
					String readValue = MRXMLUtils.getValueXpath(rootRes , xpath,"");
					if(doaction.equalsIgnoreCase("delete")){
						MRXMLDBUtils.deleteElement(request, log, schema, readValue);
					}
				}
			}
		} catch(Exception ex) {
			
		}
		return verticalPositionMode;
		
		
	  }
	    ///////////////
      
      public Boolean validateDuplicateAsset_add(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
          String value = MRXMLUtils.getValue2(elReq, "").trim();
          String[] arrWhereXpath = { "//Title ='" + value + "'" };
          return validateDuplicateDB(arrWhereXpath, action, serviceName, dbObjectName, elReq, elCustomValidate, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
      }

      public Boolean validateDuplicateUser_add(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
          String value = MRXMLUtils.getValue2(elReq, "").trim();
          String[] arrWhereXpath = { "//Username ='" + value + "'" };
          return validateDuplicateDB(arrWhereXpath, action, serviceName, dbObjectName, elReq, elCustomValidate, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
      }
      
      public Boolean validateDuplicateTitle_add(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
          String value = MRXMLUtils.getValue2(elReq, "").trim();
          boolean elCustomValidateCaseSensitive = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(elCustomValidate, "caseSensitive","").trim(), true);
          String whereTitile="//Title ='" + value + "'" ;
          if(!elCustomValidateCaseSensitive){
              whereTitile="upper-case(//Title) ='" + value.toUpperCase() + "'" ;
          }
          String[] arrWhereXpath = { whereTitile};
          return validateDuplicateDB(arrWhereXpath, action, serviceName, dbObjectName, elReq, elCustomValidate, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
      }       
      
      public Boolean validateDuplicateTitle_replace(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
          String value = MRXMLUtils.getValue2(elReq, "").trim();
          String NID = MRXMLUtils.getAttributeValueXpath(elReq, "//NID","").trim();
          String whereClause =  MRXMLUtils.getValueXpath(elReq, "//WhereXPath","").trim();
          if(NID.equals("") && whereClause.equals("")){
              //todo: looks like bulk update to all objects: should never happen
              return true;
          }
          ArrayList<String> arrWhereXpathNot = new ArrayList<String>();
          arrWhereXpathNot.add("@NID='"+NID+"'");
          arrWhereXpathNot.add(whereClause);
          String not_xpath = IpvsDBUtils.concatWhereXpath(arrWhereXpathNot);
          boolean elCustomValidateCaseSensitive = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(elCustomValidate, "caseSensitive","").trim(), true);
          String whereTitile="//Title ='" + value + "'" ;
          if(!elCustomValidateCaseSensitive){
              whereTitile="upper-case(//Title) ='" + value.toUpperCase() + "'" ;
          }
          String[] arrWhereXpath = { whereTitile," not ("+not_xpath+" ) "};
          return validateDuplicateDB(arrWhereXpath, action, serviceName, dbObjectName, elReq, elCustomValidate, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
      }
      
      public Boolean validateDuplicatePortTitleReplace(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	  Element titleEl = MRXMLUtils.getElementXpath(elReq, "//Title");
    	  String value = MRXMLUtils.getValue2(titleEl, "").trim();
          String NID = MRXMLUtils.getAttributeValueXpath(elReq, "./@NID","").trim();
          if(NID.equals("")){
              //todo: looks like bulk update to all objects: should never happen
              return true;
          }
          ArrayList<String> arrWhereXpathNot = new ArrayList<String>();
          arrWhereXpathNot.add("@NID='"+NID+"'");
          String not_xpath = IpvsDBUtils.concatWhereXpath(arrWhereXpathNot);
          boolean elCustomValidateCaseSensitive = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(elCustomValidate, "caseSensitive","").trim(), true);
          String whereTitile="//Title ='" + value + "'" ;
          if(!elCustomValidateCaseSensitive){
              whereTitile="upper-case(//Title) ='" + value.toUpperCase() + "'" ;
          }
          String[] arrWhereXpath = { whereTitile," not ("+not_xpath+" ) "};
          return validateDuplicateDB(arrWhereXpath, action, serviceName, dbObjectName, titleEl, elCustomValidate, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
      }
      public Boolean validateDuplicatePortTitle_replace(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	  
    	 List<String> portObjectList = new ArrayList<String>();
    	 portObjectList.add(AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE);
    	 portObjectList.add(AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE);
    	 portObjectList.add(AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE);
    	 for(String portObject:portObjectList)
    		 validateDuplicatePortTitleReplace(action, serviceName, portObject, elReq, elCustomValidate, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    	 return true;
      }
      
      private Boolean validateDuplicateDB(String[] arrWhereXpath, String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
          String name = elReq.getName();
          boolean elCustomValidateCaseSensitive = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(elCustomValidate, "caseSensitive","").trim(), true);
          boolean elCustomValidateallowSpaces = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(elCustomValidate, "allowSpaces","").trim(), false);
          
          String value = MRXMLUtils.getValue2(elReq, "").trim();
          if (value.trim().equals("")) {
              log.addInfo(" " + dbObjectName + ": Please provide value for '" + name + "' ", MRLog.ERROR, MRLog.ERROR);
              throw new MRException(AMExceptionCodes.VALID_ERROR_EMPTY_NAME, MRXMLUtils.generateErrorXML("value", value));
          }
          if(elCustomValidateCaseSensitive){
              if (!value.trim().toLowerCase().equals(value.trim())) {
                  MRLog.println("elCustomValidate "+elCustomValidate.asXML()  +" "+ MRXMLUtils.getAttributeValue(elCustomValidate, "caseSensitive",""));
                  log.addInfo(" " + dbObjectName + ": Invalid " + name + "='" + value + "', Capital letters are not allowed.", MRLog.ERROR, MRLog.ERROR);
                  throw new MRException(AMExceptionCodes.VALID_ERROR_CAPITAL_LETTERS_IN_NAME, MRXMLUtils.generateErrorXML("value", value));
              }
          }
          if(!elCustomValidateallowSpaces){
        	  ValidationUtils.containsWhiteSpaces(value, dbObjectName, log);
          }
          MRConfig mrConfig = MRConfig.getInstance(dbObjectName, log);
          String parentNID = schema.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
          Element result = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, schema, mrConfig.getParentNIDKey(log), dbObjectName, parentNID, null, arrWhereXpath, null, null, null, null);
          if (result != null) {
              log.addInfo(" " + dbObjectName + ": Duplicate " + name + "='" + value + "'. Please provide different '" + name + "' ", MRLog.ERROR, MRLog.ERROR);
              throw new MRException(AMExceptionCodes.VALID_ERROR_DUPLICATE, MRXMLUtils.generateErrorXML("value", value));
          }
          return true;
      }            
      
      public Boolean validateSessionNameForSpecialCharacters(String action, String serviceName, String dbObjectName, Element elReq, 
 			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
 			 ServiceAgentFactoryI serviceAgentFactory) throws Exception { 		 
 		 
    	 String title = MRXMLUtils.getValueXpath(elReq, "//Title", "").trim();	 		 			   	 
 		 ValidationUtils.containsSpecialCharacters(title); 	   			
 	   	 
 		 return true;	   	 
      }
      
      public Boolean validateTitleForSpecialCharacters(String action, String serviceName, String dbObjectName, Element elReq, 
  			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
  			 ServiceAgentFactoryI serviceAgentFactory) throws Exception { 		 
     	 String title = MRXMLUtils.getValue(elReq).trim();
  		 ValidationUtils.containsSpecialCharacters(title); 	   			
  	   	 
  		 return true;	   	 
       }
      
      public Boolean validateTitleForSpecialCharactersForPort(String action, String serviceName, String dbObjectName, Element elReq, 
   			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
   			 ServiceAgentFactoryI serviceAgentFactory) throws Exception { 		 
      	 String title = MRXMLUtils.getValue(elReq).trim();      	 
   		 ValidationUtils.containsSpecialCharacters(title, "[^a-zA-Z0-9.:_\\s$!^%+,~;=@#-]");
   		 return true;	   	 
        }
      
      public Boolean validateUserNameForSpecialCharacters(String action, String serviceName, String dbObjectName, Element elReq, 
   			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
   			 ServiceAgentFactoryI serviceAgentFactory) throws Exception { 		 
      	 String userName = MRXMLUtils.getValue(elReq).trim();
   		 ValidationUtils.containsSpecialCharactersInUserName(userName); 	   			
   	   	 
   		 return true;	   	 
        }
      
      public Boolean validateMetadataForSpecialCharacters(String action, String serviceName, String dbObjectName, Element elReq, 
   			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
   			 ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	 
    	  List<String> errTagList = new ArrayList<String>(); 
    	  List<?> childTagList = MRXMLUtils.getListXpath(elReq, ".//Tag");
    	  for (Iterator<?> i = childTagList.iterator(); i.hasNext();) {
    		  Element childEL = (Element) i.next();
    		  // Validate the well-formedness of the xml snippet.
    		  SAXReader xmlReader =  new SAXReader();
     		  xmlReader.setValidation(false);
        	  try {
        		  xmlReader.read(new StringReader(childEL.asXML()));
        	  } catch (Exception e) {
        		  log.addInfo(" " + dbObjectName + ": Invalid xml characters in Metadata Tag " + MRXMLUtils.getAttributeValue(childEL, "Name", "") + " " + e.toString(), MRLog.ERROR, MRLog.ERROR);
        		  errTagList.add(MRXMLUtils.getAttributeValue(childEL, "Name", ""));
        	  } finally {
        		  xmlReader = null;
        	  }
    	  }
    	  
    	  if(!errTagList.isEmpty()) {
    		  throw new MRException(AMExceptionCodes.VALID_ERROR_SPECIAL_CHARACTERS_IN_METADATA, StringUtils.join(errTagList, ","));
    	  }
    	  
    	  return true;	   	 
      }
      
      public Boolean validateTrimLength(String action, String serviceName, String dbObjectName, Element elReq, 
 			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
 			 ServiceAgentFactoryI serviceAgentFactory) throws Exception {    	  
    	  String trimLength = MRXMLUtils.getAttributeValue(elReq, "trimLength", "").trim();
    	  int trimLengthVal = new Integer(trimLength).intValue();
    	  if((trimLengthVal < 60000) || (trimLengthVal > 86400000)) {
    		  throw new MRException(AMExceptionCodes.VALID_TRIM_LENGTH, "Trim length should be in MilliSeconds between 1min and 24hrs.");
    	  }
    	  return true;
      }
      
      public Boolean validateTitleForWhiteSpaces(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	  String title = Utils.decode(MRXMLUtils.getValueXpath(elReq, "//Title", "").trim());
    	  if (title.equals("")) {
     	  		log.addInfo(" " + dbObjectName + ": Please provide value for '" + elReq.getName() + "' ", MRLog.ERROR, MRLog.ERROR);
     	  		throw new MRException(AMExceptionCodes.VALID_ERROR_EMPTY_NAME, MRXMLUtils.generateErrorXML("value", title));
     	  	}
  		  ValidationUtils.containsWhiteSpaces(title, dbObjectName, log);
    	  
          return true;
      }
      
      public Boolean validateDescriptionForCurlyBraces(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	 String description = MRXMLUtils.getValueXpath(elReq, "//Description", "").trim();
  		 description = ValidationUtils.escapeCurlyBraces(description);
  		 MRXMLUtils.setValueXpath(elReq, "//Description", description);
  		 
  		 return true;
  	   	 
      }
      
      public Boolean validateAssetPrivateDataForCurlyBraces(String action, String serviceName, String dbObjectName, Element elReq, Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	  String data = MRXMLUtils.getValueXpath(elReq, "//AssetPrivateData", "").trim();
   		  data = ValidationUtils.escapeCurlyBraces(data);
   		  MRXMLUtils.setValueXpath(elReq, "//AssetPrivateData", data);
   		 
   		 return true;
   	   	 
       }  
  
}
