package com.ipvsserv.nxd.mdc.util;

import java.io.StringWriter;
import java.util.Iterator;

import org.dom4j.DocumentFactory;
import org.dom4j.Element;
import org.dom4j.QName;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.XMLWriter;
import org.xmpp.packet.PacketError;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsServPacketError extends PacketError {
    private static final String ERROR_NAMESPACE = "urn:ietf:params:xml:ns:xmpp-stanzas";
    private static DocumentFactory docFactory = DocumentFactory.getInstance();
    
    private Element element = null;
    
    public IpvsServPacketError(Condition condition) {
        super(condition);
    }
    
    public IpvsServPacketError(IPVSCondition condition) {
        super(IpvsServPacketError.docFactory.createElement("error"));
        element = IpvsServPacketError.docFactory.createElement("error");
        setCondition(condition);
        setIPVSType(condition.getDefaultType());
    }
    
    public IpvsServPacketError(IPVSCondition condition, IPVSType type) {
        super(IpvsServPacketError.docFactory.createElement("error"));
        element = IpvsServPacketError.docFactory.createElement("error");
        setCondition(condition);
        setIPVSType(type);
    }
    
    public IpvsServPacketError(IPVSCondition condition, IPVSType type, String text) {
        super(IpvsServPacketError.docFactory.createElement("error"));
        element = IpvsServPacketError.docFactory.createElement("error");
        setIPVSType(type);
        setCondition(condition);
        setText(text, null);
    }
    
    public IpvsServPacketError(IPVSCondition condition, IPVSType type, String text, String lang) {
        super(IpvsServPacketError.docFactory.createElement("error"));
        element = IpvsServPacketError.docFactory.createElement("error");
        setIPVSType(type);
        setCondition(condition);
        setText(text, lang);
    }
    
    public IpvsServPacketError(Element element) {
        super(element);
        this.element = element;
    }
    
    public IPVSType getIPVSType() {
        String type = element.attributeValue("type");
        if (type != null) {
            return IPVSType.fromXMPP(type);
        } else {
            return null;
        }
    }
    
    public void setIPVSType(IPVSType type) {
        element.addAttribute("type", type == null ? null : type.toXMPP());
    }
    
    public IPVSCondition getIPVSCondition() {
        for (Iterator<?> i = element.elementIterator(); i.hasNext();) {
            Element el = (Element) i.next();
            if (el.getNamespaceURI().equals(IpvsServPacketError.ERROR_NAMESPACE) && !el.getName().equals("text")) {
                return IPVSCondition.fromXMPP(el.getName());
            }
        }
        // Looking for XMPP condition failed. See if a legacy error code exists,
        // which can be mapped into an XMPP error condition.
        String code = element.attributeValue("code");
        if (code != null) {
            try {
                return IPVSCondition.fromLegacyCode(Integer.parseInt(code));
            } catch (Exception ex) {
                // Ignore -- unable to map legacy code into a valid condition
                // so return null.
            }
        }
        return null;
    }
    
    public void setCondition(IPVSCondition condition) {
        if (condition == null) {
            throw new NullPointerException("Condition cannot be null");
        }
        // Set the error code for legacy support.
        element.addAttribute("code", Integer.toString(condition.getLegacyCode()));
        
        Element conditionElement = null;
        for (Iterator<?> i = element.elementIterator(); i.hasNext();) {
            Element el = (Element) i.next();
            if (el.getNamespaceURI().equals(IpvsServPacketError.ERROR_NAMESPACE) && !el.getName().equals("text")) {
                conditionElement = el;
            }
        }
        if (conditionElement != null) {
            element.remove(conditionElement);
        }
        
        conditionElement = IpvsServPacketError.docFactory.createElement(condition.toXMPP(), IpvsServPacketError.ERROR_NAMESPACE);
        element.add(conditionElement);
    }
    
    @Override
    public String getText() {
        return element.elementText("text");
    }
    
    @Override
    public void setText(String text) {
        setText(text, null);
    }
    
    @Override
    public void setText(String text, String lang) {
        Element textElement = element.element("text");
        // If text is null, clear the text.
        if (text == null) {
            if (textElement != null) {
                element.remove(textElement);
            }
            return;
        }
        
        if (textElement == null) {
            textElement = IpvsServPacketError.docFactory.createElement("text", IpvsServPacketError.ERROR_NAMESPACE);
            if (lang != null) {
                textElement.addAttribute(QName.get("lang", "xml", "http://www.w3.org/XML/1998/namespace"), lang);
            }
            element.add(textElement);
        }
        textElement.setText(text);
    }
    
    @Override
    public String getTextLang() {
        Element textElement = element.element("text");
        if (textElement != null) {
            return textElement.attributeValue(QName.get("lang", "xml", "http://www.w3.org/XML/1998/namespace"));
        }
        return null;
    }
    
    @Override
    public Element getElement() {
        return element;
    }
    
    @Override
    public String toXML() {
        return element.asXML();
    }
    
    @Override
    public String toString() {
        StringWriter out = new StringWriter();
        XMLWriter writer = new XMLWriter(out, OutputFormat.createPrettyPrint());
        try {
            writer.write(element);
        } catch (Exception ex) {
            // Ignore.
        }
        return out.toString();
    }
    
    public enum IPVSCondition {
        ipvs_update_node_bad_node_id("ipvs-update-node-bad-node-id", IPVSType.modify, 410),

        ipvs_add_node_bad_parent_node("ipvs-add-node-bad-parent-node", IPVSType.modify, 411),

        ipvs_add_node_already_exists("ipvs-add-node-already-exists", IPVSType.modify, 412),

        ipvs_update_node_does_not_exists("ipvs-update-node-does-not-exists", IPVSType.modify, 413),

        new_ipvs_pubsub_not_enabled("new-ipvs-pubsub-not-enabled", IPVSType.modify, 415),

        unknown_ipvs_domain("unknown-ipvs-domain", IPVSType.modify, 416),

        ipvs_service_domain_required("ipvs-service-domain-required", IPVSType.modify, 417),

        ipvs_add_or_update_failed("ipvs-add-or-update-failed", IPVSType.modify, 418),

        ipvs_invalid_node_id("ipvs-invalid-node-id", IPVSType.modify, 419),

        ipvs_db_connection_failed("ipvs-db-connection-failed", IPVSType.modify, 420),

        ipvs_db_connection_timed_out("ipvs-db-connection-timed-out", IPVSType.modify, 420),

        ipvs_db_execution_failed("ipvs-db-execution-failed", IPVSType.modify, 420),

        ipvs_invalid_xml("ipvs-invalid-xml", IPVSType.modify, 421),

        ipvs_failed_to_decode_xml("ipvs-failed-to-decode-xml", IPVSType.modify, 422),

        
        child_nodeid_not_supported("child-nodeid-not-supported", IPVSType.modify, 423),
        ipvs_invalid_read_query("ipvs-invalid-read-query", IPVSType.modify, 424),


        ipvs_error_while_add_domain("ipvs-error-while-add-domain", IPVSType.modify, 426),
        ipvs_error_while_delete_domain("ipvs-error-while-delete-domain", IPVSType.modify, 427),
        ipvs_error_while_processing_the_request("ipvs-error-while-processing-the-request", IPVSType.modify, 428),
        ipvs_error_while_add("ipvs-error-while-add", IPVSType.modify, 429),
        ipvs_error_while_replace("ipvs-error-while-replace", IPVSType.modify, 430),
        ipvs_error_while_delete("ipvs-error-while-delete", IPVSType.modify, 431),
        ipvs_error_while_read("ipvs-error-while-read", IPVSType.modify, 432),
        ipvs_error_while_read_query("ipvs-error-while-read-query", IPVSType.modify, 433),
        ipvs_error_while_get_children("ipvs-error-while-get-children", IPVSType.modify, 434),
        ipvs_error_while_execute_query("ipvs-error-while-execute-query", IPVSType.modify, 435),
        ipvs_error_while_subscribe("ipvs-error-while-subscribe", IPVSType.modify, 436),
        ipvs_error_while_unsubscribe("ipvs-error-while-unsubscribe", IPVSType.modify, 437),
        ipvs_error_db_migration_failed("ipvs-error-db-migration-failed", IPVSType.modify, 438),
        ipvs_error_db_import_export_already_running("ipvs-error-db-import-export-already-running", IPVSType.modify, 439),
        ipvs_error_db_import_export_failed("iipvs-error-db-import-export-failed", IPVSType.modify, 440),
        ipvs_max_request_limit_reached("ipvs-max-request-limit-reached", IPVSType.modify, 441),
        
        ipvs_error_while_get_subscriptions("ipvs-error-while-get-subscriptions", IPVSType.modify, 442),
        ipvs_error_while_update_subcriptions("ipvs-error-while-update-subscriptions", IPVSType.modify, 443),
        /**
         * The sender has sent XML that is malformed or that cannot be processed
         * (e.g., an IQ stanza that includes an unrecognized value of the 'type'
         * attribute); the associated error type SHOULD be "modify".
         */
        bad_request("bad-request", IPVSType.modify, 400),

        /**
         * Access cannot be granted because an existing resource or session
         * exists with the same name or address; the associated error type
         * SHOULD be "cancel".
         */
        conflict("conflict", IPVSType.cancel, 409),

        /**
         * The feature requested is not implemented by the recipient or
         * server and therefore cannot be processed; the associated error
         * type SHOULD be "cancel".
         */
        feature_not_implemented("ipvs_feature-not-implemented", IPVSType.cancel, 501),

        /**
         * The requesting entity does not possess the required permissions to
         * perform the action; the associated error type SHOULD be "auth".
         */
        forbidden("forbidden", IPVSType.auth, 403),

        /**
         * The recipient or server can no longer be contacted at this address
         * (the error stanza MAY contain a new address in the XML character
         * data of the <gone/> element); the associated error type SHOULD be
         * "modify".
         */
        gone("gone", IPVSType.modify, 302),

        /**
         * The server could not process the stanza because of a misconfiguration
         * or an otherwise-undefined internal server error; the associated error
         * type SHOULD be "wait".
         */
        internal_server_error("internal-server-error", IPVSType.wait, 500),

        /**
         * The addressed JID or item requested cannot be found; the associated
         * error type SHOULD be "cancel".
         */
        item_not_found("item-not-found", IPVSType.cancel, 404),

        /**
         * The sending entity has provided or communicated an XMPP address
         * (e.g., a value of the 'to' attribute) or aspect thereof (e.g.,
         * a resource identifier) that does not adhere to the syntax defined
         * in Addressing Scheme (Section 3); the associated error type SHOULD
         * be "modify".
         */
        jid_malformed("jid-malformed", IPVSType.modify, 400),

        /**
         * The recipient or server understands the request but is refusing
         * to process it because it does not meet criteria defined by the
         * recipient or server (e.g., a local policy regarding acceptable
         * words in messages); the associated error type SHOULD be "modify".
         */
        not_acceptable("not-acceptable", IPVSType.modify, 406),

        /**
         * The recipient or server does not allow any entity to perform
         * the action; the associated error type SHOULD be "cancel".
         */
        not_allowed("not-allowed", IPVSType.cancel, 405),

        /**
         * The sender must provide proper credentials before being allowed
         * to perform the action, or has provided improper credentials;
         * the associated error type SHOULD be "auth".
         */
        not_authorized("not-authorized", IPVSType.auth, 401),

        /**
         * The requesting entity is not authorized to access the requested
         * service because payment is required; the associated error type
         * SHOULD be "auth".
         */
        payment_required("payment-required", IPVSType.auth, 402),

        /**
         * The intended recipient is temporarily unavailable; the associated
         * error type SHOULD be "wait" (note: an application MUST NOT return
         * this error if doing so would provide information about the intended
         * recipient's network availability to an entity that is not authorized
         * to know such information).
         */
        recipient_unavailable("recipient-unavailable", IPVSType.wait, 404),

        /**
         * The recipient or server is redirecting requests for this
         * information to another entity, usually temporarily (the error
         * stanza SHOULD contain the alternate address, which MUST be a
         * valid JID, in the XML character data of the &lt;redirect/&gt; element);
         * the associated error type SHOULD be "modify".
         */
        redirect("redirect", IPVSType.modify, 302),

        /**
         * The requesting entity is not authorized to access the requested
         * service because registration is required; the associated error
         * type SHOULD be "auth".
         */
        registration_required("registration-required", IPVSType.auth, 407),

        /**
         * A remote server or service specified as part or all of the JID
         * of the intended recipient does not exist; the associated error
         * type SHOULD be "cancel".
         */
        remote_server_not_found("remote-server-not-found", IPVSType.cancel, 404),

        /**
         * A remote server or service specified as part or all of the JID of
         * the intended recipient (or required to fulfill a request) could not
         * be contacted within a reasonable amount of time; the associated
         * error type SHOULD be "wait".
         */
        remote_server_timeout("remote-server-timeout", IPVSType.wait, 504),

        /**
         * The server or recipient lacks the system resources necessary to
         * service the request; the associated error type SHOULD be "wait".
         */
        resource_constraint("resource-constraint", IPVSType.wait, 500),

        /**
         * The server or recipient does not currently provide the requested
         * service; the associated error type SHOULD be "cancel".
         */
        service_unavailable("service-unavailable", IPVSType.cancel, 503),

        /**
         * The requesting entity is not authorized to access the requested
         * service because a subscription is required; the associated error
         * type SHOULD be "auth".
         */
        subscription_required("subscription-required", IPVSType.auth, 407),

        /**
         * The error condition is not one of those defined by the other
         * conditions in this list; any error type may be associated with
         * this condition, and it SHOULD be used only in conjunction with
         * an application-specific condition.<p>
         *
         * Implementation note: the default type for this condition is
         * {@link IPVSType#wait}, which is not specified in the XMPP protocol.
         */
        undefined_condition("undefined-condition", IPVSType.wait, 500),

        /**
         * The recipient or server understood the request but was not
         * expecting it at this time (e.g., the request was out of order);
         * the associated error type SHOULD be "wait".
         */
        unexpected_request("unexpected-request", IPVSType.wait, 400);
        
        public static IPVSCondition fromXMPP(String condition) {
            //todo:
            if (condition == null) {
                throw new NullPointerException();
            }
            condition = condition.toLowerCase();
            if (bad_request.toXMPP().equals(condition)) {
                return bad_request;
            } else if (conflict.toXMPP().equals(condition)) {
                return conflict;
            } else if (feature_not_implemented.toXMPP().equals(condition)) {
                return feature_not_implemented;
            } else if (forbidden.toXMPP().equals(condition)) {
                return forbidden;
            } else if (gone.toXMPP().equals(condition)) {
                return gone;
            } else if (internal_server_error.toXMPP().equals(condition)) {
                return internal_server_error;
            } else if (item_not_found.toXMPP().equals(condition)) {
                return item_not_found;
            } else if (jid_malformed.toXMPP().equals(condition)) {
                return jid_malformed;
            } else if (not_acceptable.toXMPP().equals(condition)) {
                return not_acceptable;
            } else if (not_allowed.toXMPP().equals(condition)) {
                return not_allowed;
            } else if (not_authorized.toXMPP().equals(condition)) {
                return not_authorized;
            } else if (payment_required.toXMPP().equals(condition)) {
                return payment_required;
            } else if (recipient_unavailable.toXMPP().equals(condition)) {
                return recipient_unavailable;
            } else if (redirect.toXMPP().equals(condition)) {
                return redirect;
            } else if (registration_required.toXMPP().equals(condition)) {
                return registration_required;
            } else if (remote_server_not_found.toXMPP().equals(condition)) {
                return remote_server_not_found;
            } else if (remote_server_timeout.toXMPP().equals(condition)) {
                return remote_server_timeout;
            } else if (resource_constraint.toXMPP().equals(condition)) {
                return resource_constraint;
            } else if (service_unavailable.toXMPP().equals(condition)) {
                return service_unavailable;
            } else if (subscription_required.toXMPP().equals(condition)) {
                return subscription_required;
            } else if (undefined_condition.toXMPP().equals(condition)) {
                return undefined_condition;
            } else if (unexpected_request.toXMPP().equals(condition)) {
                return unexpected_request;
            } else {
                throw new IllegalArgumentException("Condition invalid:" + condition);
            }
        }
        
        public static IPVSCondition fromLegacyCode(int code) {
            if (bad_request.getLegacyCode() == code) {
                return bad_request;
            } else if (conflict.getLegacyCode() == code) {
                return conflict;
            } else if (feature_not_implemented.getLegacyCode() == code) {
                return feature_not_implemented;
            } else if (forbidden.getLegacyCode() == code) {
                return forbidden;
            } else if (gone.getLegacyCode() == code) {
                return gone;
            } else if (internal_server_error.getLegacyCode() == code) {
                return internal_server_error;
            } else if (item_not_found.getLegacyCode() == code) {
                return item_not_found;
            } else if (jid_malformed.getLegacyCode() == code) {
                return jid_malformed;
            } else if (not_acceptable.getLegacyCode() == code) {
                return not_acceptable;
            } else if (not_allowed.getLegacyCode() == code) {
                return not_allowed;
            } else if (not_authorized.getLegacyCode() == code) {
                return not_authorized;
            } else if (payment_required.getLegacyCode() == code) {
                return payment_required;
            } else if (recipient_unavailable.getLegacyCode() == code) {
                return recipient_unavailable;
            } else if (redirect.getLegacyCode() == code) {
                return redirect;
            } else if (registration_required.getLegacyCode() == code) {
                return registration_required;
            } else if (remote_server_not_found.getLegacyCode() == code) {
                return remote_server_not_found;
            } else if (remote_server_timeout.getLegacyCode() == code) {
                return remote_server_timeout;
            } else if (resource_constraint.getLegacyCode() == code) {
                return resource_constraint;
            } else if (service_unavailable.getLegacyCode() == code) {
                return service_unavailable;
            } else if (subscription_required.getLegacyCode() == code) {
                return subscription_required;
            } else if (undefined_condition.getLegacyCode() == code) {
                return undefined_condition;
            } else if (unexpected_request.getLegacyCode() == code) {
                return unexpected_request;
            } else {
                throw new IllegalArgumentException("Code invalid:" + code);
            }
        }
        
        private String value;
        private int code;
        private IPVSType defaultType;
        
        private IPVSCondition(String value, IPVSType defaultType, int code) {
            this.value = value;
            this.defaultType = defaultType;
            this.code = code;
        }
        
        public IPVSType getDefaultType() {
            return defaultType;
        }
        
        public int getLegacyCode() {
            return code;
        }
        
        public String toXMPP() {
            return value;
        }
    }
    
    public enum IPVSType {
        
        /**
         * Do not retry (the error is unrecoverable).
         */
        cancel("cancel"),

        /**
         * Proceed (the condition was only a warning). This represents
         * the "continue" error code in XMPP; because "continue" is a
         * reserved keyword in Java the enum name has been changed.
         */
        continue_processing("continue"),

        /**
         * Retry after changing the data sent.
         */
        modify("modify"),

        /**
         * Retry after providing credentials.
         */
        auth("auth"),

        /**
         * Retry after waiting (the error is temporary).
         */
        wait("wait");
        
        public static IPVSType fromXMPP(String type) {
            if (type == null) {
                throw new NullPointerException();
            }
            type = type.toLowerCase();
            if (cancel.toXMPP().equals(type)) {
                return cancel;
            } else if (continue_processing.toXMPP().equals(type)) {
                return continue_processing;
            } else if (modify.toXMPP().equals(type)) {
                return modify;
            } else if (auth.toXMPP().equals(type)) {
                return auth;
            } else if (wait.toXMPP().equals(type)) {
                return wait;
            } else {
                throw new IllegalArgumentException("Type invalid:" + type);
            }
        }
        
        private String value;
        
        private IPVSType(String value) {
            this.value = value;
        }
        
        public String toXMPP() {
            return value;
        }
    }
}
