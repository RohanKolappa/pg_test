package com.ipvsserv.nxd.adminapi;

import com.ipvsserv.common.config.IpvsServGlobals;

public class DBAdmin {

    /*  private static DBAdminApi admin = null;

      private static synchronized void initDBAdminApi() {
          if (admin == null) {
              admin = new SednaXmlDBAdminApi();
          }
      }

      public static DBAdminApi getDBAdminApi() {
          if (admin == null) {
              initDBAdminApi();
          }
          return admin;
      }

      public static void setDBAdminApi(DBAdminApi admin) {
          DBAdmin.admin = admin;
      }*/
    private static DBAdminApi dbAdminApi =null;
    public static synchronized DBAdminApi getDBAdminApi() {
        if(dbAdminApi==null){
            String admindbapi = IpvsServGlobals.getProperty("admindbapi.admindbapi", DBAdminApi.DB_CONNECTION_SEDNA_NATIVE);
            //TODO:default and configuration
            if (admindbapi.equalsIgnoreCase(DBAdminApi.DB_CONNECTION_SEDNA_XMLDB)) {
                //dbAdminApi= new SednaXmlDBAdminApi();
                dbAdminApi= new SednaNativeDBAdminApi();
            }else if (admindbapi.equalsIgnoreCase(DBAdminApi.DB_CONNECTION_EXIST_XMLDB)) {
                 dbAdminApi= new ExistXmlDBAdminApi();
            } else {
                dbAdminApi= new SednaNativeDBAdminApi();
            }
        }
        return dbAdminApi;
    }

    public static DBAdminApi SednaXmlDBAdminApi() {
        return new SednaXmlDBAdminApi();
    }

    public static DBAdminApi getSednaNativeDBAdminApi() {
        return new SednaNativeDBAdminApi();
    }
    
    public static DBAdminApi getExistXmlDBAdminApi() {
        return new ExistXmlDBAdminApi();
    }
}
