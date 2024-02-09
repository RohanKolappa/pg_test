package org.jivesoftware.openfire.user;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.QName;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBUpdateEvent {
    private Element updateEventElement = null;
    private boolean isDBUpdateEvent = false;
    private String subscribedNID = "";
    private String subscribedJID = "";
    private String to = "";

    private String from = "";

    public DBUpdateEvent(Document messageDoc) {
        parse(messageDoc);
    }

    private void parse(Document messageDoc) {
        if (messageDoc != null) {
            try {
                QName qname = new QName("updateEvent", Namespace.get(IpvsUsersIQ.MEDIAROOM_NAMESPACE));
                updateEventElement = messageDoc.getRootElement().element(qname);
            } catch (Exception e) {
                e.printStackTrace();
            }
            if (updateEventElement != null) {

                String[] idString = getAttributeValue(messageDoc.getRootElement(), "id", "").split("__");
                if (idString[0] != null) {
                    subscribedNID = idString[0];
                }
                if (idString[1] != null) {
                    subscribedJID = idString[1];
                }
                to = getAttributeValue(messageDoc.getRootElement(), "to", "");
                from = getAttributeValue(messageDoc.getRootElement(), "from", "");
            }
        }
    }

    public String getAttributeValue(Element el, String name, String sdefault) {
        if (el != null) {
            String value = el.attributeValue(name);
            if (value != null) {
                return value;
            }
        }
        return sdefault;
    }

    public boolean isDBUpdateEvent() {
        return isDBUpdateEvent;
    }

    public String getSubscribedNID() {
        return subscribedNID;
    }

    public String getSubscribedJID() {
        return subscribedJID;
    }

    public String getTo() {
        return to;
    }

    public String getFrom() {
        return from;
    }

    public Element getUpdateEventElement() {
        return updateEventElement;
    }

    public boolean isAddorUpdate() {
        if (updateEventElement != null) {
            String type = getAttributeValue(updateEventElement, "type", "");
            if (type.equals("ADD") || type.equals("REPLACE")) {
                return true;
            }
        }
        return false;
    }

    public boolean isAdd() {
        if (updateEventElement != null) {
            String type = getAttributeValue(updateEventElement, "type", "");
            if (type.equals("ADD")) {
                return true;
            }
        }
        return false;
    }

    public boolean isUpdate() {
        if (updateEventElement != null) {
            String type = getAttributeValue(updateEventElement, "type", "");
            if (type.equals("REPLACE")) {
                return true;
            }
        }
        return false;
    }

    public boolean isDelete() {
        if (updateEventElement != null) {
            String type = getAttributeValue(updateEventElement, "type", "");
            if (type.equals("DELETE")) {
                return true;
            }
        }
        return false;
    }

    public String getUpdateNID() {
        if (updateEventElement != null) {
            //MRLog.getInstance(MRLog.CONSOLE).addInfo("DBUpdateEvent NID:" + MRXMLUtils.getAttributeValue(updateEventElement, "nodeID", ""));
            return getAttributeValue(updateEventElement, "nodeID", "");
        }
        return "";
    }

    public String getType() {
        if (updateEventElement != null) {
            return getAttributeValue(updateEventElement, "type", "");
        }
        return "";
    }

    public String getDomain() {
        if (updateEventElement != null) {
            return getAttributeValue(updateEventElement, "domain", "");
        }
        return "";
    }

    public String getObjectType() {
        if (updateEventElement != null) {
            return getAttributeValue(updateEventElement, "objectType", "");
        }
        return "";
    }

    public String getNotNull(String val, String sdefault) {
        if (val == null) {
            return sdefault;
        }

        return val;
    }

    public int getIntValue(String propName, int idefault) {
        String propValue = getNotNull(propName, idefault + "");

        try {
            return Integer.parseInt(propValue);
        } catch (Exception ex) {
        }

        return idefault;
    }

    public int getRevision() {
        if (updateEventElement != null) {
            return getIntValue(getAttributeValue(updateEventElement, "revision", ""), -1);
        }
        return -1;
    }

    public String getListName() {
        if (updateEventElement != null) {
            return getAttributeValue(updateEventElement, "listName", "");
        }
        return "";
    }

    public String getChildXPath() {
        if (updateEventElement != null) {
            return getAttributeValue(updateEventElement, "childXPath", "");
        }
        return "";
    }

    public String getUpdateJID() {
        if (updateEventElement != null) {
            return getAttributeValue(updateEventElement, "JID", "");
        }
        return "";
    }

}
/*
<message id="default.assetresourcelist__managementserver@localhost__HIVeq" to="managementserver@localhost" 
from="ipvsdbservice.localhost">

<updateEvent 
JID="managementserver@localhost/managementserver" 
childXPath="" 
listName="" 
nodeID="default.assetresourcelist.arl_3a640a87-e2f4-497b-8af5-f1ed4ee82906" 
objectType="" 
revision="32" 
type="update" 
xmlns="ipvs:mediaroom:ipvsdbservice">
<eventData>

<AssetResource NID="default.assetresourcelist.arl_3a640a87-e2f4-497b-8af5-f1ed4ee82906" Persistence="Temporary" Rev="1" Ver="1.5" parentNID="default.assetresourcelist">
 <Contact>
  <OwnerUserJID>managementserver@localhost/managementserver</OwnerUserJID>
  <Presence>available</Presence>
 </Contact>
 <State>
  <StateFlag>Ready</StateFlag>
  <StateMsg/>
 </State>
 <TemplateAssetResourceNID/>
 <Info>
  <Groups>
   <Tag Name="" Value=""/>
  </Groups>
  <Title>managementserver</Title>
  <Description/>
  <Type>MS-Device</Type>
  <AssetPrivateKey>
   <KeyScheme/>
   <KeyData/>
  </AssetPrivateKey>
  <AssetPrivateData/>
 </Info>
</AssetResource>

</eventData>
</updateEvent></message>
*/