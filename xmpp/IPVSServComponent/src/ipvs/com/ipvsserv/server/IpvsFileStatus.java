package com.ipvsserv.server;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import com.ipvsserv.common.log.MRLog;

public class IpvsFileStatus {
  
    //this can be used by only one connection pool -- either native or xmldb api
    public static final String STATUS_RUNNIG_FILE = "/var/status/ipvscomponent.java.running";
    public static final String MIGRATION_STATUS_FILE = "/var/status/dbmigration.status";
    public static final String STATUS_INITIALIZING_FILE = "/var/status/ipvscomponent.java.initializing";
    public static final String MS_STATUS_INITIALIZING_FILE = "/var/status/mscomponent.java.initializing";
    public static final String STATUS_DB_FAILED_FILE = "/var/status/ipvscomponent.dbfail";
    //public static final String STATUS_DB_FAILED_FILE_S_NATIVE = "/var/status/ipvscomponent.dbfail.sedna.native";
    //public static final String STATUS_DB_FAILED_FILE_S_XMLDB = "/var/status/ipvscomponent.dbfail.sedna.xmldb";
    //public static final String STATUS_DB_FAILED_FILE_E_XMLDB = "/var/status/ipvscomponent.dbfail.exists";
    //public static final String STATUS_DB_FAILED_FILE_XMLDB = "/var/status/ipvscomponent.dbfail.xmldb";
    public static final String STATUS_OPENFIRE_FAILED_FILE = "/var/status/ipvscomponent.xmppfail";
    public static final String STATUS_DB_MIGRATING_FILE="/var/status/ipvscomponent.java.db.migrating";
    public static final String DB_HISTORY_FILE = "/data/sedna_dbsnapshot/status/dbactivity.history.status";
    public static final String FILE_FLAG_STATUS_DOWN_ACTION_NONE="/var/status/mserver.down.donothing";
    public static final String FILE_FLAG_STATUS_DOWN_ACTION_NONE_2="/data/sedna_dbsnapshot/status/factory_db_progress.status";
    public static final String FILE_MIGRATION_PROGRESS="/data/sedna_dbsnapshot/status/migration_db_progress.status";
    public static final String FILE_PRE_MIGRATION_DATABASE_TGZ="/data/sedna_dbsnapshot/status/pre_migration_db.tgz";
    
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
            createRunnigFile("created='" + (new Date()).toString() + "'", false);
        }
    }
    public synchronized boolean isStatusFileExists(boolean force) {
        if (disable) {
            return false;
        }
        if (force) {
            checkRunnigFile();
        }
        return fileCreated; 
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
    public synchronized void write(String msg, boolean append){
        createRunnigFile(msg, append);
    }
    protected  void createRunnigFile(String msg, boolean append) {
        File file = new File(filePath);
        if(!file.getParentFile().exists()){
            boolean b= file.getParentFile().mkdirs();
            if(!b && !file.getParentFile().exists()){
                MRLog.debug("no permission to create folder " + file.getParentFile() );
                return;
            }
        }
       
        //if (!file.exists()) {
        BufferedWriter output = null;
        try {
            //MRLog.debug("################ IpvsFileStatus CREATE_STATUS_FILE  writing to file  " + filePath +" file.exists() " + file.exists() +" fileCreated " + fileCreated);
            output = new BufferedWriter(new FileWriter(file, append));
            output.write(msg);//"created='" + (new Date()).toString() + "'");
            //MRLog.debug("################ IpvsFileStatus CREATE_STATUS_FILE  done writing to file  " + filePath +" file.exists() " + file.exists() +" fileCreated " + fileCreated);
        } catch (Exception e) {
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
   
    protected  void removeRunnigFile() {
        File file = new File(filePath);
        //if (file.exists()) {
        boolean bdeleted= file.delete();
        //MRLog.debug("################ IpvsFileStatus REMOVE_STATUS_FILE deleting File " + filePath +" isdeleted=" + bdeleted +" fileCreated " + fileCreated);
        //}
        fileCreated = false;
    }
    protected boolean checkRunnigFile() {
        File file = new File(filePath);
        fileCreated = file.exists();
        return fileCreated ;
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
    public static void createDBHistory(String header, String body){
        IpvsFileStatus historyFile = new IpvsFileStatus(IpvsFileStatus.DB_HISTORY_FILE, true);        
        Date date = new Date();
        SimpleDateFormat format = new SimpleDateFormat("EEE MMM d HH:mm:ss z yyyy");
        String dateStr = format.format(date);
        historyFile.write("\n\n\n"+dateStr + " ###### "+header+" ###############"+"\n" + body, true);

    }


}
