package com.barco.nimbus.pubsub.api.support;


import java.io.Closeable;
import java.net.URI;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.ResponseBuilder;
import javax.ws.rs.core.Response.Status;

import org.apache.log4j.Logger;

import com.barco.nimbus.pubsub.api.PubSubHTTPI;
import com.barco.nimbus.pubsub.api.PubSubI;

/**
 * Implementation was copied and adapted from NMS 1.x.
 * It is meant to work over JAX-RS.
 */
public abstract class PubSubHTTPService implements PubSubHTTPI {

	private static final Logger log2 = Logger.getLogger(PubSubHTTPService.class);
	private final Consumers state = new Consumers();
	private final int minTimeoutMs = Integer.valueOf(System.getProperty("com.barco.compose.apibean.JMSHTTPBridge.buffer.ms","2000"));
	private static final boolean ALLOW_EVENTING_WITHOUT_SUBSCRIPTION = Boolean.valueOf(System.getProperty("com.barco.compose.media.netviz.driver.NetvizDongleFunctionalDriver.eventing.without.subscription","false"));
	private final AtomicLong idgen = new AtomicLong(System.currentTimeMillis());
	
	private final PubSubI pubsub;
	
	public PubSubHTTPService(PubSubI pubsub) {
		this.pubsub = pubsub;
	}
	
	/**
	 * @param timeoutMs
	 *            the client promises to come back this fast, after having
	 *            received a message. If it does not, the jms-session is
	 *            discarded, and when coming back later, you might have missed
	 *            messages. 
	 */
	public Response getNextMessage(
			final String session,
			final int timeoutMs,
			final List<String> destination,
			final List<String> selector,
			MediaType accept,
			final List<String> expression,
			final List<String> name,
			int suspendSec,
			HttpServletRequest request
			) {
		if (suspendSec == 0) suspendSec = Integer.valueOf(System.getProperty("com.barco.nimbus.pubsub.api.suspend.sec","20"));
		try {
			int timeoutMs2 = Math.max(timeoutMs, minTimeoutMs); // have at least x seconds servers-side buffer
			if (null == accept || "".equals("accept")) accept=new MediaType("application","json");
			return state.getOrCreateConsumer(pubsub, session, destination, timeoutMs2, selector, accept, expression, request, name, suspendSec);
		} catch (Throwable x) {
			log2.error(x,x);
			return Response.serverError().entity(x).build();
		}
	}
	
	static class Consumers {
		
		final ExecutorService exe = Executors.newCachedThreadPool();
		final Map<Object,Client> consumers = new HashMap<Object,Client>();
		
		public Response getOrCreateConsumer(PubSubI pubsub, String session, List<String> destination, int timeoutMs, List<String> selector, MediaType accept, List<String> expression, HttpServletRequest request, List<String> name, final int suspendSec) {
			selector = "".equals(selector) ? null : selector;
			Client consumer = null;
			synchronized (consumers) {
				
				if (!consumers.keySet().contains(session)) {
					if (ALLOW_EVENTING_WITHOUT_SUBSCRIPTION) {
						log2.warn("Auto-creating subscription for '"+session +"'; erronous usage: use POST to subscribe to event/name");
					} else {
						Response.status(Status.PRECONDITION_FAILED).entity("No such session '" + session +"'");
					}
				}
				consumer = consumers.get(session);
				if (consumer == null 
						|| consumer.disposed 
						|| !eq(consumer.destination,destination) 
						|| !eq(consumer.selector,selector) 
						|| !eq(consumer.expression,expression) 
						|| !consumer.accept.equals(accept)) {
					if (consumer != null && !consumer.disposed) {
						consumer.dispose();
					}
					consumer = createConsumer(pubsub, destination, timeoutMs, selector, accept, expression, name, suspendSec);
					consumers.put(session, consumer);
					// for a new consumer, answer with 201 immediately
					return Response.created(URI.create(request.getRequestURI())).build();
				} 
			}
			return consumer.getNextMessageOrTimeout(suspendSec);
		}
		
		public synchronized void remove(Client c) {
			consumers.values().remove(c);
		}
	
		private Client createConsumer(PubSubI pubsub, List<String> destination, int timeoutMs, List<String> selector, MediaType accept, List<String> expression, List<String> name, int suspendSec) {
			Client out = new Client(pubsub, destination, timeoutMs, this, selector, accept, expression, name, suspendSec);
			for (Runnable run : out.consumer) {
				exe.submit(run);
			}
			return out;
		}
		
		public synchronized void dispose() {
			for (Client client : consumers.values()) {
				client.dispose();
			}
			consumers.clear();
		}

		public synchronized void disposed(Client client) {
			consumers.remove(client);
		}
	}
	
	static class Client {
		
		private final Logger log;
		
		private final Runnable[] consumer;
		private final BlockingQueue<Object[]> out = new SynchronousQueue<Object[]>(); 
		private final List<String> destination;
		private final List<String> expression;
		private final List<String> selector;
		private final MediaType accept;
		private final AtomicLong ping = new AtomicLong();
		
		private int delivered = 0;
		private volatile boolean disposed;

		
		public Client(PubSubI pubsub, List<String> destination, final int timeoutMs, final Consumers consumers, final List<String> selector, MediaType accept, final List<String> expression, List<String> name, int suspendSec) {
			this.accept = accept;
			this.expression = expression;
			this.selector = selector;
			this.destination = destination;
			this.consumer = new Runnable[destination.size()];
			this.log = Logger.getLogger(Client.class.getName()+"."+toString());
			for (int i = 0; i < destination.size(); i++) {
				String selector2 = get( selector,i);
				String expression2 = get(expression,i);
				String name2 = get(name,i);
				this.consumer[i] = createConsumer(pubsub, destination.get(i), timeoutMs, consumers, selector2, expression2, name2, suspendSec);
			}
		}

		private String get(List<String> array, int i) {
			return array.size() > i ? array.get(i) : null;
		}

		private Runnable createConsumer(final PubSubI pubsub, final String destination2, final int timeoutMs, final Consumers consumers, final String selector, final String expression, final String name, final int suspendSec) {
			final SynchronousQueue<Object[]> queue = new SynchronousQueue<Object[]>();
			PubSubI.Callback callback = new PubSubI.Callback() {
				@Override
				public void onMessage(Map<String, ?> headers, Object body) {
					queue.offer(new Object[]{headers,body});
				}
			};
			final Closeable session = pubsub.subscribe(destination2, selector, expression, callback);
			return new Runnable() {
				public void run() {
					try {
						log.trace("starting");
						run2();
					} catch (Throwable c) {
						if (!disposed) log.error("ignored: can not deliver jms message to http client",c);
					} finally {
						// mark as unusable
						dispose();
						log.trace("stopping");
						consumers.disposed(Client.this);													
					}
				}
				private void run2() throws Exception {
					try {
						while (true) {
							Object[] msg = queue.poll(suspendSec,TimeUnit.SECONDS);
							log.trace((msg==null?"no data" : "data") + " from " + destination);
							if (disposed) {
								log.trace("wont deliver msg because i am disposed");
								break;
							}
							Map<String,String> headers = (Map<String, String>) (msg == null ? null : msg[0]);
							Object msgv = msg == null ? null : msg[1];
							if (headers != null) {
								// if multiple topics are watched, add a discriminator
								if (name != null) {
									headers = new HashMap(headers);
									headers.put("name",name);
								}
								log.trace("delivering into out queue");
								boolean ok = out.offer(new Object[]{msgv, headers},timeoutMs,TimeUnit.MILLISECONDS);
								if (!ok) {
									log.debug("dropping message: client did not connect in time " + timeoutMs);
									break;
								}
							}
							if (msg == null) {
								// if peer did not come back, shut me
								// http://itrack.barco.com/browse/TSM0001-679 = "+ timeoutMs"
								if (ping.get() < (System.currentTimeMillis() - ((suspendSec * 1000) + timeoutMs))) {
									log.debug("dropping consumer: client did not reconnect im time " + timeoutMs);
									break;
								}
							}
						}
					} finally {
						try { session.close(); } catch(Exception x) {}
					}
				}
			};
		}
		
		Response getNextMessageOrTimeout(int suspendSec) {
			try {
				ping.set(System.currentTimeMillis());
				final Object[] poll = out.poll(suspendSec, TimeUnit.SECONDS);
				log.trace("delivering " + (poll == null ? "no data" : "data"));
				Object msg = poll == null ? null : poll[0];
				Map<String, String> headers = (Map<String, String>) (poll == null ? null : poll[1]);
				ResponseBuilder jaxrs = headers == null ? Response.noContent() : Response.ok(msg).type(accept);						
				if (poll != null) {
					log.trace("delivering #" + delivered);
					log.trace("delivering: " + msg + " " + headers);
					for (String key : headers.keySet()) {
						jaxrs.header("X-bms-" + key, headers.get(key));
					}
					delivered++;
				}
				Response resp = jaxrs.build();
				return resp;
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
				return null;
			} catch (RuntimeException t) {
				log.error(t,t);
				throw t;
			}
		}

		private void dispose() {
			disposed = true;
			log.trace("dispose");
		}
		
		@Override
		public String toString() {
			return (destination) +(selector==null?"":("["+(selector)+"]"))+(expression==null?"":("."+(expression)))+"("+accept+")";
		}
	}

	@PostConstruct
	public void start() {
	}

	@PreDestroy
	public void stop() {
		state.dispose();
	}

	public String getStatus() {
		return state.consumers.toString();
	}
	
	@Override
	public String subscribe() {
		String session = "" + idgen.incrementAndGet();
		synchronized (state.consumers) {
			state.consumers.put(session, null);
		}
		return session;
	}
	
	public static boolean eq(List<String> a, List<String> b) {
		return a == b || (a != null && b != null && a.equals(b));
	}
	
	@Override
	public void send(String destination, List<String> headers, Object body) {
		pubsub.send(destination, asMap3(headers), body);
	}
	
	private static Map<String,Object> asMap3(Iterable<String> keyvalues) {
		Map<String,Object> out = new HashMap<String,Object>();
		Iterator<String> i = keyvalues.iterator();
		while (i.hasNext()) {
			out.put(i.next(), i.next());
		}
		return out;
	}

}
