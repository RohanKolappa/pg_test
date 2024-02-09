package com.ipvsserv.nxd.mdc.notif;

import org.xmpp.packet.Message;

import com.ipvsserv.xmpp.mdc.NotificationsRouter;
import com.ipvsserv.xmpp.mdc.PresenceHandler;
import com.ipvsserv.xmpp.session.XmppSessionImpl;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class NotificationsRouterImpl implements NotificationsRouter {
    //private PresenceHandlerI presenceHandler=null;
    private XmppSessionImpl xmppSession = null;
    
    public NotificationsRouterImpl(PresenceHandler presenceHandler, XmppSessionImpl xmppSession) {
        this.xmppSession = xmppSession;
        //this.presenceHandler =presenceHandler;
    }
    
    public void route(Message packet) throws Exception {
        //todo: for component have pool of components to route notification message
        xmppSession.sendPacket(packet);
    }
}
