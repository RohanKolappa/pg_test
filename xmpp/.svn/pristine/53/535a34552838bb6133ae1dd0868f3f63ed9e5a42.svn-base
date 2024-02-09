package com.ipvsserv.starter;

import java.util.ArrayList;
import java.util.Hashtable;

public class ProcessArguments {
    private ArrayList<String> argList = null;
    private Hashtable<String, String> argsParam = null;
    
    public ProcessArguments(String[] args) {
        argList = new ArrayList<String>();
        argsParam = new Hashtable<String, String>();
        if (args == null) return;
        try {
            for (int i = 0; i < args.length; i++) {
                String arg = args[i];
                int iPos = arg.trim().indexOf("-");
                if (iPos == 0) {
                    String str = arg.replace('-', ' ').trim();
                    iPos = str.indexOf("=");
                    if (iPos != -1) {
                        String key = str.substring(0, iPos);
                        String value = str.substring(iPos + 1);
                        argsParam.put(key, value);
                    } else {
                        argsParam.put(str, "");
                    }
                    
                } else {
                    argList.add(arg);
                }
            }
        } catch (Exception ex) {

        }
    }
    
    public String[] getArgumentList() {
        String[] a = null;
        return argList.toArray(a);
    }
    
    public Hashtable<String, String> getArgumentParams() {
        return argsParam;
    }
    
    public String getArgumentParam(String key, String sdefault) {
        if (argsParam.containsKey(key)) {
            return argsParam.get(key);
        }
        return sdefault;
    }
}
