package com.ipvs.xmpp.client;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.params.DBParams;
import com.ipvs.xmpp.client.smack.packets.ResponseElement;

public class DBUpdateEvent {
    private Element updateEventElement = null;
    //private boolean isDBUpdateEvent = false;
    private String subscribedNID = "";
    private String subscribedJID = "";
    private String to = "";

    private String from = "";

    public DBUpdateEvent(Document messageDoc) {
        parse(messageDoc);
    }

    private void parse(Document messageDoc) {
        if (messageDoc != null && messageDoc.getRootElement() !=null) {
            Element event = messageDoc.getRootElement();
            try {
                updateEventElement = MRXMLUtils.getDescendentWithoutNamespace(event, "updateEvent");
            } catch (Exception e) {
                e.printStackTrace();
            }
            if(!event.getName().equals("message") ){
                try {
                    event = MRXMLUtils.getDescendentWithoutNamespace(messageDoc.getRootElement(), "message");
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            if (updateEventElement != null && event !=null) {
                String namespace = updateEventElement.getNamespace().getURI();
                if (namespace.equals(ResponseElement.IPVSDBSERVICE) ) {
                    //isDBUpdateEvent = true;
                    String[] idString = MRXMLUtils.getAttributeValue(event, "id", "").split("__");
                    if (idString.length > 0 && idString[0] != null) {
                        subscribedNID = idString[0];
                    }
                    if (idString.length > 1 && idString[1] != null) {
                        subscribedJID = idString[1];
                    }
                    to = MRXMLUtils.getAttributeValue(event, "to", "");
                    from = MRXMLUtils.getAttributeValue(event, "from", "");
                }
                //MRLog.getInstance(MRLog.CONSOLE).addInfo("DBUpdateEvent subscribedNID:" + subscribedNID);
            }
        }
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
            String type = MRXMLUtils.getAttributeValue(updateEventElement, "type", "");
            if (type.equals(DBParams.ADD) || type.equals(DBParams.REPLACE)) {
                return true;
            }
        }
        return false;
    }

    public boolean isAdd() {
        if (updateEventElement != null) {
            String type = MRXMLUtils.getAttributeValue(updateEventElement, "type", "");
            if (type.equals(DBParams.ADD)) {
                return true;
            }
        }
        return false;
    }

    public boolean isUpdate() {
        if (updateEventElement != null) {
            String type = MRXMLUtils.getAttributeValue(updateEventElement, "type", "");
            if (type.equals(DBParams.REPLACE)) {
                return true;
            }
        }
        return false;
    }

    public boolean isDelete() {
        if (updateEventElement != null) {
            String type = MRXMLUtils.getAttributeValue(updateEventElement, "type", "");
            if (type.equals(DBParams.DELETE)) {
                return true;
            }
        }
        return false;
    }

    public String getUpdateNID() {
        if (updateEventElement != null) {
            //MRLog.getInstance(MRLog.CONSOLE).addInfo("DBUpdateEvent NID:" + MRXMLUtils.getAttributeValue(updateEventElement, "nodeID", ""));
            return MRXMLUtils.getAttributeValue(updateEventElement, "nodeID", "");
        }
        return "";
    }

    public String getType() {
        if (updateEventElement != null) {
            return MRXMLUtils.getAttributeValue(updateEventElement, "type", "");
        }
        return "";
    }

    public String getDomain() {
        if (updateEventElement != null) {
            return MRXMLUtils.getAttributeValue(updateEventElement, "domain", "");
        }
        return "";
    }

    public String getObjectType() {
        if (updateEventElement != null) {
            return MRXMLUtils.getAttributeValue(updateEventElement, "objectType", "");
        }
        return "";
    }

    public int getRevision() {
        if (updateEventElement != null) {
            return Utils.getIntValue(MRXMLUtils.getAttributeValue(updateEventElement, "revision", ""), -1);
        }
        return -1;
    }

    public String getListName() {
        if (updateEventElement != null) {
            return MRXMLUtils.getAttributeValue(updateEventElement, "listName", "");
        }
        return "";
    }

    public String getChildXPath() {
        if (updateEventElement != null) {
            String childXPath = MRXMLUtils.getAttributeValue(updateEventElement, "childXPath", "");
            try {
                childXPath = Utils.decode(childXPath);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
            return childXPath;
        }
        return "";
    }

    public String getUpdateJID() {
        if (updateEventElement != null) {
            return MRXMLUtils.getAttributeValue(updateEventElement, "JID", "");
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