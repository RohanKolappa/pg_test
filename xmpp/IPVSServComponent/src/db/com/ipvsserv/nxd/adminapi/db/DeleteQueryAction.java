package com.ipvsserv.nxd.adminapi.db;

import java.util.Map;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;

public class DeleteQueryAction extends DBQueryAction {

    @Override
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        String dbquery = updateQuery(getQuery(), params);
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        dbadmin.deletedb_xquery(dbquery, exportDB.getDomainName());
        return "DONE";

    }

}
