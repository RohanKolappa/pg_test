package com.ipvsserv.plugin.http;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.ipvsserv.plugin.Plugin;
import com.ipvsserv.plugin.PluginManager;
import com.ipvsserv.plugin.PluginManagerImpl;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PluginIconServlet extends HttpServlet {
    
    private static final long serialVersionUID = -3794672230308794895L;
    
    public void init(ServletConfig config) throws ServletException {
        super.init(config);
    }
    
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException {
        String pluginName = ParamUtils.getParameter(request, "plugin");
        //PluginManager pluginManager = IpvsServer.getInstance().getPluginManager();
        PluginManager pluginManager = (PluginManager) IpvsServer.getInstance().getModule(PluginManagerImpl.class);
        Plugin plugin = pluginManager.getPlugin(pluginName);
        if (plugin != null) {
            // Try looking for PNG file first then default to GIF.
            File icon = new File(pluginManager.getPluginDirectory(plugin), "logo_small.png");
            boolean isPng = true;
            if (!icon.exists()) {
                icon = new File(pluginManager.getPluginDirectory(plugin), "logo_small.gif");
                isPng = false;
            }
            if (icon.exists()) {
                // Clear any empty lines added by the JSP declaration. This is required to show
                // the image in resin!
                response.reset();
                if (isPng) {
                    response.setContentType("image/png");
                } else {
                    response.setContentType("image/gif");
                }
                InputStream in = null;
                OutputStream ost = null;
                try {
                    in = new FileInputStream(icon);
                    ost = response.getOutputStream();
                    
                    byte[] buf = new byte[1024];
                    int len;
                    while ((len = in.read(buf)) >= 0) {
                        ost.write(buf, 0, len);
                    }
                    ost.flush();
                } catch (IOException ioe) {

                } finally {
                    if (in != null) {
                        try {
                            in.close();
                        } catch (Exception ex) {}
                    }
                    if (ost != null) {
                        try {
                            ost.close();
                        } catch (Exception ex) {}
                    }
                }
            }
        }
    }
}
