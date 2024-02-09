package com.ipvsserv.nxd.migration.tag;

import org.dom4j.Element;

import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.ServiceVersionMigrateImpl;
import com.ipvsserv.nxd.migration.task.MigrationTask;

public interface MigrationTag {
    public abstract void init(ServiceVersionMigrateImpl root, MigrationTag parent, Element element) throws Exception;

    //public abstract void preMigration(VariableDefination variables) throws Exception ;
    public abstract void doMigration(ScriptVariables variables) throws Exception;

    //public abstract void postMigration(VariableDefination variables) throws Exception ;

    public abstract MigrationTask getMigrationTask() throws Exception;

    public abstract void setMigrationTask(MigrationTask parent) throws Exception;

    public abstract MigrationTag getParent() throws Exception;

    public abstract void setParent(MigrationTag parent) throws Exception;

    public abstract Element getElement() throws Exception;

    public abstract void setElement(Element element) throws Exception;

    public abstract ScriptVariables getVariableDefination() throws Exception;

    public abstract void setVariableDefination(ScriptVariables variables) throws Exception;

    public abstract void destroy() throws Exception;

}
