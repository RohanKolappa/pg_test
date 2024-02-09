package com.ipvs.app;

import java.io.File;
import java.net.URL;

import org.dom4j.Document;

import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRConst;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.test.TestResourceFactoryImpl;
import com.ipvs.utils.FileUtils;

public final class ServiceUtils {

  static final String DummyXMPPImplClassName = "com.ipvs.test.TestXMPPImpl";

  @SuppressWarnings("unused")
private static String xmppImplClassName = "com.ipvs.smack.XMPPUtils.IPVSSession";
  private static ServiceClient client = null;
 // private static boolean dummyXMPPImpl = false;
    
  public static void setDummyXMPPImpl(boolean dummyXMPPImpl) {
    if(dummyXMPPImpl)
      ServiceUtils.xmppImplClassName = ServiceUtils.DummyXMPPImplClassName;
  }
    
  public static String getRequestXML(String serviceName, String requestName) {
    URL url = ClassLoader.getSystemClassLoader().getResource(
        "com/ipvs/" + serviceName + "/xml/" +
        requestName);
    File file = new File(url.getFile());
    try {
      return FileUtils.readFile(file);
    }
    catch(Exception e) {
      return "File not found: " + url.toString();
    }
  }

  public static String[] getRequestList(String serviceName) {
    try {
      URL url = ClassLoader.getSystemClassLoader().getResource("com/ipvs/" + serviceName + "/xml");
      File directory = new File(url.getFile());
      if(directory != null)
        return directory.list();
    }
    catch(Exception e) {
      // Unable to get list of classes
    }
    return new String[0];
  }
  
  public static ServiceClient getServiceClient(ServiceMessageHandlerI messageHandler) {
    if(client != null)
      return client;
    TestResourceFactoryImpl resourceFactory = new TestResourceFactoryImpl();
    client = new ServiceClient(IPVSConst.getInstance().getXmppClientClass(),
        messageHandler, resourceFactory);
    return client;
  }

  public static String getUserJID(String loginResponse) {
    try {
      Document doc = MRXMLUtils.stringToDocument(loginResponse);
      return MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.USER_JID,null);
    }
    catch(Exception e) {
      e.printStackTrace();
      return null;
    }
  }



}
