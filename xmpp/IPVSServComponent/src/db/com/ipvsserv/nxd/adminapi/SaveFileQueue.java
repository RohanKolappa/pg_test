package com.ipvsserv.nxd.adminapi;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.threadmodel.ChangeThreadName;

public class SaveFileQueue extends Thread {
    private int counter = 0;
    
    
    private SaveParams saveParams = null;
    private Queue<SaveDocument> queue = new LinkedBlockingQueue<SaveDocument>();
    private boolean stop;
    private ThreadPoolExecutor threadPool;
    private List<SaveFileQueueCompletedListener> listeners = new ArrayList<SaveFileQueueCompletedListener>();

    public void removeSaveFileQueueCompletedListener(SaveFileQueueCompletedListener listener) {
        listeners.remove(listener);
    }

    public void addSaveFileQueueCompletedListener(SaveFileQueueCompletedListener listener) {
        listeners.add(listener);
    }

    public SaveFileQueue() {
        int processInThreadCorePoolSize = IpvsServGlobals.getXMLProperty("admindbapi.exportSaveCorePoolSize", 300);
        int processInThreadMaximumPoolSize = IpvsServGlobals.getXMLProperty("admindbapi.exportSaveMaxPoolSize", 300);
        int processInThreadKeepAliveTime = IpvsServGlobals.getXMLProperty("admindbapi.exportSaveKeepAlivetimeSec", 1);

        // Create a pool of threads that will process requests received by this component. If more
        // threads are required then the command will be executed on the SocketReadThread process
        threadPool = new ThreadPoolExecutor(processInThreadCorePoolSize, processInThreadMaximumPoolSize, processInThreadKeepAliveTime, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(), new ThreadPoolExecutor.CallerRunsPolicy());

        final ThreadFactory originalThreadFactory = threadPool.getThreadFactory();
        ThreadFactory newThreadFactory = new ThreadFactory() {
            private final AtomicInteger threadId = new AtomicInteger(0);

            public Thread newThread(Runnable runnable) {
                Thread t = originalThreadFactory.newThread(new ChangeThreadName(runnable, "SaveFileQ" + threadId.incrementAndGet()));
                t.setDaemon(true);
                return t;
            }
        };
        threadPool.setThreadFactory(newThreadFactory);
    }

    public SaveFileQueue(SaveParams params) {
        this.saveParams = params;
    }

    public boolean isStop() {
        return stop;
    }

    public void setStop(boolean stop) {
        this.stop = stop;
    }

    public SaveParams getSaveParams() {
        return saveParams;
    }

    public void setSaveParams(SaveParams saveParams) {
        this.saveParams = saveParams;
    }

    public void addDocumentToQueue(String xml) {
        String docUri = "";//DBAdminUtils.getNodeName(xml);
        queue.add(new SaveDocument(this, docUri, xml));
        synchronized (this) {
            notify();
        }
    }

    public void addDocumentToQueue(String docUri, String xml) {
        queue.add(new SaveDocument(this, docUri, xml));
        synchronized (this) {
            notify();
        }
    }

    public void run() {
        try {
            while (true) {
                while (!queue.isEmpty()) {
                    SaveDocument entry = queue.poll();
                    if (entry != null) {
                        threadPool.execute(entry);
                    }
                }
                if (isStop() && queue.isEmpty() ) {
                    threadPool.shutdown();
                    threadPool.awaitTermination(600, TimeUnit.SECONDS);
                    if (MRLog.isDbadminLogEnable()) {
                        MRLog.debug("save file queue isTerminated ? "+threadPool.isTerminated()+ " completedTaskCount="+threadPool.getCompletedTaskCount() + " taskCount=" + threadPool.getTaskCount() );
                    }
                    break;
                }
                synchronized (this) {
                    wait(20);
                }
            }
        } catch (Exception ex) {
            try {
                MRLog.error("Export failed " + saveParams.getDirectory().getCanonicalPath(), ex);
                for (SaveFileQueueCompletedListener listener : listeners) {
                    if (listener != null) {
                        listener.saveQueueFailed(saveParams, counter);
                    }
                }
            } catch (Exception ex2) {
            }
            return;
        } finally {
            try {
                if(!threadPool.isTerminated()){
                    //in case of error make sure to shutdown
                    threadPool.shutdown();
                    threadPool.awaitTermination(600, TimeUnit.SECONDS);
                }
            } catch (Exception ex) {

            }
            queue=null;
            threadPool=null;
        }
        try {
            for (SaveFileQueueCompletedListener listener : listeners) {
                if (listener != null) {
                    listener.saveQueueCompleted(saveParams, counter);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    public void saveFile(SaveDocument entry) {
        try {
            long start = System.currentTimeMillis();
            if (entry.docUri == null || entry.docUri.equals("")) {
                String nid = DBAdminUtils.getNID(entry.xml);
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.parse(nid);
                entry.docUri = node.getDocUri();//DBAdminUtils.getNID(entry.xml);
            }
            if (entry.docUri == null || entry.docUri.equals("")) {
                MRLog.debug("entry.docUri is empty may be its not ipvs Doc " + saveParams.getDirectory().getPath());
                entry.docUri = saveParams.getDirectory().getName();
            }
            //MRLog.debug((System.currentTimeMillis()-start)+ " save docUri " +  entry.docUri);
            String save_path = saveParams.getDirectory().getPath() + "/" + entry.docUri;

            if (saveParams.isSaveToCol()) {
                String rootName = DBAdminUtils.getRootNodeName(entry.xml);
                String colPath = saveParams.getDirectory().getPath() + "/" + rootName;
                File file = new File(colPath);
                if (!file.exists()) {
                    file.mkdirs();
                }
                save_path = colPath + "/" + entry.docUri;
                if (saveParams.getParams().isSaveForDiff()) {
                    save_path = DBAdminUtils.getFilePath(saveParams.isRootCollection(), saveParams.getDirectory().getPath(), rootName, save_path);
                }
            }
            try {
                //counter++;
                PrintWriter out = new PrintWriter(new FileOutputStream(save_path, false));
                out.println(entry.xml);
                out.flush();
                out.close();
            } catch (IOException ex) {
                ex.printStackTrace();
                MRLog.error("IOException whilst trying to write file '" + saveParams.getDirectory().getPath() + "/" + entry.docUri + "'.");
            }
            counter++;
            //if (MRLog.isDbadminLogEnable()) {
            if((counter % 50) == 0){
                MRLog.debug("EXPORT: SAVE_FILE_DONE: time-"+System.currentTimeMillis()+"("+(System.currentTimeMillis() - start) + ") count="+(counter) + " " + entry.docUri);
            } 
            //}
            //MRLog.debug("DONE  "+(System.currentTimeMillis()-start)+ " save docUri " +  entry.docUri +" save_path " + save_path);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public static class SaveDocument implements Runnable {
        SaveFileQueue parent = null;
        String xml = null;
        String docUri = null;

        public SaveDocument(String xml) {
            this.xml = xml;
        }

        public SaveDocument(SaveFileQueue parent, String docUri, String xml) {
            this.xml = xml;
            this.docUri = docUri;
            this.parent = parent;
        }

        public void run() {
            try {
                parent.saveFile(this);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

}
