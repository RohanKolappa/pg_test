package com.barco.nimbus.pubsub.api;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

/**
 * JAX-RS exposure of pubsub (currently, only consuming events is exposed).
 * This interface with JAX-RS bindings was used in NMS 1.x.
 */
public interface PubSubHTTPI {

	/**
	 * @param session
	 *            your subscription id
	 * @param timeoutMs
	 *            server-side buffer size (in milliseconds)
	 * @param destination
	 *            the jms destination (eg topic/x)
	 * @param selector
	 *            the jms selector (eg method='createDisplay')
	 * @param accept
	 *            "application/json"
	 * @param expression
	 *            the xpath-like bean-expression (eg [0.1=key].3)
	 * @param name
	 *            the value to use in the X-bms-name response header when
	 *            delivering an event
	 * @param suspendSec
	 *            long-poll duration in seconds (after which the call will
	 *            return with 204 "no data")
	 * @param request
	 * @return 201 = subscription initiated, 200 = event delivery, 204 = no
	 *         event happened
	 */
	Response getNextMessage(String session,
			int timeoutMs,
			List<String> destination,
			List<String> selector,
			MediaType accept,
			List<String> expression,
			List<String> name,
			int suspendSec,
			HttpServletRequest request);

	String subscribe();
	
	void send(String destination, List<String> headers, Object body);
}