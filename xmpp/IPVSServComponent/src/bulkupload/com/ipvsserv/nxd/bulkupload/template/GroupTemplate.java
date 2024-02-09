package com.ipvsserv.nxd.bulkupload.template;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.dom4j.Element;

import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class GroupTemplate {
    private String groupTemplateId = "";
    private Hashtable<String, GroupTemplateMember> members = new Hashtable<String, GroupTemplateMember>();
    private List<GroupCreateReference> groupCreateReferences = new ArrayList<GroupCreateReference>();
    
    public GroupTemplate(Element el) {
        groupTemplateId = Dom4jUtils.getAttributeValue(el, "groupTemplateId", "").trim();
        List<?> list = Dom4jUtils.listXpath(el, "groupTemplateMember");
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el2 = (Element) iterator.next();
            GroupTemplateMember groupTemplateMember = new GroupTemplateMember(el2);
            members.put(groupTemplateMember.gettemplateId(), groupTemplateMember);
        }
        list = Dom4jUtils.listXpath(el, "groupCreateReferences/groupCreateReference");
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el2 = (Element) iterator.next();
            groupCreateReferences.add(new GroupCreateReference(el2));
        }
    }
    
    public List<GroupCreateReference> getGroupCreateReferences() {
        return groupCreateReferences;
    }
    
    public String getgroupTemplateId() {
        return groupTemplateId;
    }
    
    public GroupTemplateMember getgroupTemplateMember(String groupTemplateMemberId) {
        return members.get(groupTemplateMemberId);
    }
    
    public Set<String> getgroupTemplateKeys() {
        return members.keySet();
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<groupTemplate groupTemplateId='" + groupTemplateId + "'>");
        //// templateMembers
        for (GroupTemplateMember templateMember : members.values()) {
            buf.append(templateMember.toString());
        }
        ///////////////groupTemplateUpdateSettings
        buf.append("\n<groupCreateReferences>");
        for (Iterator<GroupCreateReference> iterator = groupCreateReferences.iterator(); iterator.hasNext();) {
            GroupCreateReference createReference = iterator.next();
            buf.append(createReference.toString());
        }
        buf.append("\n</groupCreateReferences>");
        buf.append("\n</groupTemplate>");
        return buf.toString();
    }
    
}
