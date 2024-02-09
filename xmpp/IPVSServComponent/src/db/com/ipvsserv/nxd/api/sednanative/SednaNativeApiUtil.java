package com.ipvsserv.nxd.api.sednanative;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.List;

import ru.ispras.sedna.driver.DriverException;
import ru.ispras.sedna.driver.SednaConnection;
import ru.ispras.sedna.driver.SednaSerializedResult;
import ru.ispras.sedna.driver.SednaStatement;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.adminapi.SaveFileQueue;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class SednaNativeApiUtil {
    public static boolean disconnect = false;
    private static SednaNativeModule sednaModule = null;
    private static Object syncObj = new Object();
    private static int highPriorityCount = 0;
    private static long highPriorityCheckOutTime = 0;
    private static long highPriorityCheckInTime = 0;
    private static long delayTIme = 250;
    private static long queryStringFormatMaxSize_bytes = -1;

    public static long getQueryStringFormatMaxSize() {
        if (queryStringFormatMaxSize_bytes == -1) {
            queryStringFormatMaxSize_bytes = IpvsServGlobals.getIntProperty("shared.queryStringFormatMaxSize_bytes", 5 * 1000);
            MRLog.debug("queryStringFormatMaxSize_bytes " + queryStringFormatMaxSize_bytes);
        }
        return queryStringFormatMaxSize_bytes;
    }

    public static void setQueryStringFormatMaxSize(long queryStringFormatMaxSize) {
        SednaNativeApiUtil.queryStringFormatMaxSize_bytes = queryStringFormatMaxSize;
    }

    private static SednaConnection getSednaConnection(int count) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        SednaConnection sednaConnection = null;
        try {
            SednaNativeApiUtil.setHighPriorityCount(1);
            sednaConnection = getSednaModule().checkOut();
            return sednaConnection;
        } finally {
            SednaNativeApiUtil.setHighPriorityCount(-1);
        }
    }

    private static SednaConnection getSednaConnectionLowPriority(int count) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        while (true) {
            if (SednaNativeApiUtil.isAvailableForLowPriority()) {
                SednaConnection sednaConnection = getSednaModule().checkOutLowPriority();
                if (sednaConnection != null) {
                    return sednaConnection;
                }
            }
            Thread.sleep(100);
        }
    }

    private static void close(SednaConnection sednaConnection, boolean bPriority, boolean killConnection) {
        getSednaModule().checkIn(sednaConnection, killConnection);
    }

    private static SednaNativeModule getSednaModule() {
        if (sednaModule == null) {
            //sednaModule = IpvsServer.getInstance().getSednaModule();
            sednaModule = (SednaNativeModule) IpvsServer.getInstance().getModule(SednaNativeModule.class);
        }
        return sednaModule;
    }

    public static void setHighPriorityCount(int add) {
        synchronized (SednaNativeApiUtil.syncObj) {
            SednaNativeApiUtil.highPriorityCount = SednaNativeApiUtil.highPriorityCount + add;
            if (add == 1) {
                SednaNativeApiUtil.highPriorityCheckOutTime = System.currentTimeMillis();
            } else if (add == -1) {
                SednaNativeApiUtil.highPriorityCheckInTime = System.currentTimeMillis();
            }
        }
    }

    public static boolean isAvailableForLowPriority() {
        long diffLastCheckOut = 0;
        long diffLastCheckIN = 0;
        long curTime = System.currentTimeMillis();
        synchronized (SednaNativeApiUtil.syncObj) {
            diffLastCheckOut = curTime - SednaNativeApiUtil.highPriorityCheckOutTime;
            diffLastCheckIN = curTime - SednaNativeApiUtil.highPriorityCheckInTime;
            if (diffLastCheckOut > SednaNativeApiUtil.delayTIme && diffLastCheckIN > SednaNativeApiUtil.delayTIme) {
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////// api ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////// api ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////// api ////////////////////////////////////////////////////////////////
    public static boolean loadDocument(String docId, String collectionName, InputStream in, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return loadDocument(docId, collectionName, in, name, null, true);
    }

    public static boolean loadDocument(String docId, String collectionName, InputStream in, String name, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
        }

        tCon = System.currentTimeMillis();
        try {

            SednaStatement ss = sednaConnection.createStatement();
            //boolean hasResults = false;
            if (collectionName != null && !collectionName.trim().equals("")) {
                ss.loadDocument(in, docId, collectionName);
            } else {
                ss.loadDocument(in, docId);
            }
            doRollback = true;
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    if (MRLog.isDbsummaryLogEnable()) {
                        MRLog.debug("Rolling back transaction...");
                    }
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("loadDocument DriverException. " + name + " (" + docId + "," + collectionName + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("loadDocument IOException. " + name + " (" + docId + "," + collectionName + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("loadDocument Exception. " + name + " (" + docId + "," + collectionName + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, true, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }

    }

    public static boolean loadDocument(String docId, String collectionName, String in, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return loadDocument(docId, collectionName, in, name, null, true);
    }

    public static boolean loadDocument(String docId, String collectionName, String in, String name, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
        }

        tCon = System.currentTimeMillis();
        try {

            SednaStatement ss = sednaConnection.createStatement();
            //boolean hasResults = false;
            if (collectionName != null && !collectionName.trim().equals("")) {
                ss.loadDocument(in, docId, collectionName);
            } else {
                ss.loadDocument(in, docId);
            }
            doRollback = true;
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    if (MRLog.isDbsummaryLogEnable()) {
                        MRLog.debug("Rolling back transaction...");
                    }
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("loadDocument DriverException. " + name + " (" + docId + "," + collectionName + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("loadDocument IOException. " + name + " (" + docId + "," + collectionName + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("loadDocument Exception. " + name + " (" + docId + "," + collectionName + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, true, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean execute(String query, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, name, true, null, true);
    }

    public static boolean execute(String query, String name, boolean bPriority, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            if (bPriority) {
                sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
            } else {
                sednaConnection = SednaNativeApiUtil.getSednaConnectionLowPriority(0);
            }
        }
        tCon = System.currentTimeMillis();
        try {
            boolean result = false;
            SednaStatement st = sednaConnection.createStatement();

            if (query.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
                InputStream in;
                try {
                    in = new ByteArrayInputStream(query.getBytes("UTF-8"));
                } catch (UnsupportedEncodingException ex) {
                    MRLog.error(ex);
                    return false;
                }
                result = st.execute(in);
            } else {
                result = st.execute(query);
            }

            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            doRollback = true;
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    if (MRLog.isDbsummaryLogEnable()) {
                        MRLog.debug("Rolling back transaction...");
                    }
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute DriverException. " + name + " ( " + query + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute IOException. " + name + " (" + query + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute Exception. " + name + " (" + query + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, bPriority, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean execute(String query, List<String> listNodes, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, listNodes, name, true, null, true);
    }

    public static boolean execute(String query, List<String> listNodes, String name, boolean bPriority, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            if (bPriority) {
                sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
            } else {
                sednaConnection = SednaNativeApiUtil.getSednaConnectionLowPriority(0);
            }
        }
        tCon = System.currentTimeMillis();
        try {

            boolean result = false;
            SednaStatement st = sednaConnection.createStatement();

            if (query.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
                InputStream in;
                try {
                    in = new ByteArrayInputStream(query.getBytes("UTF-8"));
                } catch (UnsupportedEncodingException ex) {
                    MRLog.error(ex);
                    return false;
                }
                result = st.execute(in);
            } else {
                result = st.execute(query);
            }

            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            doRollback = true;
            if (result) {
                SednaSerializedResult ssr = st.getSerializedResult();
                /*String item = ssr.next();
                int itemCount = 0;
                int itemNullCount = 0;
                while (item != null || itemNullCount <2 ) {
                    if(item != null && !item.trim().equals("")){
                        ++itemCount;
                        item = CompUtils.removeXmlProlog(item);
                        listNodes.add(item);
                    }else{
                        ++itemNullCount;
                    }
                    System.out.println(item);
                    item = ssr.next();
                }*/
                //todo: enable above code
                //some times the value can be null ... 
                //still next value can be not null ?? 
                String item = null;
                int itemCount = 0;
                while ((item = ssr.next()) != null) {
                    ++itemCount;
                    item = CompUtils.removeXmlProlog(item);
                    listNodes.add(item);
                }
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + listNodes.toString());
            }
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    if (MRLog.isDbsummaryLogEnable()) {
                        MRLog.debug("Rolling back transaction...");
                    }
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute DriverException." + name + " (" + query + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute IOException. " + name + " (" + query + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute Exception. " + name + " (" + query + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, bPriority, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean execute(String query, StringBuffer listNodes, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, listNodes, name, true, null, true);
    }

    public static boolean execute(String query, StringBuffer listNodes, String name, boolean bPriority, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            if (bPriority) {
                sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
            } else {
                sednaConnection = SednaNativeApiUtil.getSednaConnectionLowPriority(0);
            }
        }
        tCon = System.currentTimeMillis();
        try {

            boolean result = false;
            SednaStatement st = sednaConnection.createStatement();
            if (query.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
                InputStream in;
                try {
                    in = new ByteArrayInputStream(query.getBytes("UTF-8"));
                } catch (UnsupportedEncodingException ex) {
                    MRLog.error(ex);
                    return false;
                }
                result = st.execute(in);
            } else {
                result = st.execute(query);
            }

            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            doRollback = true;
            if (result) {
                SednaSerializedResult ssr = st.getSerializedResult();
                String item = null;
                int itemCount = 0;
                while ((item = ssr.next()) != null) {
                    ++itemCount;
                    item = CompUtils.removeXmlProlog(item);
                    listNodes.append(item);
                }
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + listNodes.toString());
            }
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    if (MRLog.isDbsummaryLogEnable()) {
                        MRLog.debug("Rolling back transaction...");
                    }
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute DriverException." + name + " (" + query + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute IOException. " + name + " (" + query + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute Exception. " + name + " (" + query + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, bPriority, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean execute(String query, List<String> listNodes, int startFrom, int countToFetch, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, listNodes, startFrom, countToFetch, name, true, null, true);
    }

    public static boolean execute(String query, List<String> listNodes, int startFrom, int countToFetch, String name, boolean bPriority, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            if (bPriority) {
                sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
            } else {
                sednaConnection = SednaNativeApiUtil.getSednaConnectionLowPriority(0);
            }
        }
        tCon = System.currentTimeMillis();
        try {
            if (startFrom < 1) {
                startFrom = 1;
            }
            if (countToFetch != -1 && countToFetch < 1) {
                countToFetch = 1;
            }
            int endAt = 0;
            if (!(countToFetch < 0)) {
                endAt = startFrom + countToFetch;
            }
            int iTotal = 0;
            int iCursor = 1;

            boolean result = false;
            SednaStatement st = sednaConnection.createStatement();

            if (query.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
                InputStream in;
                try {
                    in = new ByteArrayInputStream(query.getBytes("UTF-8"));
                } catch (UnsupportedEncodingException ex) {
                    MRLog.error(ex);
                    return false;
                }
                result = st.execute(in);
            } else {
                result = st.execute(query);
            }

            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            doRollback = true;
            if (result) {
                SednaSerializedResult ssr = st.getSerializedResult();
                String item = null;
                int itemCount = 0;
                while ((item = ssr.next()) != null) {
                    if (countToFetch == -1) {
                        item = CompUtils.removeXmlProlog(item);
                        listNodes.add(item);
                        iTotal = iTotal + 1;
                    } else {
                        if (iCursor < startFrom) {
                        } else if (iCursor >= endAt) {
                            break;
                        } else {
                            // if(list.contains(item))
                            item = CompUtils.removeXmlProlog(item);
                            listNodes.add(item);
                            iTotal = iTotal + 1;
                            if (MRLog.isDbqueryresultsLogEnable()) {
                                MRLog.info("queryresult  " + name + " " + item);
                            }
                        }
                        iCursor = iCursor + 1;
                    }

                    ++itemCount;
                }
            }
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    if (MRLog.isDbsummaryLogEnable()) {
                        MRLog.debug("Rolling back transaction...");
                    }
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute DriverException. " + name + " (" + query + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute IOException. " + name + " (" + query + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute Exception. " + name + " (" + query + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, bPriority, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    public static boolean execute(String query, StringBuffer bufNodes, int startFrom, int countToFetch, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, bufNodes, startFrom, countToFetch, name, true, null, true);
    }

    public static boolean execute(String query, StringBuffer bufNodes, int startFrom, int countToFetch, String name, boolean bPriority, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            if (bPriority) {
                sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
            } else {
                sednaConnection = SednaNativeApiUtil.getSednaConnectionLowPriority(0);
            }
        }
        tCon = System.currentTimeMillis();
        try {
            if (startFrom < 1) {
                startFrom = 1;
            }
            if (countToFetch != -1 && countToFetch < 1) {
                countToFetch = 1;
            }
            int endAt = 0;
            if (!(countToFetch < 0)) {
                endAt = startFrom + countToFetch;
            }
            int iTotal = 0;
            int iCursor = 1;

            boolean result = false;
            SednaStatement st = sednaConnection.createStatement();

            if (query.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
                InputStream in;
                try {
                    in = new ByteArrayInputStream(query.getBytes("UTF-8"));
                } catch (UnsupportedEncodingException ex) {
                    MRLog.error(ex);
                    return false;
                }
                result = st.execute(in);
            } else {
                result = st.execute(query);
            }

            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            doRollback = true;
            if (result) {
                SednaSerializedResult ssr = st.getSerializedResult();
                String item = null;
                //int itemCount = 0;
                while ((item = ssr.next()) != null) {
                    if (countToFetch == -1) {
                        item = CompUtils.removeXmlProlog(item);
                        bufNodes.append(item);
                        iTotal = iTotal + 1;
                    } else {
                        if (iCursor < startFrom) {
                        } else if (iCursor >= endAt) {
                            break;
                        } else {
                            // if(list.contains(item))
                            item = CompUtils.removeXmlProlog(item);
                            bufNodes.append(item);
                            iTotal = iTotal + 1;
                        }
                        iCursor = iCursor + 1;
                    }

                }
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    MRLog.debug("Rolling back transaction...");
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("execute DriverException. " + name + " (" + query + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("execute IOException. " + name + " (" + query + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("execute Exception.  " + name + " (" + query + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, bPriority, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean execute(String query, SaveFileQueue queue, String name) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, queue, name, true, null, true);
    }

    public static boolean execute(String query, SaveFileQueue queue, String name, boolean bPriority, SednaConnection sednaConnection, boolean autoCommitMode) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        boolean doRollback = false;
        boolean killConnection = false;
        //SednaConnection sednaConnection = null;
        if (sednaConnection == null) {
            //autoCommitMode=true;
            if (bPriority) {
                sednaConnection = SednaNativeApiUtil.getSednaConnection(0);
            } else {
                sednaConnection = SednaNativeApiUtil.getSednaConnectionLowPriority(0);
            }
        }
        tCon = System.currentTimeMillis();
        try {
            boolean result = false;
            SednaStatement st = sednaConnection.createStatement();
            if (query.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
                InputStream in;
                try {
                    in = new ByteArrayInputStream(query.getBytes("UTF-8"));
                } catch (UnsupportedEncodingException ex) {
                    MRLog.error(ex);
                    return false;
                }
                result = st.execute(in);
            } else {
                result = st.execute(query);
            }

            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            doRollback = true;
            if (result) {
                SednaSerializedResult ssr = st.getSerializedResult();
                String item = null;
                if (MRLog.isDbadminLogEnable()) {
                    int itemCount = 0;
                    long t1 = System.currentTimeMillis();
                    MRLog.debug(System.currentTimeMillis() + " START: " + (t1 - tCon));
                    while ((item = ssr.next()) != null) {
                        ++itemCount;
                        MRLog.debug(System.currentTimeMillis() + " itemCount " + itemCount + " : " + (System.currentTimeMillis() - t1));
                        queue.addDocumentToQueue(item);
                        t1 = System.currentTimeMillis();
                    }
                    t1 = System.currentTimeMillis();
                    MRLog.debug(System.currentTimeMillis() + " END: " + (t1 - tCon));
                } else {
                    while ((item = ssr.next()) != null) {
                        queue.addDocumentToQueue(item);
                    }
                }
            }
            if (autoCommitMode) {
                sednaConnection.commit();
            }
            return true;
        } catch (DriverException ex) {
            killConnection = true;
            if (doRollback) {
                try {
                    if (MRLog.isDbsummaryLogEnable()) {
                        MRLog.debug("Rolling back transaction...");
                    }
                    if (autoCommitMode) {
                        sednaConnection.rollback();
                    }
                } catch (DriverException derb) {
                    MRLog.debug("Transaction rollback failed (" + derb.getMessage() + ").");
                }
            }
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute DriverException." + name + " (" + query + ") " + ex.getErrorCode() + " : " + ex.getErrorMessage() + " " + ex.getDebugInfo());
        } catch (IOException ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute IOException. " + name + " (" + query + ") " + ex.getMessage());
        } catch (Exception ex) {
            killConnection = true;
            MRLog.error("Errors (" + ex.getMessage().replaceAll("\\s+", " ") + ").");
            throw new DBExecutionException("Sedna Execute Exception. " + name + " (" + query + ") " + ex.getMessage());
        } finally {
            if (autoCommitMode) {
                SednaNativeApiUtil.close(sednaConnection, bPriority, killConnection);
            }
            long t1 = System.currentTimeMillis();
            if (MRLog.isDbsummaryLogEnable()) {
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }

    }
}
