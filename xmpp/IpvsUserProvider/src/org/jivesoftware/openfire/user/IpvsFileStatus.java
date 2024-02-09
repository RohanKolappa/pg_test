package org.jivesoftware.openfire.user;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;

public class IpvsFileStatus {
    public static final String STATUS_INITIALIZING_FILE = "/var/status/openfire.java.initializing";
    
    //this can be used by only one connection pool -- either native or xmldb api
    //public static final String STATUS_RUNNIG_FILE = "/var/status/ipvscomponent.java.running";
    //public static final String STATUS_INITIALIZING_FILE = "/var/status/ipvscomponent.java.initializing";
    //public static final String STATUS_DB_FAILED_FILE_S_NATIVE = "/var/status/ipvscomponent.dbfail.sedna.native";
    //public static final String STATUS_DB_FAILED_FILE_S_XMLDB = "/var/status/ipvscomponent.dbfail.sedna.xmldb";
    //public static final String STATUS_DB_FAILED_FILE_E_XMLDB = "/var/status/ipvscomponent.dbfail.exists";
    //public static final String STATUS_DB_FAILED_FILE_XMLDB = "/var/status/ipvscomponent.dbfail.xmldb";
    //public static final String STATUS_OPENFIRE_FAILED_FILE = "/var/status/ipvscomponent.xmppfail";

    protected boolean fileCreated = false;
    protected boolean disable = false;
    protected String filePath = "";
    
    public IpvsFileStatus(String file) {
        this(file, false);
    }
//    public IpvsFileStatus(boolean fileCreate, String file) {
//        this(file, false);
//        init(fileCreate);
//    }
    public IpvsFileStatus(String file, boolean disable) {
        //MRLog.debug("################ IpvsFileStatus CREATE_STATUS_FILE 1 " + filePath + " disable " + disable +" fileCreated " + fileCreated);
        this.disable = disable;
        filePath = file;
    }
    public void init(boolean fileCreate){
        //MRLog.debug("################ IpvsFileStatus CREATE_STATUS_FILE 2 " + filePath + " disable " + disable +" fileCreated " + fileCreated);
        if (fileCreate) {
            createStatusFile(true);
        } else {
            removeStatusFile(true);
        }
    }
    public synchronized void createStatusFile() {
        createStatusFile(false);
    }

    public synchronized void removeStatusFile() {
        removeStatusFile(false);
    }

    public synchronized void createStatusFile(boolean force) {
        if (disable) {
            return;
        }
        //MRLog.debug("################ IpvsFileStatus CREATE_STATUS_FILE " + filePath + " force " + force +" fileCreated " + fileCreated);
        if (!fileCreated || force) {
            createRunnigFile();
        }
    }

    public synchronized void removeStatusFile(boolean force) {
        if (disable) {
            return;
        }
        //MRLog.debug("################ IpvsFileStatus REMOVE_STATUS_FILE " + filePath + " force " + force +" fileCreated " + fileCreated);
        if (fileCreated || force) {
            //todo:check and remove everytime 
            removeRunnigFile();
        }
    }

    protected void createRunnigFile() {
        File file = new File(filePath);
        if(!file.getParentFile().exists()){
            file.getParentFile().mkdirs();
        }
        //if (!file.exists()) {
        BufferedWriter output = null;
        try {
            //MRLog.debug("################ IpvsFileStatus CREATE_STATUS_FILE  writing to file  " + filePath +" file.exists() " + file.exists() +" fileCreated " + fileCreated);
            output = new BufferedWriter(new FileWriter(file, true));
            output.write("connectionFailed='" + (new Date()).toString() + "'");
            //MRLog.debug("################ IpvsFileStatus CREATE_STATUS_FILE  done writing to file  " + filePath +" file.exists() " + file.exists() +" fileCreated " + fileCreated);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (output != null) {
                try {
                    output.close();
                } catch (IOException e) {
                }
            }
            output = null;
        }
        //}
        fileCreated = true;
    }

    protected void removeRunnigFile() {
        File file = new File(filePath);
        //if (file.exists()) {
        boolean bdeleted= file.delete();
        //MRLog.debug("################ IpvsFileStatus REMOVE_STATUS_FILE deleting File " + filePath +" isdeleted=" + bdeleted +" fileCreated " + fileCreated);
        //}
        fileCreated = false;
    }

    public boolean isDisable() {
        return disable;
    }

    public void setDisable(boolean disable) {
        this.disable = disable;
    }

    public boolean isFileCreated() {
        return fileCreated;
    }

    public void setFileCreated(boolean fileCreated) {
        this.fileCreated = fileCreated;
    }

    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }


}
