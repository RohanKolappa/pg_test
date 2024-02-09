package com.barco.utils;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

import com.barco.utils.mr.MRException;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class CommandLineUtils {
	
	public static String executeCommand(String[] cmdArray) throws Exception {
        Runtime runtime = Runtime.getRuntime();
        
        Process proc = runtime.exec(cmdArray);
        InputStream inputstream = proc.getInputStream();
        InputStreamReader inputstreamreader = new InputStreamReader(inputstream);
        BufferedReader bufferedreader = new BufferedReader(inputstreamreader);

        StringBuffer sb = new StringBuffer();
        sb.append(cmdArray[0]);
        sb.append("\n");

        String line = null;

        while ((line = bufferedreader.readLine()) != null) {
            sb.append(line.trim());
            sb.append("\n");
        }

        try {
            if (proc.waitFor() != 0) {
                System.err.println("exit value = " + proc.exitValue() + "\n Command is=" + cmdArray[0]);
            	throw new MRException(MediaRoomServiceExceptionCodes.EXECUTION_OF_COMMANDS_FIALED,"Execution of command failed" );
            }
        } catch (InterruptedException e) {
            System.err.println(e);
        } finally {
            bufferedreader.close();
        }

        return sb.toString();
    }

}
