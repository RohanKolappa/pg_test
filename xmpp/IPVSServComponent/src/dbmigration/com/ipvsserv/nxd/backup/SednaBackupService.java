package com.ipvsserv.nxd.backup;

import java.io.File;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.nxd.api.xmldb.XmlDBModuleI;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class SednaBackupService implements VendorBackupService {
    //do anything vendor specific - sedna 

    @Override
    public void backupdb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //root collection.
        /*try {
            if(prootCollection instanceof Collection){
                Collection rootCollection =(Collection)prootCollection;
                XQueryService service = (XQueryService) dbModule.getServiceManager().getXQueryService(rootCollection);
                ResourceSet resourceSet = service.query("for $b in document('$indexes') return $b");
                try {
                    ResourceIterator iterator = resourceSet.getIterator();
                    while (iterator.hasMoreResources()) {
                        Resource resource = iterator.nextResource();
                        PrintWriter out = new PrintWriter(new FileOutputStream(directory.getPath() + "/$indexes.xml", false));
                        out.println(resource.getContent());
                        out.flush();
                        out.close();
                    }
                } catch (IOException ex) {
                    MRLog.error("IOException whilst trying to write file '" + directory.getPath() + "/$indexes.xml'.");
                } 
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }*/
    }

    private String getDomainName(File directory, String collectionNamePrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
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

    public void upgradeIndexes(String ID, String domainName, XmlDBModuleI dbModule) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            //String collectionNamePrefix =IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
            //String domainName =getDomainName(directory, collectionNamePrefix);
            MRLog.info("Upgrade Indexes for domainName '" + domainName + "'");
            if (domainName != null) {
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.setDomain(domainName);
                try {
                    dbModule.getDefaultIpvsDBDriver().deleteIndex(ID, node.getIndexName(), null);
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);
                }

                try {
                    dbModule.getDefaultIpvsDBDriver().addIndex(ID, node.getIndexName(), null, node.getCollectionName(), "/*/@NID");
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);
                }

                try {
                    dbModule.getDefaultIpvsDBDriver().deleteIndex(ID, node.getIndexNameCol(), null);
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);
                }
                try {
                    dbModule.getDefaultIpvsDBDriver().addIndex(ID, node.getIndexNameCol(), null, node.getCollectionName(), "/*/@parentNID");
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);

                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void ensureIndexes(String packetID, XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
            String domainName = getDomainName(directory, collectionNamePrefix);
            MRLog.info("create index for domainName '" + domainName + "'");
            upgradeIndexes(packetID + "_" + directory.getAbsolutePath(), domainName, dbModule);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void restoredb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String packetID = "restore_" + directory.getAbsolutePath() + "";
            ensureIndexes(packetID, dbModule, directory);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void factorydb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String packetID = "factorydb_" + directory.getAbsolutePath() + "";
            ensureIndexes(packetID, dbModule, directory);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void importdb(XmlDBModuleI dbModule, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String packetID = "importdb_" + directory.getAbsolutePath() + "";
            ensureIndexes(packetID, dbModule, directory);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public boolean isAddSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return false;
    }

    @Override
    public boolean isDeleteSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return false;
    }

    @Override
    public boolean isUpdateSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return false;
    }

}
