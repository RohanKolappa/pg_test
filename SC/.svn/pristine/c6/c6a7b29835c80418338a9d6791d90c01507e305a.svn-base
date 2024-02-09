package com.barco.nimbus.database.exist;

import org.xmldb.api.DatabaseManager;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.CompiledExpression;
import org.xmldb.api.base.Database;
import org.xmldb.api.base.Resource;
import org.xmldb.api.base.ResourceIterator;
import org.xmldb.api.base.ResourceSet;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.XQueryService;

public class XQueryExample {
    
    private static String URI = "xmldb:exist://localhost:8080/exist/xmlrpc";
    /**
     * args[0] Should be the name of the collection to access
     * args[1] Should be the XQuery to execute
     */
    public static void main(String args[]) throws Exception {
        
        final String driver = "org.exist.xmldb.DatabaseImpl";
        
        // initialize database driver
        Class<?> cl = Class.forName(driver);
        Database database = (Database) cl.newInstance();
        database.setProperty("create-database", "true");
        DatabaseManager.registerDatabase(database);
        
        Collection col = null;
        try { 
            col = DatabaseManager.getCollection(URI + args[0]);
            XQueryService xqs = (XQueryService) col.getService("XQueryService", "1.0");
            xqs.setProperty("indent", "yes");
        
            CompiledExpression compiled = xqs.compile(args[1]);
            ResourceSet result = xqs.execute(compiled);
            ResourceIterator i = result.getIterator();
            Resource res = null;
            while(i.hasMoreResources()) {
                try {
                    res = i.nextResource();
                    System.out.println(res.getContent());
                } finally {
                    //dont forget to cleanup resources
//                    try { ((EXistResource)res).freeResources(); } catch(XMLDBException xe) {xe.printStackTrace();}
                }
            }
        } finally {
            //dont forget to cleanup
            if(col != null) {
                try { col.close(); } catch(XMLDBException xe) {xe.printStackTrace();}
            }
        }
    }
}