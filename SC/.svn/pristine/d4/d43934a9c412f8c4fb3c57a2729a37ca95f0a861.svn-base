package com.barco.nimbus.pubsub.api;

import java.io.Closeable;
import java.util.Map;

/**
 * General purpose pub-sub.
 */
public interface PubSubI {
	/**
	 * Synchronously deliver event to all parties listening currently.
	 * 
	 * @param topic
	 * @param headers
	 * @param body
	 */
	void send(String topic, Map<String, ? extends Object> headers, Object body);

	/**
	 * Set up a listener for a particular topic.
	 * 
	 * @param topic
	 *            any logical name (matches up senders and consumers)
	 * @param selector
	 *            see osgi EventAdmin filter
	 * @param expression
	 *            an additional expression operating on the event body
	 * @param callback
	 *            this function will be called when an event is delivered
	 * @return to stop the subscription
	 */
	Closeable subscribe(String topic, String selector, String expression, Callback callback);

	/**
	 * The callback to be implemented, in order to consume PubSubI events.
	 */
	public static interface Callback {
		void onMessage(Map<String, ? extends Object> headers, Object body);
	}
}
