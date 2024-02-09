Notification Handling Design
-----------------------------

Note: check the file NotificationHandler.xml in model/mediaroomXML/com/ipvs/xsd/xml/.

SSNotification
---------------
1. Streaming Server send the notifications to SSNotificationListener
2. SSNotification will generate SSEvent as per rules in the NotificationHandler.xml
   Mapping rules      StreamingServerNotification to NotificationHandler
                  -----------------------------------------------------------
                         SSNotification     :   NotificationType
                         EventType          :   EventSourceList/EventSource
                         Type               :   EventTypeList/Type
                                     
3. Device App will send Message to corresponding Agents listed in the notificationHandler
4. Agents receive the message will do the corresponding action.

System Notification
--------------------
1. System software send notifications to ConfigListener
2. ConfigLitener will generate SystemEvent as per rules in NotificationHanlder.xml

 Mapping rules      StreamingServerNotification to NotificationHandler
                  -----------------------------------------------------------
                         SystemNotification :   NotificationType
                         EventSource        :   EventSourceList/EventSource
                         EventType          :   EventTypeList/Type

Step 3 and 4 as above
