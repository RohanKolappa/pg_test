package com.ipvs.notification;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Node;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.http.common.HttpAuthentication.AuthScheme;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.subscriptions.SubscriptionManager;
import com.ipvs.subscriptions.SubscriptionManager.SubscriptionCache;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public class ResourceNotifier extends BaseNotifier{

	private Document resourceTopicMapping = null;
	private static final String RESOURCE_TOPIC_MAPPING_XML_PATH = "/com/ipvs/notification/ResourceTopicMapping.xml";
	public static final String JID_NOTIFICATION_TYPE  = "JID";
	public static final String SUBSCRIBER_LIST_NOTIFICATION_TYPE  = "SubsciberList";
	public static final String JID_INFO_EL_NAME = "JIDInfo";
	private SubscriptionManager subscriptionManager = null;	
	
	public ResourceNotifier(MRDB db,MRLog log, XMPPF xf, String userJID, SubscriptionManager subscriptionManager,AuthTokenCache authTokenCache) {
		super(db,log, xf, userJID, authTokenCache);
		this.subscriptionManager = subscriptionManager;
	}

	@Override
	public void queueNotification(String notificationType, String notificationData,String notificationId,
			String message, String channel) {
		this.processNotification(notificationType,notificationData, notificationId, message, false, IPVSMessageExtension.IPVS_SESSION_XMLNS, channel);
	}

	@Override
	public void expediteNotification(String notificationType, String notificationData, String notificationId,
			String message, String channel) {
		this.processNotification(notificationType,notificationData, notificationId, message, true, IPVSMessageExtension.IPVS_SESSION_XMLNS, channel);
	}

	public String getViewObjectTypeFromNotification(NotificationRequest sn) {
		try {
			Element notificationRequestDataEl = MRXMLUtils.stringToElement(sn.getNotificationRequestData());
			String notificationType = MRXMLUtils.getAttributeValue(notificationRequestDataEl, SessionServiceConst.TYPE_ATTRIBUTE, "");
			if(notificationType.equals("PortUpdate") || notificationType.equals("PortConnectionUpdate")) {
                return SessionServiceConst.PORT;
            }
			if(notificationType.equals("FileUpdate") || notificationType.equals("FileConnectionUpdate")) {
                return SessionServiceConst.FILE;
            }
		} catch (Exception e) {
			e.printStackTrace();
		}
		return "";
	}
	
	protected  Map<String, Set<SubscriptionManager.SubscriptionCache>> getListOfUsersForAddNotification(String viewObjectType, Element updateEl) {
		try {			
			XMPPTransaction.startBlock("getListOfUsersForAddNotification");
			Map<String, Set<SubscriptionManager.SubscriptionCache>> userListVsSubId = new HashMap<String, Set<SubscriptionManager.SubscriptionCache>>();
			Hashtable<String, Hashtable<String,SubscriptionCache>> subscriberCache = this.subscriptionManager.getCachedSubscriptions(viewObjectType);
			if(subscriberCache == null) { return null; }
			Hashtable<String,SubscriptionCache> userPolicyTable;
			Set<SubscriptionCache> subscriptionCache=new HashSet<SubscriptionCache>();
			String policyXpathList = null;			
			for (String userJID : subscriberCache.keySet()) {
				
				userPolicyTable = subscriberCache.get(userJID);				
				for (String subscriptionIDKey  : userPolicyTable.keySet()) {
					SubscriptionCache cachedSubscription  = userPolicyTable.get(subscriptionIDKey);
					
					policyXpathList = cachedSubscription.getPolicyXpath();
					if(MRXMLUtils.getElementXpath(updateEl, viewObjectType+policyXpathList) == null){
						continue;
					}
					subscriptionCache.add(cachedSubscription);
										
				}
				userListVsSubId.put(userJID, subscriptionCache);
			}

			XMPPTransaction.endBlock("getListOfUsersForAddNotification");
			return userListVsSubId;
		}catch(Exception e) {
			e.printStackTrace();
		}
		return null;
	}
	
	protected  List<?> getListOfUsers(NotificationRequest sn) {
		try {			
			XMPPTransaction.startBlock("getListOfUsers");
			
			Element notificationRequestDataEl = MRXMLUtils.stringToElement(sn.notificationRequestData);
			String nid = MRXMLUtils.getAttributeValue(notificationRequestDataEl, SessionServiceConst.NID,"");
			Document subscribersDoc = this.subscriptionManager.getSubscribers(nid);
			XMPPTransaction.endBlock("getListOfUsers");
			return MRXMLUtils.getListXpath(subscribersDoc, "//"+JID_INFO_EL_NAME);
		} catch (Exception e) {
			e.printStackTrace();
		}

		return null;
	}

	protected Hashtable<String, String> getListOfNotificationMessageBasedOnTopic(NotificationRequest sn) {
		
		try {
			XMPPTransaction.startBlock("getListOfNotificationMessageBasedOnTopic");
			
			Hashtable<String, String> mapping = new Hashtable<String, String>();
			Element notificationRequestDataEl = MRXMLUtils.stringToElement(sn.getNotificationRequestData());
			
			String resourceUpdateType = MRXMLUtils.getAttributeValue(notificationRequestDataEl, SessionServiceConst.TYPE_ATTRIBUTE,"");

			if(this.resourceTopicMapping == null) {
				this.resourceTopicMapping = MRXMLUtils.loadXMLResource(RESOURCE_TOPIC_MAPPING_XML_PATH, log);				
			}
			Element resourceEl = MRXMLUtils.getElementXpath(this.resourceTopicMapping, "//ResourceUpdate[@type='"+resourceUpdateType+"']");
			Document messageDoc = MRXMLUtils.stringToDocument(sn.getNotificationMessage());
			
			if(resourceEl == null) {
				mapping.put(resourceUpdateType, MRXMLUtils.documentToStringOmitDeclaration(messageDoc));
				return mapping;
			} else {
				List<?> listOfTopic = MRXMLUtils.getListXpath(resourceEl, "./Topic");

				//add document to mapping for each topic.
				for(Object topic:listOfTopic) {
					Document messageDocCopy = messageDoc;
					Element topicEl = (Element) topic;
					String topicName  = MRXMLUtils.getAttributeValue(topicEl, SessionServiceConst.NAME,"");
					List<?> listOfRemoveItems = MRXMLUtils.getListXpath(topicEl, "./Remove");
					List<?> listOfUpdateURL = MRXMLUtils.getListXpath(topicEl, "./UpdateURL");
					
					if((listOfRemoveItems != null && !listOfRemoveItems.isEmpty()) || (listOfUpdateURL != null && !listOfUpdateURL.isEmpty())) {
						messageDocCopy = (Document) messageDoc.clone();
					}

					//remove elements
					if(listOfRemoveItems != null && !listOfRemoveItems.isEmpty()){
							//remove the nodes which are not needed.
						for(Object object :listOfRemoveItems ) {
							Element removeItem = (Element) object;
							String nodeName = MRXMLUtils.getAttributeValue(removeItem, SessionServiceConst.NODE_NAME,"");
							try {
								MRXMLUtils.nameSpaceRemoveNodeXpath(messageDocCopy.getRootElement(), nodeName);
							} catch (Exception e) {
								e.printStackTrace();
								//System.out.println("nodeName:="+nodeName);
								//System.out.println("removeItem:="+removeItem.asXML());
							}							
						}						
					}
					
					//update URL's 					
					if(listOfUpdateURL != null && !listOfUpdateURL.isEmpty()){
						List<Node> nodeList = null;
						//remove the nodes which are not needed.
						for(Object object :listOfUpdateURL ) {
							Element updateItem = (Element) object;
							String nodeName = MRXMLUtils.getAttributeValue(updateItem, SessionServiceConst.NODE_NAME,"");
							nodeList = new ArrayList<Node>();
							try {
								
								MRXMLUtils.nameSpaceGetNodeValue(messageDocCopy.getRootElement(), nodeName, nodeList); 
								
								if(nodeList.isEmpty()) {
									continue;
								}
								
								for(Node node:nodeList) {
									String urlValue =node.getText();
									
									if(urlValue == null || urlValue.isEmpty()) {
										continue;
									}
									//update url
									Matcher matcher =  Pattern.compile("(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})").matcher(urlValue);
									if(!matcher.find()) {
										continue;
									} 

									urlValue= matcher.replaceAll("_IPADDRESS_");
									String appendDestination = MRXMLUtils.getAttributeValue(updateItem, "appendDestination","false");
									String appendDeviceAgentSignature = MRXMLUtils.getAttributeValue(updateItem, "appendDeviceAgentSignature","false");
									if(appendDeviceAgentSignature.equals("true")){
										urlValue += "&_DEVICESIGNATURE_";
									} else {
										urlValue += "&_SIGNATURE_";
									}
									if(appendDestination.equals("true")){
										urlValue += "&_DSTPORTNID_";
									}

									node.setText(urlValue);									
								}
							} catch (Exception e) {
								e.printStackTrace();
								//System.out.println("nodeName:="+nodeName);
								//System.out.println("removeItem:="+removeItem.asXML());
							}							
						}						
					}					
					mapping.put(topicName, MRXMLUtils.documentToStringOmitDeclaration(messageDocCopy));
				}				
			}
			
			XMPPTransaction.endBlock("getListOfNotificationMessageBasedOnTopic");

			return mapping;


		} catch (Exception e) {			
			e.printStackTrace();
		}


		return null;
	}

	public Document generateAddNotification(NotificationRequest sn, Element updateEl) {
		try {
			Element notificationRequestDataEl = MRXMLUtils.stringToElement(sn.getNotificationRequestData());
			String notificationType = MRXMLUtils.getAttributeValue(notificationRequestDataEl, SessionServiceConst.TYPE_ATTRIBUTE, "");
			String viewObjectType = getViewObjectTypeFromNotification(sn);
			if(notificationType.isEmpty() || viewObjectType.isEmpty()) {
                return null;
            }
			
			String responseStr = SessionServiceUtils.getSessionMessageNotification(MRXMLUtils.stringToElement("<" + notificationType + "/>"));
			Document response = MRXMLUtils.stringToDocument(responseStr);
		
			Element responseEl = MRXMLUtils.stringToElement("<" + viewObjectType + "/>");
			String NID = MRXMLUtils.getAttributeValueXpath(updateEl, "//"+ viewObjectType, SessionServiceConst.NID, "");
			MRXMLUtils.setAttributeValue(responseEl, SessionServiceConst.NID, NID);
			String operation = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(updateEl, ".//"+viewObjectType), "operation","");
			
			if(operation.equals(SessionServiceConst.PORTTYPECHANGE)) {
				responseEl = MRXMLUtils.getElementXpath(updateEl, ".//"+viewObjectType);
			}
			MRXMLUtils.setAttributeValue(responseEl, "operation", operation);
			if (SessionServiceConst.PORT.equals(viewObjectType)) {
                String portType = MRXMLUtils.getAttributeValueXpath(updateEl, "//" + viewObjectType, SessionServiceConst.PORT_TYPE, "");
                if (!portType.equals("")) {
                    MRXMLUtils.setAttributeValue(responseEl, SessionServiceConst.PORT_TYPE, portType);
                }
            } else if (SessionServiceConst.FILE.equals(viewObjectType)) {
                String fileType = MRXMLUtils.getAttributeValueXpath(updateEl, "//" + viewObjectType, SessionServiceConst.FILE_TYPE, "");
                if (!fileType.equals("")) {
                    MRXMLUtils.setAttributeValue(responseEl, SessionServiceConst.FILE_TYPE, fileType);
                }
            }
			
			MRXMLUtils.setAttributeValue(responseEl, "subscriptionID", "_SUBSCRIPTIONID_");
			
			MRXMLUtils.addElementXpath(MRXMLUtils.getFirstChild(response.getRootElement()), ".", (Element)responseEl.clone());
		
			return response;
			//return MRXMLUtils.documentToStringOmitDeclaration(response);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}
	
	protected void sendDirtyNodeNotification(NotificationRequest sn, Element updateEl) {
		try {
			String viewObjectType = getViewObjectTypeFromNotification(sn);
			log.addInfo("ResourceNotifier: sendDirtyNodeNotification getListOfUsersForAddNotification: Start");
			long startTime = System.currentTimeMillis();
			Map<String, Set<SubscriptionManager.SubscriptionCache>> listOfUsers = this.getListOfUsersForAddNotification(viewObjectType, updateEl);
			log.addInfo("ResourceNotifier: sendDirtyNodeNotification getListOfUsersForAddNotification: End : Completed in: " + (System.currentTimeMillis() - startTime) + " millisec");
			if(listOfUsers != null) {	
				Document updateNotification = generateAddNotification(sn, updateEl);				
				if(updateNotification==null) { return; }
				String notification = MRXMLUtils.documentToStringOmitDeclaration(updateNotification);
				for (Map.Entry<String,Set<SubscriptionManager.SubscriptionCache>> entry : listOfUsers.entrySet()) {					
					//send dirty node subscription for each subscription
					for(SubscriptionCache subscriptionCache: entry.getValue()) {
						String localNotification = notification.replace("_SUBSCRIPTIONID_",subscriptionCache.getSubsriptionID());
						sendMessage(entry.getKey(), localNotification, sn.getNameSpace());
						log.addInfo("ResourceNotifier: sendDirtyNodeNotification to : " + entry.getKey());
					}
				}
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		return;
	}
	
	private Document getDeviceAgentAPIKey() {
		String apiKeyTable = "/var/status/APIKeyTable.xml";
		File file = new File(apiKeyTable);
		if (!file.exists()) {
			return null;
        }
		Document apiKeyDoc;
        try {
        	apiKeyDoc = MRXMLUtils.getDocument(file);
        } catch (Exception ex) {
        	return null;
        }
        return apiKeyDoc;
	}
	
	@Override
    protected void sendNotification(NotificationRequest sn) {
		//XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
		XMPPTransaction.startBlock("ResourceNotifier:sendNotification");
	try {	
			if(sn.getNotificationType().equals(SUBSCRIBER_LIST_NOTIFICATION_TYPE)) {
		
				Document notificationMsgDoc = MRXMLUtils.stringToDocument(sn.getNotificationMessage());
				MRXMLUtils.removeNameSpace(notificationMsgDoc.getRootElement());
				Element updateEl = MRXMLUtils.getFirstChild(notificationMsgDoc.getRootElement());
				String isDirtyNode = MRXMLUtils.getAttributeValueXpath(updateEl, "//@dirty","false");
		
				if(isDirtyNode.equals("true")) {
					log.addInfo("ResourceNotifier: sendNotification 'Add': Start");
					long startTime = System.currentTimeMillis();
					sendDirtyNodeNotification(sn, updateEl);
					log.addInfo("ResourceNotifier: sendNotification 'Add': End: Completed in: " + (System.currentTimeMillis() - startTime) + " millisec");
					return;
				}	
			
				//get the list of subscribers for given notification request
				List<?> listOfUsersWithSubTopics = this.getListOfUsers(sn);
				
				if(listOfUsersWithSubTopics == null || listOfUsersWithSubTopics.isEmpty()) {
					return;
				}
				
				AppServerAgent appServerAgent = AppServerAgentUtils.getAppServerAgent(xf.sf(userJID));
				Element ApiKeyTableEl = appServerAgent.getAPIKey();
				String ipaddress = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.MS_IPADDRESS, "");

				//generate list of message xml for each sub topic mentioned in the requestData
				Hashtable<String,String> listOfNotificationMessageBasedOnTopic = this.getListOfNotificationMessageBasedOnTopic(sn);
				Hashtable<String,SubscriptionCache> subscriptionCacheSet = null;
				
			    //get list of subscription from local cache
				Hashtable<String,Hashtable<String,SubscriptionCache>> subscriberCache = this.subscriptionManager.getCachedSubscriptions(getViewObjectTypeFromNotification(sn));
				
				//for each user given broadcast notification base on each topic
				for(Object jidInfoObject : listOfUsersWithSubTopics) {					
					Element jidInfoEl = (Element) jidInfoObject;
					String userJID = MRXMLUtils.getAttributeValue(jidInfoEl, SessionServiceResourceConst.JID_ATTRIBUTE,"");
					List<?> topicList = MRXMLUtils.getChildren(jidInfoEl);
					//if sub topic is empty drop the jid info
					if(topicList==null || topicList.isEmpty()) {
						continue;
					} 

					//get subscription info for user jid
					if(subscriberCache !=null) {
						subscriptionCacheSet = subscriberCache.get(userJID);
					} else {
						subscriptionCacheSet = null;
					}

					//get the unique topic names
					Set<String> uniqueValue = new HashSet<String>(MRXMLUtils.getValuesXpath(jidInfoEl, "./Topic"));
				   
				    
				    Element apiKeyELement = AppServerAgentUtils.getAppServerAgent(xf.sf(this.userJID)).getAPIKey();
				    String apiKey = MRXMLUtils.getAttributeValueXpath(apiKeyELement, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.KEY_ATTRIBUTE, "");
				    
					Document deviceAPIKey = getDeviceAgentAPIKey();
					Element deviceAPIKeyEl = null;
					if(deviceAPIKey != null) {
						deviceAPIKeyEl = deviceAPIKey.getRootElement();
					}
							
					String deviceApiKey = MRXMLUtils.getAttributeValueXpath(deviceAPIKeyEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.KEY_ATTRIBUTE, "");
					   		
					 
					//for each topic send a notification. 
					//User might receive multiple notifications if he subscribed for different topics.
					//for the same topic it will send one notification with multiple subscriptionId in it
					for(String topicName : uniqueValue) {
						//add subscription Id in the message Document
						String messageXML = listOfNotificationMessageBasedOnTopic.get(topicName);	
						if(messageXML == null) {
							continue;
						}
						List<String> subscriptionIDList = MRXMLUtils.getValuesXpath(jidInfoEl, "./Topic[.='"+topicName+"']/@subscriptionId");
						
						//add the subscription Id for given topic name (note:possible we might see multiple subscriptions for the same topic)						
						for(String subscriptionID : subscriptionIDList) {							
							String localMessageXML = messageXML;
							SubscriptionCache cachedSubscription = null;
							if(subscriptionCacheSet !=null) {
								cachedSubscription = subscriptionCacheSet.get(subscriptionID);
							}
							String cachedIPaddress= ipaddress;
							String cachedDestination = "", protocol= "http";
							String destTemplateTitle ="", authScheme=AuthScheme.EXPIRY.toString();
							String timestamp = "";
							if(cachedSubscription != null) {
								cachedIPaddress = cachedSubscription.getIpAddress();
								cachedDestination = cachedSubscription.getDestPortNID();
								protocol = cachedSubscription.getProtocol();
								destTemplateTitle = cachedSubscription.getDstTemplateTitle();
								authScheme = cachedSubscription.getAuthScheme();
								timestamp = (HttpAuthentication.INFINITE_EXPIRY.equals(cachedSubscription.getExpiryTime()))?HttpAuthentication.INFINITE_EXPIRY :String.valueOf(System.currentTimeMillis() + (Utils.getLongValue(cachedSubscription.getExpiryTime(),0) * 1000));
							}
							if(cachedIPaddress == null || cachedIPaddress.isEmpty()) {
								cachedIPaddress = ipaddress;
							}
							
							localMessageXML = localMessageXML.replace("_IPADDRESS_", cachedIPaddress);
							localMessageXML = localMessageXML.replace("http", protocol);
							
							//create signature based on auth scheme
							String signature = "";
							String deviceAgentSignature = "";
							if(authScheme != null)  {
								signature = generateSignature(apiKeyELement, userJID, timestamp, authScheme, apiKey, destTemplateTitle);
								deviceAgentSignature = generateSignature(deviceAPIKeyEl, userJID, timestamp, authScheme, deviceApiKey, destTemplateTitle);
							}
								
							localMessageXML = localMessageXML.replace("&amp;_SIGNATURE_", signature);
							localMessageXML = localMessageXML.replace("&amp;_DEVICESIGNATURE_", deviceAgentSignature);
							
							if(cachedDestination != null && !cachedDestination.isEmpty()) {
								localMessageXML = localMessageXML.replace("_DSTPORTNID_", "DstPort.portNID="+ cachedDestination);	
							} else {
								localMessageXML = localMessageXML.replace("_DSTPORTNID_", "DstPort.portNID=");
							}
							//send message for each subscription
							localMessageXML = localMessageXML.replaceAll(SessionServiceConst.SUBSCRIPTION_ID_TOKEN, subscriptionID);
							
							
							XMPPTransaction.startBlock("sendMessage");
							sendMessage(userJID, localMessageXML, sn.getNameSpace());
							XMPPTransaction.endBlock("sendMessage");
						}
						
						this.log.addInfo("ResourceUpdateNotifier:sendNotification notificationId:="+sn.getNotificationId() + " userJID:="+userJID +" topicName:="+topicName, MRLog.OK, MRLog.NOTIFY);

					}
				}
			} //send notification to given userjid
			else if(sn.getNotificationType().equals(JID_NOTIFICATION_TYPE)) {
				sendMessage(sn.getNotificationRequestData(), sn.getNotificationMessage(), sn.getNameSpace());
				//this.xf.xmpp(userJID).sendMessage(sn.getNotificationRequestData(), sn.getNotificationMessage(),sn.getNameSpace());
				this.log.addInfo("ResourceUpdateNotifier:sendNotification notificationId:="+sn.getNotificationId() + " userJID:="+sn.getNotificationRequestData(), MRLog.OK, MRLog.NOTIFY);
			} else {
				//invalid
			}
		}
		catch(Exception e) {
			e.printStackTrace();
			this.log.addInfo("BaseNotifier:sendNotification exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
		}
		
		XMPPTransaction.endBlock("ResourceNotifier:sendNotification");
		//XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());
		
	}
	
	private String generateSignature(Element apiKeyELement,String userJID,String timestamp,String authScheme,String apiKey, String destTemplateTitle) throws Exception {
		String signature = HttpAuthentication.generateSignature(apiKeyELement, timestamp, authScheme, MRRequest.createRequest(userJID), xf.xmpp(this.userJID), log, db, null, xf.sf(this.userJID));
		signature = HttpAuthentication.appendSystemAuthenticationQueryParameters("", apiKey, timestamp, authScheme, signature, userJID, destTemplateTitle);
		signature = signature.replaceAll("&", "&amp;");
		return signature;
	}

}
