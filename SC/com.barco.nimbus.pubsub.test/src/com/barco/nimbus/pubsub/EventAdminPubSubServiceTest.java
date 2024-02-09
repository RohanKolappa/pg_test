package com.barco.nimbus.pubsub;

import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import java.io.Closeable;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.component.ComponentContext;
import org.osgi.service.event.Event;
import org.osgi.service.event.EventAdmin;
import org.osgi.service.event.EventConstants;
import org.osgi.service.event.EventHandler;

import com.barco.nimbus.pubsub.api.PubSubI;
import com.barco.nimbus.pubsub.api.PubSubI.Callback;
import com.barco.nimbus.pubsub.api.support.PubSubService;
import com.barco.nimbus.pubsub.internal.EventAdminPubSubService;

public class EventAdminPubSubServiceTest {

	final EventAdminPubSubService eapss = new EventAdminPubSubService();

	@Before
	public void before() throws Exception {
		wire(eapss);
	}

	@Test
	public void testSendingOneSingleEvent() {
		String selector = null;
		String topic = "topic/test";
		String expression = null;
		final List collect = new ArrayList();
		Callback callback = new Callback() {
			@Override
			public void onMessage(Map<String, ? extends Object> headers, Object body) {
				collect.add(headers);
				collect.add(body);
			}
		};
		Closeable close = eapss.subscribe(topic, selector, expression, callback);
		try {
			Map<String, String> headers = Collections.singletonMap("1", "2");
			String body = "3";
			eapss.send(topic, headers, body);
			Iterator each = collect.iterator();
			Assert.assertEquals(headers, each.next());
			Assert.assertEquals(body, each.next());
		} finally {
			try {
				close.close();
			} catch (IOException e) {
			}
		}
	}

	/*
	 * mock away the EventAdmin dependency by having mockito delegate to our own
	 * backend impl. We also mock away the ComponentContext dependency by having
	 * mockito forward the EventHandler-service-registrations to our own
	 * backend.subscribe
	 */
	static void wire(EventAdminPubSubService instance) throws Exception {
		final PubSubService backend = new PubSubService(null);
		final ComponentContext cc = mock(ComponentContext.class);
		final Answer subscribe = new Answer() {
			@Override
			public Object answer(InvocationOnMock invocation) throws Throwable {
				final Dictionary<String, String> props = (Dictionary<String, String>) invocation.getArguments()[2];
				final EventHandler handler = (EventHandler) invocation.getArguments()[1];
				final String topic = props.get(EventConstants.EVENT_TOPIC);
				Closeable close = backend.subscribe(topic, null, null, new PubSubI.Callback() {
					@Override
					public void onMessage(Map<String, ? extends Object> headers, Object body) {
						handler.handleEvent(new Event(topic, headers));
					}
				});
				return mock(ServiceRegistration.class);
			}
		};
		final BundleContext bc = mock(BundleContext.class);
		final EventAdmin ea = mock(EventAdmin.class);
		final Answer send = new Answer() {
			public Object answer(InvocationOnMock invocation) throws Throwable {
				Event event = (Event) invocation.getArguments()[0];
				backend.send(event.getTopic(), asMap(event), null);
				return null;
			};

			private Map<String, ?> asMap(Event event) {
				Map<String, Object> out = new HashMap<String, Object>();
				for (String name : event.getPropertyNames()) {
					out.put(name, event.getProperty(name));
				}
				return out;
			}
		};
		// stub the OSGi
		{
			when(cc.getBundleContext()).thenReturn(bc);
			when(bc.registerService(eq(EventHandler.class), any(EventHandler.class), any(Dictionary.class))).then(subscribe);
			doAnswer(send).when(ea).sendEvent(any(Event.class));
		}
		// start up the OSGi component
		{
			instance.bindEventAdmin(ea);
			instance.start(cc);
		}
	}

}
