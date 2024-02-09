package com.ipvs.common;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.ResourceBundle;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.service.MRXMLUtils;


/**
 * This class represents the common exception class to be used throughouot the system
 * @author miyer
 *
 */
public final class MRException extends Exception {
    private static final long serialVersionUID = 1L;
    private static ResourceBundle prop = null;
    private static boolean bInitAttemptDone = false;
    private int code;
    private String data;
    private Locale locale = null; //TBD

    public MRException(int pCode, String pData) {
        code = pCode;
        data = pData;
    }

    public MRException(int pCode, String pData, Locale locale) {
        code = pCode;
        data = pData;
        this.locale = locale;
    }

    public String toString2() {
        return data;
    }

    public String getDescription() {
    	String desc = "UNKNOWN ERROR";
        try {
            if (!bInitAttemptDone) {
                init();
            }
            desc = getErrorMessage(code, data);
            if ((desc != null) && !desc.trim().equals(""))
            	return desc;
            
            if ((data != null) && !data.trim().equals(""))
            	return data;
        } catch(Exception e) {
        	e.printStackTrace();
        }
        return desc;
    }

    public String toString() {
    	String desc = this.getDescription();
        return "<Code>" + code + "</Code><Description>" + desc + "</Description>";    	
    }
    
    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getData() {
        return data;
    }

    public void setData(String data) {
        this.data = data;
    }

    public Locale getLocale() {
        return locale;
    }

    public void setLocale(Locale locale) {
        this.locale = locale;
    }

    public void setDataParam(String[] paramNames, String[] paramValues) {
        Document doc = null;

        try {
            try {
                doc = MRXMLUtils.stringToDocument(data);
            } catch (Exception e) {
            }

            if (doc == null) {
                if ((data != null) && !data.trim().equals("")) {
                    if ((paramNames != null) && (paramValues != null) && (paramValues.length == paramNames.length)) {
                        for (int i = 0; i < paramValues.length; i++) {
                            String paramName = paramNames[i];
                            String paramValue = paramValues[i];
                            data = data + ". " + paramName + " = " + paramValue;
                        }
                    }

                    return;
                }

                try {
                    doc = MRXMLUtils.getDocument();
                    MRXMLUtils.addElement(doc, "error");
                } catch (Exception e) {
                }
            }

            if (doc != null) {
                boolean bUpdated = false;

                if ((paramNames != null) && (paramValues != null) && (paramValues.length == paramNames.length)) {
                    for (int i = 0; i < paramValues.length; i++) {
                        String paramName = paramNames[i];
                        String paramValue = paramValues[i];

                        if (addDataParamToDoc(doc, paramName, paramValue)) {
                            bUpdated = true;
                        }
                    }
                }

                if (bUpdated) {
                    try {
                        data = MRXMLUtils.documentToString(doc);
                    } catch (Exception e) {
                    }
                }
            }
        } catch (Exception e) {
        }
    }

    public void setDataParam(String paramName, String paramValue) {
        Document doc = null;

        try {
            try {
                doc = MRXMLUtils.stringToDocument(data);
            } catch (Exception e) {
            }

            if (doc == null) {
                if ((data != null) && !data.trim().equals("")) {
                    data = data + ". " + paramName + " = " + paramValue;

                    return;
                }

                try {
                    doc = MRXMLUtils.getDocument();
                    MRXMLUtils.addElement(doc, "error");
                } catch (Exception e) {
                }
            }

            if (doc != null) {
                if (addDataParamToDoc(doc, paramName, paramValue)) {
                    try {
                        data = MRXMLUtils.documentToString(doc);
                    } catch (Exception e) {
                    }
                }
            }
        } catch (Exception e) {
        }
    }

    private boolean addDataParamToDoc(Document doc, String paramName, String paramValue) {
        boolean bUpdated = false;

        try {
            if (doc != null) {
                try {
                    Element EL = MRXMLUtils.getElement(doc.getRootElement(), paramName);
                    bUpdated = true;

                    if (EL != null) {
                        EL.detach();
                    }

                    MRXMLUtils.addElement(doc.getRootElement(), paramName, paramValue);
                } catch (Exception e) {
                	e.printStackTrace();
                }
            }
        } catch (Exception e) {
        }

        return bUpdated;
    }

    public String getDataParam(String paramName, String paramValue) {
        Document doc = null;

        try {
            try {
                doc = MRXMLUtils.stringToDocument(data);
            } catch (Exception e) {
                return "";
            }

            if (doc != null) {
                try {
                    return MRXMLUtils.getValue(doc.getRootElement(), paramName, "");
                } catch (Exception e) {
                }
            }
        } catch (Exception e) {
        }

        return "";
    }

    /////// static methods ///////////////////////
    public static String getStackTrace(Throwable throwableObj) {
        StringWriter sw = new StringWriter();
        throwableObj.printStackTrace(new PrintWriter(sw));

        return sw.toString();
    }

    private static void init() {
        if (!bInitAttemptDone) {
            bInitAttemptDone = true;

            try {
                prop = ResourceBundle.getBundle("com.ipvs.common.service.message", Locale.getDefault());
            } catch (Exception e) {
                prop = null;
            }
        }
    }

    public static String getErrorMessage(int code, String data, Locale locale) {
        //TODO
        return null;
    }

    public static String getErrorMessage(int code, String data) {
        String errorMessage = "";

        try {
            if (prop == null) {
                 return null;
            } else if (!prop.containsKey(code + "")) {
                 return null;
            }

            //read 
            errorMessage = prop.getString(code + "");

            //test if data is null/empty ,  error from prop
            if ((data == null) || data.trim().equals("")) {
                 return errorMessage;
            }

            //test if data is valid xml, else retun data 
            Document doc = null;

            try {
                doc = MRXMLUtils.stringToDocument(data);
            } catch (Exception e) {
                 return null;
            }

            if (doc == null) {
                 return null;
            }

            // test errorMessage
            if ((errorMessage == null) || errorMessage.trim().equals("")) {
                  return null;
            }

            //errorMessage
            if (errorMessage.indexOf("$") == -1) {
                 return errorMessage;
            }

            //read data from xml 
            try {
                List<?> listEL = MRXMLUtils.getList(doc.getRootElement());

                for (Iterator<?> i = listEL.iterator(); i.hasNext();) {
                    Element actionEL = (Element) i.next();
                    String tag = "$" + actionEL.getName();
                    String tagvalue = MRXMLUtils.getValue2(actionEL, "");
                    errorMessage = replaceWord(errorMessage, tag, tagvalue);
                }
            } catch (Exception e) {
                //return data;
            }
        } catch (Exception e) {
        }

        return errorMessage;
    }

    private static String replaceWord(String original, String find, String replacement) {
        try {
            int i = original.indexOf(find);

            if (i < 0) {
                return original; // return original if 'find' is not in it.
            }

            String partBefore = original.substring(0, i);
            String partAfter = original.substring(i + find.length());

            return partBefore + replacement + partAfter;
        } catch (Exception ex) {
        }

        return original;
    }
    
    public String formatStringException(){
    	return "<Exception>" + toString() + "</Exception>";
    }
}
