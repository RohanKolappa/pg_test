package com.ipvsserv.common.config.watch;

import java.util.HashMap;
import java.util.Map;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DirectorySnapshot {
    
    private static Map<String,String> files = new HashMap<String,String>();
    
    public static void addFile(String fileName) {
        files.put(fileName, fileName);
    }
    
    public static void removeFile(String fileName) {
        files.remove(fileName);
    }
    
    public static boolean containsFile(String fileName) {
        return files.containsKey(fileName);
    }
}
