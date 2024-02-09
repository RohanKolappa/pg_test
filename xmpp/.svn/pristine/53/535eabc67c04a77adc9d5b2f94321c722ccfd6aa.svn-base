package com.ipvsserv.nxd.adminapi.db;

import java.util.Hashtable;
import java.util.Map;

import com.ipvsserv.common.log.MRLog;

public class AdminApiUtils {
    public static final String PREFIX_OBJ = "#OBJ_";
    public static final String SUFFIX_OBJ = "_OBJ#";

    public static Map<String, String> parseObjectTypesInQuery(String query) throws Exception {
        Map<String, String> list = new Hashtable<String, String>();
        try {

            //modify the query with pem 
            /*
              for each $b1 in #OBJ_objectname1_OBJ# 
              for each $b2 in #OBJ_objectname2_OBJ# 
              where $b1/abc=$b2/efg
              return 
              <result>
              {$b1}
              {$b2}
              </result>
              */
            int startPos = 0;
            startPos = getParsedObjectType(query, startPos, list);
            if (startPos != -1) {
                startPos = 0;
                while (startPos != -1) {
                    startPos = getParsedObjectType(query, startPos, list);
                }
            }
            return list;

        } finally {
        }
    }

    public static int getParsedObjectType(String query, int startPos, Map<String, String> list) throws Exception {
        //may be encription is needed or cdata
        int pos1 = query.indexOf(PREFIX_OBJ, startPos);
        if (pos1 == -1) {
            return -1;
        }
        int pos2 = query.indexOf(SUFFIX_OBJ, pos1);
        if (pos2 == -1) {
            throw new Exception("Invalid QueryRequest");
        }
        String objectType = query.substring(pos1 + PREFIX_OBJ.length(), pos2);
        MRLog.debug("DB_ADMIN: getParsedObjectType " + objectType);
        
        list.put(objectType, "");
        return pos2 + 1;
    }
}
