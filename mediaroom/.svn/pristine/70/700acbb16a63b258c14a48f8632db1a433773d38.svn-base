package com.ipvs.xmpp.client.params;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.service.MRXMLUtils;

public class TimeParams extends XmppParams {

    private static SimpleDateFormat utcFormat = new SimpleDateFormat("yyyyMMdd'T'HH:mm:ss");
    private static DateFormat displayFormat = DateFormat.getDateTimeInstance();

    private String utc = null;
    private String tz = null;
    private String display = null;

    public TimeParams() {
        setElementName("query");
        setNamespace("jabber:iq:time");
    }

    public TimeParams(Calendar cal) {
        TimeZone timeZone = cal.getTimeZone();
        tz = cal.getTimeZone().getID();
        display = displayFormat.format(cal.getTime());
        // Convert local time to the UTC time.
        utc = utcFormat.format(new Date(cal.getTimeInMillis() - timeZone.getOffset(cal.getTimeInMillis())));
    }

    public Date getTime() {
        if (utc == null) {
            return null;
        }
        Date date = null;
        try {
            Calendar cal = Calendar.getInstance();
            // Convert the UTC time to local time.
            cal.setTime(new Date(utcFormat.parse(utc).getTime() + cal.getTimeZone().getOffset(cal.getTimeInMillis())));
            date = cal.getTime();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return date;
    }

    public void setTime(Date time) {
        // Convert local time to UTC time.
        utc = utcFormat.format(new Date(time.getTime() - TimeZone.getDefault().getOffset(time.getTime())));
    }

    @Override
    public void setXML(String payload) {

    }

    @Override
    public String toXML() {
        StringBuilder buf = new StringBuilder();
        buf.append("<query xmlns=\"jabber:iq:time\">");
        if (utc != null) {
            buf.append("<utc>").append(utc).append("</utc>");
        }
        if (tz != null) {
            buf.append("<tz>").append(tz).append("</tz>");
        }
        if (display != null) {
            buf.append("<display>").append(display).append("</display>");
        }
        buf.append("</query>");
        return buf.toString();
    }

    public void parseXML(String xml) {
        try {
            Document doc = MRXMLUtils.stringToDocument(xml);
            Element queryEL = MRXMLUtils.getDescendentWithoutNamespace(doc.getRootElement(), "query");

            Element utcEL = MRXMLUtils.getDescendentWithoutNamespace(queryEL, "utc");
            utc = MRXMLUtils.getValue2(utcEL, "");
            utcEL = MRXMLUtils.getDescendentWithoutNamespace(queryEL, "tz");
            tz = MRXMLUtils.getValue2(utcEL, "");
            utcEL = MRXMLUtils.getDescendentWithoutNamespace(queryEL, "display");
            display = MRXMLUtils.getValue2(utcEL, "");

        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    public String getUtc() {
        return utc;
    }

    public void setUtc(String utc) {
        this.utc = utc;
    }

    public String getTz() {
        return tz;
    }

    public void setTz(String tz) {
        this.tz = tz;
    }

    public String getDisplay() {
        return display;
    }

    public void setDisplay(String display) {
        this.display = display;
    }

}
/*
<iq id="FIRTu-80" from="managementserver@localhost/managementserver" type="get">
<query xmlns="jabber:iq:time"></query></iq>
<iq id="FIRTu-80" to="managementserver@localhost/managementserver" type="result">
<query xmlns="jabber:iq:time"><utc>20090806T10:13:35</utc><tz>Greenwich Mean Time</tz>
<display>Aug 6, 2009 10:13:35 AM GMT</display></query></iq>


*/