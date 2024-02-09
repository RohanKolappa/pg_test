package com.ipvsserv.xmpp.mdc;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IQHandleException extends Exception {
    private static final long serialVersionUID = -6456089296071297858L;
    
    public IQHandleException(String error) {
        super(error);
    }
}
