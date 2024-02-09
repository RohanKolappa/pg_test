package com.ipvs.sessionservice.handler.base;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.impl.TargetObjectLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.sessionservice.impl.SessionObjectPermissions;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public abstract class GetTagsBaseRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI {

	protected Document getTags(String requestName,String viewObjectType,MRRequest request, XMPPI xmpp, MRLog log,MRDB db, 
			MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		Document requestDoc = request.getMessage().getDocument();
		Element getTagsEL = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName);
		String targetXpath = "";
		List<String> listOfXPath = new ArrayList<String>();

		if(!request.isSuperUser()) {
			List<String> objectList = SessionObjectPermissions.getPolicyRoleObjectListForGivenViewObject(log, viewObjectType);
			String wherePermissions = MRXMLUtils.getAttributeValue(getTagsEL, SessionServiceConst.WHERE_PERMISSIONS, "");
			targetXpath = TargetObjectLib.getTargetXpath(request, xmpp, log, db, resourceFactory, serviceAgentFactory, objectList, wherePermissions, "");
			
			if(targetXpath.equals(PolicyAdminConstants.NULL)) {
				Element responseEL = MRXMLUtils.stringToElement("<"+requestName+" from='0' total='0'/>");
				return request.createResponseData(responseEL);
			}
			
			//target xPath empty means ALL read all tags from database
			if (!targetXpath.isEmpty()) {
				targetXpath = targetXpath.replaceAll(" and ", " or ");
				listOfXPath.add("(" + targetXpath + ")");
			} else {
				if (objectList != null && !objectList.isEmpty()) {
					StringBuilder tagObjectTypeXpath = new StringBuilder();
					String xpathStr = "//TagObjectTypeList/TagObjectType='";
					String delim = "";
					for (String objectResourceStr : objectList) {
						tagObjectTypeXpath.append(delim).append(xpathStr).append(objectResourceStr).append("'");
						delim = " or ";
					}
					listOfXPath.add("(" + tagObjectTypeXpath.toString() + ")");
				}
			}
		}
		
		String whereNameSubstring = MRXMLUtils.getAttributeValue(getTagsEL, SessionServiceConst.WHERE_NAME_TEXT_SUBSTRING, "");
		String whereValueSubString = MRXMLUtils.getAttributeValue(getTagsEL, SessionServiceConst.WHERE_VALUE_TEXT_SUBSTRING, "");

		if(!whereNameSubstring.isEmpty()) {
			listOfXPath.add(" matches(Groups/Tag/@Name, '" + whereNameSubstring + "', 'i')  ");
		}

		if(!whereValueSubString.isEmpty()){
			listOfXPath.add(" matches(Groups/Tag/@Value, '" + whereValueSubString + "', 'i')  ");
		}
		String sortBy = MRXMLUtils.getAttributeValue(getTagsEL, SessionServiceConst.SORT_BY);

		if("Value".equals(sortBy)){
			sortBy = "@Value";
		} else {
			sortBy = "@Name";
		}
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.TAGS + SessionServiceConst.XQUERYXML;
		request.setSuperUser(true);
		Document responseDoc = getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getTagsEL, SessionServiceConst.TAGS, sortBy, listOfXPath, xqueryFileName);
		request.setSuperUser(false);
		return  responseDoc;
	}

}
