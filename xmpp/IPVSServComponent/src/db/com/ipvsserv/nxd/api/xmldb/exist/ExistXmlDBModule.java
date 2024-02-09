package com.ipvsserv.nxd.api.xmldb.exist;

import org.xmldb.api.DatabaseManager;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.XMLDBException;

import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.api.IpvsDBDriverFactory;
import com.ipvsserv.nxd.api.xmldb.XmlDBModule;
import com.ipvsserv.server.IpvsFileStatus;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class ExistXmlDBModule extends XmlDBModule {
    public static final String NAME="XmlDB Exist Pool";
    public ExistXmlDBModule() {
        super(NAME);
        setDriver("org.exist.xmldb.DatabaseImpl");//"org.exist.xmldb.DatabaseImpl"
    }

    public ExistXmlDBModule(String moduleName) {
        super(moduleName);
        setDriver("org.exist.xmldb.DatabaseImpl");
    }

    protected void startFileStatus() {
        databaseConnectionStatus = new IpvsFileStatus(IpvsFileStatus.STATUS_DB_FAILED_FILE);
        databaseConnectionStatus.init(true);
    }

    ///////////////////////////// initialize / shutdown ///////////////////////////// 
    protected Collection getConnection(String url, String DBName, String user, String password) throws DBNotConnectedException {
        try {
            //return DatabaseManager.getCollection(url, user, password);
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("get new Connection. " + getAudit());
            }
            return DatabaseManager.getCollection(url, user, password);
        } catch (XMLDBException ex) {
            String error = ex.getMessage();
            if (error.indexOf("Could not connect") != -1) {
                initialized = false;
            } else if (error.indexOf("SE4409") != -1) {
                initialized = false;
            } else if (error.indexOf("Could not get a connection within") != -1) {
                if (error.indexOf("connections already open") != -1) {
                    initialized = false;
                } else {
                    initialized = false;
                }
            }
            MRLog.error("XMLDBException: " + ex.getMessage());
            MRLog.error(ex);
            throw new DBNotConnectedException("XMLDB getConnection failed." + getAudit());
        } catch (Exception ex) {
            MRLog.error("ClassNotFoundException: " + ex.getMessage());
            MRLog.error(ex);
            throw new DBNotConnectedException("XMLDB getConnection failed." + getAudit());
        }
    }

    @Override
    public IpvsDBDriver getDefaultIpvsDBDriver() throws Exception {
        return IpvsDBDriverFactory.getIpvsDBDriverVendor(IpvsDBDriverFactory.XMLDB_EXIST);
    }

    @Override
    public IpvsDBDriver getDefaultIpvsDBDriver(String parentIpvsModuleName) throws Exception {
        return IpvsDBDriverFactory.getIpvsDBDriver(parentIpvsModuleName, IpvsDBDriverFactory.XMLDB_EXIST);
    }

}
