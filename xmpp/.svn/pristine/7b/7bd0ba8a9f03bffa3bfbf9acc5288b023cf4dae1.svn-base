package com.ipvsserv.nxd.adminapi;

import java.io.File;

public class SaveParams {

    private File directory = null;
    private DBAdminParams params = null;

    private boolean isRootCollection = false;
    private boolean saveToCol = false;

    public SaveParams(File directory, DBAdminParams params, boolean isRootCollection, boolean saveToCol) {
        this.directory = directory;
        this.params = params;
        this.isRootCollection = isRootCollection;
        this.saveToCol = saveToCol;
    }

    public File getDirectory() {
        return directory;
    }

    public void setDirectory(File directory) {
        this.directory = directory;
    }

    public DBAdminParams getParams() {
        return params;
    }

    public void setParams(DBAdminParams params) {
        this.params = params;
    }

    public boolean isRootCollection() {
        return isRootCollection;
    }

    public void setRootCollection(boolean isRootCollection) {
        this.isRootCollection = isRootCollection;
    }

    public boolean isSaveToCol() {
        return saveToCol;
    }

    public void setSaveToCol(boolean saveToCol) {
        this.saveToCol = saveToCol;
    }

}