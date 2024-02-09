package com.barco.nimbus.module.api;

import java.util.List;

/**
 * A factory and registry for LayoutSession.
 * 
 * A Layout session exposes how a peer can watch the contents, through
 * LayoutSession.createTileURI. Whoever gets hold of this uri (and understands
 * it) can fetch the session content and live updates.
 * 
 * These URI's are final within a session (the URI is not dependent of the LR
 * being put into the session).
 * 
 * The protocol over which the content is traveling is defined by the URI
 * produced by LayoutsSession.createTileURI. The peer is expected to
 * 'understand' how to use the URI to get data out of it.
 * 
 * The layout service is not aware of the peers that are actually connected.
 * There's no dependency on DisplayService.
 * 
 * @param LR
 *            layout-reference
 * @param LC
 *            context passed along with LR in the LayoutSession.setLayout
 * @param C
 *            context required to create a LayoutSession
 * 
 */
public interface LayoutServiceI<LR, LC, C> {

	/**
	 * inform the service to setup a session. A LayoutSession consists of the
	 * whole setting up of server endpoints in order to enable destination ports
	 * to get their layout descriptions and updates.
	 */
	LayoutSessionI<LR, LC> createSession(C context);

	void dispose(LayoutSessionI<LR, LC> session);

	/** optional: all existing sessions within this LayoutService */
	Iterable<? extends LayoutSessionI<LR, LC>> getSessions();

	/** optional: listing the layouts 'available' for usage in setLayout. */
	List<LR> getLayouts();
}
