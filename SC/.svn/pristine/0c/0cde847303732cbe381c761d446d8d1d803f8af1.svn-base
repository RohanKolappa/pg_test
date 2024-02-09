package com.barco.nimbus.module.api;

import java.util.Collections;
import java.util.List;
import java.util.UUID;

/**
 * A Display is a 2D arrangement of 1 or more screens, destined to work with one
 * particular LayoutServiceI (when a Display is created, the LayoutServiceI is
 * passed in as an OSGi filter expression).
 * 
 * A screen belongs to exactly one Display. A screen relates to a destination
 * port (identified by UUID).
 */
public class Display {
	public final String id;
	public final Iterable<Screen> tiles;
	public final String layoutOSGiServiceFilter;
	public final int width,height;

	public Display(String id, int width, int height, List<Screen> tiles, String layoutOSGiServiceFilter) {
		this.id = id;
		this.width = width;
		this.height = height;
		this.tiles = Collections.unmodifiableCollection(tiles);
		this.layoutOSGiServiceFilter = layoutOSGiServiceFilter;
	}

	public static class Screen {
		public final UUID port;
		public final int x, y, w, h;
		public final double u, v;

		public Screen(UUID port, int x, int y, int w, int h, double u, double v) {
			this.port = port;
			this.x = x;
			this.y = y;
			this.w = w;
			this.h = h;
			this.u = u;
			this.v = v;
		}
	}
	
	@Override
	public boolean equals(Object obj) {
		return obj instanceof Display &&  id.equals(((Display)obj).id);
	}
	
	@Override
	public int hashCode() {
		return id.hashCode();
	}
}
