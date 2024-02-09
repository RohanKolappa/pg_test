package com.ipvsserv.nxd.mdc.notif;

import java.io.UnsupportedEncodingException;

import org.dom4j.Element;
import org.xmpp.packet.Message;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.mdc.sub.PubsubSubscription;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.xmpp.mdc.NotificationsRouter;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class NotificationUtils {
    private static boolean deliverPayloads = false;
    private static boolean enableLog = false;
    private static String servicever=null;
    //private static boolean useNewEvent = true;
    static {
        enableLog = IpvsServGlobals.getBooleanProperty("logs.enable.notificationsummary");
        servicever= IpvsServer.getInstance().getServerVersion() ;
    }

    public static void sendNotification(Message message, PubsubSubscription subscription, NotificationsRouter notificationsRouter, String from) {
        try {
            // TODO: check presence
            // hack for compatibility for our system to - getBareJID ( shoudl
            // have been full jid)
            message.setTo(subscription.getBareJID());
            message.setFrom(from);
            String id = subscription.getSubNid() + "__" + subscription.getBareJID() + "__" + CompUtils.randomString(5);
            message.setID(id);
            if (enableLog) {
                MRLog.debug(" ============ sendNotification " + id +" " + message.toXML());
            }
            notificationsRouter.route(message);
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
 
    
    // /////////////////////////new //////////////////
    public static Message getNotificationPacket(String action, String packetID, String nodeID, RequestMDCParams params, String xml, String revision) throws Exception {
        /*if(!useNewEvent){
            return NotificationUtils2.getNotificationPacket(action, packetID, nodeID, params, xml, revision); 
        }*/
        Element child=null;
        try{
            Message message = new Message();
            Element event = message.addChildElement("updateEvent", RequestIQConst.MEDIAROOM_NAME_SPACE);
            Dom4jUtils.addAttribute(event, "type", action);
            Dom4jUtils.addAttribute(event, "serviceVer", servicever);
            Dom4jUtils.addAttribute(event, "JID", params.getSubscribeJID());
            Dom4jUtils.addAttribute(event, "nodeID", nodeID);
            //Dom4jUtils.addAttribute(event, "childXPath", params.getChildXpath());
            Dom4jUtils.addAttribute(event, "objectType", params.getObjectType());
            Dom4jUtils.addAttribute(event, "listName", params.getListName());
            Dom4jUtils.addAttribute(event, "revision", revision);
            try {
	            Dom4jUtils.addAttribute(event, "childXPath", CompUtils.encode(params.getChildXpath()));
	        } catch (UnsupportedEncodingException ex) {
	            MRLog.error("error childXPath encode  " + params.getChildXpath(), ex);
	        }             
            //Element el = Dom4jUtils.addElement(event, "childXPath");
            //Dom4jUtils.setValueCdata(el, params.getChildXpath());
            
            if (NotificationUtils.deliverPayloads) {
                try{
                    child = Dom4jUtils.stringToElement("<eventData>"+xml+"</eventData>");
                }catch(Exception ex){
                    MRLog.error(xml,ex);
                }
                if(child !=null){
                    event.add(child);
                }
            }
            return message;
        }catch(Exception ex){
            throw ex;
        }
    }   
    
    //////////////// old
    
    /*public static DataForm getMetadataForm(String nodeID, boolean update, String revision) {
        DataForm form = new DataForm(DataForm.Type.result);
        FormField formField = form.addField();
        formField.setVariable("FORM_TYPE");
        formField.setType(FormField.Type.hidden);
        formField.addValue("http://jabber.org/protocol/pubsub#meta-data");
        // Add the form fields
        NotificationUtils.addFormFields(form, nodeID, true, update, revision);
        return form;
    }
    
    public static void addFormFields(DataForm form, String nodeID, boolean isEditing, boolean update, String revision) {
        FormField formField = form.addField();
        formField.setVariable("pubsub#title");
        if (isEditing) {
            formField.setType(FormField.Type.text_single);
            formField.setLabel(LocaleUtils.getLocalizedString("pubsub.form.conf.short_name"));
        }
        formField.addValue(nodeID);
        
        formField = form.addField();
        formField.setVariable("pubsub#description");
        if (isEditing) {
            formField.setType(FormField.Type.text_single);
            formField.setLabel(LocaleUtils.getLocalizedString("pubsub.form.conf.description"));
        }
        formField.addValue(nodeID);
        
        formField = form.addField();
        formField.setVariable("ipvs_addupdate");
        if (isEditing) {
            formField.setType(FormField.Type.text_single);
            formField.setLabel("IPVS AddUpdate");
        }
        if (update) {
            formField.addValue("udpate");
        } else {
            formField.addValue("add");
        }
        
        formField = form.addField();
        formField.setVariable("ipvs_revision");
        if (isEditing) {
            formField.setType(FormField.Type.text_single);
            formField.setLabel("ipvs_revision");
        }
        formField.addValue(revision);
    }
    */
   
}
