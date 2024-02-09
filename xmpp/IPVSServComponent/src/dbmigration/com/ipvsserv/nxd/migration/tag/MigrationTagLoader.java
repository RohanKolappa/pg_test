package com.ipvsserv.nxd.migration.tag;

import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.server.IpvsServer;

public class MigrationTagLoader {
    private Hashtable<String, Class<?>> tagClassList = new Hashtable<String, Class<?>>();
    public static final String TAG_PREFIX = "ipvstag_";
    private static MigrationTagLoader instance = null;
    public static final String TAG_MAPPINGS = "/com/ipvsserv/nxd/migration/tag/tagMappings.xml";

    public static synchronized MigrationTagLoader getInstance() {
        if (instance == null) {
            instance = new MigrationTagLoader();
            instance.initialize();
            //instance.loadResourceTagMappings(TAG_MAPPINGS);
        }
        return instance;
    }

    private ClassLoader[] getClassLoaders() {
        ClassLoader[] classLoaders = new ClassLoader[3];
        classLoaders[0] = MigrationTagLoader.class.getClassLoader();
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

    protected void initialize() {
        // Load IQ processing providers.
        try {
            // Get an array of class loaders to try loading the providers files from.
            ClassLoader[] classLoaders = getClassLoaders();
            for (ClassLoader classLoader : classLoaders) {
                Enumeration<URL> providerEnum = classLoader.getResources("META-INF/ipvstag.providers");
                while (providerEnum.hasMoreElements()) {
                    URL url = providerEnum.nextElement();
                    try {
                        loadResourceTagMappings(url);
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void loadResourceTagMappings(String xmlFilePath) {
        /*
         <tagMapping>
            <name>CreateCollection</name>
            <class>com.ipvsserv.nxd.migration.taq.impl.CreateCollectionTask</class>
        </tagMapping>
         */
        try {
            Document doc = Dom4jUtils.loadXMLResource(xmlFilePath);
            List<?> lsit = doc.getRootElement().elements("tagMapping");
            for (Iterator<?> iterator = lsit.iterator(); iterator.hasNext();) {
                try {
                    Element el = (Element) iterator.next();
                    String tagName = Dom4jUtils.value(el, "name", "");
                    String className = Dom4jUtils.value(el, "class", "");
                    Class<?> cls = Thread.currentThread().getContextClassLoader().loadClass(className);
                    addMigrationTag(tagName, cls);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public void loadResourceTagMappings(URL xmlFilePath) {
        /*
         <tagMapping>
            <name>CreateCollection</name>
            <class>com.ipvsserv.nxd.migration.taq.impl.CreateCollectionTask</class>
        </tagMapping>
         */
        try {
            Document doc = Dom4jUtils.getDocument(xmlFilePath);
            List<?> lsit = doc.getRootElement().elements("tagMapping");
            for (Iterator<?> iterator = lsit.iterator(); iterator.hasNext();) {
                try {
                    Element el = (Element) iterator.next();
                    String tagName = Dom4jUtils.value(el, "name", "");
                    String className = Dom4jUtils.value(el, "class", "");
                    Class<?> cls = Thread.currentThread().getContextClassLoader().loadClass(className);
                    addMigrationTag(tagName, cls);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    ////////////////// set / get ///////////////////
    public MigrationTag getMigrationTag(String tagName) throws InstantiationException, IllegalAccessException, ClassNotFoundException {
        return getMigrationTag(tagName, Thread.currentThread().getContextClassLoader());
    }

    public MigrationTag getMigrationTag(String tagName, Class<?> classLoaderC) throws InstantiationException, IllegalAccessException, ClassNotFoundException {
        return getMigrationTag(tagName, classLoaderC.getClassLoader());
    }

    public MigrationTag getMigrationTag(String tagName, ClassLoader classLoader) throws InstantiationException, IllegalAccessException, ClassNotFoundException {
        tagName = checkTagName(tagName);
        if (tagClassList.containsKey(tagName)) {
            Class<?> cls = tagClassList.get(tagName);
            return (MigrationTag) cls.newInstance();
        }
        String className = getTagClassName(tagName);
        Class<?> cls = classLoader.loadClass(className);
        return addMigrationTag(tagName, cls);
    }

    //////////
    public MigrationTag addMigrationTag(String tagName, Class<?> cls) throws InstantiationException, IllegalAccessException {
        if (cls != null) {
            Object obj = cls.newInstance();
            if (obj instanceof MigrationTag) {
                tagClassList.put(tagName, cls);
                return (MigrationTag) cls.newInstance();
            }
        }
        throw new ClassCastException("Invalid class");
    }

    ////////////////
    private String checkTagName(String tag) {
        if (!tag.startsWith(TAG_PREFIX)) {
            return tag;
        }
        String tagName = tag.substring(tag.indexOf('_') + 1);
        return tagName;
    }

    public String getTagClassName(String tag) {
        return "com.ipvsserv.nxd.migration.tag.impl." + tag + "Tag";
    }
}
