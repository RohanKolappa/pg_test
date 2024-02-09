package com.ipvsserv.nxd.dbtask.test.sednaxmldb;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.dbtask.test.TestModule;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class SednaXmlDBTestModule extends TestModule {
    public SednaXmlDBTestModule() {
        super("SednaXmlDBTestModule");
        taskName = IpvsServGlobals.APP_SEDNA_XMLDB_DBTEST;
    }
    
    public SednaXmlDBTestModule(String moduleName) {
        super(moduleName);
        taskName = IpvsServGlobals.APP_SEDNA_XMLDB_DBTEST;
    }
    
    public void start() throws Exception {
        try {
            super.start();
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            super.destroy();
            
        }
    }
}
