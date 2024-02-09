package com.ipvsserv.nxd.adminapi;

public class DBAdminParams {
    private boolean deleteColRecursive = false;
    private boolean saveToColEnable = false;
    private boolean saveForDiff = false;

    public boolean isSaveForDiff() {
        return saveForDiff;
    }

    public boolean isSaveToColEnable() {
        return saveToColEnable;
    }

    public void setSaveForDiff(boolean saveForDiff) {
        this.saveForDiff = saveForDiff;
    }

    public void setSaveToColEnable(boolean saveToColEnable) {
        this.saveToColEnable = saveToColEnable;
    }

    public boolean isDeleteColRecursive() {
        return deleteColRecursive;
    }

    public void setDeleteColRecursive(boolean deleteColRecursive) {
        this.deleteColRecursive = deleteColRecursive;
    }
}
