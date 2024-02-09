package com.ipvs.device;

import org.dom4j.Document;

public interface DeviceAppI {
    public String setTXConnectionParameters(String srcPortElementID, Document streamDoc);
}
