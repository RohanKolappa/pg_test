package com.barco.nimbus.pubsub.internal;

import java.io.Closeable;
import java.io.IOException;
import java.util.AbstractMap;
import java.util.AbstractSet;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicReference;

import org.apache.log4j.Logger;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.component.ComponentContext;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.osgi.service.component.annotations.Reference;
import org.osgi.service.component.annotations.ReferenceCardinality;
import org.osgi.service.event.Event;
import org.osgi.service.event.EventAdmin;
import org.osgi.service.event.EventConstants;
import org.osgi.service.event.EventHandler;

import com.barco.nimbus.pubsub.api.PubSubI;
import com.barco.nimbus.pubsub.api.TransformI;

/**
 * Implementation of PubSubI using osgi EventAdmin.
 * 
 * The event body (in PubSubI speak) is put in the osgi Event dictionary
 * under the key "".
 * 
 * If a TransformI service is available, it will be used to evaluate the
 * expression, transforming the event object and filtering delivery.
 */
@Component(immediate=true)
public class EventAdminPubSubService implements PubSubI {

	final static Logger log = Logger.getLogger(EventAdminPubSubService.class);
	
	final AtomicReference<EventAdmin> ea = new AtomicReference<>(); 
	final AtomicReference<ComponentContext> cc = new AtomicReference<ComponentContext>(); 
	final List<TransformI> transform = new CopyOnWriteArrayList<>();
	final Object SKIP = new Object();
	final String BODY = "";

	@Activate
	public void start(ComponentContext context) throws Exception {
		this.cc.set(context);
	}
	
	@Deactivate
	public void stop(ComponentContext context) throws Exception {
		this.cc.compareAndSet(context,null);
	}
	
	@Reference
	public void bindEventAdmin(EventAdmin ea) {
		this.ea.set(ea);
	}
	
	public void unbindEventAdmin(EventAdmin ea) {
		this.ea.compareAndSet(ea, null);
	}
	
	@Reference(cardinality=ReferenceCardinality.MULTIPLE)
	public void bindTransform(TransformI ea) {
		this.transform.add(ea);
	}
	
	public void unbindTransform(TransformI ea) {
		this.transform.remove(ea);
	}
	
	@Override
	public void send(String topic, Map<String, ?> headers, Object body) {
		EventAdmin ea = this.ea.get();
		if (ea != null) {
			Map<String,Object> copy = new HashMap<String, Object>(headers == null ? Collections.<String,String>emptyMap() : headers);
			copy.put(BODY, body);
			log.info(topic + ":" + copy);
			ea.sendEvent(new Event(topic,Collections.unmodifiableMap(copy)));
		} else {
			throw new RuntimeException("can not deliver event: no EventAdmin service!");
		}
	}

	@Override
	public Closeable subscribe(final String topic, String selector, final String expression, final Callback callback) {
		final EventHandler service = new EventHandler() {
			@Override
			public void handleEvent(final Event event) {
				Object body = event.getProperty(BODY);
				TransformI transform = findTransform(topic);
				if (expression != null && transform != null) {
					body = transform.transform(body, expression, SKIP);
				}
				if (body == SKIP) return;
				final List<String> names2 = Arrays.asList(event.getPropertyNames());
				final List<String> names = new ArrayList<String>(names2);
				names.removeAll(Arrays.asList(EventConstants.EVENT_TOPIC,BODY));
				callback.onMessage(new AbstractMap<String,Object>(){
					@Override
					public Set<Map.Entry<String,Object>> entrySet() {
						return new AbstractSet<Map.Entry<String,Object>>(){
							@Override
							public Iterator<Map.Entry<String,Object>> iterator() {
								return new Iterator<Map.Entry<String,Object>>() {
									Iterator<String> delegate = names.iterator();
											
									@Override
									public boolean hasNext() {
										return delegate.hasNext();
									}

									@Override
									public Map.Entry<String, Object> next() {
										final String key = delegate.next();
										return new Map.Entry<String,Object>() {

											@Override
											public String getKey() {
												return key;
											}

											@Override
											public Object getValue() {
												return BODY.equals(key) ? null : event.getProperty(key);
											}

											@Override
											public Object setValue(Object arg0) {
												throw new RuntimeException();
											}};
									}

									@Override
									public void remove() {
										throw new RuntimeException();
									}
								};
							}

							@Override
							public int size() {
								return names.size();
							}};
					}}, body);
			}
		};
		Dictionary<String, String> properties = new Hashtable<>();
		properties.put(EventConstants.EVENT_TOPIC, topic);
		if (selector != null && !"".equals(selector)) {
			properties.put(EventConstants.EVENT_FILTER, selector);
		}
		ComponentContext context = cc.get();
		if (context == null) throw new RuntimeException();
		final ServiceRegistration<EventHandler> registration = context.getBundleContext().registerService(EventHandler.class, service, properties);
		return new Closeable() {
			@Override
			public void close() throws IOException {
				registration.unregister();
			}
		};
	}

	private TransformI findTransform(String topic) {
		// TODO pick the Transforn instance that fits the topic;
		// typically legacy NMS-topics will use the NMS transform,
		// where "ipvs" topics will use xpath.
		return transform.isEmpty() ? null : transform.get(0);
	}

}
