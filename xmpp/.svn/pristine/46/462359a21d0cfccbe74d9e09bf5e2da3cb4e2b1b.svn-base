package com.ipvsserv.nxd.adminapi.db;

import java.util.Map;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.nxd.migration.DBMigrationFileSystemMediaModule;

public class DBMigrateMediaAction extends DBQueryAction {
    //not used yet
	@Override
	public Object execute(Map<String, String> params)
			throws DBNotConnectedException, DBConnectTimedoutException,
			DBExecutionException, InterruptedException, Exception {
		DBMigrationFileSystemMediaModule module = new DBMigrationFileSystemMediaModule();
		module.initialize(params);
		module.start();
		module.stop();
		return "DONE";
	}

//	public Object execute2(Map<String, String> params)
//			throws DBNotConnectedException, DBConnectTimedoutException,
//			DBExecutionException, InterruptedException, Exception {
//		String message_mig = "";
//		String dbServiceVersion = "";
//		String softwareServiceVersion = "";
//		String serviceVersion = "";
//		// todo://ScriptVariablesGlobal.setModuleName(getIpvsModuleName());
//		ScriptVariablesGlobal scriptGlobals = new ScriptVariablesGlobal(
//				softwareServiceVersion, dbServiceVersion, DBAdmin
//						.getDBAdminApi().getIpvsDBDriver(),
//				DBMigrationModuleBase.MIGRATION_TYPE_MEDIA);
//		String migrationError = "";
//		try {
//			// createMigrationStatus
//			// todo://createMigrationStatus(dbServiceVersion,
//			// softwareServiceVersion);
//			// create a copy for xsl appling
//			// todo://copyFiles();
//			String loopServiceVersion = dbServiceVersion;
//			ServiceVersionMigrateImpl migrateServiceVersion = null;
//			// MRLog.debug("Apply xsl on local file system on folder '"+xslApplyFolder+"'");
//			while (!scriptGlobals.getNextServiceVersion(loopServiceVersion)
//					.equals("")) {
//				try {
//					// MRLog.debug("sv-" + serviceVersion + " dv-" +
//					// scriptGlobals.getaftermigration_serviceVersion() +" l " +
//					// loopServiceVersion);
//					serviceVersion = scriptGlobals.getServiceVersion();
//					// status_startDBMigration
//					// todo://status_startDBMigration();
//
//					// do migrate from current version to next post migration
//					// version
//					migrateServiceVersion = new ServiceVersionMigrateImpl(
//							scriptGlobals);
//					migrateServiceVersion.init(migrateServiceVersion,
//							migrateServiceVersion, null);
//					migrateServiceVersion.start();
//
//					// set version in db
//					loopServiceVersion = scriptGlobals
//							.getaftermigration_serviceVersion();
//					// todo://updateDBServiceVersion(scriptGlobals.getaftermigration_serviceVersion());
//
//					// status_doneDBMigration
//					// todo://status_doneDBMigration();
//				} finally {
//					ServiceVersionMigrateImpl.removeInstance();
//				}
//			}
//			// removeMigrationStatus
//			// todo://removeMigrationStatus(dbServiceVersion,
//			// softwareServiceVersion);
//			// put back the data into database
//			// todo://uploadFiles();
//
//			// createIndexes();
//
//			return "done";
//		} catch (Exception ex) {
//			MRLog.error("ERROR Database migration", ex);
//			migrationError = message_mig
//					+ " Database migration failed, during migration ver "
//					+ serviceVersion + " to "
//					+ scriptGlobals.getaftermigration_serviceVersion() + " "
//					+ ex.getMessage();
//			// debug(migrationError);
//			// IpvsServer.getInstance().stop();
//			throw new Exception(migrationError);
//		}
//
//		// return migrationError;
//	}
}
