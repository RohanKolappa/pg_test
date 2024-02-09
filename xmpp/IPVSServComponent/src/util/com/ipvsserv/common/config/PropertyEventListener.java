package com.ipvsserv.common.config;

import java.util.Map;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface PropertyEventListener {
    
    public void propertySet(String property, Map<String, Object> params);
    
    public void propertyDeleted(String property, Map<String, Object> params);
    
    public void xmlPropertySet(String property, Map<String, Object> params);
    
    public void xmlPropertyDeleted(String property, Map<String, Object> params);
    
}
