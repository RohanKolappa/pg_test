package com.ipvsserv.nxd.api.xmldb;

import java.util.Hashtable;

import net.cfoster.sedna.IndexManagementService;
import net.cfoster.sedna.ModuleManagementService;
import net.cfoster.sedna.RoleManagementService;
import net.cfoster.sedna.SednaUpdateService;
import net.cfoster.sedna.UserManagementService;

import org.xmldb.api.base.Collection;
import org.xmldb.api.base.Service;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.CollectionManagementService;
import org.xmldb.api.modules.TransactionService;
import org.xmldb.api.modules.XPathQueryService;
import org.xmldb.api.modules.XQueryService;
import org.xmldb.api.modules.XUpdateQueryService;

import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class ServiceManagerWrapper {
    private Hashtable<String, String> serviceVersions = new Hashtable<String, String>();

    public String getServiceVersion(String serviceName) {
        if (serviceVersions.containsKey(serviceName)) {
            return serviceVersions.get(serviceName);
        }
        return null;
    }

    public void setServiceVersion(Collection collection, String serviceName, String version) {
        Service serivce;
        try {
            serivce = collection.getService(serviceName, version);
            if (serivce != null) {
                if (!serviceVersions.containsKey(serviceName)) {
                    fillServicesList(collection);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);

        }
    }

    public void fillServicesList(Collection rootCollection) {
        try {
            serviceVersions = new Hashtable<String, String>();
            Service[] services = rootCollection.getServices();
            for (int i = 0; i < services.length; i++) {
                Service service = services[i];
                serviceVersions.put(service.getName(), service.getVersion());
            }
        } catch (Exception ex) {
            MRLog.error("Exception: " + ex.getMessage());
        }
    }

    public Service getService(Collection collection, String serviceName, String pversion) throws XMLDBException {
        String version = getServiceVersion(serviceName);
        if (version != null) {
            //get the version from services list.
            return collection.getService(serviceName, version);
        } else {
            //may be there, if added by application after connecting to db
            Service serivce = collection.getService(serviceName, pversion);
            if (serivce != null) {
                if (getServiceVersion(serviceName) == null) {
                    fillServicesList(collection);
                }
            }
            return serivce;
        }
    }

    public Service getCollectionManagementService(Collection collection) throws XMLDBException {
        return (CollectionManagementService) getService(collection, "CollectionManagementService", "1.0");
    }

    public Service getIndexManagementService(Collection collection) throws XMLDBException {
        return (IndexManagementService) getService(collection, "IndexManagementService", "1.0");
    }

    public Service getModuleManagementService(Collection collection) throws XMLDBException {
        return (ModuleManagementService) getService(collection, "ModuleManagementService", "1.0");
    }

    public Service getRoleManagementService(Collection collection) throws XMLDBException {
        return (RoleManagementService) getService(collection, "RoleManagementService", "1.0");
    }

    public Service getSednaUpdateService(Collection collection) throws XMLDBException {
        return (SednaUpdateService) getService(collection, "SednaUpdateService", "1.0");
    }

    public Service getTransactionService(Collection collection) throws XMLDBException {
        return (TransactionService) getService(collection, "TransactionService", "1.0");
    }

    public Service getUserManagementService(Collection collection) throws XMLDBException {
        return (UserManagementService) getService(collection, "UserManagementService", "1.0");
    }

    public Service getXPathQueryService(Collection collection) throws XMLDBException {
        return (XPathQueryService) getService(collection, "XPathQueryService", "1.0");
    }

    public Service getXQueryService(Collection collection) throws XMLDBException {
        return (XQueryService) getService(collection, "XQueryService", "1.0");
    }

    public Service getXUpdateQueryService(Collection collection) throws XMLDBException {
        return (XUpdateQueryService) getService(collection, "XUpdateQueryService", "1.0");
    }
}
