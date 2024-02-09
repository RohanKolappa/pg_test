package com.ipvsserv.nxd.mdc.util;

import java.io.UnsupportedEncodingException;

import org.dom4j.Element;

import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class MRPayloadHelper {
    static Element payloadEL = null;
    
    public static synchronized Element payload() {
        if (MRPayloadHelper.payloadEL == null) {
            StringBuffer buf = new StringBuffer();
            buf.append("<payload xmlns=\"http://jabber.org/protocol/pubsub\"></payload>");
            MRPayloadHelper.payloadEL = Dom4jUtils.stringToDocument(buf.toString()).getRootElement();
        }
        return MRPayloadHelper.payloadEL.createCopy();
    }
    
    /*public static Element addPayloadTextEncode(String xml) {
    	Element payload = payload();
    	if (xml != null && !xml.trim().equals(""))
    		payload.setText(URLEncoder.encode(xml));
    	return payload;
    }*/

    public static Element addPayloadText(String xml) throws UnsupportedEncodingException {
        Element payload = MRPayloadHelper.payload();
        //if (xml != null && !xml.trim().equals(""))
        //    payload.setText( Utils.encode(xml)    );
        payload.setText(xml);
        return payload;
    }
    //	
    // // For Bulk Add/Update/Delete
    // // ////////////////////// insert ////////////////////////
    // public static void copy(Element from, Element to) {
    // }
    //
    // public static void copyAttrs(Element from, Element to) {
    //
    // }
    //
    // public static void copyChilds(Element from, Element to) {
    //
    // }
    //
    // // ////////////////////// update ////////////////////////
    // public static void update(Element from, Element to) {
    //
    // }
    //
    // public static void updateAttrs(Element from, Element to) {
    //
    // }
    //
    // public static void updateChilds(Element from, Element to) {
    //
    // }
    //
    // // ////////////////////// delete ////////////////////////
    // public static void delete(Element from, Element to) {
    //
    // }
    //
    // public static void deleteAttrs(Element from, Element to) {
    //
    // }
    //
    // public static void deleteChilds(Element from, Element to) {
    //
    // }
    //
    // // ////////////////////// find ////////////////////////
    // public static String nodeIDXpath(String nodID) {
    // return nodID;
    // }
    //
    // public static Element findChild(Document doc, String nodID) {
    // return null;
    // }
    //
    // public static List findChilds(Document doc, String nodID) {
    // return null;
    // }
}
