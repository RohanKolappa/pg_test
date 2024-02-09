package com.barco.nimbus.utils.example.jaxrs;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;

@Path("examples/todo")
public interface IExampleJAXRSService {

	/**
	 * Note that jetty is configured through system properties:
	 * <ul>
	 * <li>
	 * -Dorg.osgi.service.http.port=9090
	 * <li>
	 * -Dorg.eclipse.equinox.http.jetty.autostart=true
	 * 
	 * Therefore the service can be called here: GET
	 * http://localhost:9090/services/examples/todo
	 */
	@GET
	@Produces("application/json")
	Item[] getAll();

	public static class Item {
		static int gen;
		public final int owner;
		public final int id = gen++;
		public String description;

		public Item(int owner) {
			this.owner = owner;
		}
	}
}
