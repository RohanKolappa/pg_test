package com.ipvsserv.plugin.http;

import javax.servlet.http.HttpServletRequest;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class ParamUtils {
    
    public static String getParameter(HttpServletRequest request, String name) {
        return getParameter(request, name, false);
    }
    
    public static String getParameter(HttpServletRequest request, String name, boolean emptyStringsOK) {
        String temp = request.getParameter(name);
        if (temp != null) {
            if (temp.equals("") && !emptyStringsOK) {
                return null;
            } else {
                return temp;
            }
        } else {
            return null;
        }
    }
    
    public static String[] getParameters(HttpServletRequest request, String name) {
        if (name == null) {
            return new String[0];
        }
        String[] paramValues = request.getParameterValues(name);
        if (paramValues == null || paramValues.length == 0) {
            return new String[0];
        } else {
            java.util.List<String> values = new java.util.ArrayList<String>(paramValues.length);
            for (int i = 0; i < paramValues.length; i++) {
                if (paramValues[i] != null && !"".equals(paramValues[i])) {
                    values.add(paramValues[i]);
                }
            }
            return values.toArray(new String[] {});
        }
    }
    
    public static boolean getBooleanParameter(HttpServletRequest request, String name) {
        return getBooleanParameter(request, name, false);
    }
    
    public static boolean getBooleanParameter(HttpServletRequest request, String name, boolean defaultVal) {
        String temp = request.getParameter(name);
        if ("true".equals(temp) || "on".equals(temp)) {
            return true;
        } else if ("false".equals(temp) || "off".equals(temp)) {
            return false;
        } else {
            return defaultVal;
        }
    }
    
    public static int getIntParameter(HttpServletRequest request, String name, int defaultNum) {
        String temp = request.getParameter(name);
        if (temp != null && !temp.equals("")) {
            int num = defaultNum;
            try {
                num = Integer.parseInt(temp);
            } catch (Exception ignored) {}
            return num;
        } else {
            return defaultNum;
        }
    }
    
    public static int[] getIntParameters(HttpServletRequest request, String name, int defaultNum) {
        String[] paramValues = request.getParameterValues(name);
        if (paramValues == null || paramValues.length == 0) {
            return new int[0];
        }
        int[] values = new int[paramValues.length];
        for (int i = 0; i < paramValues.length; i++) {
            try {
                values[i] = Integer.parseInt(paramValues[i]);
            } catch (Exception ex) {
                values[i] = defaultNum;
            }
        }
        return values;
    }
    
    public static double getDoubleParameter(HttpServletRequest request, String name, double defaultNum) {
        String temp = request.getParameter(name);
        if (temp != null && !temp.equals("")) {
            double num = defaultNum;
            try {
                num = Double.parseDouble(temp);
            } catch (Exception ignored) {}
            return num;
        } else {
            return defaultNum;
        }
    }
    
    public static long getLongParameter(HttpServletRequest request, String name, long defaultNum) {
        String temp = request.getParameter(name);
        if (temp != null && !temp.equals("")) {
            long num = defaultNum;
            try {
                num = Long.parseLong(temp);
            } catch (Exception ignored) {}
            return num;
        } else {
            return defaultNum;
        }
    }
    
    public static long[] getLongParameters(HttpServletRequest request, String name, long defaultNum) {
        String[] paramValues = request.getParameterValues(name);
        if (paramValues == null || paramValues.length == 0) {
            return new long[0];
        }
        long[] values = new long[paramValues.length];
        for (int i = 0; i < paramValues.length; i++) {
            try {
                values[i] = Long.parseLong(paramValues[i]);
            } catch (Exception ex) {
                values[i] = defaultNum;
            }
        }
        return values;
    }
    
    public static String getAttribute(HttpServletRequest request, String name) {
        return getAttribute(request, name, false);
    }
    
    public static String getAttribute(HttpServletRequest request, String name, boolean emptyStringsOK) {
        String temp = (String) request.getAttribute(name);
        if (temp != null) {
            if (temp.equals("") && !emptyStringsOK) {
                return null;
            } else {
                return temp;
            }
        } else {
            return null;
        }
    }
    
    public static boolean getBooleanAttribute(HttpServletRequest request, String name) {
        String temp = (String) request.getAttribute(name);
        if (temp != null && temp.equals("true")) {
            return true;
        } else {
            return false;
        }
    }
    
    public static int getIntAttribute(HttpServletRequest request, String name, int defaultNum) {
        String temp = (String) request.getAttribute(name);
        if (temp != null && !temp.equals("")) {
            int num = defaultNum;
            try {
                num = Integer.parseInt(temp);
            } catch (Exception ignored) {}
            return num;
        } else {
            return defaultNum;
        }
    }
    
    public static long getLongAttribute(HttpServletRequest request, String name, long defaultNum) {
        String temp = (String) request.getAttribute(name);
        if (temp != null && !temp.equals("")) {
            long num = defaultNum;
            try {
                num = Long.parseLong(temp);
            } catch (Exception ignored) {}
            return num;
        } else {
            return defaultNum;
        }
    }
}
