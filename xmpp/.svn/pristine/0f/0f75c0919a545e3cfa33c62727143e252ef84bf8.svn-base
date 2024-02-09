package com.ipvsserv.nxd.migration.task;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.threadmodel.ChangeThreadName;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.migration.DBMigrationModule;
import com.ipvsserv.nxd.migration.ExternalDBMigrationModule;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.ServiceVersionMigrateImpl;
import com.ipvsserv.nxd.migration.tag.MigrationTag;
import com.ipvsserv.nxd.migration.tag.MigrationTagLoader;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServModule;
import com.ipvsserv.server.IpvsServer;

public class MigrationTask extends IpvsServBasicModule {
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
    private ServiceVersionMigrateImpl root = null;
    private MigrationTasks migrationTasks = null;

    private String xpath;
    private Element element = null;

    public MigrationTask() {
        super("MigrationTask");
    }

    public MigrationTask(String moduleName) {
        super(moduleName);
    }

    public MigrationTask(MigrationTasks migrationTasks) {
        this();
        this.migrationTasks = migrationTasks;
    }

    private void log(String msg) {
        root.getMigrationModule().debug("Task " + msg);
    }

    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            IpvsServModule mod = getParent();
            if (mod == null) {
                //thorw error
            }
            if (mod instanceof DBMigrationModule) {
                DBMigrationModule dbMigrationModule = (DBMigrationModule) mod;
                root = dbMigrationModule.getMigrateServiceVersion();
            } else if (mod instanceof ExternalDBMigrationModule) {
                ExternalDBMigrationModule dbMigrationModule = (ExternalDBMigrationModule) mod;
                root = dbMigrationModule.getMigrateServiceVersion();
            }

            dbActionID = getProperty("name", "");
            name = dbActionID;
            ////////////// if endless loop ////////////////////
            loopEndlessly = CompUtils.booleanValue(params.get("loopEndlessly"), root.getSettings().getBooleanProperty("DBTestActionsSequence.loopEndlessly", false));
            initialDelay = CompUtils.longValue(params.get("initialDelay"), root.getSettings().getLongProperty("DBTestActionsSequence.initialDelay", initialDelay));
            delay = CompUtils.longValue(params.get("delay"), root.getSettings().getLongProperty("DBTestActionsSequence.delay", delay));
            String unitStr = getProperty("delayUnit", root.getSettings().getProperty("DBTestActionsSequence.delayUnit", "SECONDS"));
            if (unitStr.equalsIgnoreCase("SECONDS"))
                unit = TimeUnit.SECONDS;
            else if (unitStr.equalsIgnoreCase("MINUTES"))
                unit = TimeUnit.MINUTES;
            else if (unitStr.equalsIgnoreCase("HOURS"))
                unit = TimeUnit.HOURS;
            else
                unit = TimeUnit.SECONDS;

            ///////////////////
            ipvsDomain = getProperty("ipvsDomain", root.getSettings().getProperty("DBTestActionsSequence.ipvsDomain", "default"));
            collectionPrefix = getProperty("collectionPrefix", root.getSettings().getProperty("DBTestActionsSequence.collectionPrefix", "ipvsCol"));
            documentPrefix = getProperty("documentPrefix", root.getSettings().getProperty("DBTestActionsSequence.documentPrefix", "ipvsDoc"));

            // thread settings ///////////////////
            //if more than one thread -numberOfThreads=1 -numberOfLoops=6
            numberOfThreads = CompUtils.intValue(params.get("numberOfThreads"), -1);
            numberOfLoops = CompUtils.intValue(params.get("numberOfLoops"), -1);
            if (numberOfThreads == -1) {
                numberOfThreads = CompUtils.intValue(IpvsServGlobals.getProcessArguments().getArgumentParam("numberOfThreads", root.getSettings().getProperty("DBTestActionsSequence.numberOfThreads", "1")), 1);
            }
            if (numberOfLoops == -1) {
                numberOfLoops = CompUtils.intValue(IpvsServGlobals.getProcessArguments().getArgumentParam("numberOfLoops", root.getSettings().getProperty("DBTestActionsSequence.numberOfLoops", "1")), 1);
            }
            //numberOfThreads = CompUtils.intValue(params.get("numberOfThreads"), TestGlobals.getIntProperty("DBTestActionsSequence.numberOfThreads", 1));
            //numberOfLoops = CompUtils.intValue(params.get("numberOfLoops"), TestGlobals.getIntProperty("DBTestActionsSequence.numberOfLoops", 1));
            if (numberOfThreads < 1)
                numberOfThreads = 1;
            if (numberOfLoops < 1)
                numberOfLoops = 1;
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
    public void verifyState() throws Exception {
        if (bverifyState) {
            return;
        }
        synchronized (this) {
            if (bverifyState)
                return;
            bverifyState = true;
        }
        try {
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
                    //MRLog.debug("DONE. Wait to complete the already scheduled request. Request in progress:" + processingList.size() + " ");
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
                    long ave = timeTakenTOexecuteDB / numberOfLoops;
                    logSummary(ave, "");
                    return;
                }
            }
        }
        loopEndlesslyCounter++;

    }

    public void execute(int loop) throws Exception {
        try {
            //log("Start execute task : " + getName());
            log("" + getName());
            ScriptVariables variables = new ScriptVariables();
            List<?> list = element.elements();
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                if (el.getName().startsWith(MigrationTagLoader.TAG_PREFIX)) {
                    try {
                        //log("Start execute tag : " + el.getName());
                        boolean enable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(el, "enable", "true"), true);
                        if (enable) {
                            //log("execute tag : " + el.asXML().replace('\n',' '));
                            MigrationTag tag = MigrationTagLoader.getInstance().getMigrationTag(el.getName());
                            tag.setMigrationTask(this);
                            tag.init(root, null, el);
                            //tag.preMigration(variables);
                            tag.doMigration(variables);
                        }
                        //tag.postMigration(variables);
                    } catch (Exception ex) {
                        MRLog.error(ex);
                    } finally {
                        //log("End execute tag : " + el.asXML().replace('\n',' '));
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            //log("End  execute task : " + getName());
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
        //log("stopping task : " + getIpvsModuleName() );                        
        destroy();
    }

    @Override
    public void destroy() throws Exception {
        super.destroy();
        try {
            if (threadPool != null)
                threadPool.shutdown();
        } catch (Exception ex) {

        }
    }

    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }

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

    private void waitForResult() {
        long timeWaiting = System.currentTimeMillis();
        boolean timedout = false;
        synchronized (this) {
            while (true) {
                try {
                    if (System.currentTimeMillis() - timeWaiting > timeout) {
                        timedout = true;
                        //break;
                        log(" TIMED_OUT: " + getName() + " MigrationTask still processing " + processingList.size() + ". Waiting again ....");
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
                //processingList.clear();
            }
        }
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

    private boolean checkDone() {
        if (!loopEndlessly && isExecuted()) {
            if (sizeThreadProcessing() > 0)
                return true;
            try {
                raiseProcessingDoneEvent();
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            try {
                destroy();
                return true;
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        return false;
    }

    private void logSummary(long ave, String doc) {
        StringBuffer buf = new StringBuffer();
        String sum = "r" + numberOfLoops + "t" + numberOfThreads + "(" + dbActionID + " task tag :" + appName + "DONE. SUMMARY)";
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

    /////////////////////////

    public String getXpath() {
        return xpath;
    }

    public void setXpath(String xpath) {
        this.xpath = xpath;
    }

    public Element getElement() {
        return element;
    }

    public void setElement(Element element) {
        this.element = element;
    }

    public MigrationTasks getMigrationTasks() {
        return migrationTasks;
    }

    public void setMigrationTasks(MigrationTasks migrationTasks) {
        this.migrationTasks = migrationTasks;
    }

    //
    //protected void logExec(long t0, long loop, String doc) {
    //  timeTakenTOexecuteDB += (System.currentTimeMillis() - t0);
    //  long total = (loop + 1);//(endlessLoop + 1) * (loop + 1);
    //  long ave = 0;
    //  if (total > 0)
    //      ave = timeTakenTOexecuteDB / total;
    //
    //  StringBuffer buf = new StringBuffer();
    //  String sum = "r" + numberOfLoops + "t" + numberOfThreads + "(" + getThreadName() + ":" + appName + " EXEC)";
    //  if (numberOfLoops == 1 || total == 1) {
    //      buf.append((System.currentTimeMillis() - t0) + "ms ");
    //  } else {
    //      buf.append((System.currentTimeMillis() - t0) + "ms ");
    //      buf.append("" + total + "\\" + numberOfLoops + " ");
    //      buf.append(ave + "\\" + timeTakenTOexecuteDB + "ms ");
    //  }
    //  //if(numberOfThreads>1)
    //  buf.append(sum);
    //  if (loopEndlesslyCounter > 0) {
    //      buf.append("repeat:" + loopEndlesslyCounter + " ");
    //  }
    //  buf.append(doc);
    //  MRLog.debug2(buf.toString());
    //}

    //    protected Hashtable<String, String> getReplaceList() {
    //        boolean enableReplace = CompUtils.booleanValue(getPropertyDeep("enableReplace", "false"), false);
    //        Hashtable<String, String> replacelist = new Hashtable<String, String>();
    //        if (enableReplace) {
    //            String find = "replaceStrFind";
    //            String with = "replaceStrWith";
    //            String key = null;
    //            String value = null;
    //            int i = 0;
    //            while (true) {
    //                i++;
    //                key = getPropertyDeep(find + i, null);
    //                value = getPropertyDeep(with + i, null);
    //                if (key != null && value != null) {
    //                    replacelist.put(key, value);
    //                } else if (key == null) {
    //                    break;
    //                }
    //            }
    //        }
    //        return replacelist;
    //    }
    //
    //    protected String getPropertyDeep(String key, String sdefault) {
    //        String value = params.get(key);
    //        if (value != null) {
    //            return value;
    //        }
    //        value = root.getSettings().getProperty(dbActionID + "." + key, null);
    //        if (value != null) {
    //            return value;
    //        }
    //        value = root.getSettings().getProperty("DBTestActionsSequence." + key, null);
    //        if (value != null) {
    //            return value;
    //        }
    //        return sdefault;
    //    }
    //
    //    protected String getPropertyXmlFile(String key, String parentFolder, String sdefault) throws Exception {
    //        try {
    //            String xmlFilePath = getPropertyDeep(key, sdefault);
    //            String prefix = root.getSettings().getProperty("DBTestActionsSequence." + IpvsServGlobals.getIpvsAppName() + "_file_prefix", null);
    //            if (prefix != null && !prefix.trim().equals("")) {
    //                File file = new File(xmlFilePath);
    //                String fileName = file.getName();
    //                String newFileName = prefix + fileName;
    //                if (fileName.length() < xmlFilePath.length()) {
    //                    newFileName = xmlFilePath.substring(0, xmlFilePath.lastIndexOf(fileName)) + xmlFilePath;
    //                    //todo: test this
    //                }
    //                newFileName = verifyAndGetTestXmlPath(newFileName, parentFolder, sdefault);
    //                if (newFileName != null)
    //                    return newFileName;
    //            }
    //            xmlFilePath = verifyAndGetTestXmlPath(xmlFilePath, parentFolder, sdefault);
    //            File file = new File(xmlFilePath);
    //            if (!file.exists()) {
    //                throw new Exception(name + " xmlFilePath '" + xmlFilePath + "' not found");
    //            }
    //            return xmlFilePath;
    //        } catch (Exception ex) {
    //            throw ex;
    //        }
    //    }
    //
    //    protected String verifyAndGetTestXmlPath(String xmlFilePath, String parentFolder, String sdefault) throws Exception {
    //        try {
    //            File file = new File(xmlFilePath);
    //            if (file.exists()) {
    //                return xmlFilePath;
    //            }
    //            if (parentFolder != null && !parentFolder.trim().equals("")) {
    //                parentFolder = parentFolder.replace("/", File.separator).replace("\\", File.separator);
    //                if (!parentFolder.endsWith(File.separator))
    //                    parentFolder = parentFolder + File.separator;
    //                try {
    //                    xmlFilePath = IpvsServGlobals.verifyAndGetTestXmlPath(parentFolder + xmlFilePath);
    //                } catch (Exception ex) {
    //                }
    //            }
    //            xmlFilePath = IpvsServGlobals.verifyAndGetTestXmlPath(xmlFilePath);
    //            ;
    //            return xmlFilePath;
    //        } catch (Exception ex) {
    //            //MRLog.error(ex);
    //        }
    //        return null;
    //    }
    //
    //    public int getThreadNumber(int loopNumber) {
    //        /* if (numberOfThreads == 1) return 0;
    //         try {
    //             int col = numberOfLoops / loopNumber;
    //             if (DBTestActionShared.getInstance().getCollections() >= col) {
    //                 return col;
    //             }
    //         } catch (Exception ex) {}*/
    //        return 0;
    //    }
    //
    //    protected String replace(String xml, Hashtable<String, String> replacelist, int loopNumber) {
    //        /*<param name='replaceStrFind'>{0}</param>
    //        <param name='replaceStrWith'>loopNumber</param>  
    //        <param name='replaceStrFind'>{1}</param>
    //        <param name='replaceStrWith'>threadNumber</param>   
    //        <param name='replaceStrFind'>{2}</param>
    //        <param name='replaceStrWith'>outerLoop</param> */
    //        for (Enumeration<String> e = replacelist.keys(); e.hasMoreElements();) {
    //            String find = e.nextElement();
    //            String with = replacelist.get(find);
    //            if (with.equalsIgnoreCase("loopNumber")) {
    //                xml = xml.replaceAll(find, loopNumber + "");
    //            } else if (with.equalsIgnoreCase("threadNumber")) {
    //                xml = xml.replaceAll(find, getThreadNumber(loopNumber) + "");
    //            } else if (with.equalsIgnoreCase("outerLoop")) {
    //                xml = xml.replaceAll(find, loopEndlesslyCounter + "");
    //            }
    //        }
    //        return xml;
    //    }

}
