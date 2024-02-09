import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import org.jboss.resteasy.plugins.server.servlet.HttpServletDispatcher;

import com.ipvs.common.MRLog;
import com.ipvs.http.server.IPVSHttpServer;


public class TestHttpServerLocally {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		IPVSHttpServer httpServer = new IPVSHttpServer(10088, MRLog.getInstance(MRLog.CONSOLE));
	    ServletContextHandler servletContextHandler = new ServletContextHandler(httpServer, "/controller", true, false);
		
		
		ServletHolder servletDispatcher = new ServletHolder(HttpServletDispatcher.class);		
		ServletHolder servletDispatcher1 = new ServletHolder(TestServlet.class);		
		
		//Instead of restreasy.scan we are using resteasy.resources for this release.
		//Once we have created structures for web-inf etc we will use resteasy.scan 
		servletDispatcher.setInitParameter("resteasy.resources", TestHttpAsynch.class.getName());
	    servletContextHandler.addServlet(servletDispatcher , "/*");	
	    servletContextHandler.addServlet(servletDispatcher1 , "/servlet");	
		httpServer.startServer();
		try {
			httpServer.join();
		} catch (InterruptedException e) {
			
			e.printStackTrace();
		}

	}

}
