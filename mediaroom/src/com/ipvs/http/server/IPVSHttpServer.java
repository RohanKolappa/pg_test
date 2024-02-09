package com.ipvs.http.server;

import org.eclipse.jetty.server.Server;

import com.ipvs.common.MRLog;

public class IPVSHttpServer extends Server {

	MRLog log;


	public IPVSHttpServer(final int port,final MRLog log) {
		super(port);
		this.log=log;		
	}

	public void startServer() {

		try {
			this.start();			
			log.addInfo("IPVSHttpServer: Started JettyServer",MRLog.INFO,MRLog.NOTIFY);	    

		} catch(Exception e) {
			e.printStackTrace();
			log.addInfo("IPVSHttpServer: Start HttpServer Failed",MRLog.INFO,MRLog.NOTIFY);	    	
		}

	}

	public void stopServer() {

		try {
			this.stop();
			log.addInfo("IPVSHttpServer: Stopped Jetty Server",MRLog.INFO,MRLog.NOTIFY);	
		} catch (Exception e) {
			log.addInfo("IPVSHttpServer: Stop HttpServer Failed",MRLog.INFO,MRLog.NOTIFY);
			e.printStackTrace();
		}

	}





}
