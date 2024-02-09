package com.barco.nimbus.displaymanagement.bms;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.net.URI;
import java.rmi.MarshalledObject;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;

import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.QueryParam;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.osgi.service.component.annotations.Reference;

import com.barco.compose.base.geometry.Bounds;
import com.barco.compose.base.license.LicenseService;
import com.barco.compose.configuration.service.grunit.DisplayConnectionFacade;
import com.barco.compose.configuration.service.grunit.DisplaySessionMgr;
import com.barco.compose.configuration.service.grunit.DisplaySessionMgrFacadeBean;
import com.barco.compose.configuration.service.grunit.Unit;
import com.barco.compose.configuration.service.grunit.UnitUpdate;
import com.barco.compose.configuration.service.grunit.spi.TXContext;
import com.barco.compose.configuration.service.grunit.watch.WatchersConfiguration;
import com.barco.compose.display.bean.util.AdaptationStrategy;
import com.barco.compose.display.facade.DispletPropertyBindingMgrFacadeBean;
import com.barco.compose.display.facade.HintMgrFacadeBean;
import com.barco.compose.display.facade.LayoutMgrFacadeBean;
import com.barco.compose.display.service.ClientSynchUpdateHistoryLocal;
import com.barco.compose.media.bean.Source;
import com.barco.compose.util.BeanUtils;
import com.barco.compose.util.ReflectionUtil;
import com.barco.compose.util.jms.JmsProducer;
import com.barco.compose.util.synch.UpdateHandler;
import com.barco.compose.util.synch.jms.JmsCommunicationBuilder;
import com.barco.compose.util.synch.named.NamedUpdate;
import com.barco.nimbus.module.api.Display;
import com.barco.nimbus.module.api.DisplayServiceI;
import com.barco.nimbus.module.api.LayoutServiceI;
import com.barco.nimbus.module.api.LayoutSessionI;
import com.barco.nimbus.pubsub.api.PubSubI;
import com.barco.nimbus.pubsub.api.PubSubI.Callback;

/**
 * The code in this package will move into a separate bundle. That bundle
 * depends on the com.barco.compose bundles in neo.barco.com/svn (the NMS
 * codebase) that contains the BMS layout management.
 * 
 * This implementation bridges to the BMS DisplaySessionMgr, a layouting
 * mechanism that uses a displet tree and registry-backed databinding, to
 * translate a hierarchical displet tree into a flat list of units (unit =
 * elementary rendering widget).
 * 
 * The list of units, and the incremental changes, is published into pubsub, and
 * over other channels (jgroups) using different formats (jgroups:
 * application/x-java-serialized-object; http-pub-sub-bridge: json).
 * 
 * A html-javascript app called "content2.html" implements the rendering in a
 * browser context using http-long-polling (contentType json) to subscribe to
 * pubsub, and HTTP GET to get state (contentType json).
 * 
 * BMS legacy ngp-display-agent connects over jgroups to get the units (state)
 * and updates (contentType application/x-java-serialized-object), and renders
 * in an openGL context.
 * 
 * NMS DiagnosticTool 'preview' connects over http-long-polling to subscribe to
 * pubsub (using contentType application/x-java-serialized-object, HTTP get to
 * fetch the state (contentType application/x-java-serialized-object), and
 * renders in an openGL context.
 */
@Component
public class BMSLayoutService implements LayoutServiceI<String, BMSLayoutService.Context, Map<?, ?>>, BMSSessionServiceI {

	private static final Logger log = Logger.getLogger(BMSLayoutService.class);
	
	public static final String jgroupsConfigPath = "/nimbus/BMSLayoutService/jgroups-config?session=_";
	public static final String contentHtmlPath = "/barco-webservice/content2.html?session=_";
	public static final String unitHTTPPath = "/barco-webservice/rest/NetworkWall/session/_";
	public static final String unitPubSubHTTPPath = "/barco-webservice/rest/NetworkWall/event?name=session3&selector-session=_";
	public static final String APPLICATION_X_JAVA_SERIALIZED_OBJECT = "application/x-java-serialized-object";
	
	private final static ClassLoader cl = BMSLayoutService.class.getClassLoader();

	private final AtomicReference<ClientSynchUpdateHistoryLocal<String>> cshl = new AtomicReference<>();
	private final AtomicReference<DisplayConnectionFacade> dcf = new AtomicReference<>();
	private final AtomicReference<DisplayServiceI> ds = new AtomicReference<>();
	private final AtomicReference<EntityManagerFactory> emf = new AtomicReference<>();
	private final AtomicReference<WatchersConfiguration> watchers = new AtomicReference<WatchersConfiguration>();
	private final AtomicReference<PubSubI> ps = new AtomicReference<PubSubI>();
	
	private final Map<Integer,Session> sessions = new HashMap<>();
	
	private final DisplaySessionMgrFacadeBean dsm = new DisplaySessionMgrFacadeBean() {
		@Override
		protected <E> E clone2(E in) {
			BMSHTTPTunnelService.unwrap(in);
			try {
				return (E) new MarshalledObject(in).get();
			} catch (IOException e) {
				throw new RuntimeException(e);
			} catch (ClassNotFoundException e) {
				throw new RuntimeException(e);
			}
		};
	};
	private final ThreadLocal<EntityManager> tlem = new ThreadLocal<>();
	private final EntityManager em = createThreadLocalEntityManager(tlem);
	private final DisplaySessionMgr dsm2 = createDSMWithEMInjected(dsm,emf,tlem);

	private final AtomicInteger idgen = new AtomicInteger();

	private Closeable subscription;
	
	@Activate
	public void start() {
		dsm.cshb = cshl.get();
		dsm.displayConnectionService2 = dcf.get();
		dsm.displayMgr = new com.barco.compose.configuration.service.grunit.spi.DisplayMgrFacade() {
			@Override
			public com.barco.compose.media.bean.Display findDisplay(int id) {
				Session session = sessions.get(id);
				com.barco.compose.media.bean.Display display = new com.barco.compose.media.bean.Display();
				display.setBounds(new Bounds(0,0,session.width,session.height));
				display.setName(session.name);
				return display;
			}
		};
		dsm.ejbcontext = ReflectionUtil.createProxy(TXContext.class);
		dsm.error = ReflectionUtil.createProxy(JmsProducer.class);
		dsm.producerT = ReflectionUtil.createProxy(JmsProducer.class);
		dsm.watcherConfig = watchers.get();
		dsm.sourceMgr = new com.barco.compose.configuration.service.grunit.spi.SourceMgrFacade() {
			
			@Override
			public void resolveLazyProxies(Iterable<Source> each) {
			}
			
			@Override
			public void preloadSourceArtifacts() {
			}
			
			@Override
			public Source findSource(String name) {
				throw new RuntimeException();
			}
			
			@Override
			public Source findSource(int id) {
				throw new RuntimeException();
			}
			
			@Override
			public Collection<Source> findLogicalSourcesContaining(Collection<Integer> ids) {
				return Collections.emptyList();
			}
		};
		LayoutMgrFacadeBean lmfb = new LayoutMgrFacadeBean();
		lmfb.manager = em;
		dsm.layoutMgr = lmfb; 
		DispletPropertyBindingMgrFacadeBean pb = new DispletPropertyBindingMgrFacadeBean();
		pb.manager = em;
		dsm.bindingMgr = pb; 
		HintMgrFacadeBean hm = new HintMgrFacadeBean();
		hm.manager = em;
		dsm.hintMgr = hm; 
		dsm.licenseService = ReflectionUtil.createProxy(LicenseService.class); 
		dsm.interactive = true; 
		dsm.start();
		Callback callback = new Callback() {
			@Override
			public void onMessage(Map<String, ? extends Object> headers, Object body) {
				UpdateHandler q = dsm.getInteractiveCommandHandlers();
				JmsCommunicationBuilder.consume(headers, body, q);
			}
		};
		subscription = ps.get().subscribe("topic/clientsynch", null, null, callback);
	}
	
	@Deactivate
	public void stop() throws IOException {
		subscription.close();
	}
	
	private static final DisplaySessionMgr createDSMWithEMInjected(final DisplaySessionMgrFacadeBean dsm, final AtomicReference<EntityManagerFactory> emf, final ThreadLocal<EntityManager> tlem) {
		return (DisplaySessionMgr) Proxy.newProxyInstance(cl, new Class[]{DisplaySessionMgr.class}, new InvocationHandler() {
			@Override
			public Object invoke(Object arg0, Method arg1, Object[] arg2) throws Throwable {
				EntityManager em = emf.get().createEntityManager();
				tlem.set(em);
				try {
					return arg1.invoke(dsm, arg2);
				} finally {
					tlem.set(null);
				}
			}
		});
	}
	
	private final static EntityManager createThreadLocalEntityManager(final ThreadLocal<EntityManager> tlem) {
		return (EntityManager) Proxy.newProxyInstance(cl, new Class[]{EntityManager.class}, new InvocationHandler() {
			@Override
			public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
				return method.invoke(tlem.get(),args);
			}
		});
	}

	@Reference
	public void setWatchersConfiguration(WatchersConfiguration i) {
		watchers.set(i);
	}
	
	public void unsetWatchersConfiguration(WatchersConfiguration i) {
		watchers.compareAndSet(i, null);
	}
	
	@Reference
	public void setDisplayService(DisplayServiceI i) {
		ds.set(i);
	}
	
	public void unsetDisplayService(DisplayServiceI i) {
		ds.compareAndSet(i, null);
	}
	
	@Reference
	public void setDisplayConnection(DisplayConnectionFacade i) {
		dcf.set(i);
	}
	
	public void unsetDisplayConnection(DisplayConnectionFacade i) {
		dcf.compareAndSet(i, null);
	}
	
	@Reference
	public void setCommandStack(ClientSynchUpdateHistoryLocal<String> i) {
		cshl.set(i);
	}
	
	public void unsetCommandStack(ClientSynchUpdateHistoryLocal<String> i) {
		cshl.compareAndSet(i, null);
	}
	
	@Reference(target="(osgi.unit.name=bmslayoutdb)")
	public void setEntityManagerFactory(EntityManagerFactory emf) {
		this.emf.set(emf);
	}
	
	public void unsetEntityManagerFactory(EntityManagerFactory emf) {
		this.emf.compareAndSet(emf, null);
	}
	
	@Reference
	public void setPubSub(PubSubI ps) {
		this.ps.set(ps);
	}

	public void unsetPubSub(PubSubI ps) {
		this.ps.compareAndSet(ps, null);
	}
	
	@Override
	public LayoutSessionI<String, Context> createSession(Map<?, ?> context) {
		int width = (Integer) context.get("width");
		int height = (Integer) context.get("height");
		String name = (String) context.get("name");
		int id = idgen.incrementAndGet();
		Session session = new Session(this, emf.get(), id, width, height, name);
		sessions.put(id,session);
		return session;
	}

	@Override
	public void dispose(LayoutSessionI<String, Context> session) {
		sessions.values().remove(session);
		dsm2.setDisplayToLayout(((Session)session).id, DisplaySessionMgrFacadeBean.EMPTY_LAYOUT.getId(), AdaptationStrategy.DEFAULT);
	}

	@Override
	public Iterable<? extends LayoutSessionI<String, Context>> getSessions() {
		return sessions.values();
	}

	@Override
	public List<String> getLayouts() {
		return new ArrayList<String>(BeanUtils.<String>filter(dsm.layoutMgr.getLayoutReferences(),"name"));
	}

	public static class Context {
		/*
		 * An NMS layout has a certain size, the session has a certain size. The
		 * layout most be stretched to fill the session space. This stretching
		 * is configurable (with or without preservation of aspect ratio, don't
		 * stretch but center, or only shrink, never grow ... Things like that
		 * will be in the context.
		 */
	}

	private static class Session implements LayoutSessionI<String, Context> {

		private final int id;
		private final int width, height;
		private final String name;
		private final BMSLayoutService service;
		private final EntityManagerFactory emf;

		public Session(BMSLayoutService dsm, EntityManagerFactory emf, int id, int width, int height, String name) {
			this.service = dsm;
			this.emf = emf;
			this.id = id;
			this.width = width;
			this.height = height;
			this.name = name;
		}

		@Override
		public void setLayout(String layer, String layoutname, Context context) {
			LayoutMgrFacadeBean lmfb = new LayoutMgrFacadeBean();
			lmfb.manager = emf.createEntityManager();
			int layoutId = lmfb.findLayout(layoutname).getId();
			service.dsm2.setDisplayToLayout(id, layoutId, AdaptationStrategy.DEFAULT);
		}

		@Override
		public List<URI> createSubscriptionURI(Display display, com.barco.nimbus.module.api.LayoutSessionI.PeerProperties props) {

			/*
			 * The viewport x,y,w,h,u,v is communicated as a query-param in the
			 * first URI that is returned. The receiver must filter the
			 * rendering-instructions to retain only what intersects with its
			 * x,y,w,h and drop stuff outside its viewport. The u,v scaling
			 * factors then need to be applied in order to stretch the retained
			 * w*h area to fill the intended area on the local screen.
			 */
			double u = (double) width / (double) display.width;
			double v = (double) height / (double) display.height;
			String viewport = props.x * u + "," + props.y * v + "," + props.w * u + "," + props.h * v + "," + props.u / u + "," + props.v / v;

			if (props.accept.contains("text/html")) {
				return Arrays.asList(URI.create(contentHtmlPath.replace("_", "" + id)));
			} else if (props.accept.contains(APPLICATION_X_JAVA_SERIALIZED_OBJECT) && props.accept.contains("jgroups")) {
				String query = "&initial_hosts=" + createInitialHosts(display) + "&viewport=" + viewport;
				return Arrays.asList(URI.create(jgroupsConfigPath.replace("_", "" + id + query)));
			} else if (props.accept.contains(APPLICATION_X_JAVA_SERIALIZED_OBJECT)) {
				return Arrays.asList(URI.create(unitHTTPPath.replace("_", "" + id) + "&viewport=" + viewport),
						URI.create(unitPubSubHTTPPath.replace("_", "" + id)));
			} else {
				throw new RuntimeException();
			}
		}

		private String createInitialHosts(Display display) {
			// TODO list the ip's of each peer in this display + the ip of the
			// server
			return System.getProperty("jboss.bind.address");
		}
	}

	@GET
	@Path(jgroupsConfigPath)
	public String getJGroupsConfig(@QueryParam("session") int id, @QueryParam("initial_hosts") String initial_hosts, @QueryParam("viewport") String viewport) {
		InputStream is = getClass().getResourceAsStream("tcp-jgroups.xml");
		try (Scanner scanner = new Scanner(is, "UTF-8")) {
			return scanner.next().replaceAll("{port}", "" + (7800 + (id % 1000))).replaceAll("{initial_hosts}", initial_hosts);
		}
	}

	@Override
	public Iterable<?> getDisplayContents(int display, String token) {
		List<UnitUpdate> out = new ArrayList();
		Iterable<String> layers = dsm.getLayers(display);
		for (String layer : layers) {
			UnitUpdate uu = dsm.renderDisplaySession(display, layer);
			Iterator<Unit> u = uu.units.iterator();
			while (u.hasNext()) {
				if (u.next().type==Unit.Type.LAYOUT) {
					u.remove();
				}
			}
			out.add(uu);
		}
		// if no such session, do render empty: otherwise the demonstrator.html does not work on first layout switch
		return out;
	}
}

