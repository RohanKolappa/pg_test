package com.ipvsserv.nxd.api;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.xmpp.mdc.DBApi;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBApiManager {
    private static DBApi dbapi = null;
    public static synchronized DBApi getDBApi(String parentIpvsModuleName) throws Exception {
        if (dbapi == null) {
            String dbAPI_element = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".impl.dbAPI_element", "");
            String className = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".impl." + dbAPI_element, "");
            if (!className.equals("")) {
                MRLog.debug(" getDBApi CLASS using " + className);
                try {
                    dbapi = (DBApi) Class.forName(className).newInstance();
                } catch (InstantiationException ex) {
                    MRLog.error(ex);
                } catch (IllegalAccessException ex) {
                    MRLog.error(ex);
                } catch (ClassNotFoundException ex) {
                    MRLog.error(ex);
                }
            }
            if (dbapi == null) {
                MRLog.debug(" DBApiManager getDBApi() '" + parentIpvsModuleName + ".impl.dbAPI'   not found  className='" + className + "'");
                throw new Exception(" DBApiManager getDBApi() '" + parentIpvsModuleName + ".impl.dbAPI'   not found className='" + className + "'");
            }
        }
        return dbapi;
    }
    /* public static synchronized DBApiI getDBApi_SendaNative(String parentIpvsModuleName)  throws Exception{
         if (sendaNative == null) {
             String className = IpvsServGlobals.getXMLProperty( parentIpvsModuleName + ".impl.dbAPI_sednaNative", "");
             if (!className.equals("")) {
                 MRLog.debug(" getDBApi_SendaNative CLASS using " + className);
                 try {
                     sendaNative = (DBApiI) Class.forName(className).newInstance();
                 } catch (InstantiationException ex) {
                     MRLog.error(ex);
                 } catch (IllegalAccessException ex) {
                     MRLog.error(ex);
                 } catch (ClassNotFoundException ex) {
                     MRLog.error(ex);
                 }
             }
             if (sendaNative == null) {
                 MRLog.debug(" DBApiManager getDBApi_SendaNative() '" + parentIpvsModuleName + ".impl.dbAPI'   not found className='"+className +"'");
                 sendaNative = new SednaXmlDBDriverImpl();
                 throw new Exception(" DBApiManager getDBApi_SendaNative() '" + parentIpvsModuleName + ".impl.dbAPI'   not found className='"+className +"'");
                 
             }
         }
         return sendaNative;        
     }    
     public static synchronized DBApiI getDBApi_SendaXmlDB(String parentIpvsModuleName)  throws Exception{
         if (sendaXmlDB == null) {
             String className = IpvsServGlobals.getXMLProperty( parentIpvsModuleName + ".impl.dbAPI_sednaXmlDB", "");
            if (!className.equals("")) {
                 MRLog.debug(" getDBApi_SendaXmlDB CLASS using " + className);
                 try {
                     sendaXmlDB = (DBApiI) Class.forName(className).newInstance();
                 } catch (InstantiationException ex) {
                     MRLog.error(ex);
                 } catch (IllegalAccessException ex) {
                     MRLog.error(ex);
                 } catch (ClassNotFoundException ex) {
                     MRLog.error(ex);
                 }
             }
             if (sendaXmlDB == null) {
                 MRLog.debug(" DBApiManager getDBApi_SendaXmlDB() '" + parentIpvsModuleName + ".impl.dbAPI'   not found className='"+className +"'");
                 sendaXmlDB = new ExistXmlDBDriverImpl();
                 throw new Exception(" DBApiManager getDBApi_SendaXmlDB() '" + parentIpvsModuleName + ".impl.dbAPI'   not found className='"+className +"'");
             }
         }
         return sendaXmlDB;        
     }  
     public static synchronized DBApiI getDBApi_XmlDB(String parentIpvsModuleName)  throws Exception{
         if (xmlDB == null) {
             String className = IpvsServGlobals.getXMLProperty( parentIpvsModuleName + ".impl.dbAPI_xmlDB", "");
            if (!className.equals("")) {
                 MRLog.debug(" getXmlDB CLASS using " + className);
                 try {
                     xmlDB = (DBApiI) Class.forName(className).newInstance();
                 } catch (InstantiationException ex) {
                     MRLog.error(ex);
                 } catch (IllegalAccessException ex) {
                     MRLog.error(ex);
                 } catch (ClassNotFoundException ex) {
                     MRLog.error(ex);
                 }
             }
             if (xmlDB == null) {
                 MRLog.debug(" DBApiManager getDBApi_XmlDB() '" + parentIpvsModuleName + ".impl.dbAPI_xmlDB'   not found className='"+className +"'");
                 xmlDB = new XmlDBDriverImpl();
                 throw new Exception(" DBApiManager getDBApi_XmlDB() '" + parentIpvsModuleName + ".impl.dbAPI_xmlDB'   not found className='"+className +"'");
             }
         }
         return sendaXmlDB;        
     }     
     public static synchronized DBApiI getDBApi_ExistXmlDB(String parentIpvsModuleName)  throws Exception{
         if (existXmlDB == null) {
             String className = IpvsServGlobals.getXMLProperty( parentIpvsModuleName + ".impl.dbAPI_eXistXmlDB", "");
            if (!className.equals("")) {
                 MRLog.debug(" getDBApi_ExistXmlDB CLASS using " + className);
                 try {
                     existXmlDB = (DBApiI) Class.forName(className).newInstance();
                 } catch (InstantiationException ex) {
                     MRLog.error(ex);
                 } catch (IllegalAccessException ex) {
                     MRLog.error(ex);
                 } catch (ClassNotFoundException ex) {
                     MRLog.error(ex);
                 }
             }
             if (existXmlDB == null) {
                 MRLog.debug(" DBApiManager getDBApi_ExistXmlDB() '" + parentIpvsModuleName + ".impl.dbAPI_eXistXmlDB'   not found className='"+className +"'");
                 existXmlDB = new ExistXmlDBDriverImpl();
                 throw new Exception(" DBApiManager getDBApi_ExistXmlDB() '" + parentIpvsModuleName + ".impl.dbAPI_eXistXmlDB'   not found className='"+className +"'");
             }
         }
         return sendaXmlDB;        
     } */
}
