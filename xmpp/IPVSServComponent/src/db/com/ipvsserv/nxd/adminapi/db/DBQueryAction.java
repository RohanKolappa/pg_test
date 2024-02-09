package com.ipvsserv.nxd.adminapi.db;

import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.server.IpvsServer;

public abstract class DBQueryAction {
    private static Hashtable<String, Class<?>> dbqueryClassList = new Hashtable<String, Class<?>>();
    private static boolean loadedTag=false;
    /////////////////////////
    private static ClassLoader[] getClassLoaders() {
        ClassLoader[] classLoaders = new ClassLoader[3];
        classLoaders[0] = DBQueryAction.class.getClassLoader();
        classLoaders[1] = Thread.currentThread().getContextClassLoader();
        classLoaders[2] = IpvsServer.getInstance().getIpvsClassLoader();
        // Clean up possible null values. Note that #getClassLoader may return a null value.
        List<ClassLoader> loaders = new ArrayList<ClassLoader>();
        for (ClassLoader classLoader : classLoaders) {
            if (classLoader != null) {
                loaders.add(classLoader);
            }
        }
        return loaders.toArray(new ClassLoader[loaders.size()]);
    }
    protected static void initialize() {
        // Load IQ processing providers.
        if(loadedTag){
            return ;
        }
        try {
            // Get an array of class loaders to try loading the providers files from.
            ClassLoader[] classLoaders = getClassLoaders();
            for (ClassLoader classLoader : classLoaders) {
                Enumeration<URL> providerEnum = classLoader.getResources("META-INF/ipvsimportexport.providers");
                while (providerEnum.hasMoreElements()) {
                    URL url = providerEnum.nextElement();
                    try {
                        loadResourceTagMappings(url);
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                }
            }
        } catch (Exception ex) {
           MRLog.error("META-INF/ipvsimportexport.providers", ex);
        }
        loadedTag=true;
    }  
    public static void loadResourceTagMappings(URL xmlFilePath) {
        /*
        <actionMapping>
            <name>UpdateImportFileNIDs</name>
            <class>com.ipvsserv.nxd.adminapi.mediafile.UpdateImportFileNIDsAction</class>
        </actionMapping>    
         */
        try {
            Document doc = Dom4jUtils.getDocument(xmlFilePath);
            List<?> lsit = doc.getRootElement().elements("actionMapping");
            for (Iterator<?> iterator = lsit.iterator(); iterator.hasNext();) {
                try {
                    Element el = (Element) iterator.next();
                    String tagName = Dom4jUtils.value(el, "name", "");
                    String className = Dom4jUtils.value(el, "class", "");
                    Class<?> cls = Thread.currentThread().getContextClassLoader().loadClass(className);
                    addDBQueryHandler(tagName, cls);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    ///////////////////////
    
    public static DBQueryAction getInstance(String type) throws Exception {
        return getDBQueryHandler(type);
    }

    public static DBQueryAction addDBQueryHandler(String tagName, Class<?> cls) throws InstantiationException, IllegalAccessException {
        if (cls != null) {
            Object obj = cls.newInstance();
            if (obj instanceof DBQueryAction) {
                dbqueryClassList.put(tagName, cls);
                return (DBQueryAction) obj;
            }
        }
        throw new ClassCastException("Invalid class");
    }

    public static DBQueryAction getDBQueryHandler(String type) throws InstantiationException, IllegalAccessException, ClassNotFoundException {
        return getDBQueryHandler(type, Thread.currentThread().getContextClassLoader());
    }

    public static DBQueryAction getDBQueryHandler(String type, Class<?> classLoaderC) throws InstantiationException, IllegalAccessException, ClassNotFoundException {
        return getDBQueryHandler(type, classLoaderC.getClassLoader());
    }

    public static DBQueryAction getDBQueryHandler(String type, ClassLoader classLoader) throws InstantiationException, IllegalAccessException, ClassNotFoundException {
        if (dbqueryClassList.containsKey(type)) {
            Class<?> cls = dbqueryClassList.get(type);
            return (DBQueryAction) cls.newInstance();
        }
        String className = getDBQueryHandlerName(type);
        Class<?> cls = null;
        try{
            cls = classLoader.loadClass(className);
            return addDBQueryHandler(type, cls);
        }catch(Exception ex){
            if(!loadedTag){
                initialize();
                if (dbqueryClassList.containsKey(type)) {
                    cls = dbqueryClassList.get(type);
                    return (DBQueryAction) cls.newInstance();
                }
            }
            throw new ClassNotFoundException(className);
        }
        
        
    }

    public static String getDBQueryHandlerName(String type) {
        if (type.indexOf('.') != -1) {
            return type;
        }
        return "com.ipvsserv.nxd.adminapi.db." + type + "Action";
    }

    ////////////
    protected String query = null;
    protected String name = null;
    protected String type = null;
    protected int priority = 0;
    protected boolean enable = false;
    protected DBActionManager exportDB = null;
    protected Element queryEL = null;
    protected DBQueryAction parent = null;
    protected Hashtable<String, String> readQueryList = new Hashtable<String, String>();

    public DBQueryAction() {

    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getPriority() {
        return priority;
    }

    public void setPriority(int priority) {
        this.priority = priority;
    }

    public boolean isEnable() {
        return enable;
    }

    public void setEnable(boolean enable) {
        this.enable = enable;
    }

    public void setQueryEL(Element queryEL) {
        this.queryEL = queryEL;
        //<query type="update_nodes" name="status_exporting" priority="1" enable="true">
        setName(Dom4jUtils.getAttributeValue(queryEL, "name", ""));
        setType(Dom4jUtils.getAttributeValue(queryEL, "type", ""));
        setPriority(CompUtils.intValue(Dom4jUtils.getAttributeValue(queryEL, "priority", ""), -1));
        setEnable(CompUtils.booleanValue(Dom4jUtils.getAttributeValue(queryEL, "enable", ""), false));
        setQuery(Dom4jUtils.value(queryEL, ""));
    }

    public String getQuery() {
        return query;
    }

    public void setQuery(String query) {
        this.query = query;
    }
    public String getReadQuery(String name,  Map<String, String> params) throws Exception{
        String dbquery = Dom4jUtils.getValueXpath(queryEL, "//query[@type='ReadQuery' and @name='"+name+"' ]", "");
        dbquery = updateQuery(dbquery, params);
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        StringBuffer buf = new StringBuffer(); 
        dbadmin.getIpvsDBDriver().execute("ReadQuery", dbquery, buf);
        //CompUtils.writeFile(exportDB.getDBIndexFilePath() , "<index>"+buf.toString()+"</index>");
        //readQueryList.put(name, "<a>"+buf.toString()+"</a>");

        return buf.toString();
    }
    public Object execute(DBActionManager exportDB, Element queryEL, Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        this.exportDB = exportDB;
        setQueryEL(queryEL);
        if (isEnable()) {
            return execute(params);
        } else {
            return null;
        }
    }

    public String updateQuery(String query, Map<String, String> params) {
        for (String key : params.keySet()) {
            query = query.replace("PARAM_" + key, params.get(key));
        }
        return query;
    }

    public DBQueryAction getParent() {
        return parent;
    }

    public void setParent(DBQueryAction parent) {
        this.parent = parent;
    }

    public abstract Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception;
    
    
}
