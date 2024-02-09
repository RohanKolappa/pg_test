package com.ipvs.proxy.impl;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Random;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.proxy.app.ProxyServer;


public class ProxyClientUtils {
    public static final Random wheel = new Random();
    private static int heartbeatFrequency = 1000;
    private static Document heartbeatTemplate = null;

    private static Element setValues(Element parentEL, StringTokenizer st, MRLog log) {
        String srcUser = null;
        String srcUserJID = null;
        String srcUserNickName = null;
        String destUser = null;
        String destUserJID = null;
        String destUserNickName = null;
        String remote = null;
        String connection = null;
        String state = null;
        String startTime = null;

        if (st.hasMoreTokens()) {
            srcUser = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            srcUserJID = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            srcUserNickName = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            destUser = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            destUserJID = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            destUserNickName = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            connection = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            remote = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            state = st.nextToken();
        }

        if (st.hasMoreTokens()) {
            startTime = st.nextToken();
        }

        try {
            MRXMLUtils.setValueXpath(parentEL, "StreamData/sourceInfo/agentName", srcUser);
            MRXMLUtils.setValueXpath(parentEL, "StreamData/sourceInfo/JID", srcUserJID);
            MRXMLUtils.setValueXpath(parentEL, "StreamData/sourceInfo/nickName", srcUserNickName);
            MRXMLUtils.setValueXpath(parentEL, "StreamData/destInfo/agentName", destUser);
            MRXMLUtils.setValueXpath(parentEL, "StreamData/destInfo/JID", destUserJID);
            MRXMLUtils.setValueXpath(parentEL, "StreamData/destInfo/nickName", destUserNickName);
            MRXMLUtils.setValueXpath(parentEL, "StreamData/profileInfo/connect", connection);
            MRXMLUtils.setValueXpath(parentEL, "StreamData/profileInfo/remote", remote);
            MRXMLUtils.setValueXpath(parentEL, "StreamStatus/state", state);
            MRXMLUtils.setValueXpath(parentEL, "StreamStatus/startTime", startTime);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return parentEL;
    }

    private static String generateStreamListResponse(StringTokenizer message, MRLog log) {
        String xmlFilePath = ProxyConstants.XMLFILEPATH + "getstreamlistresponse.xml";
        Document msgdoc = null;
        Element rootel = null;
        Element streamEL = null;

        try {
            msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());

            return null;
        }

        try {
    
            rootel = MRXMLUtils.getElementXpath(msgdoc, "//StreamInfoList");
            streamEL = MRXMLUtils.getElementXpath(msgdoc, "//StreamInfo");

            if (streamEL != null) {
                streamEL.detach();
                MRXMLUtils.removeContent(rootel);
            }
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());

            return null;
        }

        while (message.hasMoreTokens()) {
            StringTokenizer str = new StringTokenizer(message.nextToken("&"), ProxyConstants.SEPERATOR);

            if ((str != null) && (str.countTokens() != 0)) {
                setValues(streamEL, str, log);

                try {
                    MRXMLUtils.addElement(rootel, (Element) streamEL.detach().clone());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

      
        String result = null;

        if (rootel != null) {
            try {
                MRXMLUtils.setValueXpath(msgdoc, "//requestID", "");
                result = MRXMLUtils.documentToStringOmitDeclaration(msgdoc) + "\n";
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return result;
    }

    private static String generateAuthNotifyResponse(StringTokenizer message, MRLog log) {
        String xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamnotification.xml";
        Document msgdoc = null;

        Element streamEL = null;
        String requestID = null;

        try {
            if (message.hasMoreTokens()) {
                requestID = message.nextToken();
            }

            msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
            streamEL = MRXMLUtils.getElementXpath(msgdoc, "//Data");
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());

            return null;
        }

        setValues(streamEL, message, log);

        String result = null;

        if (streamEL != null) {
            try {
                MRXMLUtils.setValueXpath(msgdoc, "//requestID", requestID);
                result = MRXMLUtils.documentToStringOmitDeclaration(msgdoc) + "\n";
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return result;
    }

    public static String generate(StringTokenizer st, MRLog log) {
        try {
            String result = null;
            String requestFile = null;

            if (st.hasMoreTokens()) {
                requestFile = st.nextToken().trim();
            }

            if (requestFile == null) {
                return null;
            }

            if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHNOTIFY) == 0) {
                return generateAuthNotifyResponse(st, log);
            } else if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHREQUEST) == 0) {
                return ProxyServer.generateAuthRequest(st, log);
            } else if (requestFile.compareToIgnoreCase(ProxyConstants.LISTRESPONSE) == 0) {
                return generateStreamListResponse(st, log);
            } else if ((requestFile.compareToIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.UPDATERECORDINGREQUEST) == 0)) {
                return ProxyServer.generateStartUpdateRecordingRequest(st, requestFile, log);
            } else if ((requestFile.compareToIgnoreCase(ProxyConstants.STOPRECORDINGREQUEST) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.PAUSERECORDINGREQUEST) == 0) ||
                    (requestFile.compareToIgnoreCase(ProxyConstants.RESUMERECORDINGREQUEST) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.RECORDINGSTATUSREQUEST) == 0)) {
                return ProxyServer.generateStopPauseResumeRecordingRequest(st, requestFile, log);
            } else if ((requestFile.compareToIgnoreCase(ProxyConstants.CREATEBOOKMARKREQUEST) == 0)) {
                return ProxyServer.generateBookMarkRequest(st, requestFile, log);
            } else if ((requestFile.compareToIgnoreCase(ProxyConstants.RECORDINGNOTIFICATION) == 0)) {
                return generateRecordingNotification(st, requestFile, log);
            } else if (requestFile.compareToIgnoreCase(ProxyConstants.MEDIATIMESTAMPREQUEST) == 0) {
                return ProxyServer.generateMediaTimestampRequest(st, requestFile, log);
            } else if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHORIZATIONUPDATEREQUEST) == 0) {
                return ProxyServer.generateAuthorizationUpdateRequest(st, requestFile, log);
            } else if ((requestFile.compareToIgnoreCase(ProxyConstants.STARTRECORDINGRESPONSE) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.UPDATERECORDINGRESPONSE) == 0) ||
                    (requestFile.compareToIgnoreCase(ProxyConstants.STOPRECORDINGRESPONSE) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.PAUSERECORDINGRESPONSE) == 0) ||
                    (requestFile.compareToIgnoreCase(ProxyConstants.RESUMERECORDINGRESPONSE) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.RECORDINGSTATUSRESPONSE) == 0)) {
                return generateRecordingResponse(st, requestFile, log);
            } else if ((requestFile.compareToIgnoreCase(ProxyConstants.CREATEBOOKMARKRESPONSE) == 0)) {
                return generateBookMarkResponse(st, requestFile, log);
            } else if (requestFile.compareToIgnoreCase(ProxyConstants.MEDIATIMESTAMPRESPONSE) == 0) {
                return generateMediaTimestampResponse(st, requestFile, log);
            } else if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHORIZATIONUPDATERESPONSE) == 0) {
                return generateAuthorizationUpdateResponse(st, requestFile, log);
            }

            String user = null;
            String nickname = null;
            String remote = null;
            String requestID = null;
            String action = null;

            if (st.hasMoreTokens()) {
                user = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                nickname = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                remote = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                requestID = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                action = st.nextToken();
            }

            if (action == null) {
                action = "Good";
            }

            if (nickname == null) {
                nickname = user;
            }

            if (remote == null) {
                remote = "No";
            }

            if (requestFile != null) {
                String xmlFilePath = null;
                Document msgdoc = null;
                Element el = null;

                if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHREQUEST) == 0) {
                    xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamrequest.xml";
                } else if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHNOTIFY) == 0) {
                    xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamnotification.xml";
                } else if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHRESPONSE) == 0) {
                    xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamresponse.xml";
                } else if (requestFile.compareToIgnoreCase(ProxyConstants.LISTRESPONSE) == 0) {
                    xmlFilePath = ProxyConstants.XMLFILEPATH + "getstreamlistresponse.xml";
                } else if (requestFile.compareToIgnoreCase(ProxyConstants.LISTREQUEST) == 0) {
                    xmlFilePath = ProxyConstants.XMLFILEPATH + "getstreamlistrequest.xml";
                } else if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHORIZATIONUPDATEREQUEST) == 0) {
                    xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizationupdaterequest.xml";
                }

                try {
                    msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
                } catch (Exception e) {
                    log.addInfo("Generate :" + e.toString());
                    e.printStackTrace();

                    return null;
                }

                try {
                    el = MRXMLUtils.getRootElement(msgdoc);
                } catch (Exception e) {
                    log.addInfo("Generate :" + e.toString());

                    return null;
                }

                if ((requestFile.compareToIgnoreCase(ProxyConstants.AUTHREQUEST) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.AUTHRESPONSE) == 0) || (requestFile.compareToIgnoreCase(ProxyConstants.AUTHORIZATIONUPDATEREQUEST) == 0)) {
                    if ((el != null)) {
                        try {
                            if (!(requestFile.compareToIgnoreCase(ProxyConstants.AUTHNOTIFY) == 0)) {
                                MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
                                MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
                            }

                            MRXMLUtils.setValueXpath(el, "//destInfo/JID", user);
                            MRXMLUtils.setValueXpath(el, "//destInfo/nickName", nickname);
                            MRXMLUtils.setValueXpath(el, "//profileInfo/connect", "yes");
                            MRXMLUtils.setValueXpath(el, "//profileInfo/remote", remote);
                        } catch (Exception e) {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }
                    }

                    if (requestID == null) {
                        requestID = Long.toHexString(wheel.nextLong());
                    }

                    if (el != null) {
                        try {
                            MRXMLUtils.setValueXpath(el, "//requestID", requestID);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }

                    if (requestFile.compareToIgnoreCase(ProxyConstants.AUTHRESPONSE) == 0) {
                        if (el != null) {
                            try {
                                MRXMLUtils.setValueXpath(el, "//Result", action);
                            } catch (Exception e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                            }
                        }
                    }
                }

                if (el != null) {
                    try {
                        result = MRXMLUtils.elementToString(el) + "\n";
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }

            return result;
        } catch (Exception e) {
            e.printStackTrace();

            return null;
        }
    }

    public static String setBlank(String inputString) {
        if ((inputString != null) && inputString.equalsIgnoreCase("None")) {
            inputString = "";
        }

        return inputString;
    }

    private static String generateAuthorizationUpdateResponse(StringTokenizer st, String requestFile, MRLog log) {
        String xmlFilePath = null;
        xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizationupdateresponse.xml";

        try {
            Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
            Element el = MRXMLUtils.getRootElement(msgdoc);
            String requestID = "";
            String user = "";
            String nickname = "";
            String sourceAgentName = "";
            String sourceJID = "";
            String sourceNickName = "";
            String destAgentName = "";
            String destJID = "";
            String destNickName = "";
            String agentName = "";
            String connect = "";
            String remote = "";

            if (st.hasMoreTokens()) {
                requestID = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                agentName = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                user = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                nickname = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                sourceAgentName = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                sourceJID = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                sourceNickName = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                destAgentName = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                destJID = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                destNickName = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                connect = setBlank(st.nextToken());
            }

            if (st.hasMoreTokens()) {
                remote = setBlank(st.nextToken());
            }

            MRXMLUtils.setValueXpath(el, "//header/requestID", requestID);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/agentName", agentName);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
            MRXMLUtils.setValueXpath(el, "//sourceInfo/agentName", sourceAgentName);
            MRXMLUtils.setValueXpath(el, "//sourceInfo/JID", sourceJID);
            MRXMLUtils.setValueXpath(el, "//sourceInfo/nickName", sourceNickName);
            MRXMLUtils.setValueXpath(el, "//destInfo/agentName", destAgentName);
            MRXMLUtils.setValueXpath(el, "//destInfo/JID", destJID);
            MRXMLUtils.setValueXpath(el, "//destInfo/nickName", destNickName);
            MRXMLUtils.setValueXpath(el, "//profileInfo/connect", connect);
            MRXMLUtils.setValueXpath(el, "//profileInfo/remote", remote);

            String result = MRXMLUtils.elementToString(el) + "\n";

            return result;
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());
            e.printStackTrace();

            return null;
        }
    }

    public static String generateHeartBeat(String ack, String seq, MRLog log)
        throws Exception {
        String xmlFilePath = null;
        xmlFilePath = ProxyConstants.XMLFILEPATH + "heartbeat.xml";

        String requestID = Long.toHexString(wheel.nextLong());
	if(ProxyClientUtils.heartbeatTemplate == null)
	  ProxyClientUtils.heartbeatTemplate = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	Document msgdoc = (Document)ProxyClientUtils.heartbeatTemplate.clone();
        Element el = MRXMLUtils.getRootElement(msgdoc);

        MRXMLUtils.setValueXpath(el, "//header/requestID", requestID);
        MRXMLUtils.setValueXpath(el, "//seq", seq);
        MRXMLUtils.setValueXpath(el, "//ack", ack);

        String result = MRXMLUtils.elementToString(el) + "\n";

        return result;
    }

    public static String generateRecordingNotification(StringTokenizer st, String requestFile, MRLog log) {
        String xmlFilePath = null;
        xmlFilePath = ProxyConstants.XMLFILEPATH + "recordingnotification.xml";

        String requestID = Long.toHexString(wheel.nextLong());

        try {
            Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
            Element el = MRXMLUtils.getRootElement(msgdoc);
            String user = "joe@abc";
            String nickname = "joe";
            String id = "123";
            String startTime = "12";
            String endTime = "12";
            String date = "01/01/2006";
            String status = "Recording";
            String diskSpaceUsed = "3.0";
            String totalDiskSpaceAvailable = "3.0";
            String errorCode = "0";
            String errorDescription = "None";
            MRXMLUtils.setValueXpath(el, "//header/requestID", requestID);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
            MRXMLUtils.setValueXpath(el, "//recordStatus/ID", id);
            MRXMLUtils.setValueXpath(el, "//recordStatus/Status", status);
            MRXMLUtils.setValueXpath(el, "//recordStatus/StartTime", startTime);
            MRXMLUtils.setValueXpath(el, "//recordStatus/EndTime", endTime);
            MRXMLUtils.setValueXpath(el, "//recordStatus/Date", date);
            MRXMLUtils.setValueXpath(el, "//recordStatus/DiskSpaceUsed", diskSpaceUsed);
            MRXMLUtils.setValueXpath(el, "//recordStatus/TotalDiskSpaceAvailable", totalDiskSpaceAvailable);
            MRXMLUtils.setValueXpath(el, "//recordStatus/ErrorCode", errorCode);
            MRXMLUtils.setValueXpath(el, "//recordStatus/ErrorDescritption", errorDescription);

            String result = MRXMLUtils.elementToString(el) + "\n";

            return result;
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());
            e.printStackTrace();

            return null;
        }
    }

    private static String generateBookMarkResponse(StringTokenizer st, String requestType, MRLog log) {
        String xmlFilePath = null;

        if (requestType.compareToIgnoreCase(ProxyConstants.CREATEBOOKMARKRESPONSE) == 0) {
            xmlFilePath = ProxyConstants.XMLFILEPATH + "createbookmarkresponse.xml";
        }
        try {
            Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
            Element el = MRXMLUtils.getRootElement(msgdoc);
            String requestID = "";
            String agentName = "";
            String user = "";
            String nickname = "";
            String id = "";
            String timeStamp = "";
            String notes = "";

            if (st.hasMoreTokens()) {
                requestID = st.nextToken();

                if ((requestID == null) || requestID.equalsIgnoreCase("None")) {
                    requestID = "";
                }
            }

            if (st.hasMoreTokens()) {
                agentName = st.nextToken();

                if ((agentName == null) || agentName.equalsIgnoreCase("None")) {
                    agentName = "";
                }
            }

            if (st.hasMoreTokens()) {
                user = st.nextToken();

                if ((user == null) || user.equalsIgnoreCase("None")) {
                    user = "";
                }
            }

            if (st.hasMoreTokens()) {
                nickname = st.nextToken();

                if ((nickname == null) || nickname.equalsIgnoreCase("None")) {
                    nickname = "";
                }
            }

            if (st.hasMoreTokens()) {
                id = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                timeStamp = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                notes = st.nextToken();

                if ((notes == null) || notes.equalsIgnoreCase("None")) {
                    notes = "";
                }
            }

            MRXMLUtils.setValueXpath(el, "//header/requestID", requestID);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/agentName", agentName);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
            MRXMLUtils.setValueXpath(el, "//bookmarkInfo/ID", id);
            MRXMLUtils.setValueXpath(el, "//bookmarkInfo/Timestamp", timeStamp);
            MRXMLUtils.setValueXpath(el, "//bookmarkInfo/Notes", notes);

            String result = MRXMLUtils.elementToString(el) + "\n";

            return result;
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());
            e.printStackTrace();

            return null;
        }
    }

    private static String generateMediaTimestampResponse(StringTokenizer st, String requestType, MRLog log) {
        String xmlFilePath = ProxyConstants.XMLFILEPATH + "mediatimestampresponse.xml";

        try {
            Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
            Element el = MRXMLUtils.getRootElement(msgdoc);
            String requestID = "";
            String agentName = "";
            String user = "";
            String nickname = "";
            String id = "";
            String timestamp = "";

            if (st.hasMoreTokens()) {
                requestID = st.nextToken();

                if ((requestID == null) || requestID.equalsIgnoreCase("None")) {
                    requestID = "";
                }
            }

            if (st.hasMoreTokens()) {
                agentName = st.nextToken();

                if ((agentName == null) || agentName.equalsIgnoreCase("None")) {
                    agentName = "";
                }
            }

            if (st.hasMoreTokens()) {
                user = st.nextToken();

                if ((user == null) || user.equalsIgnoreCase("None")) {
                    user = "";
                }
            }

            if (st.hasMoreTokens()) {
                nickname = st.nextToken();

                if ((nickname == null) || nickname.equalsIgnoreCase("None")) {
                    nickname = "";
                }
            }

            if (st.hasMoreTokens()) {
                id = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                timestamp = st.nextToken();
            }

            MRXMLUtils.setValueXpath(el, "//header/requestID", requestID);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/agentName", agentName);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);
            MRXMLUtils.setValueXpath(el, "//ID", id);
            MRXMLUtils.setValueXpath(el, "//Timestamp", timestamp);

            String result = MRXMLUtils.elementToString(el) + "\n";

            return result;
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());
            e.printStackTrace();

            return null;
        }
    }

    private static String generateRecordingResponse(StringTokenizer st, String requestType, MRLog log) {
        String xmlFilePath = null;

        if (requestType.compareToIgnoreCase(ProxyConstants.STARTRECORDINGRESPONSE) == 0) {
            xmlFilePath = ProxyConstants.XMLFILEPATH + "startrecordingresponse.xml";
        } else if (requestType.compareToIgnoreCase(ProxyConstants.UPDATERECORDINGRESPONSE) == 0) {
            xmlFilePath = ProxyConstants.XMLFILEPATH + "updaterecordingresponse.xml";
        } else if (requestType.compareToIgnoreCase(ProxyConstants.STOPRECORDINGRESPONSE) == 0) {
            xmlFilePath = ProxyConstants.XMLFILEPATH + "stoprecordingresponse.xml";
        } else if (requestType.compareToIgnoreCase(ProxyConstants.PAUSERECORDINGRESPONSE) == 0) {
            xmlFilePath = ProxyConstants.XMLFILEPATH + "pauserecordingresponse.xml";
        } else if (requestType.compareToIgnoreCase(ProxyConstants.RESUMERECORDINGRESPONSE) == 0) {
            xmlFilePath = ProxyConstants.XMLFILEPATH + "resumerecordingresponse.xml";
        } else if (requestType.compareToIgnoreCase(ProxyConstants.RECORDINGSTATUSRESPONSE) == 0) {
            xmlFilePath = ProxyConstants.XMLFILEPATH + "recordingstatusresponse.xml";
        }

        try {
            Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
            Element el = MRXMLUtils.getRootElement(msgdoc);
            String requestID = null;
            String agentName = null;
            String user = "";
            String nickname = "";
            String id = "";
            String startTime = "";
            String endTime = "";
            String date = "";
            String status = "";
            String diskSpaceUsed = "";
            String totalDiskSpaceAvailable = "";
            String errorCode = "";
            String errorDescription = "";
            String timeOffset = "";
            String patient = "";
            String procedureType = "";
            String physician = "";
            String notes = "";

            if (st.hasMoreTokens()) {
                requestID = st.nextToken();

                if (requestID.equalsIgnoreCase("None")) {
                    requestID = "";
                }
            }

            if (st.hasMoreTokens()) {
                agentName = st.nextToken();

                if (agentName.equalsIgnoreCase("None")) {
                    agentName = "";
                }
            }

            if (st.hasMoreTokens()) {
                user = st.nextToken();

                if (user.equalsIgnoreCase("None")) {
                    user = "";
                }
            }

            if (st.hasMoreTokens()) {
                nickname = st.nextToken();

                if (nickname.equalsIgnoreCase("None")) {
                    nickname = "";
                }
            }

            if (st.hasMoreTokens()) {
                id = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                startTime = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                endTime = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                date = st.nextToken();

                if (date.equalsIgnoreCase("None")) {
                    date = "";
                }
            }

            if (st.hasMoreTokens()) {
                status = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                diskSpaceUsed = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                totalDiskSpaceAvailable = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                errorCode = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                errorDescription = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                timeOffset = st.nextToken();
            }

            if (st.hasMoreTokens()) {
                patient = st.nextToken();

                if (patient.equalsIgnoreCase("None")) {
                    patient = "";
                }
            }

            if (st.hasMoreTokens()) {
                procedureType = st.nextToken();

                if (procedureType.equalsIgnoreCase("None")) {
                    procedureType = "";
                }
            }

            if (st.hasMoreTokens()) {
                physician = st.nextToken();

                if (physician.equalsIgnoreCase("None")) {
                    physician = "";
                }
            }

            if (st.hasMoreTokens()) {
                notes = st.nextToken();

                if (notes.equalsIgnoreCase("None")) {
                    notes = "";
                }
            }

            if (errorDescription.equalsIgnoreCase("NoError")) {
                errorDescription = ProxyExceptionCodes.getErrorDescription(Integer.valueOf(errorCode));
            }

            MRXMLUtils.setValueXpath(el, "//header/requestID", requestID);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/agentName", agentName);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/JID", user);
            MRXMLUtils.setValueXpath(el, "//requestorInfo/nickName", nickname);

            if (requestType.compareToIgnoreCase(ProxyConstants.RECORDINGSTATUSRESPONSE) == 0) {
                MRXMLUtils.setValueXpath(el, "//recordInfo/ID", id);
                MRXMLUtils.setValueXpath(el, "//recordInfo/TimeOffset", timeOffset);
                MRXMLUtils.setValueXpath(el, "//recordInfo/Patient", patient);
                MRXMLUtils.setValueXpath(el, "//recordInfo/ProcedureType", procedureType);
                MRXMLUtils.setValueXpath(el, "//recordInfo/Physician", physician);
                MRXMLUtils.setValueXpath(el, "//recordInfo/Notes", notes);
            }

            MRXMLUtils.setValueXpath(el, "//recordStatus/ID", id);
            MRXMLUtils.setValueXpath(el, "//recordStatus/Status", status);
            MRXMLUtils.setValueXpath(el, "//recordStatus/StartTime", startTime);
            MRXMLUtils.setValueXpath(el, "//recordStatus/EndTime", endTime);
            MRXMLUtils.setValueXpath(el, "//recordStatus/Date", date);
            MRXMLUtils.setValueXpath(el, "//recordStatus/DiskSpaceUsed", diskSpaceUsed);
            MRXMLUtils.setValueXpath(el, "//recordStatus/TotalDiskSpaceAvailable", totalDiskSpaceAvailable);
            MRXMLUtils.setValueXpath(el, "//recordStatus/ErrorCode", errorCode);
            MRXMLUtils.setValueXpath(el, "//recordStatus/ErrorDescription", errorDescription);

            String result = MRXMLUtils.elementToString(el) + "\n";

            return result;
        } catch (Exception e) {
            log.addInfo("Generate :" + e.toString());
            e.printStackTrace();

            return null;
        }
    }

    
    public static String getRequestId(String message) {
        String requestID = null;
        Document msgdoc = null;
        Element el = null;

        try {
            msgdoc = MRXMLUtils.getDocument(message);
            el = MRXMLUtils.getRootElement(msgdoc);
            requestID = MRXMLUtils.getValueXpath(el, "//requestID", "unknown");
        } catch (Exception e) {
        }

        return requestID;
    }

    public static String getRequestName(String message) {
        String requestName = null;
        Document msgdoc = null;
        Element el = null;

        try {
            msgdoc = MRXMLUtils.getDocument(message);
            el = MRXMLUtils.getRootElement(msgdoc);
            requestName = el.getName();
        } catch (Exception e) {
        }

        return requestName;
    }

    public static String getAck(String message) {
        String ack = null;
        Document msgdoc = null;
        Element el = null;

        try {
            msgdoc = MRXMLUtils.getDocument(message);
            el = MRXMLUtils.getRootElement(msgdoc);
            ack = MRXMLUtils.getValueXpath(el, "//seq", "0"); // seq from client is ack for server and vice versa
        } catch (Exception e) {
        }

        return ack;
    }

    public static String getResult(String message) {
        String result = null;
        Document msgdoc = null;
        Element el = null;

        try {
            msgdoc = MRXMLUtils.getDocument(message);
            el = MRXMLUtils.getRootElement(msgdoc);
            result = MRXMLUtils.getValueXpath(el, "//Result", "unknown");
        } catch (Exception e) {
        }

        return result;
    }

    
    
    public static synchronized void socketWrite(BufferedWriter writer, String writeString, MRLog log) {
        try {
            writer.write(writeString);
            writer.flush();
        } catch (IOException e) {
            log.addInfo("Send : " + e.toString());
        }
    }

    public static int getHeartbeatFrequency() {
        return heartbeatFrequency;
    }

    public static void setHeartbeatFrequency(int heartbeatFrequency) {
        ProxyClientUtils.heartbeatFrequency = heartbeatFrequency;
    }

}
