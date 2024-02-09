package com.ipvsserv.common.util;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Properties;
import java.util.Random;
import java.util.StringTokenizer;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;

public class CompUtils {

    private static Random randGen = new Random();

    private static char[] numbersAndLetters = ("0123456789abcdefghijklmnopqrstuvwxyz" + "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").toCharArray();

    public static String randomString(int length) {
        if (length < 1) {
            return null;
        }
        // Create a char buffer to put random letters and numbers in.
        char[] randBuffer = new char[length];
        for (int i = 0; i < randBuffer.length; i++) {
            randBuffer[i] = numbersAndLetters[randGen.nextInt(71)];
        }
        return new String(randBuffer);
    }

    public static boolean isNullOrEmpty(String val) {
        if (val == null || val.trim().equals("")) {
            return true;
        }
        return false;
    }

    public static boolean isBoolean(String val) {
        if (!isNullOrEmpty(val)) {
            val = val.trim();
            if (val.equalsIgnoreCase("true") || val.equalsIgnoreCase("1") || val.equalsIgnoreCase("false") || val.equalsIgnoreCase("0")) {
                return true;
            }
        }
        return false;
    }

    public static boolean isInt(String val) {
        if (!isNullOrEmpty(val)) {
            val = val.trim();
            try {
                Integer.parseInt(val);
                return true;
            } catch (Exception ex) {
            }
        }
        return false;
    }

    public static boolean isFloat(String val) {
        if (!isNullOrEmpty(val)) {
            val = val.trim();
            try {
                Float.parseFloat(val);
                return true;
            } catch (Exception ex) {
            }
        }
        return false;
    }

    public static boolean isValidValue(String val, String mustbeValue) {
        if (!isNullOrEmpty(val)) {
            val = val.trim();
            if (val.equalsIgnoreCase(mustbeValue)) {
                return true;
            }
        }
        return false;
    }

    public static boolean isValidValue(String val, String[] mustbeValueArr) {
        if (!isNullOrEmpty(val)) {
            val = val.trim();
            for (int i = 0; i < mustbeValueArr.length; i++) {
                if (val.equalsIgnoreCase(mustbeValueArr[i])) {
                    return true;
                }
            }
        }
        return false;
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

    public static long longValue(String val, long defaultValue) {
        if (val != null) {
            val = val.trim();
            try {
                return Long.parseLong(val);
            } catch (Exception ex) {
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

    public static String nonEmpty(String val, String defaultValue) {
        if (val != null) {
            return val.trim();
        }
        return defaultValue;
    }

    public static float isFloat(String val, float defaultValue) {
        if (val != null) {
            val = val.trim();
            try {
                return Float.parseFloat(val);
            } catch (Exception ex) {
            }
        }
        return defaultValue;
    }

    ////////////////////////////////////////////////////////////

    public static Properties loadProperties(String propFile) throws Exception {
        return CompUtils.loadPropertiesFile(new Properties(), propFile, CompUtils.class);
    }

    public static Properties loadProperties(Properties props, String propFile) throws Exception {
        return CompUtils.loadPropertiesFile(props, propFile, CompUtils.class);
    }

    /////////////////////////
    public static String getCheckSum(String msg) throws NoSuchAlgorithmException {

        MessageDigest complete = MessageDigest.getInstance("MD5");
        byte[] b = complete.digest(msg.getBytes());
        String result = "";
        for (int i = 0; i < b.length; i++) {
            result += Integer.toString((b[i] & 0xff) + 0x100, 16).substring(1);
        }

        return result;
    }

    public static String encode(String data) throws UnsupportedEncodingException {//throws UnsupportedEncodingException{
        return CompUtils.encode(data, "UTF-8");
    }

    public static String decode(String data) throws UnsupportedEncodingException {//throws UnsupportedEncodingException{
        return CompUtils.decode(data, "UTF-8");
    }

    public static String encode(String data, String encoding) throws UnsupportedEncodingException {//throws UnsupportedEncodingException{
        if (data != null) {
            //return URLEncoder.encode(data,"UTF-8");
            //stripNonValidXMLCharacters(data, "encode before");
            data = URLEncoder.encode(data, encoding);
            //hasNonValidXMLCharacters(data, "encode after");
            //stripNonValidXMLCharacters ( data, "encode after");
            return data;
        }
        return data;
    }

    public static String decode(String data, String encoding) throws UnsupportedEncodingException {//throws UnsupportedEncodingException{
        if (data != null) {
            //return URLDecoder.decode(data,"UTF-8");
            //stripNonValidXMLCharacters(data, "decode before");
            data = URLDecoder.decode(data, encoding);
            //hasNonValidXMLCharacters(data, "decode after");
            //stripNonValidXMLCharacters(data, "decode after");
            return data;
        }
        return data;
    }

    static boolean debug = true;
    static boolean enableStrip = false;

    public static String stripNonValidXMLCharacters(String xml, String stage) {
        if (!CompUtils.debug) {
            //iSValidXMLText(xml);
            return xml;
        }
        try {
            //MRLog.debug(" CHECKSUM  " + stage + " :"+xml.length()+ " :"+getCheckSum(xml)+" :" ) ;
        } catch (Exception ex) {
            MRLog.error(ex);
        }

        boolean invalid = false;
        //StringBuffer out = new StringBuffer(); // Used to hold the output.

        char current; // Used to reference the current character.
        //http://cse-mjmcl.cse.bris.ac.uk/blog/2007/02/14/1171465494443.html
        if (xml == null || "".equals(xml)) {
            return ""; // vacancy test.
        }
        for (int i = 0; i < xml.length(); i++) {
            current = xml.charAt(i); // NOTE: No IndexOutOfBoundsException caught here; it should not happen.

            if (current == 0x9 || current == 0xA || current == 0xD || current >= 0x20 && current <= 0xD7FF || current >= 0xE000 && current <= 0xFFFD || current >= 0x10000 && current <= 0x10FFFF) {

                //if(enableStrip){
                //out.append(current);
                //}
            } else {
                //out.append(current);
                MRLog.debug("IN VALID CHAR:" + stage + " ='" + current + "' '" + Character.getNumericValue(current) + "' '" + Character.getType(current) + "' at " + i);
                invalid = true;

                /*try {
                    Character charObj = new Character(current);
                    MRLog.debug("IN VALID CHAR:" +stage+ "' at " + i +" " + charObj.toString().getBytes());// +":"+ Integer.parseInt(charObj.toString(),16)) ;
                } catch (Exception ex) {
                    MRLog.error(ex);
                }*/

            }

        }
        if (invalid) {
            /*if(xml.length()>500*1000){
                //MRLog.info("IN VALID CHAR:" +stage+" :"+out.toString()+" =>"+ xml.substring(0,100) ) ;
                MRLog.debug("IN VALID CHAR:" +stage+" '"+out.toString() +"'  " + xml.length()+":"+" =>" + xml.substring(0,100) ) ;
            }else{
                //MRLog.info("IN VALID CHAR:" +stage+" :"+out.toString()+" =>"+ xml ) ;
                MRLog.debug("IN VALID CHAR:" +stage+" '"+out.toString() +"' " + xml ) ;
            }*/
        }
        //if(!enableStrip)
        return xml;
        //return out.toString();
    }

    public static String stripNonValidXMLCharactersDecode(String xml, String stage, boolean force) throws UnsupportedEncodingException {
        if (!CompUtils.debug) {
            //iSValidXMLText(xml);
            return xml;
        }
        try {
            MRLog.debug(" CHECKSUM  " + stage + " :" + xml.length() + " :" + CompUtils.getCheckSum(xml) + " :");
        } catch (NoSuchAlgorithmException ex) {
            MRLog.error(ex);
        }

        boolean invalid = false;
        StringBuffer out = new StringBuffer(); // Used to hold the output.
        char current; // Used to reference the current character.
        //http://cse-mjmcl.cse.bris.ac.uk/blog/2007/02/14/1171465494443.html
        if (xml == null || "".equals(xml)) {
            return ""; // vacancy test.
        }
        for (int i = 0; i < xml.length(); i++) {
            current = xml.charAt(i); // NOTE: No IndexOutOfBoundsException caught here; it should not happen.

            if (current == 0x9 || current == 0xA || current == 0xD || current >= 0x20 && current <= 0xD7FF || current >= 0xE000 && current <= 0xFFFD || current >= 0x10000 && current <= 0x10FFFF) {

                if (force) {
                    out.append(current);
                }
            } else {
                //out.append(current);
                MRLog.debug("IN VALID CHAR:" + stage + " ='" + current + "' '" + Character.getNumericValue(current) + "' '" + Character.getType(current) + "' at " + i);
                MRLog.error("IN VALID CHAR:" + stage + " ='" + current + "' '" + Character.getNumericValue(current) + "' '" + Character.getType(current) + "' at " + i);
                invalid = true;

                /*try {
                    Character charObj = new Character(current);
                    MRLog.debug("IN VALID CHAR:" +stage+ "' at " + i +" " + charObj.toString().getBytes());// +":"+ Integer.parseInt(charObj.toString(),16)) ;
                } catch (Exception ex) {
                    MRLog.error(ex);
                }*/

            }

        }
        if (invalid) {
            /*if(xml.length()>500*1000){
                //MRLog.info("IN VALID CHAR:" +stage+" :"+out.toString()+" =>"+ xml.substring(0,100) ) ;
                MRLog.debug("IN VALID CHAR:" +stage+" '"+out.toString() +"'  " + xml.length()+":"+" =>" + xml.substring(0,100) ) ;
            }else{
                //MRLog.info("IN VALID CHAR:" +stage+" :"+out.toString()+" =>"+ xml ) ;
                MRLog.debug("IN VALID CHAR:" +stage+" '"+out.toString() +"' " + xml ) ;
            }*/
        }
        //if(!enableStrip)
        return URLDecoder.decode(out.toString(), "UTF-8");
        //return out.toString();
    }

    public static boolean hasNonValidXMLCharacters(String xml, String stage) {
        if (!CompUtils.debug) {
            //iSValidXMLText(xml);
            return false;
        }
        try {
            MRLog.debug(" CHECKSUM  " + stage + " :" + xml.length() + " :" + CompUtils.getCheckSum(xml) + " :");
        } catch (NoSuchAlgorithmException ex) {
            MRLog.error(ex);
        }

        boolean invalid = false;
        //StringBuffer out = new StringBuffer(); // Used to hold the output.
        char current; // Used to reference the current character.
        //http://cse-mjmcl.cse.bris.ac.uk/blog/2007/02/14/1171465494443.html
        if (xml == null || "".equals(xml)) {
            return false; // vacancy test.
        }
        for (int i = 0; i < xml.length(); i++) {
            current = xml.charAt(i); // NOTE: No IndexOutOfBoundsException caught here; it should not happen.

            if (current == 0x9 || current == 0xA || current == 0xD || current >= 0x20 && current <= 0xD7FF || current >= 0xE000 && current <= 0xFFFD || current >= 0x10000 && current <= 0x10FFFF) {

                //if(enableStrip){
                //out.append(current);
                //}
            } else {
                //out.append(current);
                MRLog.debug("IN VALID CHAR:" + stage + " ='" + current + "' '" + Character.getNumericValue(current) + "' '" + Character.getType(current) + "' at " + i);
                invalid = true;

                /*try {
                    Character charObj = new Character(current);
                    MRLog.debug("IN VALID CHAR:" +stage+ "' at " + i +" " + charObj.toString().getBytes());// +":"+ Integer.parseInt(charObj.toString(),16)) ;
                } catch (Exception ex) {
                    MRLog.error(ex);
                }*/

            }

        }
        if (invalid) {

        }
        //if(!enableStrip)
        return invalid;
        //return out.toString();
    }

    public static boolean isValidChar(char current, int i, String stage, String msg) {
        if (current == 0x9 || current == 0xA || current == 0xD || current >= 0x20 && current <= 0xD7FF || current >= 0xE000 && current <= 0xFFFD || current >= 0x10000 && current <= 0x10FFFF) {
            return true;
        } else {
            MRLog.debug("-----------------------IN VALID CHAR:" + msg + " " + stage + " ='" + current + "' '" + Character.getNumericValue(current) + "' '" + Character.getType(current) + "' at " + i);

        }
        return false;
    }

    public static boolean compareChars(String msg1, String msg2, String stage) {
        boolean invalid = false;
        //StringBuffer out = new StringBuffer(); // Used to hold the output.
        char current1; // Used to reference the current character.
        char current2; // Used to reference the current character.
        //http://cse-mjmcl.cse.bris.ac.uk/blog/2007/02/14/1171465494443.html
        if (msg1 == null || "".equals(msg1)) {
            return false; // vacancy test.
        }
        for (int i = 0; i < msg1.length(); i++) {
            try {
                current1 = msg1.charAt(i); // NOTE: No IndexOutOfBoundsException caught here; it should not happen.
                current2 = msg2.charAt(i); // NOTE: No IndexOutOfBoundsException caught here; it should not happen.
                //isValidChar(current, i , stage," msg1");
                //isValidChar(current2, i , stage," msg2");

                if (current1 != current2) {
                    MRLog.debug("Different CHAR:  " + stage + " msg1='" + current1 + "' " + " msg2='" + current2 + "' at " + i);
                    invalid = true;

                    /*try {
                        Character charObj = new Character(current);
                        MRLog.debug("IN VALID CHAR:" +stage+ "' at " + i +" " + charObj.toString().getBytes());// +":"+ Integer.parseInt(charObj.toString(),16)) ;
                    } catch (Exception ex) {
                        MRLog.error(ex);
                    }*/

                }
            } catch (Exception ex) {

            }

        }
        if (invalid) {

        }
        //if(!enableStrip)
        return invalid;
        //return out.toString();
    }

    public static boolean iSValidXMLText(String xml) {
        boolean valid = true;
        //less efficient on big data
        if (xml != null) {
            valid = xml.matches("^([\\x09\\x0A\\x0D\\x20-\\x7E]|" //# ASCII
                    + "[\\xC2-\\xDF][\\x80-\\xBF]|" //# non-overlong 2-byte
                    + "\\xE0[\\xA0-\\xBF][\\x80-\\xBF]|" //# excluding overlongs
                    + "[\\xE1-\\xEC\\xEE\\xEF][\\x80-\\xBF]{2}|" //# straight 3-byte
                    + "\\xED[\\x80-\\x9F][\\x80-\\xBF]|" //# excluding surrogates
                    + "\\xF0[\\x90-\\xBF][\\x80-\\xBF]{2}|" //# planes 1-3
                    + "[\\xF1-\\xF3][\\x80-\\xBF]{3}|" //# planes 4-15
                    + "\\xF4[\\x80-\\x8F][\\x80-\\xBF]{2})*$"); //# plane 16
        }
        if (!valid) {
            MRLog.info("IN VALID CHAR:" + xml);
            //MRLog.debug("IN VALID CHAR:" + xml ) ;
        }
        return valid;
    }

    /////////////////////////
    public static void deleteFolder(File directory) {
        try {
            if(!directory.exists())
                return;
            //restore files from current directory
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    file.delete();
                }
            }
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (file.isDirectory()) {
                    deleteFolder(file);
                }
            }
            directory.delete();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    public static String getContents(File aFile) {
        StringBuilder contents = new StringBuilder();
        try {
            BufferedReader input = new BufferedReader(new FileReader(aFile));
            try {
                String line = null;
                while ((line = input.readLine()) != null) {
                    contents.append(line);
                    contents.append(System.getProperty("line.separator"));
                }
            } finally {
                input.close();
            }
        } catch (IOException ex) {
            MRLog.error(ex);
        }
        return contents.toString();
    }

    /////////////////////////////////
    public static Properties loadPropertiesFile(Properties props, String propFile, Class<?> c) throws Exception {
        InputStream is = null;
        try {
            File file = new File(propFile);
            if (!file.exists()) {
                throw new Exception("Properties not found or invalid." + propFile);
            }
            is = new FileInputStream(new File(propFile));
            props.load(is);
            props.setProperty("props.inited", "true");
            return props;
        } catch (Exception ex) {
            MRLog.error(ex);
            throw new Exception("Properties not found or invalid." + propFile);
        } finally {
            try {
                if (is != null) {
                    is.close();
                }
            } catch (Exception ex) {
            }
        }

    }

    public static Properties loadPropertiesResource(Properties props, String propFile, Class<?> c) throws Exception {
        try {
            props.load(c.getResourceAsStream(propFile));
            props.setProperty("props.inited", "true");
            return props;
        } catch (Exception ex) {
            MRLog.error(ex);
            throw new Exception("Properties not found or invalid." + propFile);
        }

    }

    public static String getCurrentFolderPath() {
        String path = "";
        FileWriter os = null;
        try {
            String name = "temp.log";
            File file = new File(name);
            os = new FileWriter(file);
            os.write(" Server started " + new Date() + " " + System.currentTimeMillis());
            path = file.getCanonicalPath().replace('\\', '/');
            if (path.indexOf(name) != 1) {
                path = path.substring(0, path.indexOf(name));
                if (path.endsWith("/")) {
                    path = path.substring(0, path.length() - 1);
                }
            }
        } catch (Exception ex) {

        } finally {
            try {
                if (os != null) {
                    os.close();
                }
            } catch (Exception ex) {
            }
            os = null;
        }
        return path.replace('\\', '/');
    }

    public static ArrayList<String> split(String str, String token) {
        ArrayList<String> arrayList = new ArrayList<String>();
        try {
            StringTokenizer tok = new StringTokenizer(str, token);
            while (tok.hasMoreTokens()) {
                String nodeTemp = tok.nextToken();
                arrayList.add(nodeTemp);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return arrayList;
    }

    public static String removeXmlProlog(String str) {
        try {
            if (str == null) {
                return str;
            }
            if(str.startsWith("<?")){
                int ipos = str.indexOf("<?");
                if (ipos == -1) {
                    return str;
                }
                ipos = str.indexOf(">", ipos);
                if (ipos == -1) {
                    return str;
                }
                return (str + " ").substring(ipos + 1);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return str;
    }

    public static boolean isFileExist(String filePath) {
        File file = new File(filePath);
        return file.exists();
    }

    public static boolean writeFile(String fname, String buffer) throws Exception {
        return writeFile(fname, buffer, false);
    }
    public static boolean writeFile(String fname, String buffer, boolean append) throws Exception {
        boolean result = true;
        BufferedWriter output = new BufferedWriter(new FileWriter(new File(fname),append));
        output.write(buffer);
        output.close();
        return result;
    }
    public static boolean deleteFile(String fname) throws Exception {
        boolean result = true;
        File df = new File(fname);
        result = df.delete();
        df = null;
        return result;
    }

    public static boolean createFolder(String filePath) {
        if (!CompUtils.isFileExist(filePath)) {
            File file = new File(filePath);
            return file.mkdirs();
        }
        return false;
    }

    public static String readFile(String propFile) throws Exception {
        try {
            File file = new File(propFile);
            if(file.exists() && !file.isFile()){
                throw new Exception("File not found or may be a directory." + propFile + " " );
            }
            return readFile(new FileInputStream(new File(propFile)));
        } catch (Exception ex) {
            throw new Exception("File not found or invalid." + propFile + " " + ex.getMessage());
        }
    }

    public static String readFileResource(String propFile, Class<?> c) throws Exception {
        try {
            return readFile(c.getResourceAsStream(propFile));
        } catch (Exception ex) {
            throw new Exception("Properties not found or invalid." + propFile + " " + ex.getMessage());
        }
    }

    private static String readFile(InputStream fis) throws Exception {
        BufferedReader reader = null;
        StringBuffer buf = new StringBuffer();
        try {
            // Here BufferedInputStream is added for fast reading.
            reader = new BufferedReader(new InputStreamReader(fis));
            String line;
            while ((line = reader.readLine()) != null) {
                buf.append(line + "\n");
            }

        } catch (Exception ex) {
            throw new Exception("readFile error ." + " " + ex.getMessage());
        } finally {
            // dispose all the resources after using them.
            try {
                fis.close();
            } catch (Exception e) {
            }
            try {
                reader.close();
            } catch (Exception e) {
            }

        }
        return buf.toString();
    }

    public static void copy(File inFile, File outFile) throws IOException {
        FileInputStream fin = null;
        FileOutputStream fout = null;
        try {
            fin = new FileInputStream(inFile);
            fout = new FileOutputStream(outFile);
            copy(fin, fout);
        } finally {
            try {
                if (fin != null) {
                    fin.close();
                }
            } catch (IOException ex) {
                // do nothing
            }
            try {
                if (fout != null) {
                    fout.close();
                }
            } catch (IOException ex) {
                // do nothing
            }
        }
    }

    public static void copy(InputStream in, OutputStream out) throws IOException {
        // Do not allow other threads to intrude on streams during copy.
        synchronized (in) {
            synchronized (out) {
                byte[] buffer = new byte[256];
                while (true) {
                    int bytesRead = in.read(buffer);
                    if (bytesRead == -1) {
                        break;
                    }
                    out.write(buffer, 0, bytesRead);
                }
            }
        }
    }
    public static void deleteChildrenDirectory(File delFile) {
        //System.out.println("start deleteDirectory " + delFile.getAbsolutePath());
        try {
            if (delFile.exists()) {
                if (delFile.isDirectory()) {
                    File files[] = delFile.listFiles();
                    for (int i = 0; i < files.length; i++) {
                        deleteDirectory(files[i]);
                    }
                } else {
                    boolean deleted = delFile.delete();
                    if (!deleted) {
                        MRLog.debug("deleteChildrenDirectory failed to delete " + delFile.getAbsolutePath());
                    }
                }
            }
             
        } catch (Exception ex) {
            MRLog.error("deleteDirectory " + delFile.getAbsolutePath(), ex);
        } finally {
           //System.out.println("done deleteDirectory " + delFile.getAbsolutePath());
        }
    }
    public static void deleteDirectory(File delFile) {
        //System.out.println("start deleteDirectory " + delFile.getAbsolutePath());
        try {
            if (delFile.exists()) {
                if (delFile.isDirectory()) {
                    File files[] = delFile.listFiles();
                    for (int i = 0; i < files.length; i++) {
                        deleteDirectory(files[i]);
                    }
                } else {
                    boolean deleted = delFile.delete();
                    if (!deleted) {
                        MRLog.debug("deleteDirectory failed to delete " + delFile.getAbsolutePath());
                    }
                }
            }
            delFile.delete();
        } catch (Exception ex) {
            MRLog.error("deleteDirectory " + delFile.getAbsolutePath(), ex);
        } finally {
           //System.out.println("done deleteDirectory " + delFile.getAbsolutePath());
        }
    }

    public static void copyDirectory(String source, String destination) throws IOException {
        copyDirectory(new File(source), new File(destination));
    }

    public static void copyDirectory(File source, File destination) throws IOException {
        try {
            if (source.isDirectory()) {
                //MRLog.debug("copyDirectory directory " + source.getAbsolutePath());
                if (!destination.exists()) {
                    destination.mkdir();
                }
                String files[] = source.list();
                for (int i = 0; i < files.length; i++) {
                    copyDirectory(new File(source, files[i]), new File(destination, files[i]));
                }
            } else {
                if (!source.exists()) {
                    MRLog.debug("ERROR: copyDirectory File or directory does not exist. " + source.getAbsolutePath());
                    return;
                } else {
                    try {
                        InputStream in = new FileInputStream(source);
                        OutputStream out = new FileOutputStream(destination);
                        // Transfer bytes from in to out
                        byte[] buf = new byte[1024];
                        int len;
                        while ((len = in.read(buf)) > 0) {
                            out.write(buf, 0, len);
                        }
                        in.close();
                        out.close();
                    } catch (Exception ex) {
                        ex.printStackTrace();
                        MRLog.error(source.getAbsolutePath(), ex);
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error("copyDirectory " + source.getAbsolutePath(), ex);
        }

    }

    public static void printArray(String[] array) {
        if (array != null) {
            StringBuffer buf = new StringBuffer();
            for (int i = 0; i < array.length; i++) {
                if (i != 0) {
                    buf.append(" ");
                }
                buf.append(array[i]);
            }
            MRLog.debug(buf.toString());
        }
    }
    public static String joinArray(String[] array, String saparator) {
        if (array != null) {
            StringBuffer buf = new StringBuffer();
            for (int i = 0; i < array.length; i++) {
                if (i != 0) {
                    buf.append(saparator);
                }
                buf.append(array[i]);
            }
           return buf.toString();
        }
        return saparator;
    }
    public static void copyfile(String srcFile, String destFile) throws Exception {
        copyfile(new File(srcFile), new File(destFile));
    }

    public static void copyfile(File srcFile, File destFile) throws Exception {

        InputStream in = null;
        OutputStream out = null;
        try {
            in = new FileInputStream(srcFile);

            //For Append the file.
            //          OutputStream out = new FileOutputStream(f2,true);

            //For Overwrite the file.
            out = new FileOutputStream(destFile, false);

            byte[] buf = new byte[1024];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
        } finally {
            try {
                if (in != null)
                    in.close();
            } catch (Exception ex) {
            }
            try {
                if (out != null)
                    out.close();
            } catch (Exception ex) {
            }
        }
        //System.out.println("File copied.");

    }
    /*static boolean  bDebug = false;
    public static int executeProcess2(String[] cmd, StringBuffer buf) throws Exception {
        return executeProcess(cmd, buf, bDebug);
    }
    public static int executeProcess(String[] cmd, StringBuffer buf) throws Exception {
        return executeProcess(cmd, buf, bDebug);
    }*/
    public static int executeProcess(String[] cmd, StringBuffer buf) throws Exception {
         return executeProcess(cmd, buf, false);
    }
    public static int executeProcess(String[] cmd, StringBuffer buf, boolean forceWindows) throws Exception {
        //if(skip){
        //   String cmdstr = joinArray(cmd, " ");
        //   MRLog.debug("Script output: " + cmdstr);
        //   return 0;    
       //} 
       if(!forceWindows && IpvsServGlobals.isWindowsOS()){
           MRLog.debug("start execute commnd.......... " );
           MRLog.debug("" + joinArray(cmd, " "));
           Thread.sleep(60*1000);
           MRLog.debug("end execute commnd.......... " );
           return 0;
       }
       Runtime rt = Runtime.getRuntime();
       Process process = rt.exec(cmd);
       BufferedReader stdout = new BufferedReader(new InputStreamReader(process.getInputStream()));
       String line = stdout.readLine();
       int i=0;
       while (line != null) {
           i=+line.length();
           if(i>500*1000){
               i=line.length();
               buf = new StringBuffer();
           }
           buf.append(line);
           line = stdout.readLine();
          
       }
       int exitVal = process.waitFor();
       if (exitVal != 0) {
           MRLog.debug("Opps.. Script exited with non zero value ....!!! exitVal= "  + exitVal +" \n " + joinArray(cmd, " ")+"\n Script output: " + buf.toString());
       }
       return exitVal;
   }
    private static String os="";
    public static String getOS(){
        try{
            if(os !=null && !os.equals(""))
                return os;  
            
            os = System.getProperty("os.name").toLowerCase();
            //default
            if(os == null || os.equals(""))
                os = "linux";
            if (os.indexOf("nt")!=-1 || os.indexOf("windows 20")!=-1  || os.indexOf("windows 9")!=-1 || os.indexOf("windows xp")!=-1 ) {
                os = "windows";
            }else if (os.startsWith("mac")  ) {
                os = "mac";
            /*}else if (os.indexOf("hp-ux") != -1 ) {
                os = "linux";
            }else if (os.indexOf("sunos") != -1 ) {
                os = "linux";
            }else if (os.indexOf("linux") != -1 ) {
                os = "linux";*/
            } else {
                os = "linux";
            }           
        }catch(Exception ex){
        }
        return  os;
    }    

    public static String windowsToLinuxPath(String dirNameRoot){
        if (IpvsServGlobals.isWindowsOS()) {
            dirNameRoot = dirNameRoot.substring(dirNameRoot.indexOf(":")+1).replace("\\", "/").trim();
        }
        return dirNameRoot;
    }   
}
