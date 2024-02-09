package com.ipvsserv.nxd.migration.task;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class MigrationTaskShared {
    private static MigrationTaskShared instance = null;

    public synchronized static MigrationTaskShared getInstance() {
        if (instance == null)
            instance = new MigrationTaskShared();
        return instance;
    }

    public MigrationTaskShared() {
        init();
    }

    public synchronized void init() {

    }

}
