package com.ipvsserv.xmpp.mdc;

import org.xmpp.packet.IQ;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface IQHandler extends Runnable {
    public boolean process(IQ iq);
}
