package com.ipvs.utils;

import java.lang.reflect.Method;
import java.util.List;

import javax.ws.rs.WebApplicationException;
import javax.ws.rs.ext.Provider;

import org.eclipse.jetty.http.HttpStatus;
import org.jboss.resteasy.annotations.interception.ServerInterceptor;
import org.jboss.resteasy.core.Headers;
import org.jboss.resteasy.core.ResourceMethod;
import org.jboss.resteasy.core.ServerResponse;
import org.jboss.resteasy.spi.Failure;
import org.jboss.resteasy.spi.HttpRequest;
import org.jboss.resteasy.spi.interception.AcceptedByMethod;
import org.jboss.resteasy.spi.interception.PreProcessInterceptor;

/**
 * 
 * This interceptor checks whether a incoming REST-call has a supported Accept header. This interceptor only intercepts method-invocations on methods annotated with @ProducesBarcoJson.
 * The supported Accept-headers are defined in {@link RestAcceptHeaders}
 * The first provided accept-header that is valid is considered.
 *
 */
@Provider
@ServerInterceptor
public class ProducesBarcoJsonInterceptor implements PreProcessInterceptor, AcceptedByMethod {

	@Override
	public boolean accept(Class type, Method method) {
		return method.isAnnotationPresent(ProducesBarcoJson.class);
	}


	@Override
	public ServerResponse preProcess(HttpRequest request, ResourceMethod method) throws Failure, WebApplicationException {
		List<String> accept = request.getHttpHeaders().getRequestHeader("Accept");
		boolean isValidAcceptHeader = false;
		String firstAcceptHeader = "";
		for (String acceptHeader : accept) {
			if(RestAcceptHeaders.isValidAcceptHeader(acceptHeader)){
				isValidAcceptHeader = true;
				firstAcceptHeader = acceptHeader;
				break;
			}
		}
		
		if(firstAcceptHeader.contains(RestAcceptHeaders.ACCEPT_HEADER_PREFIX)){
			//check if acceptHeader and resourcePath correspond
			if(request.getUri().getPath().toLowerCase().contains( RestAcceptHeaders.resourceType(firstAcceptHeader).toLowerCase())){
				return null;
			}else {
				return new ServerResponse("acceptheader " + firstAcceptHeader + " does not correspond with resourcepath " + request.getUri().getPath(), HttpStatus.BAD_REQUEST_400, new Headers<Object>());
			}
		}else if(isValidAcceptHeader){
			//not all valid headers contain the barco prefix
			return null;
		}

		return new ServerResponse("request does not have a supported Accept header: " + accept.toString(), HttpStatus.BAD_REQUEST_400, new Headers<Object>());			

	}

}
