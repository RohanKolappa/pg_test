package com.ipvs.utils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import javax.ws.rs.core.MediaType;

public class RestAcceptHeaders {
	
	protected static final String ACCEPT_HEADER_PREFIX = "application/vnd.barco.nimbus";

	
	public static final List<String> ACCEPT_HEADERS;
	
	protected enum AcceptHeaderDataType{
		json,
		xml;
	}
	
	public enum ResourceTypes{
		Device,
		Dir,
		File,
		User,
		Profile,
		Port
	}
	
	public enum ResourceProperties{
		Tags,
		DisplayStatus,
		DisplayConfig,
		Info,
		ObserverInfo,
		StreamURL,
		PVRInfo,
		MetaData,
		Password,
		WindowProperties,
		DisplayProfile
	}
	static{
		
		List<String> allHeaders = new ArrayList<String>();
		for (ResourceTypes resourceType : ResourceTypes.values()) {
			for (AcceptHeaderDataType headerType : AcceptHeaderDataType.values()) {
				allHeaders.add((ACCEPT_HEADER_PREFIX + "." +resourceType.name() + "+" + headerType.name()));
			}
		}

		allHeaders.add(MediaType.APPLICATION_XML);
		allHeaders.add(MediaType.APPLICATION_JSON);
		allHeaders.add("*/*");
		
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceProperties.Tags.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceProperties.Tags.name() + "+" + "xml" );
		
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Dir + "-" + ResourceProperties.Tags.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Dir + "-" + ResourceProperties.Tags.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.File + "-" + ResourceProperties.Tags.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.File + "-" + ResourceProperties.Tags.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.User + "-" + ResourceProperties.Tags.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.User + "-" + ResourceProperties.Tags.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.Tags.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.Tags.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.DisplayConfig.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.DisplayConfig.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.DisplayStatus.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.DisplayStatus.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.WindowProperties.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.WindowProperties.name() + "+" + "xml" );
		
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.Info.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.Info.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.ObserverInfo.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.ObserverInfo.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.StreamURL.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.StreamURL.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.PVRInfo.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Port + "-" + ResourceProperties.PVRInfo.name() + "+" + "xml" );
		
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Device + "-" + ResourceProperties.Info.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.Device + "-" + ResourceProperties.Info.name() + "+" + "xml" );
		
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.File + "-" + ResourceProperties.MetaData.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.File + "-" + ResourceProperties.MetaData.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.File + "-" + ResourceProperties.Info.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.File + "-" + ResourceProperties.Info.name() + "+" + "xml" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.User + "-" + ResourceProperties.Password.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceTypes.User + "-" + ResourceProperties.Password.name() + "+" + "xml" );
		
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceProperties.DisplayProfile.name() + "+" + "json" );
		allHeaders.add(ACCEPT_HEADER_PREFIX + "." + ResourceProperties.DisplayProfile.name() + "+" + "xml" );
		
		ACCEPT_HEADERS =  Collections.synchronizedList(allHeaders);
	}
	
	public static boolean isValidAcceptHeader(String header){
		return ACCEPT_HEADERS.contains(header);
	}
	
	public static Object[] allValidAcceptHeaders(){
		return ACCEPT_HEADERS.toArray();
	}

	public static String resourceType(String header) {
		if(header.contains(ACCEPT_HEADER_PREFIX)){
			if(header.indexOf("-") != -1){
				return header.substring(ACCEPT_HEADER_PREFIX.length()+1, header.indexOf('-'));			
			}else {
				return header.substring(ACCEPT_HEADER_PREFIX.length()+1, header.indexOf('+'));
				
			}			
		}else {
			return "";
		}
	}
	
	public static String resourceProperty(String header) {
		if(header.indexOf("-") != -1){
			return header.substring(header.indexOf("-")+1, header.indexOf('+'));			
		}else {
			return "";
		}

	}
}
