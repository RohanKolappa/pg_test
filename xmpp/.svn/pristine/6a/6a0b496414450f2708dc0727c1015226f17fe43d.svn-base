package com.ipvsserv.nxd.api.xmldb.exist;

import java.io.File;

import com.ipvsserv.common.config.IpvsServGlobals;

public class ExistDBUtils {
    public static String getRootCollectionName() {
        return "/db";
    }
    public static String getRootCollectionDefEmpty(String collectionName){
        if(collectionName==null || collectionName.trim().equals("")){
            return "/db";
        }
        return "/db";
    }
    public static boolean isRootCollection(String collectionName) {
        return ("/db".equalsIgnoreCase(collectionName) || "db".equalsIgnoreCase(collectionName));
    }

    public static boolean isIpvsCollection(String collectionName) {
        if (collectionName == null || collectionName.equals("") || isRootCollection(collectionName)) {
            return false;
        }
        File file = new File(collectionName);
        if(IpvsServGlobals.isUseListCol()) {
            if(getDomainNameFromFolderName(collectionName) != null) {
                return true;
            }
        }else{
            if (IpvsServGlobals.getDomainDBSchema().getDomainName(file.getName()) != null) {
                return true;
            }
        }
        
        return false;
    }
    
    public static boolean isIpvsSCol(String collectionName) {
        if (collectionName == null || collectionName.equals("") || isRootCollection(collectionName)) {
            return false;
        }
        File file = new File(collectionName);
        if (IpvsServGlobals.getDomainDBSchema().getDomainName(file.getName()) != null) {
            return true;
        }
        return false;
    } 
    
    public static String getIpvsCollectionName(String collectionName) {
        if (collectionName == null || collectionName.equals("") ) {
            return collectionName;
        }
        if(collectionName.startsWith("/db")){
            return collectionName;
        }
         
        return "/db"+collectionName.trim();
    }  
    
    public static String getDomainNameFromFolderName(String folderName) {
        String collist_prefix = IpvsServGlobals.getListColPrefix();
        if(folderName.startsWith(collist_prefix)){
            String[] arr = folderName.split("[.]");
            if(arr.length==3){
                return arr[1];
            }
        }
        return null;
    }    
}
