package com.ipvsserv.nxd.adminapi.db;

import java.util.Map;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;

public class UpdateQueryAction extends DBQueryAction {
    @Override
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        String dbquery = updateQuery(getQuery(), params);
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        NIDSchemeMDC requestMDCParams = new NIDSchemeMDC();
        requestMDCParams.setDomain(exportDB.getDomainName());
        dbadmin.getIpvsDBDriver().executeXQueryToUpdate("UpdateQuery", requestMDCParams.getCollectionNameSCol(), dbquery);
        return "DONE";
    }
}
