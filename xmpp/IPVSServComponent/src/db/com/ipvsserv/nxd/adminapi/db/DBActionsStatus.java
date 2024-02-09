package com.ipvsserv.nxd.adminapi.db;


public class DBActionsStatus {
   // private static Hashtable<String, Object> pendingList = new Hashtable<String, Object>();

    public static void addToPendingList(String key, Object value) {
        /*synchronized (pendingList) {
            pendingList.put(key, value);
        }*/
    }

    public static void remveFromPendingList(String key) {
       /* synchronized (pendingList) {
            pendingList.remove(key);
        }*/
    }

    public static boolean isFromPendingList(String key) {
       /* synchronized (pendingList) {
            return pendingList.containsKey(key);
        }*/
        return false;
    }
}
