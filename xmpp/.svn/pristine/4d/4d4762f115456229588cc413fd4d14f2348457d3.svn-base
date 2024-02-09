package com.ipvsserv.nxd.backup;

import java.io.File;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.nxd.api.xmldb.XmlDBModuleI;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
//do anything vendor specific - sedna 
public interface VendorBackupService {
    public abstract void backupdb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void restoredb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void importdb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean isDeleteSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean isAddSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean isUpdateSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void factorydb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void upgradeIndexes(String ID, String domainName, XmlDBModuleI dbModule) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

}
