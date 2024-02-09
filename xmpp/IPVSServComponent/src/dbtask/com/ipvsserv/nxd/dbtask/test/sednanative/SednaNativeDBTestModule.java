package com.ipvsserv.nxd.dbtask.test.sednanative;

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
public class SednaNativeDBTestModule extends TestModule {
    public SednaNativeDBTestModule() {
        super("SednaNativeDBTestModule");
        taskName = IpvsServGlobals.APP_SEDNA_NATIVE_DBTEST;
    }
    
    public SednaNativeDBTestModule(String moduleName) {
        super(moduleName);
        taskName = IpvsServGlobals.APP_SEDNA_NATIVE_DBTEST;
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
