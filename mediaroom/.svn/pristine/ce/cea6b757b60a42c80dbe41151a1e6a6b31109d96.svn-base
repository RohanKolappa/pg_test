package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.service.MRXMLUtils;

public class AssetAdminUtils {
    /*public static Document getResponseDoc(MRRequest request) throws Exception{
        Document resDoc  = MRXMLUtils.getDocument();
        Element rootRes  = MRXMLUtils.addElement(resDoc, request.getResponseDataName());
        Element respRoot = MRXMLUtils.newElement(request.getHandlerBaseName());
        MRXMLUtils.addChildElement(respRoot, rootRes);
        return resDoc;
    }*/
    /*
    public static void main(String[] args){
         System.out.println(AddExportFileRequestHandler.class.getName());
         System.out.println(AddExportFileRequestHandler.class.getCanonicalName());
         System.out.println(AddExportFileRequestHandler.class.getSimpleName());
         System.out.println(AddExportFileRequestHandler.class.getPackage());
          
    }*/
    public static Element getResponseDoc(Class<?> cls) throws Exception {
        String requestHandlerName = cls.getSimpleName();
        int index = requestHandlerName.lastIndexOf(ServiceClient.REQUEST);
        if(index == -1)
            throw new Exception("Invalid class");
       
        String requestBaseName = requestHandlerName.substring(0, index);//MRRequest.getResponseName(requestHandlerName);
        Document doc  = MRXMLUtils.getDocument();
        Element root  = MRXMLUtils.addElement(doc, requestBaseName + ServiceClient.RESPONSE + "Data");
        Element respRoot =  MRXMLUtils.addElement(root, requestBaseName);
        return respRoot;
    }
}
