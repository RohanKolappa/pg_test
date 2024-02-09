package com.ipvsserv.nxd.adminapi.db;

import java.util.Map;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;

public class ExportQueryIndexAction extends DBQueryAction {

    @Override
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        String dbquery = updateQuery(getQuery(), params);
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        StringBuffer buf = new StringBuffer(); 
        dbadmin.getIpvsDBDriver().execute("ExportQueryIndex", dbquery, buf);
        CompUtils.writeFile(exportDB.getDBIndexFilePath() , "<index>"+buf.toString()+"</index>");
        return "DONE";
    }

}
