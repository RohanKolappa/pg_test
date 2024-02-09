package com.barco.nimbus.displaymanagement.bms;

import java.util.Collections;
import java.util.List;
import java.util.concurrent.atomic.AtomicReference;

import javax.ws.rs.core.Request;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.nimbus.pubsub.api.PubSubHTTPI;
import com.barco.nimbus.pubsub.api.support.PubSubHTTPSubscriptionService;

@Component
public class BMSEventSubscriptionService extends PubSubHTTPSubscriptionService implements BMSEventSubscriptionServiceI {

	final Logger log = Logger.getLogger(BMSEventSubscriptionService.class);
	final AtomicReference<PubSubHTTPI> ps = new AtomicReference<PubSubHTTPI>();
	
	@Reference
	public void setPubSub(PubSubHTTPI ps) {
		this.ps.set(ps);
	}
	
	public void unsetPubSub(PubSubHTTPI ps) {
		this.ps.compareAndSet(ps, null);
	}
	
	public BMSEventSubscriptionService() {
		super("/barco-webservice/rest/jms",Collections.singletonMap("session3", "destination=topic%2Fsession%2F{selector-session}&selector=&expression="));
	}
	
	@Override
	public Response subscribe(List<String> names, String accept, String room, String sink, String session, String expression, UriInfo request, Request req) {
		// This class must physically implement the method, for JAX-RS to recognize it
		return super.subscribe(names, accept, room, sink, session, expression, request, req);
	}
	
	@Activate
	public void start() {
		log.info("started");
	}
	
	@Override
	protected String fill(UriInfo request, String params) {
		return params.replace("{selector-session}", request.getQueryParameters().getFirst("selector-session"));
	}

	@Override
	protected String subscribe() {
		return ps.get().subscribe();
	}
}
