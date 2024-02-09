package com.barco.nimbus.module.api;

import java.util.UUID;

/**
 * The service through which a display is associated with content.
 */
public interface DisplayServiceI {
	/**
	 * @param filter
	 * @return the set of display's that match the filter
	 */
	Iterable<? extends Display> getDisplays(String filter);

	/**
	 * Combine a number of DestinationPort objects into a logical Display,
	 * destined to work with a particular LayoutService.
	 * 
	 * This also calls setLayoutSession, either with the LayoutSession that was
	 * previously associated to this display (in case of update), or with null.
	 * 
	 * This notifies all destination ports of their Contents.
	 */
	void setDisplay(Display display);

	/**
	 * Notify (push) each DestinationPort object (via device agent or pubsub) to
	 * connect to that session. At this point, each destination port gets
	 * configured with its Content, and hence connects to the LayoutService for
	 * that Display.
	 * 
	 * Whatever happens within the LayoutSession (layout updates) is
	 * communicated over the LayoutService-specific implementation.
	 */
	LayoutSessionI<?, ?> setLayoutSession(Display display, LayoutSessionI<?, ?> session);

	/**
	 * @return the session of that display, or null
	 */
	LayoutSessionI<?, ?> getLayoutSession(Display display);

	/**
	 * When a destination port starts, it fetches its state synchronously
	 * through this call, and also subscribes for Content updates over pubsub
	 * topic/peer2peer/itsUUID
	 */
	Content getContent(UUID port);

}
