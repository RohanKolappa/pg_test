package com.barco.device.vng.Media;

import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.ServiceException;
import com.barco.device.vng.Media.streamingserver.NetworkResource;

public interface StreamingServerI {

    public void createSSInput(Element inputElement, Document streamDataDoc, Document mediaSourceDataDoc) throws Exception;
    public void refreshSSInput(Element inputElement, Document streamDataDoc) throws Exception;
    public void updateSSOutput(Element outputElement, Document streamDataDoc) throws Exception;
    public void updateSSInput(Element inputElement, Document streamDataDoc) throws Exception;
    public void updateSSNetworkInput(Element inputElement,Document streamDataDoc,String action) throws Exception;
    public void deleteSSInput(Element inputElement) throws Exception;
    public void createSSOutput(Element outputElement, Element inputElement, Document streamDataDoc, Document mediaSourceDataDoc) throws Exception;
    public void deleteSSOutput(Element outputElement,Document streamDataDoc) throws Exception;
    public void stopSSOutputAt(Element instanceElement, Element streamElement, long offset) throws Exception;
    public void stopSSOutputAtSync(List<Element> streamElementList, long offset) throws Exception;
    public void startSSInputAtSync(Hashtable<String, Element> syncSourceList) throws Exception;
    public void startSSOutput(Element instanceElement, Element streamElement) throws Exception;
    public void stopSSInput(Element instanceElement) throws Exception;
    public void startSSInput(Element instanceElement, Element streamElement) throws Exception;
    public long getCurrentTC(Element ouputElement) throws ServiceException,Exception;
    public long getOffsetAtTC(long timecode,String mediaUUID,String srcDirPath) throws ServiceException,Exception;
    public void processStreamingServerNotification(String notifyMessage) throws ServiceException,Exception;
    public boolean isInputInstanceStarted(Element inputElement) throws Exception;
    public void getInputInstanceStatus(Element inputElement) throws Exception;
    public int getMaxEncoderInstances(String streamType);
    public void init() throws ServiceException,Exception;
    public NetworkResource getNetworkResource();
    //public void setNetworkResource(NetworkResource networkResource);
    public String getIPAddress() throws Exception;
    public long getTCAtOffset(String mediaUUID, long offset,String rootDir) throws Exception;   
    public void setupInternalInstances(Document doc) throws Exception;
}
