package com.ipvsserv.nxd.dbtask;

import java.io.FileOutputStream;
import java.nio.channels.FileLock;

public class FileLocking {
    private String fileName="file.txt";
  


    private FileOutputStream fileOutputStream= null;
    private FileLock fileLock = null; 
    public FileLocking(){
        
    }
    public void open() throws Exception {
        fileOutputStream=  new FileOutputStream(fileName);
    }  
    public void lock() throws Exception {
        fileLock = fileOutputStream.getChannel().tryLock();
    }  
    public void unlock() throws Exception {
        fileLock.release();
        
    }
    public void close() throws Exception {
        fileOutputStream.close();
    } 
    
    
    public static void main(String[] args) throws Exception {
        FileOutputStream fos= new FileOutputStream("file.txt");
        FileLock fl = fos.getChannel().tryLock();
        if(fl != null) {
          System.out.println("Locked File");
          Thread.sleep(100);
          fl.release();
          System.out.println("Released Lock");
        }
        fos.close();
    }
    public String getFileName() {
        return fileName;
    }
    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

}
