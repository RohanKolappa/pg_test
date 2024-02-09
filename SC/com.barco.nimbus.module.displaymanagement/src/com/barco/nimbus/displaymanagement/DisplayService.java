package com.barco.nimbus.displaymanagement;

import java.net.URI;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicReference;

import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.nimbus.module.api.Content;
import com.barco.nimbus.module.api.Display;
import com.barco.nimbus.module.api.DisplayServiceI;
import com.barco.nimbus.module.api.LayoutSessionI;
import com.barco.nimbus.module.api.LayoutSessionI.PeerProperties;
import com.barco.nimbus.pubsub.api.PubSubI;

/**
 * Remembers the displays = which destination port (identified by UUID) belongs
 * to which logical grouping called Display.
 * 
 * Associates each display with 0 or 1 LayoutSession. This association is
 * published as {@link Content} objects to /topic/peer2peer/destinationPortUUID,
 * and can be queried using getContent(destinationPortUUID).
 */
@Component
public class DisplayService implements DisplayServiceI {

	private final Collection<Display> sinks = new HashSet<Display>();
	private final Map<Display, LayoutSessionI<?,?>> sessions = new HashMap<>();
	private final AtomicReference<PubSubI> pubsub = new AtomicReference<>();
	private final String prefix = "topic/peer2peer/";

	@Reference
	public void bind(PubSubI p) {
		pubsub.set(p);
	}

	public void unbind(PubSubI p) {
		pubsub.compareAndSet(p, null);
	}

	@Override
	public Iterable<? extends Display> getDisplays(String filter) {
		return Collections.unmodifiableCollection(sinks);
	}

	@Override
	public void setDisplay(Display display) {
		sinks.remove(display);
		sinks.add(display);
		LayoutSessionI<?,?> session = sessions.get(display);
		setLayoutSession(display,session);
	}

	@Override
	public LayoutSessionI<?,?> setLayoutSession(Display display, LayoutSessionI<?,?> session) {
		LayoutSessionI<?,?> prev = session == null ? sessions.remove(display) : sessions.put(display, session);
		for (Display.Screen tile : display.tiles) {
			Content body = createState(session, display, tile);
			// configure port with that uri
			// this impl expects that a peer subscribes to
			// topic/peer2peer/itsUUID
			pubsub.get().send(prefix + tile.port, null, body);
		}
		return prev;
	}

	private Content createState(LayoutSessionI<?,?> session, Display display, Display.Screen tile) {
		List<URI> uri = session == null ? null : session.createSubscriptionURI(display, new PeerProperties(tile.port, tile.x, tile.y, tile.w, tile.h, tile.u, tile.v,Collections.<String>emptyList()));
		return new Content(null, uri);
	}

	@Override
	public Content getContent(UUID uuid) {
		for (Display display : sinks) {
			for (Display.Screen tile : display.tiles) {
				if ((tile.port).equals(uuid)) {
					return createState(sessions.get(display), display, tile);
				}
			}
		}
		return null;
	}
	
	@Override
	public LayoutSessionI<?,?> getLayoutSession(Display display) {
		return sessions.get(display);
	}
}
