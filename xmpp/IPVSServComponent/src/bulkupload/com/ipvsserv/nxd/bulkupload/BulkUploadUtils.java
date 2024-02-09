package com.ipvsserv.nxd.bulkupload;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import org.dom4j.Attribute;
import org.dom4j.CDATA;
import org.dom4j.Element;
import org.dom4j.Text;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.bulkupload.template.GroupCreateReference;
import com.ipvsserv.nxd.bulkupload.template.GroupTemplate;
import com.ipvsserv.nxd.bulkupload.template.UpdateElementDef;
import com.ipvsserv.nxd.bulkupload.template.UpdateElementXpathDef;
import com.ipvsserv.nxd.bulkupload.template.UpdateXmlDefination;
import com.ipvsserv.nxd.bulkupload.template.UpdateXmlDefinations;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class BulkUploadUtils {
    private static Random wheel = new Random(8989899);
    
    /*public static void applyNID(Element element, String parentNID, int row) throws Exception {
        try {
            String NID = IpvsDBUtils.parseNID_BULK(element, parentNID, true);
            Dom4jUtils.addAttribute(element, IpvsDBUtils.NODEID_2, NID);
            Dom4jUtils.addAttribute(element, IpvsDBUtils.PARSE_2, "true");
            IpvsDBUtils.parseNIDChilds_BULK(element, NID, true);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }*/

    public static void updateValueGroup(Hashtable<String, Element> templateMembers, GroupTemplate templateGroup) throws Exception {
        for (Iterator<GroupCreateReference> iterator = templateGroup.getGroupCreateReferences().iterator(); iterator.hasNext();) {
            GroupCreateReference createReference = iterator.next();
            Element fromXpathElement = templateMembers.get(createReference.getFromXpath_templateMemberId());
            Element setXpathElement = templateMembers.get(createReference.getSetXpath_templateMemberId());
            if (fromXpathElement == null || setXpathElement == null) {
                //if(fromXpathElement==null)
                //    throw new Exception( "not found member template='" + createReference.getFromXpath_templateMemberId() +"'. During updateValueGroup "+templateGroup.getGroupTemplateId() + " for fromXpath_groupTemplateMemberId ");
                // if(setXpathElement==null)
                //    throw new Exception( "not found member template='" + createReference.getSetXpath_templateMemberId()  +"'. During updateValueGroup "+templateGroup.getGroupTemplateId() + " for setXpath_groupTemplateMemberId ") ;
            } else {
                Object fromXpathObject = fromXpathElement.selectObject(createReference.getFromXpath());
                Object setXpathObject = setXpathElement.selectObject(createReference.getSetXpath());
                String value = BulkUploadUtils.getNodeValue(fromXpathObject, createReference.getFromXpath());
                //MRLog.debug("///////////////////updateValueGroup ///////////////////////////////////////////////////////");            
                //MRLog.debug("fromXpath " + createReference.getFromXpath() + " setXpath " + createReference.getSetXpath() +" value "+ value);
                //MRLog.debug("///////////////////updateValueGroup ///////////////////////////////////////////////////////");            
                BulkUploadUtils.setNodeValue(setXpathObject, value, createReference.getFromXpath());
            }
        }
    }
    
    public static void updateValue(Element element, UpdateXmlDefinations updateXmlDefinations, int row) {
        MRLog.debug("updateXmlDefinationId enter ");
        for (String updateXmlDefinationId : updateXmlDefinations.getUpdateXmlDefinationKeys()) {
            UpdateXmlDefination updateXmlDefination = updateXmlDefinations.getUpdateXmlDefination(updateXmlDefinationId);
            MRLog.debug("updateXmlDefinationId " + updateXmlDefinationId +" " + updateXmlDefination.getUpdateElementDefValues().size());
            for (Iterator<UpdateElementDef> iterator = updateXmlDefination.getUpdateElementDefValues().iterator(); iterator.hasNext();) {
                UpdateElementDef updateValue = iterator.next();
                for (Iterator<UpdateElementXpathDef> iterator2 = updateValue.getTemplateUpdateXpaths().iterator(); iterator2.hasNext();) {
                    UpdateElementXpathDef xpath = iterator2.next();
                    Object object = element.selectObject(xpath.getXpath());
                    String value = getNodeValue(object, xpath.getXpath());
                    value = getUpdateSettingsValue(element, value, updateValue, row);
                    //MRLog.debug("/////////////////////////////updateValue/////////////////////////////////////////////");
                    //MRLog.debug("xpath "+xpath.getXpath()+" value " + value);
                    //MRLog.debug("//////////////////////////////updateValue////////////////////////////////////////////");
                    setNodeValue(object, value, xpath.getXpath());
                }
            }
        }
        MRLog.debug("updateXmlDefinationId out ");
    }
    public static void updateValue2(Element element, UpdateXmlDefinations updateXmlDefinations, List<String> updateXmlDefinationList, int row) {
        MRLog.debug("updateXmlDefinationId enter ");
        for (String updateXmlDefinationId : updateXmlDefinationList) {
            UpdateXmlDefination updateXmlDefination = updateXmlDefinations.getUpdateXmlDefination(updateXmlDefinationId);
            MRLog.debug("updateXmlDefinationId " + updateXmlDefinationId +" " + updateXmlDefination.getUpdateElementDefValues().size());
            for (Iterator<UpdateElementDef> iterator = updateXmlDefination.getUpdateElementDefValues().iterator(); iterator.hasNext();) {
                UpdateElementDef updateValue = iterator.next();
                for (Iterator<UpdateElementXpathDef> iterator2 = updateValue.getTemplateUpdateXpaths().iterator(); iterator2.hasNext();) {
                    UpdateElementXpathDef xpath = iterator2.next();
                    Object object = element.selectObject(xpath.getXpath());
                    String value = getNodeValue(object, xpath.getXpath());
                    value = getUpdateSettingsValue(element, value, updateValue, row);
                    //MRLog.debug("/////////////////////////////updateValue/////////////////////////////////////////////");
                    //MRLog.debug("xpath "+xpath.getXpath()+" value " + value);
                    //MRLog.debug("//////////////////////////////updateValue////////////////////////////////////////////");
                    setNodeValue(object, value, xpath.getXpath());
                }
            }
        }
        MRLog.debug("updateXmlDefinationId out ");
    }
    public static String getUpdateSettingsValue(Element element, String value, UpdateElementDef updateValue, int counter) {
        //MRLog.debug("updateValue keepOriginalText " + updateValue.isKeepOriginalText() + " seed " + updateValue.getSeed() + " prefix" + updateValue.getPrefix() + " suffix " + updateValue.getSuffix() + " " + " type " + updateValue.getType() + " ");
        StringBuffer buf = new StringBuffer();
        if (updateValue.isKeepOriginalText()) {
            buf.append(value);
        }
        buf.append(updateValue.getPrefix());
        if (updateValue.getType().equalsIgnoreCase("randomNumber")) {
            wheel.setSeed(updateValue.getSeed());
            buf.append(wheel.nextInt());
        } else if (updateValue.getType().equalsIgnoreCase("incrementNumber")) {
            int currentCounter = counter + updateValue.getStartNumber();
            buf.append(currentCounter);
        } else if (updateValue.getType().equalsIgnoreCase("setvalue")) {
            buf.append(updateValue.getValue());
        } else if (updateValue.getType().equalsIgnoreCase("fromXpath")) {
            Object object = element.selectObject(updateValue.getFromXpath());
            String value2 = getNodeValue(object, updateValue.getFromXpath());
            buf.append(value2);
        }
        return buf.toString();
    }
    
    public static String getNodeValue(Object object, String xpath) {
        //MRLog.debug(" getNodeValue " + object.toString() +" xpath " + xpath );
        if (object instanceof Attribute) {
            Attribute attribute = (Attribute) object;
            return attribute.getText();
        } else {
            Element el = null;
            if (object instanceof List<?>) {
                List<?> list = (List<?>) object;
                if (list.size() == 0) return "";
                return getNodeValue(list.get(0), xpath);
            } else if (object instanceof Element) {
                el = (Element) object;
                return el.getText();
            } else if (object instanceof CDATA) {
                CDATA cdata = (CDATA) object;
                el = cdata.getParent();
                return el.getText();
            } else if (object instanceof Text) {
                Text cdata = (Text) object;
                el = cdata.getParent();
                return el.getText();
            }
        }
        return "";
    }
    
    public static void setNodeValue(Object object, String value, String xpath) {
        //MRLog.debug(" setNodeValue " + object.toString() +" xpath " + xpath +" " + value);
        if (object instanceof Attribute) {
            Attribute attribute = (Attribute) object;
            attribute.setValue(value);
        } else {
            Element el = null;
            if (object instanceof List<?>) {
                List<?> list = (List<?>) object;
                if (list.size() == 0) return;
                setNodeValue(list.get(0), value, xpath);
            } else if (object instanceof Element) {
                el = (Element) object;
                el.setText(value);
            } else if (object instanceof CDATA) {
                CDATA cdata = (CDATA) object;
                el = cdata.getParent();
                el.setText(value);
            } else if (object instanceof Text) {
                Text cdata = (Text) object;
                el = cdata.getParent();
                el.setText(value);
            }
        }
    }
}
