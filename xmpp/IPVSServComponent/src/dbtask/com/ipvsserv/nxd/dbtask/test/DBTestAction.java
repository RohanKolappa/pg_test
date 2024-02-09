package com.ipvsserv.nxd.dbtask.test;

import java.io.File;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.threadmodel.ChangeThreadName;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public abstract class DBTestAction extends IpvsServBasicModule {
    protected int numberOfLoops = 1;
    protected boolean loopEndlessly = false;
    protected int loopEndlesslyCounter = 0;
    protected int numberOfThreads = 1;
    protected ThreadPoolExecutor threadPool;
    protected long threadKeepAliveTime = 10;
    protected long timeTakenTOexecuteDB = 0;
    private int timeout = 100 * 60 * 1000;
    private int waitTime = 5 * 1000;
    
    protected String collectionPrefix;
    protected String documentPrefix;
    protected String dbActionID;
    protected IpvsDBDriver db = null;
    protected String ipvsDomain = "default";
    protected long t0 = System.currentTimeMillis();
    protected int maxInQueueWaitingEnd = 0;
    protected boolean bverifyState = false;
    protected boolean isMaxInQueueWaiting = false;
    protected int maxInQueueWaitingStart = 0;
    
    protected static String appName = IpvsServGlobals.getIpvsAppName();
    
    public DBTestAction(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            
            dbActionID = getProperty("name", "");
            name = dbActionID;
            ////////////// if endless loop ////////////////////
            loopEndlessly = CompUtils.booleanValue(params.get("loopEndlessly"), TestGlobals.getBooleanProperty("DBTestActionsSequence.loopEndlessly", false));
            initialDelay = CompUtils.longValue(params.get("initialDelay"), TestGlobals.getLongProperty("DBTestActionsSequence.initialDelay", initialDelay));
            delay = CompUtils.longValue(params.get("delay"), TestGlobals.getLongProperty("DBTestActionsSequence.delay", delay));
            String unitStr = getProperty("delayUnit", TestGlobals.getProperty("DBTestActionsSequence.delayUnit", "SECONDS"));
            if (unitStr.equalsIgnoreCase("SECONDS"))
                unit = TimeUnit.SECONDS;
            else if (unitStr.equalsIgnoreCase("MINUTES"))
                unit = TimeUnit.MINUTES;
            else if (unitStr.equalsIgnoreCase("HOURS"))
                unit = TimeUnit.HOURS;
            else
                unit = TimeUnit.SECONDS;
            
            ///////////////////
            ipvsDomain = getProperty("ipvsDomain", TestGlobals.getProperty("DBTestActionsSequence.ipvsDomain", "default"));
            collectionPrefix = getProperty("collectionPrefix", TestGlobals.getProperty("DBTestActionsSequence.collectionPrefix", "ipvsCol"));
            documentPrefix = getProperty("documentPrefix", TestGlobals.getProperty("DBTestActionsSequence.documentPrefix", "ipvsDoc"));
            
            // thread settings ///////////////////
            //if more than one thread -numberOfThreads=1 -numberOfLoops=6
            numberOfThreads = CompUtils.intValue(params.get("numberOfThreads"), -1);
            numberOfLoops = CompUtils.intValue(params.get("numberOfLoops"), -1);
            if (numberOfThreads == -1) {
                numberOfThreads = CompUtils.intValue(IpvsServGlobals.getProcessArguments().getArgumentParam("numberOfThreads", TestGlobals.getProperty("DBTestActionsSequence.numberOfThreads", "1")), 1);
            }
            if (numberOfLoops == -1) {
                numberOfLoops = CompUtils.intValue(IpvsServGlobals.getProcessArguments().getArgumentParam("numberOfLoops", TestGlobals.getProperty("DBTestActionsSequence.numberOfLoops", "1")), 1);
            }
            //numberOfThreads = CompUtils.intValue(params.get("numberOfThreads"), TestGlobals.getIntProperty("DBTestActionsSequence.numberOfThreads", 1));
            //numberOfLoops = CompUtils.intValue(params.get("numberOfLoops"), TestGlobals.getIntProperty("DBTestActionsSequence.numberOfLoops", 1));
            if (numberOfThreads < 1) numberOfThreads = 1;
            if (numberOfLoops < 1) numberOfLoops = 1;
            //if (numberOfThreads > 1) {
            // Create a pool of threads that will process requests received by this component. If more
            // threads are required then the command will be executed on the SocketReadThread process
            threadPool = new ThreadPoolExecutor(numberOfThreads, numberOfThreads, threadKeepAliveTime, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(), new ThreadPoolExecutor.CallerRunsPolicy());
            
            final ThreadFactory originalThreadFactory = threadPool.getThreadFactory();
            ThreadFactory newThreadFactory = new ThreadFactory() {
                private final AtomicInteger threadId = new AtomicInteger(0);
                
                public Thread newThread(Runnable runnable) {
                    Thread t = originalThreadFactory.newThread(new ChangeThreadName(runnable, dbActionID + "_" + threadId.incrementAndGet()));
                    t.setDaemon(true);
                    return t;
                }
            };
            threadPool.setThreadFactory(newThreadFactory);
            //}
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
    @Override
    public void start() throws Exception {
        try {
            db = IpvsServer.getInstance().getDefaultIpvsDBDriver();
            super.startMonitor();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void stop() throws Exception {
        destroy();
    }
    
    @Override
    public void destroy() throws Exception {
        super.destroy();
        try {
            if (threadPool != null) threadPool.shutdown();
        } catch (Exception ex) {

        }
    }
    
    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }
    
    private boolean checkDone() {
        if (!loopEndlessly && isExecuted()) {
            if (sizeThreadProcessing() > 0) return true;
            try {
                destroy();
                return true;
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        return false;
    }
    
    protected Hashtable<String, String> getReplaceList() {
        boolean enableReplace = CompUtils.booleanValue(getPropertyDeep("enableReplace", "false"), false);
        Hashtable<String, String> replacelist = new Hashtable<String, String>();
        if (enableReplace) {
            String find = "replaceStrFind";
            String with = "replaceStrWith";
            String key = null;
            String value = null;
            int i = 0;
            while (true) {
                i++;
                key = getPropertyDeep(find + i, null);
                value = getPropertyDeep(with + i, null);
                if (key != null && value != null) {
                    replacelist.put(key, value);
                } else if (key == null) {
                    break;
                }
            }
        }
        return replacelist;
    }
    
    protected String getPropertyDeep(String key, String sdefault) {
        String value = params.get(key);
        if (value != null) {
            return value;
        }
        value = TestGlobals.getProperty(dbActionID + "." + key, null);
        if (value != null) {
            return value;
        }
        value = TestGlobals.getProperty("DBTestActionsSequence." + key, null);
        if (value != null) {
            return value;
        }
        return sdefault;
    }
    
    protected String getPropertyXmlFile(String key, String parentFolder, String sdefault) throws Exception {
        try {
            String xmlFilePath = getPropertyDeep(key, sdefault);
            String prefix = TestGlobals.getProperty("DBTestActionsSequence." + IpvsServGlobals.getIpvsAppName() + "_file_prefix", null);
            if (prefix != null && !prefix.trim().equals("")) {
                File file = new File(xmlFilePath);
                String fileName = file.getName();
                String newFileName = prefix + fileName;
                if (fileName.length() < xmlFilePath.length()) {
                    newFileName = xmlFilePath.substring(0, xmlFilePath.lastIndexOf(fileName)) + xmlFilePath;
                    //todo: test this
                }
                newFileName = verifyAndGetTestXmlPath(newFileName, parentFolder, sdefault);
                if (newFileName != null) return newFileName;
            }
            xmlFilePath = verifyAndGetTestXmlPath(xmlFilePath, parentFolder, sdefault);
            File file = new File(xmlFilePath);
            if (!file.exists()) {
                throw new Exception(name + " xmlFilePath '" + xmlFilePath + "' not found");
            }
            return xmlFilePath;
        } catch (Exception ex) {
            throw ex;
        }
    }
    
    protected String verifyAndGetTestXmlPath(String xmlFilePath, String parentFolder, String sdefault) throws Exception {
        try {
            File file = new File(xmlFilePath);
            if (file.exists()) {
                return xmlFilePath;
            }
            if (parentFolder != null && !parentFolder.trim().equals("")) {
                parentFolder = parentFolder.replace("/", File.separator).replace("\\", File.separator);
                if (!parentFolder.endsWith(File.separator)) parentFolder = parentFolder + File.separator;
                try {
                    xmlFilePath = IpvsServGlobals.verifyAndGetTestXmlPath(parentFolder + xmlFilePath);
                } catch (Exception ex) {}
            }
            xmlFilePath = IpvsServGlobals.verifyAndGetTestXmlPath(xmlFilePath);
            ;
            return xmlFilePath;
        } catch (Exception ex) {
            //MRLog.error(ex);
        }
        return null;
    }
    
    public int getThreadNumber(int loopNumber) {
        if (numberOfThreads == 1) return 0;
        try {
            int col = numberOfLoops / loopNumber;
            if (DBTestActionShared.getInstance().getCollections() >= col) {
                return col;
            }
        } catch (Exception ex) {}
        return 0;
    }
    
    protected String replace(String xml, Hashtable<String, String> replacelist, int loopNumber) {
        /*<param name='replaceStrFind'>{0}</param>
        <param name='replaceStrWith'>loopNumber</param>  
        <param name='replaceStrFind'>{1}</param>
        <param name='replaceStrWith'>threadNumber</param>   
        <param name='replaceStrFind'>{2}</param>
        <param name='replaceStrWith'>outerLoop</param> */
        for (Enumeration<String> e = replacelist.keys(); e.hasMoreElements();) {
            String find = e.nextElement();
            String with = replacelist.get(find);
            if (with.equalsIgnoreCase("loopNumber")) {
                xml = xml.replaceAll(find, loopNumber + "");
            } else if (with.equalsIgnoreCase("threadNumber")) {
                xml = xml.replaceAll(find, getThreadNumber(loopNumber) + "");
            } else if (with.equalsIgnoreCase("outerLoop")) {
                xml = xml.replaceAll(find, loopEndlesslyCounter + "");
            }
        }
        return xml;
    }
    
    private Hashtable<String, String> processingList = new Hashtable<String, String>();
    
    private synchronized void addThreadProcessing(String name) {
        processingList.put(name, "");
    }
    
    private synchronized void removeThreadProcessing(String name) {
        processingList.remove(name);
        try {
            if (processingList.size() == 0) {
                notify();
            }
            if (isMaxInQueueWaiting) {
                if (processingList.size() < maxInQueueWaitingEnd) {
                    notify();
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private synchronized int sizeThreadProcessing() {
        return processingList.size();
    }
    
    public abstract void execute(int loop) throws Exception;
    
    ///////////////////////////////////////
    public int getLoopEachThreadsTimes() {
        return numberOfLoops;
    }
    
    public String getThreadName() {
        try {
            return Thread.currentThread().getName();
        } catch (Exception ex) {
            return "";
        }
    }
    
    @Override
    public void verifyState() throws Exception {
        if (bverifyState) {
            return;
        }
        synchronized (this) {
            if (bverifyState) return;
        }
        try {
            synchronized (this) {
                bverifyState = true;
            }
            timeTakenTOexecuteDB = 0;
            maxInQueueWaitingStart = 500; //todo: move it in initialization
            maxInQueueWaitingEnd = 300;
            for (int loopNumber = 0; loopNumber < numberOfLoops; loopNumber++) {
                addThreadProcessing(loopNumber + "");
                threadPool.execute(new DBActionThread(loopNumber));
                if (processingList.size() > maxInQueueWaitingStart) {
                    MRLog.debug("Wait to complete the already scheduled request. Request in progress:" + processingList.size() + " ");
                    isMaxInQueueWaiting = true;
                    waitForResult();
                    MRLog.debug("DONE. Wait to complete the already scheduled request. Request in progress:" + processingList.size() + " ");
                    isMaxInQueueWaiting = false;
                }
                
            }
            isMaxInQueueWaiting = false;
            waitForResult();
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            if (processingList.size() == 0) {
                bverifyState = false;
                executed = true;
                if (checkDone()) {
                    MRLog.debug("////////////////////////////" + name + "////////////////////////////" + bverifyState + " threadProcessing " + processingList.size());
                    long ave = timeTakenTOexecuteDB / numberOfLoops;
                    logSummary(ave, "");
                    MRLog.debug("////////////////////////////" + name + "////////////////////////////");
                    return;
                }
            }
        }
        loopEndlesslyCounter++;
        
    }
    
    private void waitForResult() {
        long timeWaiting = System.currentTimeMillis();
        boolean timedout = false;
        synchronized (this) {
            while (true) {
                try {
                    if (System.currentTimeMillis() - timeWaiting > timeout) {
                        timedout = true;
                        break;
                    }
                    wait(waitTime);
                    if (processingList.size() == 0) {
                        break;
                    }
                    if (isMaxInQueueWaiting) {
                        if (processingList.size() < maxInQueueWaitingEnd) {
                            break;
                        }
                    }
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
            if (timedout) {
                processingList.clear();
            }
        }
    }
    
    private void logSummary(long ave, String doc) {
        StringBuffer buf = new StringBuffer();
        String sum = "r" + numberOfLoops + "t" + numberOfThreads + "(" + dbActionID + ":" + appName + " SUMMARY)";
        
        if (numberOfLoops == 1) {
            buf.append(ave + "ms ");
        } else {
            buf.append(ave + "\\" + timeTakenTOexecuteDB + "ms ");
        }
        buf.append(sum);
        if (loopEndlesslyCounter > 0) {
            buf.append("repeat:" + loopEndlesslyCounter + " ");
        }
        buf.append(doc);
        MRLog.debug2(buf.toString());
    }
    
    protected void logExec(long t0, long loop, String doc) {
        timeTakenTOexecuteDB += (System.currentTimeMillis() - t0);
        long total = (loop + 1);//(endlessLoop + 1) * (loop + 1);
        long ave = 0;
        if (total > 0) ave = timeTakenTOexecuteDB / total;
        
        StringBuffer buf = new StringBuffer();
        String sum = "r" + numberOfLoops + "t" + numberOfThreads + "(" + getThreadName() + ":" + appName + " EXEC)";
        if (numberOfLoops == 1 || total == 1) {
            buf.append((System.currentTimeMillis() - t0) + "ms ");
        } else {
            buf.append((System.currentTimeMillis() - t0) + "ms ");
            buf.append("" + total + "\\" + numberOfLoops + " ");
            buf.append(ave + "\\" + timeTakenTOexecuteDB + "ms ");
        }
        //if(numberOfThreads>1)
        buf.append(sum);
        if (loopEndlesslyCounter > 0) {
            buf.append("repeat:" + loopEndlesslyCounter + " ");
        }
        buf.append(doc);
        MRLog.debug2(buf.toString());
    }
    
    public class DBActionThread implements Runnable {
        private int loop = 0;
        
        DBActionThread(int loop) {
            this.loop = loop;
        }
        
        @Override
        public void run() {
            // If the task is already running, return.
            try {
                if (!IpvsServer.getInstance().isShuttingDown()) {
                    try {
                        execute(loop);
                    } catch (Exception ex) {
                        MRLog.debug(ex.getMessage());
                    }
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            } finally {
                removeThreadProcessing("" + loop);
            }
        }
    }
    
}
