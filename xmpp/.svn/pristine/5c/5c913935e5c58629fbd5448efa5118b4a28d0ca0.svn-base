package com.ipvsserv.common.config;

import java.util.List;
import java.util.Map;
import java.util.concurrent.CopyOnWriteArrayList;

import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PropertyEventDispatcher {
    
    private static List<PropertyEventListener> listeners = new CopyOnWriteArrayList<PropertyEventListener>();
    
    private PropertyEventDispatcher() {
    // Not instantiable.
    }
    
    public static void addListener(PropertyEventListener listener) {
        if (listener == null) {
            throw new NullPointerException();
        }
        listeners.add(listener);
    }
    
    public static void removeListener(PropertyEventListener listener) {
        listeners.remove(listener);
    }
    
    public static void dispatchEvent(String property, EventType eventType, Map<String, Object> params) {
        for (PropertyEventListener listener : listeners) {
            try {
                switch (eventType) {
                    case property_set: {
                        listener.propertySet(property, params);
                        break;
                    }
                    case property_deleted: {
                        listener.propertyDeleted(property, params);
                        break;
                    }
                    case xml_property_set: {
                        listener.xmlPropertySet(property, params);
                        break;
                    }
                    case xml_property_deleted: {
                        listener.xmlPropertyDeleted(property, params);
                        break;
                    }
                    default:
                        break;
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
    }
    
    public enum EventType {
        
        /**
         * A property was set.
         */
        property_set,

        /**
         * A property was deleted.
         */
        property_deleted,

        /**
         * An XML property was set.
         */
        xml_property_set,

        /**
         * An XML property was deleted.
         */
        xml_property_deleted
    }
}
