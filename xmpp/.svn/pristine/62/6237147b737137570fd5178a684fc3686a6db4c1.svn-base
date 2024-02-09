package org.jivesoftware.openfire.user;

import org.jivesoftware.openfire.XMPPServer;

public class CreateServerStartedStatus extends Thread {
    public void run(){
        try{
            IpvsFileStatus initializingFile = new IpvsFileStatus (IpvsFileStatus.STATUS_INITIALIZING_FILE);
            XMPPServer instance = XMPPServer.getInstance();
            long curTime= System.currentTimeMillis();
            int timeout=100*1000;
            while(true){
                if(instance.isStarted() ) {
                    MRDebug.log("removeStatusFile : " + IpvsFileStatus.STATUS_INITIALIZING_FILE);
                    initializingFile.removeStatusFile(true);
                    break;
                }
                long elapsed=System.currentTimeMillis()-curTime;
                if(elapsed >timeout ){
                    MRDebug.log("TIMED OUT removeStatusFile :" + IpvsFileStatus.STATUS_INITIALIZING_FILE);
                    break;
                }
                Thread.sleep(200);
            }
            
        }catch(Exception ex){
            ex.printStackTrace();
        }
    }
}
