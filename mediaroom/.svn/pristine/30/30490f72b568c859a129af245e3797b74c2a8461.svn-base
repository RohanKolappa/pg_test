package com.ipvs.sessionservice.resource.handler;


import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MultivaluedMap;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.jboss.resteasy.annotations.Suspend;
import org.jboss.resteasy.spi.AsynchronousResponse;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.utils.ProducesBarcoJson;
import com.ipvs.utils.RestAcceptHeaders;



@Path("/Resource")
public class HttpResourceRequestHandler extends BaseHtppRequestHandler{

	public HttpResourceRequestHandler (){

	}

	@Context ServletContext servletContext;
	@Context HttpServletRequest servletRequest;
	@GET
	@Path("/{resourceName}")
	@ProducesBarcoJson
	public void handleGetRequest(@Context UriInfo uriInfo, @Context HttpHeaders headers, @PathParam("resourceName") String resource, final @Suspend(1000) AsynchronousResponse asynResponse) {     
		String acceptHeader = headers.getRequestHeader("Accept").get(0);
		handleGetRequestWithChild(uriInfo, headers, resource, RestAcceptHeaders.resourceProperty(acceptHeader),asynResponse);
	}

	/**
	 * deprecated because the resourceChild should be passed via the AcceptHeader. Not in the path. 
	 */
	@Deprecated
	@GET
	@ProducesBarcoJson
	@Path("/{resourceName}/{resourceChild}")
	public void  handleGetRequestWithChild(@Context UriInfo uriInfo, @Context HttpHeaders headers, @PathParam("resourceName") String resource, @PathParam("resourceChild") String resourcechild, final @Suspend(1000) AsynchronousResponse asynResponse){
		String requestName = "Get"+ resource + resourcechild;
	    super.handleHttpRequest(servletContext, uriInfo, headers, requestName,null, asynResponse);
	}
	
	@Override
	protected String getResponseString(Document responseDoc,String requestName) throws Exception {
		if(responseDoc == null) {
			return "<Resource/>";
		}
		Element responseEL  = MRXMLUtils.getElementXpath(responseDoc, "//"+requestName);
		responseEL.setName("Resource");
		return responseEL.asXML();
	}	

	@POST	
	@ProducesBarcoJson
	@Path("/{resourceName}")
	public void handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers,@QueryParam("NID") String byNID,@QueryParam("title") String title, @PathParam("resourceName") String resource, final @Suspend(1000) AsynchronousResponse asynResponse) {    
		setInfo(servletContext);
		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
		 handlePostRequestWithChild(xml, uriInfo, headers, byNID, title, resource, RestAcceptHeaders.resourceProperty(acceptHeader),asynResponse);
	}
	
	/**
	 * deprecated because the resourceChild should be passed via the AcceptHeader. Not in the path. 
	 */
	@Deprecated
	@POST	
	@ProducesBarcoJson
	@Path("/{resourceName}/{resourceChild}")
	public void handlePostRequestWithChild(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers,@QueryParam("NID") String byNID,@QueryParam("title") String title, @PathParam("resourceName") String resource, @PathParam("resourceChild") String resourceChild,final @Suspend(1000) AsynchronousResponse asynResponse) {    
		setInfo(servletContext);
		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
		if(acceptHeader == null) {
            acceptHeader = DEFAULT_MIME_SUFFIX;
        }
		
		try {
		    Continuation cc = getCachedHttpContinuation(servletRequest,acceptHeader);
            if(cc != null) {
                super.handleRedispatchedRequest(cc, servletContext,acceptHeader,asynResponse);
                return;
            }
			if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
				xml = MRXMLUtils.jsonToXMLString(xml);
			}	
			Document requestBody = MRXMLUtils.stringToDocument(xml);
			String requestPrefix = "Set";
			//if title or nid not provided them consider its a add 
			if(title ==null && byNID==null){
				requestPrefix = "Add";
			} else {
				requestBody.getRootElement().addAttribute(SessionServiceResourceConst.BY_NID_ATTRIBUTE, byNID);
				requestBody.getRootElement().addAttribute(SessionServiceResourceConst.BY_TITLE_ATTRIBUTE, title);
			}	
			String requestName = requestPrefix + resource + resourceChild;
			requestBody.getRootElement().setName(requestName);			
		     super.handleHttpRequest(servletContext, uriInfo, headers, requestName,requestBody.getRootElement(),asynResponse);

		} catch (Exception e) {
			log.addInfo("Http handleRequest Failed:",MRLog.INFO,MRLog.NOTIFY);
			//e.printStackTrace();
		   Response res = Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(e.toString()).build();
		    asynResponse.setResponse(res);
            return;
		}

	}


	@DELETE
	@Path("/{resourceName}")
	public void handleDeleteRequest(@Context UriInfo uriInfo, @Context HttpHeaders headers,@QueryParam("NID") String byNID,@QueryParam("title") String title, @PathParam("resourceName") String resource,final @Suspend(1000) AsynchronousResponse asynResponse) {    
		try {				
			String requestPrefix = "Delete";
			
			String requestName = requestPrefix + resource;
			Element requestElement = MRXMLUtils.newElement(requestName);
			
			//if title or nid not provided them consider its a add 
			if(title ==null && byNID==null){
			    Response res = Response.status(HttpServletResponse.SC_BAD_REQUEST).type("application/xml").build();
			    asynResponse.setResponse(res);
	            return;
			} else {
				requestElement.addAttribute(SessionServiceResourceConst.BY_NID_ATTRIBUTE, byNID);
				requestElement.addAttribute(SessionServiceResourceConst.BY_TITLE_ATTRIBUTE, title);
				MultivaluedMap<String, String> parameters = uriInfo.getQueryParameters();
				for(String key : parameters.keySet())  
	            {   if( !key.equals("NID") && !key.equals("title")) { //as NID and title are handled above
                    requestElement.addAttribute( key, StringUtils.join(parameters.get(key), ","));
                }
	            }
			}
			super.handleHttpRequest(servletContext, uriInfo, headers, requestName,requestElement, asynResponse);

		} catch (Exception e) {
			log.addInfo("Http handleRequest Failed:",MRLog.INFO,MRLog.NOTIFY);
			//e.printStackTrace();
		    Response res = Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type("application/xml").entity(e.toString()).build();
		    asynResponse.setResponse(res);
            return;
		}
	}
}