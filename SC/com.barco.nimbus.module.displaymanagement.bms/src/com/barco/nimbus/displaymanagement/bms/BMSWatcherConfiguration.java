package com.barco.nimbus.displaymanagement.bms;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.Closeable;
import java.io.IOException;
import java.net.URI;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.Compose;
import com.barco.compose.configuration.service.grunit.watch.Watcher;
import com.barco.compose.configuration.service.grunit.watch.WatcherFactory;
import com.barco.compose.configuration.service.grunit.watch.WatchersConfiguration;
import com.barco.compose.display.bean.Displet;
import com.barco.compose.media.bean.LogicalSource;
import com.barco.compose.media.bean.Source;
import com.barco.compose.media.bean.Stream;
import com.barco.compose.util.MapUtil;
import com.barco.nimbus.module.api.RegistryServiceI;
import com.barco.nimbus.pubsub.api.PubSubI;

@Component
public class BMSWatcherConfiguration implements WatchersConfiguration {
	
	final Logger log = Logger.getLogger(BMSWatcherConfiguration.class);

	final AtomicReference<RegistryServiceI> reg = new AtomicReference<>();
	final AtomicReference<PubSubI> pubsub = new AtomicReference<>();
	final WatcherFactory registry = new WatcherFactory() {
		
		@Override
		public Watcher create(final PropertyChangeListener output, final String property, String expression, final Displet displet, Source source) {
			final List<String> key = Arrays.asList(expression.substring(1, expression.length()-1).split("/")); 
			final Closeable subscribe = pubsub.get().subscribe("topic/registry", null, null, new PubSubI.Callback() {
				String prev;
				@Override
				public void onMessage(Map<String, ? extends Object> headers, Object body) {
					Iterable<List<String>> update = (Iterable<List<String>>) body;
					for (List<String> line : update) {
						if (line.subList(0, line.size() - 1).equals(key)) {
							String value = line.get(line.size() - 1);
							boolean changed = value == null ? prev != null : !value.equals(prev);
							if (changed) {
								output.propertyChange(new PropertyChangeEvent(displet,property,prev,value));							
								prev = value;
							}
						}
					}
				}
			});
			RegistryWatcher watcher = new RegistryWatcher(reg.get(),key) {
				public void dispose() {
					try {
						subscribe.close();
					} catch (IOException e) {
						throw new RuntimeException();
					}
				};
			};
			return watcher;
		}
	};
	final WatcherFactory constant = new WatcherFactory() {
		
		@Override
		public Watcher create(PropertyChangeListener output, String property, final String expression, Displet displet, Source source) {
			return new Watcher() {

				@Override
				public Object evaluate() throws Exception {
					return expression;
				}

				@Override
				public void dispose() {
				}
			};
		}
	};
	final WatcherFactory source = new WatcherFactory() {
		
		private LogicalSource asSource(String name) {
			LogicalSource source = new LogicalSource(name);
			Stream stream = new Stream(Compose.APPLICATION_X_BARCO_SDP_IN_SAP,URI.create("sap://224.2.127.254:9875/?s="+name));// TODO org.jboss.resteasy.util.Encode.encodeQueryParam(name)
			source.addChild(stream);
			return source;
		}
		
		@Override
		public Watcher create(final PropertyChangeListener output, final String property, String expression, Displet displet, Source source) {
			final PropertyChangeListener output2 = new PropertyChangeListener() {
				
				@Override
				public void propertyChange(PropertyChangeEvent arg0) {
					output.propertyChange(new PropertyChangeEvent(arg0.getSource(), property, null, asSource((String)arg0.getNewValue())));
				}
			};
			final Watcher reg = registry.create(output2, "." + property, expression, displet, source);
			return new Watcher() {

				@Override
				public Object evaluate() throws Exception {
					String name = (String) reg.evaluate();
					log.info("presenting " + name);
					return asSource(name);
				}

				@Override
				public void dispose() {
					reg.dispose();
				}
				
			};
		}
	};
	
	@Reference
	public void bindRegistry(RegistryServiceI i) {
		reg.set(i);
	}

	public void unbindRegistry(RegistryServiceI i) {
		reg.compareAndSet(i, null);
	}

	@Reference
	public void bind(PubSubI pubsub) {
		this.pubsub.set(pubsub);
	}
	
	public void unbind(PubSubI pubsub) {
		this.pubsub.compareAndSet(pubsub,null);
	}
	
	@Override
	public Map<String, ? extends WatcherFactory> createFactories() {
		return (Map)MapUtil.map("registry",registry,"source",source,"constant",constant);
	}

	@Override
	public Map<String, String> createAliases() {
		return MapUtil.map("registry","registry://{0}",
				"source.dior","source://{`region.name`/`name`/src}",
				"onclick.dior","constant://putRegistry('`region.name`/`name`/src')",
				"dior.umd.content","constant://`source.name`",
				"dior.umd.visible","constant://true",
				"dior.umd.color","constant://white");
	}

}

class RegistryWatcher implements Watcher {

	final RegistryServiceI registry;
	final List<String> key;

	public RegistryWatcher(RegistryServiceI reg, List<String> key) {
		this.registry = reg;
		this.key = key;
	}

	@Override
	public Object evaluate() throws Exception {
		return registry.get(key);
	}

	@Override
	public void dispose() {
	}
}