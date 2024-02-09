package com.ipvsserv.nxd.api.xmldb.exist;

public class DatabaseConnectionStatus extends com.ipvsserv.nxd.api.sednanative.DatabaseConnectionStatus {
    //this can be used by only one connection pool -- either native or xmldb api
    private static final String RUNNIG_FILE = "/var/run/ipvscomponent.xmldb.connected";

    public DatabaseConnectionStatus() {
        super();
        filePath = RUNNIG_FILE;
    }
}
