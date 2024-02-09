package com.ipvsserv.common.util;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class Version {
    
    private int major;
    
    private int minor;
    
    private int micro;
    
    private int statusVersion;
    
    private ReleaseStatus status;
    
    private String versionString;
    
    public Version(int major, int minor, int micro, ReleaseStatus status, int statusVersion) {
        this.major = major;
        this.minor = minor;
        this.micro = micro;
        this.status = status;
        this.statusVersion = statusVersion;
        if (status != null) {
            if (status == ReleaseStatus.Release) {
                versionString = major + "." + minor + "." + micro;
            } else {
                if (statusVersion >= 0) {
                    versionString = major + "." + minor + "." + micro + " " + status.toString() + " " + statusVersion;
                } else {
                    versionString = major + "." + minor + "." + micro + " " + status.toString();
                }
            }
        } else {
            versionString = major + "." + minor + "." + micro;
        }
    }
    
    public String getVersionString() {
        return versionString;
    }
    
    public ReleaseStatus getStatus() {
        return status;
    }
    
    public int getMajor() {
        return major;
    }
    
    public int getMinor() {
        return minor;
    }
    
    public int getMicro() {
        return micro;
    }
    
    public int getStatusVersion() {
        return statusVersion;
    }
    
    public enum ReleaseStatus {
        Release(""), Release_Candidate("RC"), Beta("Beta"), Alpha("Alpha");
        
        private String status;
        
        private ReleaseStatus(String status) {
            this.status = status;
        }
        
        @Override
        public String toString() {
            return status;
        }
    }
}
