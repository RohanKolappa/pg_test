package com.ipvsserv.nxd.adminapi.db;

import java.io.File;
import java.util.Map;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;

public class CreateZipAction extends DBQueryAction {

    @Override
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        // /bin/tar -czvf "${temp_folder}/db/msdb.tgz" -C ${temp_folder} ${DB_FOLDER}
        File dir = new File(exportDB.getWorkarea_folder());
        File file = new File(exportDB.getZipFilePath());
        file.getParentFile().mkdirs();
        
        //todo: remove this code and delete 
        file.delete();
        
        
        int retVal = 0;
        StringBuffer buf = new StringBuffer();
        if (CompUtils.getOS().endsWith("windows")) {
            String[] cmd = { "C:/etc/scripts/create_db_tar.bat",  dir.getAbsolutePath(), file.getName()  };
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN:  creating Zip '" + exportDB.getZipFilePath() + "' " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
            }
            retVal = CompUtils.executeProcess(cmd, buf);
            if (retVal != 0) {
                int i=0;
                while(!file.exists()){
                    Thread.sleep(5000);
                    retVal = CompUtils.executeProcess(cmd, buf);
                    i++;
                    if(i>20){
                        break;
                    }
                }
                if (retVal != 0) {
                    throw new Exception("Error during create zip. retVal="+retVal+" " +CompUtils.joinArray(cmd, " ") +" "+ buf.toString()  );
                }
            }  
        } else {
            //http://www.linux.org.za/Lists-Archives/glug-9807/msg00016.html
            /*
                GNU `tar' returns only a few exit statuses.  I'm really aiming
                simplicity in that area, for now.  If you are not using the `--compare'
                (`-d') option, zero means that everything went well, besides maybe
                innocuous warnings.  Nonzero means that something went wrong.  Right
                now, as of today, "nonzero" is almost always 2, except for remote
                operations, where it may be 128.
                
                ...which helps a lot :) As far as I can tell, in my case I get that when
                one of the files is being written to or is somehow changing at the time
                tar reads it. I have checked the tarball quite a number of times and the
                tar file looks fine to me, so I have just being ignoring the return
                code and all is still happy. Short of tracing the execution of tar to
                find out exactly why it is returning 2, I don't know what else to
                suggest. If you find any more info I'd like to know...
             */
            //Thread.sleep(200);
            String tarOption = "-cvf";
            if (exportDB.getMediaDirZipType().equalsIgnoreCase("tar")) {
                tarOption = "-cvf";
            } else if (exportDB.getMediaDirZipType().equalsIgnoreCase("tgz")) {
                tarOption = "-czvf";
            } else {
                tarOption = "-cvf";
            }
            String[] cmd = { "tar",  "-C" , dir.getAbsolutePath(),  tarOption, exportDB.getZipFilePath(), "." };
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN:  creating Zip '" + exportDB.getZipFilePath() + "' " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
            }
            retVal = CompUtils.executeProcess(cmd, buf);
            if (retVal != 0) {
                //file.deleteOnExit();
                Thread.sleep(1000);
                retVal = CompUtils.executeProcess(cmd, buf);
            }
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: " + retVal + " created Zip '" + exportDB.getZipFilePath() + "' " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
            }
            if (retVal != 0) {
                throw new Exception("Error during create zip. retVal="+retVal+" " +CompUtils.joinArray(cmd, " ") +" "+ buf.toString()  );
            }
        }
        /*if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("delete temp folder '" + dir.getAbsolutePath());
        }
        CompUtils.deleteDirectory(dir);*/
        return "DONE";
    }

}
