package com.ipvsserv.nxd.adminapi.db;

import java.io.File;
import java.util.Map;

import org.dom4j.Element;
 

public interface DBActionManager {
    public abstract String getDomainName();

    public abstract Element getRequestEL();

    public abstract String getType();

    public abstract long getTimeout();

    public abstract String getZipFolderPath();

    public abstract String getScriptFilePath();

    public abstract Map<String, String> getParams();

    public abstract String getWorkarea_folder();

    public abstract String getZipFilePath();

    public abstract int getMaxPriority();

    public abstract Object invokeActions(DBQueryAction action, Element EL) throws Exception;

    public abstract String getMediaDirZipType();
   
    public abstract String getDBIndexFilePath();
    
    public abstract File getDBImportFolder();

}
