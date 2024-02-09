package com.barco.nimbus.displaymanagement.bms;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.atomic.AtomicReference;

import javax.naming.NamingException;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.EntityTransaction;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.compose.display.facade.DispletPropertyBindingMgrFacadeBean;
import com.barco.compose.display.facade.HintMgrFacadeBean;
import com.barco.compose.display.facade.LayoutMgrFacadeBean;
import com.barco.compose.display.service.ClientSynchUpdateHistoryBean;
import com.barco.compose.display.service.ClientSynchUpdateHistoryLocal;
import com.barco.compose.util.HTTPTunnelDispatcher;
import com.barco.compose.util.ReflectionUtil;

@Component
public class BMSHTTPTunnelService extends HTTPTunnelDispatcher implements BMSHTTPTunnelServiceI {

	final Logger log = Logger.getLogger(BMSHTTPTunnelService.class);
	final AtomicReference<EntityManagerFactory> emf = new AtomicReference<EntityManagerFactory>();
	final ThreadLocal<EntityManager> em = new ThreadLocal<EntityManager>();
	final AtomicReference<ClientSynchUpdateHistoryLocal> cs = new AtomicReference<ClientSynchUpdateHistoryLocal>();

	@Reference
	public void bindCS(ClientSynchUpdateHistoryLocal cs) {
		this.cs.set(cs);
	}
	
	public void unbindCS(ClientSynchUpdateHistoryLocal cs) {
		this.cs.compareAndSet(cs, null);
	}
	
	@Reference(target = "(osgi.unit.name=bmslayoutdb)")
	public void bind(EntityManagerFactory emf) {
		this.emf.set(emf);
	}

	public void unbind(EntityManagerFactory emf) {
		this.emf.compareAndSet(emf, null);
	}

	@Override
	protected Object getTarget(String name) throws NamingException {
		// name is like com.barco.bms.server/com.barco.compose.display.server/LayoutMgrFacadeBean!com.barco.compose.display.facade.remote.ILayoutMgr
		// com.barco.bms.server/com.barco.compose.display.server/LayoutMgrFacadeBean!com.barco.compose.display.facade.remote.ILayoutMgr
		String beanName = name.substring(name.lastIndexOf('/')+1, name.indexOf('!'));
		String interfaceName = name.substring(name.indexOf('!') + 1);
		log.info(beanName);
		if (LayoutMgrFacadeBean.class.getSimpleName().equals(beanName)) {
			LayoutMgrFacadeBean lmfb = new LayoutMgrFacadeBean();
			lmfb.manager = em.get();
			return lmfb;
		} else if (DispletPropertyBindingMgrFacadeBean.class.getSimpleName().equals(beanName)) {
			DispletPropertyBindingMgrFacadeBean b = new DispletPropertyBindingMgrFacadeBean();
			b.manager = em.get();
			return b;
		} else if (HintMgrFacadeBean.class.getSimpleName().equals(beanName)) {
			HintMgrFacadeBean h = new HintMgrFacadeBean();
			h.manager = em.get();
			return h;
		} else if (ClientSynchUpdateHistoryBean.class.getSimpleName().equals(beanName)){
			return cs.get();
		}
		throw new RuntimeException("no such service '" + name + "'");
	}

	@Override
	public Object EJBInvocation(String name, String method_, List<String> args, Object body) throws Exception {
		String beanName = name.substring(name.lastIndexOf('/')+1, name.indexOf('!'));
		if ("ConfigurationMgrFacadeBean".equals(beanName)) {
			return null;
		}
		em.set(emf.get().createEntityManager());
		Object out = null;
		try {
			EntityTransaction transaction = em.get().getTransaction();
			transaction.begin();
			boolean commit = false;
			try {
				out = super.invoke(name, method_, args, body);
				commit = true;
			} finally {
				if (commit) {
					transaction.commit();
				} else {
					try {
						transaction.rollback();
					} catch (Exception x) {
						log.debug("suppressed: tx.rollback fails",x);
					}
				}
			}
		} finally {
			em.remove();
		}
		Object x = unwrap(out);
		log.info(method_ + ": " + x);
		return x;
	}

	private static class StripJPAImplementation extends ReflectionUtil.DepthFirstObjectFieldVisitior {

		@SuppressWarnings("unchecked")
		protected void replace(Field field, Object source, Object value, Class type) throws IllegalAccessException {
			if (type.getName().startsWith("org.eclipse.persistence.indirection")) {
				if (/* Persistent */Set.class.isAssignableFrom(type)) {
					field.set(source, new HashSet((Collection) value));
				} else if (/* Persistent */List.class.isAssignableFrom(type)) {
					field.set(source, new ArrayList((Collection) value));
				}
			}
		}
	}

	public static Object unwrap(Object out) {
		AtomicReference<Object> w = new AtomicReference(out);
		new StripJPAImplementation().visit(w);
		return w.get();
	}
}
