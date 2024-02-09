package com.barco.nimbus.displaymanagement;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;

import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.nimbus.module.api.RegistryServiceI;
import com.barco.nimbus.pubsub.api.PubSubI;

/**
 * A service that remembers a Collection<List<String>>
 * and events about it.
 */
@Component
public class RegistryService implements RegistryServiceI {

	private final Map<List<String>,List<String>> state = new HashMap<>();
	private final AtomicReference<PubSubI> pubsub = new AtomicReference<>();
	
	@Reference
	public void bind(PubSubI p) {
		pubsub.set(p);
	}
	
	public void unbind(PubSubI p) {
		pubsub.compareAndSet(p, null);
	}

	@Override
	public void set(Iterable<List<String>> arg) {
		for (List<String> each : arg) {
			state.put(each.subList(0, each.size()-1),each);
		}
		pubsub.get().send("topic/registry",null,arg);
	}

	@Override
	public String get(List<String> arg) {
		List<String> item = state.get(arg);
		return item == null ? null : item.get(item.size()-1);
	}

	@Override
	public Iterable<List<String>> get(String filter) {
		throw new RuntimeException(); // to be implemented
	}
}
