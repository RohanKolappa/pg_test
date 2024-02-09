package com.ipvsserv.common.certificate;

import java.security.KeyStore;
import java.security.cert.X509Certificate;
import java.util.List;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface CertificateEventListener {
    
    void certificateCreated(KeyStore keyStore, String alias, X509Certificate cert);
    
    void certificateDeleted(KeyStore keyStore, String alias);
    
    void certificateSigned(KeyStore keyStore, String alias, List<X509Certificate> certificates);
}
