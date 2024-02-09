package com.ipvs.sessionservice.resource.handler;

import java.util.Date;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.utils.json.JSONException;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;

@Path("/{parameter: Service/RegisterUser|RegisterUser}")
public class RegisterUserRequestHandler extends BaseHtppRequestHandler {

    @Context
    ServletContext servletContext;
    private static String localConfigFile = "/etc/config/Device.xml";

    @POST
    @Consumes({ MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML })
    @Produces({ MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML })
    public Response handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers)
            throws Exception {
        setInfo(servletContext);
        String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
        if (acceptHeader == null) {
            acceptHeader = DEFAULT_MIME_SUFFIX;
        }
        // check if userselfRegistery enabled
        Document configDoc = MRXMLUtils.loadXMLFile(localConfigFile, log); 
        String userSelfRegistryEnable = MRXMLUtils.getValueXpath(configDoc, "//UserSelfRegistry/Enable", "false");
        boolean userSelfRegistry = Boolean.parseBoolean(userSelfRegistryEnable);
        
        if (!userSelfRegistry) {
            return Response.status(HttpServletResponse.SC_FORBIDDEN).type(acceptHeader)
                    .entity("RegisterUser Failed! User SelfRegistry is not allowed!").build();
        }
        try {
            String name = "";
            String password = "";

            try {

                if (acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
                	xml = MRXMLUtils.jsonToXMLString(xml);
                }

                Element requestBodyEl = MRXMLUtils.stringToElement(xml);
                name = MRXMLUtils.getAttributeValue(requestBodyEl, "name", "");
                password = MRXMLUtils.getAttributeValue(requestBodyEl, "password", "");
            } catch (Exception e) {
                log.addInfo("RegisterUserRequestHandler: RegisterUser Request Failed. Bad Request " + e.toString());
                return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(e.toString())
                        .build();
            }
            
            MRRequest request = MRRequest.createRequest("");
            
            // add user in db
            new AddUserRequestHandler().addUserInDB(request, name, name, password, "", new Date().toString(),
                    new Date().toString(), null, xmpp, log, db, SessionServiceConst.USER);

            Response response = Response.status(HttpServletResponse.SC_OK).type(acceptHeader)
                    .entity("User has been added successfully!").build();
            return response;

        } catch (Exception e) {
            log.addInfo("RegisterUserRequestHandler: RegisterUser Failed. " + e.toString());
            e.printStackTrace();
            String responseStr = "<Exception>"+e.toString()+"</Exception>";
			if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
			    	responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
            }
			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(responseStr).build();
           
        }
    }

    @Override
    protected String getResponseString(Document responseDoc, String requestName) throws Exception {

        return "";
    }

}
