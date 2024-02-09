package com.ipvsserv.nxd.adminapi;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import org.dom4j.Document;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.sednanative.SednaNativeModule;
import com.ipvsserv.server.IpvsServer;

public class DBAdminUtils {
    public static String getDomainName(File directory, String collectionNamePrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //restore files from current directory
        File[] dirs = directory.listFiles();
        for (int i = 0; i < dirs.length; i++) {
            File file = dirs[i];
            if (file.isDirectory()) {
                String collectionName = file.getName();
                if (collectionName.startsWith(collectionNamePrefix)) {
                    int pos = collectionName.indexOf(collectionNamePrefix) + collectionNamePrefix.length();
                    String domainName = collectionName.substring(pos);
                    return domainName;
                }
            }
        }
        return null;
    }

    public static String getFilePath(boolean isRootCollection, String folder, String rootName, String save_path) {
        if (isRootCollection) {
            return save_path;
        }
        int counter = 1;
        String save_path2 = folder + "/" + rootName + "/" + rootName + ".xml";
        File file = new File(save_path2);
        while (file.exists()) {
            save_path2 = folder + "/" + rootName + "/" + rootName + "_" + counter + ".xml";
            file = new File(save_path2);
            counter++;
        }
        return file.getAbsolutePath();
    }

    public static String getNodeName(String xml) {
        String NID = "";
        try {
            Document doc = Dom4jUtils.stringToDocument(xml);
            NID = doc.getRootElement().getName();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return NID;
    }

    public static String getRootNodeName(String xml) {
        String NID = "";
        try {
            xml = xml.trim();
            int iPos = xml.indexOf('<');
            if (xml.startsWith("<?")) {
                iPos = xml.indexOf('<', iPos);
            }
            StringBuffer buf = new StringBuffer();
            for (int i = iPos + 1; i < xml.length(); i++) {
                char c = xml.charAt(i);
                if (c == ' ' || c == '>') {
                    break;
                }
                buf.append(c);
            }
            NID = buf.toString();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return NID;
    }

    public static String getNID(String xml) {
        String NID = "";
        try {
            xml = xml.trim();
            int iPos = xml.indexOf("NID=");
            StringBuffer buf = new StringBuffer();
            for (int i = iPos + 4; i < xml.length(); i++) {
                char c = xml.charAt(i);
                if (c == '\'' || c == '"') {

                } else {
                    buf.append(c);
                }
                if (c == '>' || c == ' ') {
                    break;
                }
            }
            NID = buf.toString().trim();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return NID;
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

    ///////////////////// new api 

    public static void create_zip(File directory, String zipFileName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {

    }

    public static void extract_zip(File directory, String zipFileName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {

    }

    public static File getScript(String prefix) throws IOException {
        File parent =  new File(IpvsServGlobals.getProperty("admindbapi.dbBackupArea", "/data/sedna_dbsnapshot/backup"), "scripts");
        parent.mkdirs(); 
        File xquery_file = File.createTempFile(prefix, ".xquery", parent);
        return xquery_file;
    }
    public static File getScriptFolder(String prefix) throws IOException {
        File parent =  new File(IpvsServGlobals.getProperty("admindbapi.dbBackupArea", "/data/sedna_dbsnapshot/backup"), "scripts");
        parent.mkdirs(); 
        File xquery_file = File.createTempFile(prefix, "_dir", parent);
        String path = xquery_file.getAbsolutePath();
        xquery_file.delete();
        xquery_file = new File(path);
        xquery_file.mkdirs();
        return xquery_file;
    }
    public static void execute_xquery_file_native(String xquery, StringBuffer buf) throws Exception {
        long t0 = System.currentTimeMillis();
        File xquery_file = new File(xquery);
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug(t0 + " execute_xquery_file_native \n " + CompUtils.getContents(xquery_file));
        }

        String sednaInstallPath = IpvsServGlobals.getProperty("admindbapi.sedna_install_path", "/home/onair/sedna");
        sednaInstallPath = sednaInstallPath + "/bin/se_term";
        if (CompUtils.getOS().equalsIgnoreCase("windows")) {
            SednaNativeModule module = (SednaNativeModule) IpvsServer.getInstance().getModule(SednaNativeModule.class);
            String host = module.getProperty("host", "127.0.0.1");
            String[] cmdArgs = { sednaInstallPath, "-host", host, "-file", xquery_file.getAbsolutePath(), "msdb" };
            int exitVal=CompUtils.executeProcess(cmdArgs, buf);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " execute_xquery_file_native  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
            }
            if (MRLog.isDbqueryLogEnable() ||  MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN:  execute_xquery_file_native \n " + CompUtils.getContents(xquery_file));
            }
            if (exitVal != 0) {
                MRLog.debug("DB_ADMIN: Error during execute_xquery_file_native. retVal="+exitVal+" " +CompUtils.joinArray(cmdArgs, " ") +" \n "+ buf.toString()  );
                MRLog.debug(CompUtils.getContents(xquery_file)  );
            }else{
                MRLog.debug("DB_ADMIN: Done execute_xquery_file_native. retVal="+exitVal+" " +CompUtils.joinArray(cmdArgs, " ") +" \n "+ buf.toString()  );
            }
        } else {
            String[] cmdArgs = { sednaInstallPath, "-file", xquery_file.getAbsolutePath(), "msdb" };
            int exitVal=CompUtils.executeProcess(cmdArgs, buf);
            if (exitVal != 0) {
                MRLog.debug("DB_ADMIN: Error during execute_xquery_file_native. retVal="+exitVal+" " +CompUtils.joinArray(cmdArgs, " ") +" \n "+ buf.toString()  );
                MRLog.debug(CompUtils.getContents(xquery_file)  );
            }else{
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " execute_xquery_file_native  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
                }
                if (MRLog.isDbqueryLogEnable() ||  MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN:  execute_xquery_file_native \n " + CompUtils.getContents(xquery_file));
                }
            }
        }
        MRLog.debug("DB_ADMIN: "+ System.currentTimeMillis() + " (" + (System.currentTimeMillis() - t0) + ") " + xquery_file.getAbsolutePath() );
    }
//
//    public static int resetFactcoryDatabase() throws Exception {
//
//        StringBuffer buf = new StringBuffer();
//        int exitVal = 0;
//
//        //schedule the db creation on next reboot
//        //Factory/LastBackup/LastImported
//        try {
//            String[] cmdArgs = { "/etc/scripts/sednactl.sh", "reset", "Factory" };
//            exitVal = CompUtils.executeProcess(cmdArgs, buf);
//            CompUtils.printArray(cmdArgs);
//            MRLog.debug(" " + buf.toString());
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//        try {
//            String[] cmd = { "/bin/mserverctl", "manual" };
//            exitVal = CompUtils.executeProcess(cmd, buf);
//            CompUtils.printArray(cmd);
//            MRLog.debug(" " + buf.toString());
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//        try {
//
//            String[] cmd2 = { "/etc/scripts/sednactl.sh", "restart" };
//            exitVal = CompUtils.executeProcess(cmd2, buf);
//            CompUtils.printArray(cmd2);
//            MRLog.debug(" " + buf.toString());
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//        try {
//           
//            String[] cmd3 = { "/bin/mserverctl", "auto" };
//            exitVal = CompUtils.executeProcess(cmd3, buf);
//            CompUtils.printArray(cmd3);
//            MRLog.debug(" " + buf.toString());
//           
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//        while(true){
//            StringBuffer buf2 = new StringBuffer();
//            try {
//                Thread.sleep(10 * 1000);
//               
//                String[] cmd4 = { "/etc/scripts/sednactl.sh", "status" };
//                exitVal = CompUtils.executeProcess(cmd4, buf2);
//                CompUtils.printArray(cmd4);
//                MRLog.debug(" " + buf2.toString());
//                if(buf2.toString().toLowerCase().indexOf("ok") !=-1){
//                    break;
//                }
//                
//            } catch (Exception ex) {
//                MRLog.debug(buf2.toString());
//                break;
//            }
//        }
//        try{
//            DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
//            dbadmin.getDBModule().setStarted(false);
//           
//        }catch(Exception ex){
//            ex.printStackTrace();
//        }        
//        return exitVal;
//
//    }
//
//    public static int recreateDatabase(String restoreFrom, String tgz_path) throws Exception {
//        StringBuffer buf = new StringBuffer();
//        int exitVal = 0;
//
//        //schedule the db creation on next reboot
//        //Factory/LastBackup/LastImported
//        if (restoreFrom.equals("Factory")) {
//            String[] cmdArgs = { "/etc/scripts/sednactl.sh", "reset", restoreFrom.toLowerCase() };
//            exitVal = CompUtils.executeProcess(cmdArgs, buf);
//
//            String[] cmd = { "/bin/mserverctl", "manual" };
//            exitVal = CompUtils.executeProcess(cmd, buf);
//
//            String[] cmd2 = { "/etc/scripts/sednactl.sh", "restart" };
//            exitVal = CompUtils.executeProcess(cmd2, buf);
//
//            String[] cmd3 = { "/bin/mserverctl", "auto" };
//            exitVal = CompUtils.executeProcess(cmd3, buf);
//
//            Thread.sleep(10 * 1000);
//
//        } else if (restoreFrom.equals("LastBackup")) {
//            //TODO: is back up exists ? else throw error. call getDatabaseManagementStatus and parse.
//            String[] cmdArgs = { "/etc/scripts/sednactl.sh", "restoreSnapshot", restoreFrom.toLowerCase() };
//            exitVal = CompUtils.executeProcess(cmdArgs, buf);
//        } else if (restoreFrom.equals("LastImported")) {
//            //todo: integration test 
//            File file = new File(tgz_path);//"/data/saved_databases/importdb.tgz");
//            if (!file.exists()) {
//                //since the file is not there, can not restore from
//                throw new Exception("'" + tgz_path + "' does not exist.");
//            }
//            String[] cmdArgs = { "/etc/scripts/sednactl.sh", "schedule_importdb", "importdb", restoreFrom.toLowerCase() };
//            exitVal = CompUtils.executeProcess(cmdArgs, buf);
//        }
//
//        //check for errors in schedule
//        if (exitVal != 0) {
//            //script should not return non zero value.
//            throw new Exception(buf.toString());
//        }
//        if (buf.toString().toLowerCase().indexOf("error") != -1) {
//            //there was some error in script.
//            throw new Exception(buf.toString());
//        }
//        return exitVal;
//    }
}
