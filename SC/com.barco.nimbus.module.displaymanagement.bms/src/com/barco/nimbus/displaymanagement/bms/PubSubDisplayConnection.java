package com.barco.nimbus.displaymanagement.bms;

import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;

import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.compose.configuration.service.grunit.DisplayConnectionFacade;
import com.barco.compose.configuration.service.grunit.UnitUpdate;
import com.barco.nimbus.pubsub.api.PubSubI;

@Component
public class PubSubDisplayConnection implements DisplayConnectionFacade {

	private final AtomicReference<PubSubI> pubsub = new AtomicReference<>();
	
	@Reference
	public void setPubSub(PubSubI i) {
		pubsub.set(i);
	}
	
	public void unsetPubSub(PubSubI i) {
		pubsub.compareAndSet(i, null);
	}

	@Override
	public void sendData(Collection<UnitUpdate> update, int session) {
		boolean incr = false;
		for (UnitUpdate uu : update) {
			incr = incr || uu.incremental;
		}
		Map<String, ? extends Object> headers = Collections.singletonMap("incremental", incr);
		pubsub.get().send("topic/session/" + session, headers, update);
	}

}
