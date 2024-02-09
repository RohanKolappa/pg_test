package com.ipvsserv.nxd.migration.tag;

import java.util.List;

import org.dom4j.Element;

import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.ServiceVersionMigrateImpl;
import com.ipvsserv.nxd.migration.task.MigrationTask;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class MigrationTagImpl implements MigrationTag {
    protected String taskName;
    protected IpvsDBDriver db = null;
    protected Element element = null;
    protected MigrationTag parent = null;
    protected ServiceVersionMigrateImpl root = null;
    protected ScriptVariables variables = new ScriptVariables();
    protected MigrationTask migrationTask = null;

    public MigrationTagImpl(String moduleName) {
        taskName = moduleName;
    }

    protected void log(String msg) {
        root.getMigrationModule().debug(" " + msg);
    }

    @Override
    public void init(ServiceVersionMigrateImpl root, MigrationTag parent, Element element) throws Exception {
        this.element = element;
        this.parent = parent;
        this.root = root;
        //db = IpvsServer.getInstance().getDefaultIpvsDBDriver();
        db = root.getScriptGlobals().getDBAPI();
    }

    @Override
    public void destroy() throws Exception {

    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        System.out.println("doMigration " + this.getName());

    }

    public MigrationTask getMigrationTask() throws Exception {
        return migrationTask;
    }

    public void setMigrationTask(MigrationTask migrationTask) throws Exception {
        this.migrationTask = migrationTask;
    }

    @Override
    public Element getElement() throws Exception {
        return element;
    }

    @Override
    public void setElement(Element element) throws Exception {
        this.element = element;
    }

    @Override
    public MigrationTag getParent() throws Exception {
        return parent;
    }

    @Override
    public void setParent(MigrationTag parent) throws Exception {
        this.parent = parent;
    }

    @Override
    public ScriptVariables getVariableDefination() throws Exception {
        return variables;
    }

    @Override
    public void setVariableDefination(ScriptVariables variables) throws Exception {
        this.variables = variables;
    }

    /////////////
    protected String getProperty(String name, String sdefault) {
        return null;
    }

    protected String getName() {
        return taskName;
    }

    protected void logExec(long t0, String string) {
    }

    protected String getPropertyXmlFile(String string, String queryFolderPath, Object object) {
        return null;
    }

    protected String getPropertyDeep(String string, Object object) {
        return null;
    }
    
    private List<String> list=null;
    public List<String>  getDomainNames( ) throws Exception {
        if(list==null){
            list = db.getDomainNames();
        }
        return list ;
    }
    /*private String documents =null;
    public String  getDocuments( ) throws Exception {
        if(documents==null){
            list = db.;
        }
        return list ;
    }*/

}
