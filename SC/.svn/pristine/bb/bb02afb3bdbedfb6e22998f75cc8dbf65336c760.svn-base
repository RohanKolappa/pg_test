package com.barco.nimbus.license;

import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.glassfish.jersey.media.multipart.FormDataMultiPart;

import com.barco.nimbus.license.api.LicenseServiceHTTPI;

@Path("/")
public interface LicenseJAXRSI extends LicenseServiceHTTPI {

	/**
	 * @see com.barco.nimbus.license.api.LicenseServiceHTTPI#get()
	 */
	@GET
	@Path("/get")
	@Produces(MediaType.APPLICATION_JSON)
	public Response get();

	/**
	 * @see com.barco.nimbus.license.api.LicenseServiceHTTPI#put(org.glassfish.jersey.media.multipart.FormDataMultiPart)
	 */
	@POST
	@Path("/put")
	@Consumes({ MediaType.MULTIPART_FORM_DATA })
	public Response put(FormDataMultiPart stream);
}
