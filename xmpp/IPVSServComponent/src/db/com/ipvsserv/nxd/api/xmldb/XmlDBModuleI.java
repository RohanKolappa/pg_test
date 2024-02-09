package com.ipvsserv.nxd.api.xmldb;

import java.util.Map;

import org.xmldb.api.base.Collection;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.server.IpvsServModule;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface XmlDBModuleI extends IpvsServModule {

    public abstract ServiceManagerWrapper getServiceManager();

    public abstract void setServiceManager(ServiceManagerWrapper serviceManager);

    public abstract int getCheckInCount();

    public abstract int getCheckOutCount();

    ///////////////////////////// checkOut ///////////////////////////// 
    public abstract Collection checkOutLowPriority() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract Collection checkOut() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void checkIn(Collection con, boolean kill);

    //////////////////// module /////////////////////////////////////
    public abstract void initialize(Map<String, String> params) throws Exception;

    public abstract void start() throws Exception;

    public abstract void stop() throws Exception;

    public abstract void destroy() throws Exception;

    public abstract void verifyState() throws Exception;

    public abstract int getLastKnownState() throws Exception;

    public abstract IpvsDBDriver getDefaultIpvsDBDriver() throws Exception;

    public abstract IpvsDBDriver getDefaultIpvsDBDriver(String parentIpvsModuleName) throws Exception;

}
