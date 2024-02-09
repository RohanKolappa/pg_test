package com.barco.nimbus.pubsub.internal;

import java.util.List;
import java.util.concurrent.atomic.AtomicReference;

import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.nimbus.pubsub.api.PubSubHTTPI;
import com.barco.nimbus.pubsub.api.PubSubI;

@Component
public class PubSubHTTPService implements PubSubHTTPJAXRS, PubSubHTTPI {
	
	private final AtomicReference<PubSubI> ps = new AtomicReference<>(); 
	private com.barco.nimbus.pubsub.api.support.PubSubHTTPService delegate;

	@Reference
	public void bind(PubSubI pubsub) {
		this.ps.set(pubsub);
		this.delegate = new com.barco.nimbus.pubsub.api.support.PubSubHTTPService(pubsub){};
	}
	
	public void unbind(PubSubI pubsub) {
		this.ps.compareAndSet(pubsub,null);
		this.delegate = null;
	}

	@Override
	public Response getNextMessage(String session, int timeoutMs, List<String> destination, List<String> selector, MediaType accept, List<String> expression,
			List<String> name, int suspendSec, HttpServletRequest request) {
		return delegate.getNextMessage(session, timeoutMs, destination, selector, accept, expression, name, suspendSec, request);
	}

	@Override
	public String subscribe() {
		return delegate.subscribe();
	}
	
	@Override
	public void send(String destination, List<String> headers, Object body) {
		delegate.send(destination, headers, body);
	}
}
