package com.ipvs.assetadminservice.handler;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.impl.PEMImplementation;


public class AssetAdminPEMUtils {
    public static String generateAssetResourceXPath(String whereXPath, String objectType, String serviceName, MRRequest request,
        MRDB schema, MRLog log) throws MRException, Exception {
        MRConfig config = null;
        config = MRConfig.getInstance(serviceName, AMConst.AssetResource, log);

        String[] arrXpath = { whereXPath };
        Element whereAssistXML = null;

        int whereSelectNodeDepth = 20;
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement("AssetResourceList");
        MRXMLDBUtils.findChildElements(parentNID, responseEL, null, whereSelectNodeDepth, whereAssistXML, arrXpath, request,
            schema, log);

        List<String> assetResourceNidList = PEMImplementation.getNIDList(responseEL, AMConst.AssetResource);
        StringBuffer xPath = new StringBuffer();

        if ((assetResourceNidList != null) && (assetResourceNidList.size() != 0)) {
        	
            xPath.append("//"+AMConst.ASSER_RESOURCE_NID+"[ ");

            for (Iterator<String> objIT = assetResourceNidList.iterator(); objIT.hasNext();) {
                xPath.append(MRXMLUtils.buildXPath(".", objIT.next()));

                if (objIT.hasNext()) {
                    xPath.append(" or ");
                }
            }

            xPath.append(" ] ");
        }

        return xPath.toString();
    }
}
