package com.barco.nimbus.pubsub.api.support;

import java.io.Closeable;
import java.io.IOException;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.CopyOnWriteArrayList;

import org.apache.log4j.Logger;

import com.barco.nimbus.pubsub.api.PubSubI;
import com.barco.nimbus.pubsub.api.TransformI;

/**
 * Ad hoc plain java implementation of PubSubI.
 * 
 * TODO This in NOT ready for production:
 * 
 * <li>message delivery happens in the caller thread, therefore a slow consumer
 * will stall the whole delivery.
 * 
 * <li>If a consumer fails, delivery stops so some other consumers might never
 * see the message.
 * 
 * <li>message delivery happens in the caller thread, so concurrent sends would
 * cause concurrent delivery, so different clients might see messages in
 * different order
 */
public class PubSubService implements PubSubI {

	private final Logger log = Logger.getLogger(PubSubService.class);
	private final ConcurrentMap<String, List<Client>> subscriptions = new ConcurrentHashMap<String, List<Client>>();
	private final Object SKIP = new Object(); 
	private final TransformI transform;
	
	public PubSubService(TransformI transform) {
		this.transform = transform;
	}
	
	private static interface Client {
		void deliver(Map<String, ?> headers, Object body);
	}
	
	private static interface Selector {
		boolean isSelected(Map<String, ?> headers, Object body);
	}
	
	@Override
	public void send(String topic, Map<String, ?> headers, Object body) {
		List<Client> subscribers = subscriptions.get(topic);
		if (subscribers == null) return;
		log.trace("deliver " + topic + ":" + headers + " to " + subscribers.size() + " subscribers");
		for (Client client: subscribers) {
			client.deliver(headers, body);
		}
	}

	@Override
	public Closeable subscribe(final String topic, final String selector, final String expression, final Callback callback) {
		final Selector select = (selector == null || "".equals(selector)) ? null : createJMSSelector(selector);
		final Client client = new Client() {
			public void deliver(Map<String, ?> headers, Object body) {
				if (select == null || select.isSelected(headers, body)) {
					if (expression != null && transform != null) {
						body = transform.transform(body, expression, SKIP);
						if (body == SKIP) return;
					}					
					callback.onMessage(headers, body);
				}
			}
		};
		CopyOnWriteArrayList<Client> fresh = new CopyOnWriteArrayList<Client>();
		List<Client> clients = subscriptions.putIfAbsent(topic, fresh);
		if (clients == null) clients = fresh;
		clients.add(client);
		return new Closeable() {
			@Override
			public void close() throws IOException {
				subscriptions.get(topic).remove(client);
				subscriptions.remove(topic, Collections.EMPTY_LIST);
			}
		};
	}

	/**
	 *  Simple implementation that understands simple expressions of these form
	 *  
	 *  - a=b and c=d
	 *  - a=b or c=d
	 *  
	 *  This mimics a subset of jms selector - the subset used in NMS.
	 */
	private Selector createJMSSelector(final String selector) {
		return new Selector() {
			@Override
			public boolean isSelected(Map<String, ?> headers, Object body) {
				try {
				return evalOr(selector,headers);
				}catch(Exception t) {
					log.error(selector + " "+headers,t);
					return false;
				}
			}

			private boolean evalOr(String expr, Map<String, ?> headers) {
				for (final String sub : expr.split(" or ")){
					if (evalAnd(headers, sub)) return true;
				}
				return false;
			}

			private boolean evalAnd(Map<String, ?> headers, String expr) {
				for (final String and_ : expr.split(" and ")){
					final String[] lr = and_.split("=");
					Object object = headers.get(lr[0]);
					boolean accept = 
						object == null ? false: 
						object instanceof Number ? object.toString().equals(lr[1]) : 
						object instanceof String ? ("'" + object.toString() + "'").equals(lr[1]) : 
						false;
					if (!accept) return false;
				}
				return true;
			}
		};
	}

}
