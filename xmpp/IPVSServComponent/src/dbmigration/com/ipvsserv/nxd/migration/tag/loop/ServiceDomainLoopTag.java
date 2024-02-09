package com.ipvsserv.nxd.migration.tag.loop;

import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class ServiceDomainLoopTag extends MigrationTagImpl {
    private static int collectionType = 1;

    public ServiceDomainLoopTag() {
        super("CreateCollectionTag");
    }

    public ServiceDomainLoopTag(String moduleName) {
        super(moduleName);
    }

}
