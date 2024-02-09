package com.ipvsserv.plugin.http;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import javax.servlet.GenericServlet;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.jasper.JasperException;
import org.apache.jasper.JspC;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;
import org.xml.sax.SAXException;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.plugin.Plugin;
import com.ipvsserv.plugin.PluginDevEnvironment;
import com.ipvsserv.plugin.PluginManager;
import com.ipvsserv.starter.IpvsServClassLoader;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PluginServlet extends HttpServlet {
    
    private static final long serialVersionUID = -4134550327410483503L;
    private static Map<String, GenericServlet> servlets;
    private static PluginManager pluginManager;
    private static ServletConfig servletConfig;
    
    static {
        servlets = new ConcurrentHashMap<String, GenericServlet>();
    }
    
    public void init(ServletConfig config) throws ServletException {
        super.init(config);
        servletConfig = config;
    }
    
    public void service(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        String pathInfo = request.getPathInfo();
        if (pathInfo == null) {
            response.setStatus(HttpServletResponse.SC_NOT_FOUND);
        } else {
            try {
                // Handle JSP requests.
                if (pathInfo.endsWith(".jsp")) {
                    if (handleDevJSP(pathInfo, request, response)) {
                        return;
                    }
                    handleJSP(pathInfo, request, response);
                }
                // Handle servlet requests.
                else if (getServlet(pathInfo) != null) {
                    handleServlet(pathInfo, request, response);
                }
                // Handle image/other requests.
                else {
                    handleOtherRequest(pathInfo, response);
                }
            } catch (Exception ex) {
                MRLog.error(ex);
                response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
            }
        }
    }
    
    public static void registerServlets(PluginManager manager, Plugin plugin, File webXML) {
        pluginManager = manager;
        if (!webXML.exists()) {
            MRLog.error("Could not register plugin servlets, file " + webXML.getAbsolutePath() + " does not exist.");
            return;
        }
        // Find the name of the plugin directory given that the webXML file
        // lives in plugins/[pluginName]/web/web.xml
        String pluginName = webXML.getParentFile().getParentFile().getParentFile().getName();
        try {
            // Make the reader non-validating so that it doesn't try to resolve external
            // DTD's. Trying to resolve external DTD's can break on some firewall configurations.
            SAXReader saxReader = new SAXReader(false);
            try {
                saxReader.setFeature("http://apache.org/xml/features/nonvalidating/load-external-dtd", false);
            } catch (SAXException e) {
                MRLog.warn("Error setting SAXReader feature", e);
            }
            Document doc = saxReader.read(webXML);
            // Find all <servlet> entries to discover name to class mapping.
            List classes = doc.selectNodes("//servlet");
            Map<String, Class> classMap = new HashMap<String, Class>();
            for (int i = 0; i < classes.size(); i++) {
                Element servletElement = (Element) classes.get(i);
                String name = servletElement.element("servlet-name").getTextTrim();
                String className = servletElement.element("servlet-class").getTextTrim();
                classMap.put(name, manager.loadClass(plugin, className));
            }
            // Find all <servelt-mapping> entries to discover name to URL mapping.
            List names = doc.selectNodes("//servlet-mapping");
            for (int i = 0; i < names.size(); i++) {
                Element nameElement = (Element) names.get(i);
                String name = nameElement.element("servlet-name").getTextTrim();
                String url = nameElement.element("url-pattern").getTextTrim();
                // Register the servlet for the URL.
                Class servletClass = classMap.get(name);
                if (servletClass == null) {
                    MRLog.error("Unable to load servlet, " + name + ", servlet-class not found.");
                    continue;
                }
                Object instance = servletClass.newInstance();
                if (instance instanceof GenericServlet) {
                    // Initialize the servlet then add it to the map..
                    ((GenericServlet) instance).init(servletConfig);
                    servlets.put(pluginName + url, (GenericServlet) instance);
                } else {
                    MRLog.warn("Could not load " + (pluginName + url) + ": not a servlet.");
                }
            }
        } catch (Throwable e) {
            MRLog.error(e);
        }
    }
    
    public static void unregisterServlets(File webXML) {
        if (!webXML.exists()) {
            MRLog.error("Could not unregister plugin servlets, file " + webXML.getAbsolutePath() + " does not exist.");
            return;
        }
        // Find the name of the plugin directory given that the webXML file
        // lives in plugins/[pluginName]/web/web.xml
        String pluginName = webXML.getParentFile().getParentFile().getParentFile().getName();
        try {
            SAXReader saxReader = new SAXReader(false);
            saxReader.setFeature("http://apache.org/xml/features/nonvalidating/load-external-dtd", false);
            Document doc = saxReader.read(webXML);
            // Find all <servelt-mapping> entries to discover name to URL mapping.
            List names = doc.selectNodes("//servlet-mapping");
            for (int i = 0; i < names.size(); i++) {
                Element nameElement = (Element) names.get(i);
                String url = nameElement.element("url-pattern").getTextTrim();
                // Destroy the servlet than remove from servlets map.
                GenericServlet servlet = servlets.get(pluginName + url);
                if (servlet != null) {
                    servlet.destroy();
                }
                servlets.remove(pluginName + url);
                servlet = null;
            }
        } catch (Throwable e) {
            MRLog.error(e);
        }
    }
    
    private void handleJSP(String pathInfo, HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        // Strip the starting "/" from the path to find the JSP URL.
        String jspURL = pathInfo.substring(1);
        
        GenericServlet servlet = servlets.get(jspURL);
        if (servlet != null) {
            servlet.service(request, response);
        } else {
            response.setStatus(HttpServletResponse.SC_NOT_FOUND);
        }
    }
    
    private void handleServlet(String pathInfo, HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        // Strip the starting "/" from the path to find the JSP URL.
        GenericServlet servlet = getServlet(pathInfo);
        if (servlet != null) {
            servlet.service(request, response);
        } else {
            response.setStatus(HttpServletResponse.SC_NOT_FOUND);
        }
    }
    
    private GenericServlet getServlet(String pathInfo) {
        pathInfo = pathInfo.substring(1).toLowerCase();
        
        GenericServlet servlet = servlets.get(pathInfo);
        if (servlet == null) {
            for (String key : servlets.keySet()) {
                int index = key.indexOf("/*");
                String searchkey = key;
                if (index != -1) {
                    searchkey = key.substring(0, index);
                }
                if (searchkey.startsWith(pathInfo) || pathInfo.startsWith(searchkey)) {
                    servlet = servlets.get(key);
                    break;
                }
            }
        }
        return servlet;
    }
    
    private void handleOtherRequest(String pathInfo, HttpServletResponse response) throws Exception {
        String[] parts = pathInfo.split("/");
        // Image request must be in correct format.
        if (parts.length < 3) {
            response.setStatus(HttpServletResponse.SC_NOT_FOUND);
            return;
        }
        
        String contextPath = "";
        int index = pathInfo.indexOf(parts[1]);
        if (index != -1) {
            contextPath = pathInfo.substring(index + parts[1].length());
        }
        
        File pluginDirectory = new File(IpvsServGlobals.getPluginsDir());
        File file = new File(pluginDirectory, parts[1] + File.separator + "web" + contextPath);
        
        // When using dev environment, the images dir may be under something other that web.
        Plugin plugin = pluginManager.getPlugin(parts[1]);
        PluginDevEnvironment environment = pluginManager.getDevEnvironment(plugin);
        
        if (environment != null) {
            file = new File(environment.getWebRoot(), contextPath);
        }
        if (!file.exists()) {
            response.setStatus(HttpServletResponse.SC_NOT_FOUND);
            return;
        } else {
            // Content type will be GIF or PNG.
            String contentType = "image/gif";
            if (pathInfo.endsWith(".png")) {
                contentType = "image/png";
            } else if (pathInfo.endsWith(".swf")) {
                contentType = "application/x-shockwave-flash";
            } else if (pathInfo.endsWith(".css")) {
                contentType = "text/css";
            } else if (pathInfo.endsWith(".js")) {
                contentType = "text/javascript";
            } else if (pathInfo.endsWith(".html") || pathInfo.endsWith(".htm")) {
                contentType = "text/html";
            }
            
            // setting the content-disposition header breaks IE when downloading CSS
            // response.setHeader("Content-disposition", "filename=\"" + file + "\";");
            response.setContentType(contentType);
            // Write out the resource to the user.
            InputStream in = null;
            ServletOutputStream out = null;
            try {
                in = new BufferedInputStream(new FileInputStream(file));
                out = response.getOutputStream();
                
                // Set the size of the file.
                response.setContentLength((int) file.length());
                
                // Use a 1K buffer.
                byte[] buf = new byte[1024];
                int len;
                while ((len = in.read(buf)) != -1) {
                    out.write(buf, 0, len);
                }
            } finally {
                try {
                    in.close();
                } catch (Exception ignored) {
                    // Ignore.
                }
                try {
                    out.close();
                } catch (Exception ignored) {
                    // Ignore.
                }
            }
        }
    }
    
    private boolean handleDevJSP(String pathInfo, HttpServletRequest request, HttpServletResponse response) {
        String jspURL = pathInfo.substring(1);
        
        // Handle pre-existing pages and fail over to pre-compiled pages.
        int fileSeperator = jspURL.indexOf("/");
        if (fileSeperator != -1) {
            String pluginName = jspURL.substring(0, fileSeperator);
            Plugin plugin = pluginManager.getPlugin(pluginName);
            
            PluginDevEnvironment environment = pluginManager.getDevEnvironment(plugin);
            // If development mode not turned on for plugin, return false.
            if (environment == null) {
                return false;
            }
            File webDir = environment.getWebRoot();
            if (webDir == null || !webDir.exists()) {
                return false;
            }
            
            File pluginDirectory = pluginManager.getPluginDirectory(plugin);
            
            File compilationDir = new File(pluginDirectory, "classes");
            compilationDir.mkdirs();
            
            String jsp = jspURL.substring(fileSeperator + 1);
            
            int indexOfLastSlash = jsp.lastIndexOf("/");
            String relativeDir = "";
            if (indexOfLastSlash != -1) {
                relativeDir = jsp.substring(0, indexOfLastSlash);
                relativeDir = relativeDir.replaceAll("//", ".") + '.';
            }
            
            File jspFile = new File(webDir, jsp);
            String filename = jspFile.getName();
            int indexOfPeriod = filename.indexOf(".");
            if (indexOfPeriod != -1) {
                filename = "dev" + CompUtils.randomString(4);
            }
            
            JspC jspc = new JspC();
            if (!jspFile.exists()) {
                return false;
            }
            try {
                jspc.setJspFiles(jspFile.getCanonicalPath());
            } catch (IOException ex) {
                MRLog.error(ex);
            }
            jspc.setOutputDir(compilationDir.getAbsolutePath());
            jspc.setClassName(filename);
            jspc.setCompile(true);
            
            jspc.setClassPath(getClasspathForPlugin(plugin));
            try {
                jspc.execute();
                
                try {
                    Object servletInstance = pluginManager.loadClass(plugin, "org.apache.jsp." + relativeDir + filename).newInstance();
                    HttpServlet servlet = (HttpServlet) servletInstance;
                    servlet.init(servletConfig);
                    servlet.service(request, response);
                    return true;
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
                
            } catch (JasperException ex) {
                MRLog.error(ex);
            }
        }
        return false;
    }
    
    private static String getClasspathForPlugin(Plugin plugin) {
        final StringBuilder classpath = new StringBuilder();
        
        File pluginDirectory = pluginManager.getPluginDirectory(plugin);
        
        PluginDevEnvironment pluginEnv = pluginManager.getDevEnvironment(plugin);
        
        IpvsServClassLoader pluginClassloader = pluginManager.getPluginClassloader(plugin);
        
        for (URL url : pluginClassloader.getURLs()) {
            File file = new File(url.getFile());
            
            classpath.append(file.getAbsolutePath()).append(";");
        }
        
        // Load all jars from lib
        File libDirectory = new File(pluginDirectory, "lib");
        File[] libs = libDirectory.listFiles();
        final int no = libs != null ? libs.length : 0;
        for (int i = 0; i < no; i++) {
            File libFile = libs[i];
            classpath.append(libFile.getAbsolutePath()).append(';');
        }
        
        File openfireRoot = pluginDirectory.getParentFile().getParentFile().getParentFile();
        File openfireLib = new File(openfireRoot, "target//lib");
        
        classpath.append(openfireLib.getAbsolutePath()).append("//servlet.jar;");
        classpath.append(openfireLib.getAbsolutePath()).append("//ipvscomponent.jar;");
        classpath.append(openfireLib.getAbsolutePath()).append("//jasper-compiler.jar;");
        classpath.append(openfireLib.getAbsolutePath()).append("//jasper-runtime.jar;");
        
        if (pluginEnv.getClassesDir() != null) {
            classpath.append(pluginEnv.getClassesDir().getAbsolutePath()).append(";");
        }
        return classpath.toString();
    }
}
