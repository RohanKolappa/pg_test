package com.barco.nimbus.displaymanagement.bms;

import java.io.Closeable;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.osgi.service.component.annotations.Reference;

import com.barco.compose.display.service.ClientSynchUpdateHistoryBean;
import com.barco.compose.display.service.ClientSynchUpdateHistoryLocal;
import com.barco.nimbus.pubsub.api.PubSubI;
import com.barco.nimbus.pubsub.api.PubSubI.Callback;

@Component
public class CommandStack implements ClientSynchUpdateHistoryLocal<String> {

	private final static Logger log = Logger.getLogger(CommandStack.class);
	private final ClientSynchUpdateHistoryBean delegate = new ClientSynchUpdateHistoryBean();
	private final AtomicReference<PubSubI> ps = new AtomicReference<PubSubI>();
	private final Callback callback = new Callback() {
		@Override
		public void onMessage(Map<String, ? extends Object> headers, Object body) {
			// copy the headers: it is not serialisable,
			// but it needs to be, in order to be able to deliver them later
			ClientSynchUpdateHistoryBean.Util.handle(CommandStack.this, new HashMap<String, Object>(headers), body);
		}
	};
	private Closeable subscription;

	@Reference
	public void setPubSub(PubSubI ps) {
		this.ps.set(ps);
	}

	public void unsetPubSub(PubSubI ps) {
		this.ps.compareAndSet(ps, null);
	}

	@Activate
	public void start() {
		delegate.start();
		subscription = ps.get().subscribe("topic/clientsynch", null, null, callback);
	}
	
	@Deactivate
	public void stop() throws IOException {
		subscription.close();
		delegate.stop();
	}

	@Override
	public void remove(String tag) {
		delegate.remove(tag);
	}

	@Override
	public void add(String tag, Object item) {
		delegate.add(tag, item);
	}

	@Override
	public List get(String tag) {
		return delegate.get(tag);
	}

	@Override
	public void setObserving(String tag, boolean listen) {
		delegate.setObserving(tag, listen);
	}

}
