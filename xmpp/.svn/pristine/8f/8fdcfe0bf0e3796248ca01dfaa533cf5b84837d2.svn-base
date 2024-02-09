package com.ipvsserv.nxd.migration.xslt;

import java.io.File;
import java.util.Hashtable;
import java.util.List;

import org.dom4j.Element;

import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;
import com.ipvsserv.nxd.xslt.FileSystemApplyXSLUtils;
import com.ipvsserv.nxd.xslt.FileSystemApplyXSLUtils.ActionsListData;
import com.ipvsserv.server.IpvsServer;

public class ApplyXslFileSystemTag extends MigrationTagImpl {

    public ApplyXslFileSystemTag() {
        super("ApplyXslFileSystemTag");
    }

    public ApplyXslFileSystemTag(String moduleName) {
        super(moduleName);
    }

    @SuppressWarnings("unchecked")
    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        /*
        <ipvstag_ApplyXslFileSystem enable="true">
        
            <xslMapping enable="true" type="xmlFilename" xmlFilename="service.version">
                <xsl_transform xsl="../../shared/xsl/_updateServiceVersionXml.xsl" />
            </xslMapping>
                                
            <xslMapping enable="true" type="objectType" objectType="*">
                <xsl_transform xsl="../../shared/xsl/_updateServiceVersion.xsl" />
            </xslMapping>
            
            <xslMapping enable="true"  type="objectType" objectType="AssetResource">
                <xsl_transform xsl="xsl/AssetResource.xsl" />
                <xsl_transform xsl="xsl/add_MediaStreamDstPortResource.xsl"  resultType="newdoc" xpath="/AssetResource[Info/Type='IPVSClient-Device']"/>
                <xsl_transform xsl="xsl/add_IPVSClient-Device.xsl" resultType="newdoc" xpath="/AssetResource[Info/Type='IPVSClient-Device']"/>
            </xslMapping>

            <xslMapping enable="true"  type="objectType" objectType="MediaStreamRelayPortResource">
                <xsl_transform xsl="xsl/MediaStreamRelayPortResource.xsl" />
            </xslMapping>

            <xslMapping enable="true"  type="objectType" objectType="MediaStreamSrcPortResource">
                <xsl_transform xsl="xsl/MediaStreamSrcPortResource.xsl" />
            </xslMapping>            

            <xslMapping enable="true"  type="objectType" objectType="MediaStreamFileResource">
                <doc_delete />
            </xslMapping>

        </ipvstag_ApplyXslFileSystem>  
        */
    	Hashtable<String, ActionsListData> xslMappings = new Hashtable<String, ActionsListData>();
        String migrationFolder = root.getScriptGlobals().getDBMigrationFolder();
        List<Element> listMappings = (List<Element>) Dom4jUtils.getList(element, "xslMapping");
        for (Element mapEL : listMappings) {
            boolean enable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(mapEL, "enable", ""), true);
            if (enable) {
                ActionsListData actionsListData = new ActionsListData(mapEL, migrationFolder);
                xslMappings.put(actionsListData.name, actionsListData);
            }
        }
        File directory = new File(root.getMigrationModule().getXslApplyFolder());
        if (!directory.exists())
            throw new DBExecutionException("Could not find directory '" + directory + "'.");
        if (!directory.isDirectory())
            throw new DBExecutionException("'" + directory + "' is not a directory.");
        if (!directory.canRead())
            throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");

        boolean saveToColEnable = true;
        boolean applyServiceVersion=false;
        String afterServiceVer = root.getMigrationModule().getServiceVersion().getaftermigration_serviceVersion().trim();
        String softwareServiceVer = IpvsServer.getInstance().getServerVersion().trim();
        if(afterServiceVer.equals(softwareServiceVer)){
            applyServiceVersion=true;
        }
        Hashtable<String, String> parameters = new Hashtable<String, String>();
        parameters = root.getScriptGlobals().getList();
         
        
        DBAdminParams params = new DBAdminParams();
        params.setSaveToColEnable(saveToColEnable);
        FileSystemApplyXSLUtils.applyXslOnDirectory(directory, "", params, xslMappings, applyServiceVersion, parameters);
    }
}
