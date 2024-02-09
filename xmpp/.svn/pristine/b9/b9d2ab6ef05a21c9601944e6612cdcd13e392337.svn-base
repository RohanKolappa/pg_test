package com.ipvsserv.nxd.mdc.comp;

import java.util.Iterator;
import java.util.List;

import org.xmpp.packet.IQ;
import org.xmpp.packet.Presence;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.mdc.iq.DomainMDC;
import com.ipvsserv.nxd.mdc.iq.IQHandlerMDCImpl;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.xmpp.mdc.IQHandler;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PubSubEngineImpl extends PubSubEngineBase {
    public PubSubEngineImpl(String ID, String componentId) {
        super(ID, componentId);
        isProcessInThreadDisable = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".thread.disable", false);
        //MRLog.debug("#################################PubSubEngineSednaMultiCompImpl constructor called");
    }
    
    //////////////////// PresenceListenerI ////////////////
    @Override
    public void unavailableEvent(Presence presence, String jid) {
        List<String> list = domainManager.getDomainNames();
        for (Iterator<String> i = list.iterator(); i.hasNext();) {
            String domainName = i.next();
            DomainMDC domain = (DomainMDC) domainManager.getDomain(domainName);
            if (domain != null) {
                domain.unsubscribePresence(presenceHandler, presence, jid, domainName);
            }
        }
    }
    
    ////////////////////process IQ //////////////// 
    private Object object = new Object();
    @Override
    public boolean process(IQ iq) {
        try {
            IQHandler handler = new IQHandlerMDCImpl(iq, MRLog.getThreadName(), this, domainManager);
            if (isProcessInThreadDisable) {
                synchronized (object) {
                    return handler.process(iq);
                }
            } else {
                threadPool.execute(handler);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            iq = null;
        }
        return true;
        
    }
    
    ////////////// start / stop //////////////////////
    protected void startEngine() throws Exception {
        //check sedna was connected and first read to db went fine
        dbInititated(); //make sure to load domains - if sedna was never up and connected
    }
    
    protected void shutdownEngine() throws Exception {
        try {
            // reset vars so that restart will really empty cached data
            isDBInititated = false;
            domainManager.clear();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        try {
            presenceHandler.clear();
            if(threadPool!=null){
                threadPool.shutdown();
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        threadPool=null;
        
    }
    
    ////////////// dbInititated //////////////////////
    public synchronized void dbInititated() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        if (isDBInititated) {
            return;
        }
        try {
            if (!IpvsServer.getInstance().isDBReady()) {
                return;
            }
            List<String> domainNames = getDBApi().getDomainNames();
            for (Iterator<String> iterator = domainNames.iterator(); iterator.hasNext();) {
                String domainName = iterator.next();
                DomainMDC domain = (DomainMDC) domainManager.addDomain(domainName);
                domain.getSubs().setDBApi(getDBApi());
                domain.loadSubscriptions();
            }
            isDBInititated = true;
        } catch (DBNotConnectedException ex) {
            isDBInititated = false;
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            isDBInititated = false;
            throw ex;
        } catch (DBExecutionException ex) {
            isDBInititated = false;
            throw ex;
        } catch (Exception ex) {
            MRLog.error(ex);
            isDBInititated = false;
        }
        
    }
    
}
