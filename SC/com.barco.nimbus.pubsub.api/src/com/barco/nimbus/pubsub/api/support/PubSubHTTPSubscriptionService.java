package com.barco.nimbus.pubsub.api.support;

import java.net.URI;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import javax.ws.rs.core.Request;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.apache.log4j.Logger;

public abstract class PubSubHTTPSubscriptionService {
	
	final Logger log = Logger.getLogger(PubSubHTTPSubscriptionService.class);
	
	final protected Map<String,String> file;
	final List<String> GET_HEAD = Arrays.asList("GET","HEAD");
	final String root;
	
	public PubSubHTTPSubscriptionService(String string, Map<String, String> map) {
		this.root = string;
		this.file = map;
	}

	public Response redirectToJmsBridge(String which2, UriInfo request, Request req) {
		StringBuilder uri2 = new StringBuilder();
		for (String which : which2.split(",")) {
			String params = (String)file.get(which);
			if (params == null) {
				return Response.status(404).entity("No handler for event/" + which).build();
			}
			params = fill(request, params);
			
			uri2.append(params);
			uri2.append("&name="+which);
			uri2.append("&"); // fix http://itrack.barco.com/browse/TSM0001-665			
		}
		uri2.deleteCharAt(uri2.length()-1);
		String pre = createPrefix(request);
		String q = request.getRequestUri().getRawQuery() == null ? "" : ("&"+request.getRequestUri().getRawQuery());
		String uri = pre + root + "/get?" + uri2 + q;		
		log.trace(uri);
		if (GET_HEAD.contains(req.getMethod().toUpperCase(Locale.ENGLISH))) {
			return Response.status(303).header("Location", uri).build();
		} else {
			String session = subscribe();
			return Response.status(201).header("Location", uri + "&session=" + session).build();
		}
	}

	protected abstract String fill(UriInfo request, String params);

	private String createPrefix(UriInfo info) {
		URI request = info.getAbsolutePath();
		return request.getScheme() + "://" + request.getHost() + ":" + request.getPort();
	}
	
	protected abstract String subscribe();

	public Response subscribe(List<String> names, String accept, String room, String sink, String session, String expression, UriInfo request, Request req) {
		StringBuilder which2 = new StringBuilder();
		for (String partialLinkName : names) {
			which2.append(partialLinkName);
			which2.append(',');
		}
		if (which2.length() > 0) {
			which2.deleteCharAt(which2.length()-1);
		}
		return redirectToJmsBridge(which2.toString(), request, req);
	}

}
