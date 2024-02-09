package com.ipvsserv.nxd.migration;

import java.util.Hashtable;

public class ScriptVariables {
    protected Hashtable<String, String> list = new Hashtable<String, String>();
    protected Hashtable<String, Object> objectlist = new Hashtable<String, Object>();

    public Hashtable<String, String> getList() {
        return list;
    }

    public void reset() {
        list.clear();
        objectlist.clear();
    }

    public boolean isVariableExists(String name) {
        if (list.containsKey(name)) {
            return true;
        }
        if (objectlist.containsKey(name)) {
            return true;
        }
        return false;
    }

    public void setVariable(String name, String value) {
        list.put(name, value);
    }

    public String getVariable(String name, String pDefault) {
        String value = list.get(name);
        if (value == null || value.trim().equals("")) {
            return pDefault;
        }
        return value;
    }

    public void setObject(String name, String value) {
        objectlist.put(name, value);
    }

    public Object getObject(String name, Object pDefault) {
        Object value = list.get(name);
        if (value == null) {
            return pDefault;
        }
        return value;
    }
}
/*

<variable name="serviceVersion" description="current migration service version"/>
<variable name="migrationFolder" description="migration folder e.g. /etc/migration/1.0/database"/>
<variable name="workareafolder" description="work folder e.g. /data/dbmigration/workarea/1.0/migration"/>
<variable name="workareatempfolder" description="work temp folder e.g. /data/dbmigration/workarea/1.0/migration/temp"/>
<variable name="workareabackupfolder" description="work backup folder e.g. /data/dbmigration/workarea/1.0/pre_migration_bck_db"/>
<variable name="servicedomain" description="servicedomain - e.g. default"/>
<variable name="ipvs_collection" description="for current servicedomain - collection name e.g. IPVSc_default"/>
<variable name="ipvs_colindex" description="for current servicedomain - col index name e.g. IPVSis_colnids_default"/>
<variable name="ipvs_nodeindex" description="for current servicedomain - node index name e.g. IPVSi_nids_default"/>
<variable name="serviceVersion_db" description="current database service version"/>
<variable name="serviceVersion_software" description="current software service version"/>

*/