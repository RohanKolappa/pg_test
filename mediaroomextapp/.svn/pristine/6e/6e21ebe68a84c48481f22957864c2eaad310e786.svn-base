package com.barco.utils.mr;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Random;
import java.util.Stack;
import java.util.StringTokenizer;
import java.util.UUID;


public class Utils {	
	private static final String INVALID_HOST_ADDRESS = "0.0.0.0";
    protected static final String COUNT_KEY = "COUNT_KEY_" + Long.toHexString(new Random().nextLong());    

    // execCommand(xmpp, "/usr/bin/wget -O - " + url, log)
    /* public static String wget(String urlstr, MRLog log) {
         BufferedReader in = null;
         StringBuffer response = new StringBuffer(256);
    
         try {
             URL url = new URL(urlstr);
             URLConnection yc = url.openConnection();
             in = new BufferedReader(new InputStreamReader(yc.getInputStream()));
    
             String input = null;
    
             while ((input = in.readLine()) != null) {
                 response.append(input + "\n");
             }
    
             log.addInfo(urlstr + " resp: " + response.toString());
             System.out.println(urlstr + " resp: " + response.toString() + "");
         } catch (Exception ex) {
             ex.printStackTrace();
         } finally {
             if (in != null) {
                 try {
                     in.close();
                 } catch (Exception ex) {
                 }
             }
    
             in = null;
         }
    
         return response.toString().trim();
     }
    */
    public static Properties loadProperties(String propFile) throws Exception {
        return loadPropertiesFile(new Properties(), propFile, Utils.class);
    }

    public static Properties loadProperties(Properties props, String propFile)
        throws Exception {
        return loadPropertiesFile(props, propFile, Utils.class);
    }

    public static Properties loadPropertiesFile(Properties props, String propFile, Class<?> c)
        throws Exception {
        InputStream in = null;

        try {
            in = new FileInputStream(new File(propFile));
            props.load(in);
            props.setProperty("props.inited", "true");

            return props;
        } catch (Exception ex) {
            //ex.printStackTrace();
            throw new Exception("Properties not found or invalid." + propFile);
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (Exception ex) {
                }
            }

            in = null;
        }
    }

    public static Properties loadPropertiesResource(Properties props, String propFile, Class<?> c)
        throws Exception {
        InputStream in = null;

        try {
            in = c.getResourceAsStream(propFile);
            props.load(in);
            props.setProperty("props.inited", "true");

            return props;
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new Exception("Properties not found or invalid." + propFile);
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (Exception ex) {
                }
            }

            in = null;
        }
    }

    public static String getCurrentFolderPath() {
        String path = "";
        FileWriter os = null;

        try {
            String name = "temp.log";
            File file = new File(name);
            os = new FileWriter(file);
            os.write(" Server started " + (new Date()) + " " + System.currentTimeMillis());
            path = file.getCanonicalPath().replace('\\', '/');

            if (path.indexOf(name) != 1) {
                path = path.substring(0, path.indexOf(name));

                if (path.endsWith("/")) {
                    path = path.substring(0, path.length() - 1);
                }
            }
        } catch (Exception ex) {
        } finally {
            if (os != null) {
                try {
                    os.close();
                } catch (Exception ex) {
                }
            }
        }

        return path.replace('\\', '/');
    }

    public static ArrayList<String> split(String str, String token) {
        ArrayList<String> arrayList = new ArrayList<String>();

        try {
            StringTokenizer tok = new StringTokenizer(str, token);

            //int iCount = 0;
            //String nodeTempPrev = "";
            while (tok.hasMoreTokens()) {
                String nodeTemp = tok.nextToken();
                arrayList.add(nodeTemp);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }

        return arrayList;
    }

    public static String decode(String data) throws UnsupportedEncodingException {
        if (data != null) {
            return URLDecoder.decode(data, "UTF-8");
        }
        return data;
    }

    public static String encode(String data) throws UnsupportedEncodingException {
        if (data != null) {
            return URLEncoder.encode(data, "UTF-8");
        }
        return data;
 
    }

    public static String getNotNull(String val, String sdefault) {
        if (val == null) {
            return sdefault;
        }

        return val;
    }

    public static boolean getBooleanValue(String val, boolean bdefault) {
        if (val==null|| val.trim().equals("")) {
            return bdefault;
        }
        val = val.trim().toLowerCase();
        if (val.equals("true") || val.equals("1")) {
            return true;
        }
        if (val.equals("false") || val.equals("0")) {
            return false;
        }
        return bdefault;
    }
    
    public static boolean getBoolean(String propName, boolean bdefault) {
        String propValue = getNotNull(propName, bdefault + "");
        try {
        	return Boolean.parseBoolean(propValue);
        }catch(Exception exp){
        	
        }
        return  bdefault; 
    }


    public static int getIntValue(String propName, int idefault) {
        String propValue = getNotNull(propName, idefault + "");

        try {
            return Integer.parseInt(propValue);
        } catch (Exception ex) {
        }

        return idefault;
    }
    
    public static double getDoubleValue(String propName, double ddefault) {
        String propValue = getNotNull(propName, ddefault + "");

        try {
            return Double.parseDouble(propValue);
        } catch (Exception ex) {
        }

        return ddefault;
    }


    public static long getLongValue(String propName, long idefault) {
        String propValue = getNotNull(propName, idefault + "");

        try {
            return Long.parseLong(propValue);
        } catch (Exception ex) {
        }

        return idefault;
    }

    public static boolean booleanValue(String val, boolean defaultValue) {
        if (val != null) {
            val = val.trim();

            if (val.equalsIgnoreCase("true") || val.equalsIgnoreCase("1")) {
                return true;
            }

            if (val.equalsIgnoreCase("false") || val.equalsIgnoreCase("0")) {
                return false;
            }
        }

        return defaultValue;
    }

    public static int intValue(String val, int defaultValue) {
        if (val != null) {
            val = val.trim();

            try {
                return Integer.parseInt(val);
            } catch (Exception ex) {
            }
        }

        return defaultValue;
    }

    //Get UUID From java UUID class.
    public static String getUUID() {
        UUID objUUID = UUID.randomUUID();

        return objUUID.toString();
    }

    @SuppressWarnings("unchecked")
	public static Object unserializePHPSerializedObject(String serializedString)
        throws IllegalStateException {
        Object var;
        Stack<Object> objVars = new Stack<Object>();
        StringTokenizer varTokens = new StringTokenizer(serializedString, ":");

        while (varTokens.hasMoreTokens()) {
            /* determine variable type */
            String typeToken = varTokens.nextToken(":{};");

            switch (typeToken.charAt(0)) {
            case 'a':

                Map<Object, Object> array = new HashMap<Object, Object>();
                /* store the array element counts in the map */
                array.put(COUNT_KEY, Integer.valueOf(varTokens.nextToken(":")));
                /* this must be an array */
                objVars.push(array);

                break;

            case 'b':
                var = Boolean.valueOf(varTokens.nextToken(":;").equals("1"));

                if (objVars.empty()) {
                    /* this must be a standalone boolean */
                    return var;
                } else {
                    /* add this boolean to its array */
                    addVarToArray(objVars, var, 'b');
                }

                break;

            case 'd':
                var = Double.valueOf(varTokens.nextToken(":;"));

                if (objVars.empty()) {
                    /* this must be a standalone double */
                    return var;
                } else {
                    /* add this double to its array */
                    addVarToArray(objVars, var, 'd');
                }

                break;

            case 'i':
                var = Integer.valueOf(varTokens.nextToken(":;"));

                if (objVars.empty()) {
                    /* this must be a standalone integer */
                    return var;
                } else {
                    /* add this integer to its array */
                    addVarToArray(objVars, var, 'i');
                }

                break;

            case 'N':
                var = null;

                if (objVars.empty()) {
                    /* this must be a standalone null */
                    return var;
                } else {
                    /* add this null to its array */
                    addVarToArray(objVars, var, 'N');
                }

                break;

            case 's':

                int len = Integer.parseInt(varTokens.nextToken(":")) + 2; /* the string length including quotes */
                StringBuffer varStr = new StringBuffer(varTokens.nextToken(":;"));

                while (varStr.length() < len) {
                    /* append the last delimiter since it was within the string */
                    if (!varStr.toString().equals("\"a")) {
                        varStr.append(";");
                    }

                    varStr.append(varTokens.nextToken(";"));
                }

                varStr.setLength(varStr.length() - 1); /* strip trailing string quotes */

                varStr.deleteCharAt(0); /* get rid of leading string quotes */
                var = varStr.toString();

                if (objVars.empty()) {
                    /* this must be a standalone string */
                    return var;
                } else {
                    /* add this string to its array */
                    addVarToArray(objVars, var, 's');
                }

                break;

            default:

                /* we shouldn't be here */
                throw new IllegalStateException("Unexpected token found: " + typeToken);
            }

            /* build nested arrays or return final array */
            if (objVars.peek().getClass().equals(HashMap.class)) {
                int count = (Integer) ((Map) objVars.peek()).get(COUNT_KEY);

                if (count == 0) {
                    /* get the last array stored */
                    var = objVars.pop();

                    /*we no longer need the element count */
                    ((Map) var).remove(COUNT_KEY);

                    if (objVars.empty()) {
                        /* this must be a standalone array */
                        return var;
                    } else {
                        addVarToArray(objVars, var, 'a');
                    }
                }
            }
        }

        return null;
    }

    @SuppressWarnings("unchecked")
	private static void addVarToArray(Stack<Object> objVars, Object var, char varType) {
        /* if the object is a valid key type and previous object was not a key */
        if (((varType == 'i') || (varType == 's')) && objVars.peek().getClass().equals(HashMap.class)) {
            /* this object must be an array key
                        * push it until we get its value
                        */
            objVars.push(var);
        } else {
            /* this object must be an array value */
            Object key = objVars.pop();

            Map<Object, Object> array = (Map) objVars.peek();
            array.put(key, var);

            /* decrement our array element count */
            int count = (Integer) array.get(COUNT_KEY);
            array.put(COUNT_KEY, new Integer(count - 1));
        }
    }

    public static boolean isStringObject(Object obj) {
        if ((obj == null) || !obj.getClass().equals(String.class)) {
            return false;
        }

        return true;
    }

    public static boolean isHashMapObject(Object obj) {
        if ((obj == null) || !obj.getClass().equals(HashMap.class)) {
            return false;
        }

        return true;
    }

    public static boolean isIntegerObject(Object obj) {
        if ((obj == null) || !obj.getClass().equals(Integer.class)) {
            return false;
        }

        return true;
    }

    public static String getClassName(String classPath) {
        int firstChar;
        firstChar = classPath.lastIndexOf('.') + 1;
        
        if(firstChar == -1) {
        	return null;
        }
        return classPath.substring(firstChar);
    }    

    public final static boolean validateIPAddress( String  ipAddress )
    {
        String[] parts = ipAddress.split( "\\." );      
        if ( parts.length != 4 || ipAddress.equals(INVALID_HOST_ADDRESS))
        {
            return false;
        }
        
        for ( String s : parts )
        {
            int i = Integer.parseInt( s );
            if ( (i < 0) || (i > 255) )
            {
                return false;
            }
        }

        return true;
    }
    
    
}


