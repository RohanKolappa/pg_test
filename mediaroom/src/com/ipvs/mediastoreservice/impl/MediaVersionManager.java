package com.ipvs.mediastoreservice.impl;

import java.util.ArrayList;
import java.util.List;

import com.ipvs.common.MRLog;
import com.ipvs.systemservice.impl.ServiceVersionManager;

public class MediaVersionManager extends ServiceVersionManager{

    private static final List<String> versionList = new ArrayList<String>();
    // Version constants
    public static final String MEDIA_STORE_VERSION_2_DOT_4 = "0.24";
    public static final String MEDIA_STORE_VERSION_3_DOR_2 = "0.32";
 

    public static void init(MRLog log) {
	versionList.add(getServiceVersion(log));
	versionList.add(MEDIA_STORE_VERSION_3_DOR_2);
	versionList.add(MEDIA_STORE_VERSION_2_DOT_4);
    } 
    
    public static boolean is2dot4Media(String mediaVersion) {
    	return MEDIA_STORE_VERSION_2_DOT_4.equals(mediaVersion);
    }
    
    public static boolean is3dot2Media(String mediaVersion) {
    	return MEDIA_STORE_VERSION_3_DOR_2.equals(mediaVersion);
    }
    
    public static String getMediaCurrentVersion(MRLog log) {
    	return getServiceVersion(log);
    }
    
    public static List<String> getVersionList(){
	return versionList;
    }

}
