package com.barco.nimbus.displaymanagement.bms;

import java.util.Arrays;
import java.util.Collections;
import java.util.concurrent.atomic.AtomicReference;

import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.compose.util.MapUtil;
import com.barco.nimbus.displaymanagement.bms.BMSLayoutService.Context;
import com.barco.nimbus.module.api.LayoutServiceI;
import com.barco.nimbus.module.api.RegistryServiceI;

@Component
public class AtBootActivateLayout {
	
	private final AtomicReference<LayoutServiceI> ls = new AtomicReference<>();
	private final AtomicReference<RegistryServiceI> rs = new AtomicReference<>();
	
	@Reference(target="(component.name=com.barco.nimbus.displaymanagement.bms.BMSLayoutService)")
	public void setLayoutService(LayoutServiceI ls) {
		this.ls.set(ls);
	}
	
	public void unsetLayoutService(LayoutServiceI ls) {
		this.ls.compareAndSet(ls, null);
	}
	
	@Reference
	public void setRegistryService(RegistryServiceI rs) {
		this.rs.set(rs);
	}
	
	public void unsetRegistryService(RegistryServiceI rs) {
		this.rs.compareAndSet(rs, null);
	}
	
	@Activate
	public void start() {
		// demo: activates a session and update it every 20 seconds;
		// http://172.16.10.222:9090/barco-webservice/content2.html?session=0 
		final String name = "sink0";
		rs.get().set(Collections.singleton(Arrays.asList(name,"1","src","mySource1")));
		ls.get().createSession(MapUtil.map("width",320,"height",480,"name",name)).setLayout("0", "/Default/full.layout", new Context());
		Thread thread = new Thread(new Runnable(){
			int i = 2;
			@Override
			public void run() {
				while (!Thread.interrupted()) {
					rs.get().set(Collections.singleton(Arrays.asList(name,"1","src","mySource" + i++)));
					try {
						Thread.sleep(20000);
					} catch (InterruptedException e) {
					}
				}
			}
		});
		thread.setDaemon(true);
		thread.start();
	}
}
