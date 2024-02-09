package com.ipvsserv.nxd.api.sednanative;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;

import com.ipvsserv.common.log.MRLog;

public class DatabaseConnectionStatus {
    //this can be used by only one connection pool -- either native or xmldb api
    private static final String RUNNIG_FILE = "/var/status/ipvscomponent.dbfail.sednanative";
    protected boolean connectionFailed = false;
    protected String filePath = RUNNIG_FILE;

    public DatabaseConnectionStatus() {
        //filePath= RUNNIG_FILE;
    }

    public synchronized boolean isConnectedFailed() {
        return connectionFailed;
    }

    public synchronized void setConnected(boolean isConnectionFailed) {
        MRLog.debug("################ " + isConnectionFailed + " DatabaseConnectionStatus " + filePath);
        if (isConnectionFailed && !connectionFailed) {
            createRunnigFile();
        } else {
            if (connectionFailed) {
                //todo:check and remove everytime 
                removeRunnigFile();
            }
        }
    }

    protected synchronized void createRunnigFile() {
        File file = new File(filePath);
        if (!file.exists()) {
            BufferedWriter output = null;
            try {
                output = new BufferedWriter(new FileWriter(file));
                output.write("connectionFailed='" + (new Date()).toString() + "'");
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
        }
        connectionFailed = true;
    }

    protected synchronized void removeRunnigFile() {
        File file = new File(RUNNIG_FILE);
        if (file.exists()) {
            file.delete();
        }
        connectionFailed = false;
    }
}
