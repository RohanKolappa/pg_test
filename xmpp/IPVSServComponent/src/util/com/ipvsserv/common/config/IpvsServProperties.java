package com.ipvsserv.common.config;

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsServProperties implements Map<String, String> {
    //todo:
    /*
    private static final String LOAD_PROPERTIES = "SELECT name, propValue FROM jiveProperty";
    private static final String INSERT_PROPERTY = "INSERT INTO jiveProperty(name, propValue) VALUES(?,?)";
    private static final String UPDATE_PROPERTY = "UPDATE jiveProperty SET propValue=? WHERE name=?";
    private static final String DELETE_PROPERTY = "DELETE FROM jiveProperty WHERE name LIKE ?";
     */
    private static class IpvsServPropertyHolder {
        private static final IpvsServProperties instance = new IpvsServProperties();
        static {
            instance.init();
        }
    }
    private Map<String, String> properties;
    public static IpvsServProperties getInstance() {
        return IpvsServPropertyHolder.instance;
    }
    private IpvsServProperties() {}
    public void init() {
        if (properties == null) {
            properties = new ConcurrentHashMap<String, String>();
        } else {
            properties.clear();
        }
        
        loadProperties();
    }
    public int size() {
        return properties.size();
    }
    
    public void clear() {
        throw new UnsupportedOperationException();
    }
    
    public boolean isEmpty() {
        return properties.isEmpty();
    }
    
    public boolean containsKey(Object key) {
        return properties.containsKey(key);
    }
    
    public boolean containsValue(Object value) {
        return properties.containsValue(value);
    }
    
    public Collection<String> values() {
        return Collections.unmodifiableCollection(properties.values());
    }
    
    public void putAll(Map<? extends String, ? extends String> t) {
        for (Map.Entry<? extends String, ? extends String> entry : t.entrySet()) {
            put(entry.getKey(), entry.getValue());
        }
    }
    
    public Set<Map.Entry<String, String>> entrySet() {
        return Collections.unmodifiableSet(properties.entrySet());
    }
    
    public Set<String> keySet() {
        return Collections.unmodifiableSet(properties.keySet());
    }
    
    public String get(Object key) {
        return properties.get(key);
    }
    public Collection<String> getChildrenNames(String parentKey) {
        Collection<String> results = new HashSet<String>();
        for (String key : properties.keySet()) {
            if (key.startsWith(parentKey + ".")) {
                if (key.equals(parentKey)) {
                    continue;
                }
                int dotIndex = key.indexOf(".", parentKey.length() + 1);
                if (dotIndex < 1) {
                    if (!results.contains(key)) {
                        results.add(key);
                    }
                } else {
                    String name = parentKey + key.substring(parentKey.length(), dotIndex);
                    results.add(name);
                }
            }
        }
        return results;
    }
 
    public Collection<String> getPropertyNames() {
        return properties.keySet();
    }
    
    public String remove(Object key) {
        String value;
        synchronized (this) {
            value = properties.remove(key);
            // Also remove any children.
            Collection<String> propNames = getPropertyNames();
            for (String name : propNames) {
                if (name.startsWith((String) key)) {
                    properties.remove(name);
                }
            }
            deleteProperty((String) key);
        }
        
        // Generate event.
        Map<String, Object> params = Collections.emptyMap();
        PropertyEventDispatcher.dispatchEvent((String) key, PropertyEventDispatcher.EventType.property_deleted, params);
        
        // Send update to other cluster members.
        //CacheFactory.doClusterTask(PropertyClusterEventTask.createDeteleTask((String) key));
        
        return value;
    }
    
    void localRemove(String key) {
        properties.remove(key);
        // Also remove any children.
        Collection<String> propNames = getPropertyNames();
        for (String name : propNames) {
            if (name.startsWith(key)) {
                properties.remove(name);
            }
        }
        
        // Generate event.
        Map<String, Object> params = Collections.emptyMap();
        PropertyEventDispatcher.dispatchEvent(key, PropertyEventDispatcher.EventType.property_deleted, params);
    }
    
    public String put(String key, String value) {
        if (key == null || value == null) {
            throw new NullPointerException("Key or value cannot be null. Key=" + key + ", value=" + value);
        }
        if (key.endsWith(".")) {
            key = key.substring(0, key.length() - 1);
        }
        key = key.trim();
        String result;
        synchronized (this) {
            if (properties.containsKey(key)) {
                if (!properties.get(key).equals(value)) {
                    updateProperty(key, value);
                }
            } else {
                insertProperty(key, value);
            }
            
            result = properties.put(key, value);
        }
        
        // Generate event.
        Map<String, Object> params = new HashMap<String, Object>();
        params.put("value", value);
        PropertyEventDispatcher.dispatchEvent(key, PropertyEventDispatcher.EventType.property_set, params);
        
        // Send update to other cluster members.
        //CacheFactory.doClusterTask(PropertyClusterEventTask.createPutTask(key, value));
        
        return result;
    }
    
    void localPut(String key, String value) {
        properties.put(key, value);
        
        // Generate event.
        Map<String, Object> params = new HashMap<String, Object>();
        params.put("value", value);
        PropertyEventDispatcher.dispatchEvent(key, PropertyEventDispatcher.EventType.property_set, params);
    }
    
    public String getProperty(String name, String defaultValue) {
        String value = properties.get(name);
        if (value != null) {
            return value;
        } else {
            return defaultValue;
        }
    }
    
    public boolean getBooleanProperty(String name) {
        return Boolean.valueOf(get(name));
    }
    
    public boolean getBooleanProperty(String name, boolean defaultValue) {
        String value = get(name);
        if (value != null) {
            return Boolean.valueOf(value);
        } else {
            return defaultValue;
        }
    }
    
    private void insertProperty(String name, String value) {
    /*Connection con = null;
    PreparedStatement pstmt = null;
    try {
        con = DbConnectionManager.getConnection();
        pstmt = con.prepareStatement(INSERT_PROPERTY);
        pstmt.setString(1, name);
        pstmt.setString(2, value);
        pstmt.executeUpdate();
    }
    catch (SQLException ex) {
        Log.error(e);
    }
    finally {
        try { if (pstmt != null) { pstmt.close(); } }
        catch (Exception ex) { Log.error(e); }
        try { if (con != null) { con.close(); } }
        catch (Exception ex) { Log.error(e); }
    }*/

    }
    
    private void updateProperty(String name, String value) {
    /*Connection con = null;
    PreparedStatement pstmt = null;
    try {
        con = DbConnectionManager.getConnection();
        pstmt = con.prepareStatement(UPDATE_PROPERTY);
        pstmt.setString(1, value);
        pstmt.setString(2, name);
        pstmt.executeUpdate();
    }
    catch (SQLException ex) {
        Log.error(e);
    }
    finally {
        try { if (pstmt != null) { pstmt.close(); } }
        catch (Exception ex) { Log.error(e); }
        try { if (con != null) { con.close(); } }
        catch (Exception ex) { Log.error(e); }
    }*/
    }
    
    private void deleteProperty(String name) {
    /*Connection con = null;
    PreparedStatement pstmt = null;
    try {
        con = DbConnectionManager.getConnection();
        pstmt = con.prepareStatement(DELETE_PROPERTY);
        pstmt.setString(1, name + "%");
        pstmt.executeUpdate();
    }
    catch (SQLException ex) {
        Log.error(e);
    }
    finally {
        try { if (pstmt != null) { pstmt.close(); } }
        catch (Exception ex) { Log.error(e); }
        try { if (con != null) { con.close(); } }
        catch (Exception ex) { Log.error(e); }
    }*/
    }
    
    private void loadProperties() {

    /*Connection con = null;
    PreparedStatement pstmt = null;
    try {
        con = DbConnectionManager.getConnection();
        pstmt = con.prepareStatement(LOAD_PROPERTIES);
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            String name = rs.getString(1);
            String value = rs.getString(2);
            properties.put(name, value);
        }
        rs.close();
    }
    catch (Exception ex) {
        Log.error(e);
    }
    finally {
        try { if (pstmt != null) { pstmt.close(); } }
        catch (Exception ex) { Log.error(e); }
        try { if (con != null) { con.close(); } }
        catch (Exception ex) { Log.error(e); }
    }*/
    }
}
