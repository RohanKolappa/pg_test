package com.barco.nimbus.displaymanagement.bms;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Arrays;

import javax.persistence.EntityManagerFactory;
import javax.persistence.EntityTransaction;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

import com.barco.compose.display.facade.DispletPropertyBindingMgrFacadeBean;
import com.barco.compose.display.facade.LayoutImportService;
import com.barco.compose.display.facade.LayoutMgrFacadeBean;

@Component
public class AtBootImportLayout {
	
	final LayoutImportService lis = new LayoutImportService();
	
	@Reference(target="(osgi.unit.name=bmslayoutdb)")
	public void bind(EntityManagerFactory emf) {
		lis.manager = emf.createEntityManager();
		LayoutMgrFacadeBean la = new LayoutMgrFacadeBean();
		la.manager = lis.manager;
		lis.layoutmgr = la;
		DispletPropertyBindingMgrFacadeBean p = new DispletPropertyBindingMgrFacadeBean();
		p.manager = lis.manager;
		lis.pbMgr = p;
	}
	
	public void unbind(EntityManagerFactory emf) {
	}
	
	@Activate
	public void start()	{
		if (!lis.layoutmgr.getLayoutReferences().isEmpty()) return;
		EntityTransaction t = lis.manager.getTransaction();
		t.begin();
		try {
			lis.addAllLayoutsInZip(collectFiles("/home/barco/"));
			t.commit();
			Logger.getLogger(AtBootImportLayout.class).info(lis.layoutmgr.getLayoutReferences());
			t = null;
		} finally {
			if (t != null) t.rollback();
		}
	}
	
	private Iterable<File> collectFiles(String folder) {
		return Arrays.asList(new File(folder).listFiles(new FilenameFilter() {
			public boolean accept(File dir, String name) {
				return name.indexOf(".template.zip") != -1;
			}
		}));
	}

}
