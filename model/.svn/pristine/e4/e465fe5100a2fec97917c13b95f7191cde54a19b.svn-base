
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->



    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">1.5</xsl:attribute>
    </xsl:template>


    <!-- Add ATTRIBUTES CollaborationConfig ( were absent in 'objectXml_1.3' but added to  'objectXml_1.5') -->
    <!--  xsd was diffent from objectxml -->
    <xsl:template mode="add-atts" match="/CollaborationConfig">
      <xsl:choose>
          <xsl:when test="@Ver">
          </xsl:when>
          <xsl:otherwise>
              <xsl:attribute name="Ver">1.3</xsl:attribute>
          </xsl:otherwise>
      </xsl:choose> 
      <xsl:choose>
          <xsl:when test="@NID">
          </xsl:when>
          <xsl:otherwise>
              <xsl:attribute name="NID"></xsl:attribute>
          </xsl:otherwise>
      </xsl:choose> 
      <xsl:choose>
          <xsl:when test="@Rev">
          </xsl:when>
          <xsl:otherwise>
              <xsl:attribute name="Rev">1</xsl:attribute>
          </xsl:otherwise>
      </xsl:choose> 
      <xsl:choose>
          <xsl:when test="@parentNID">
          </xsl:when>
          <xsl:otherwise>
              <xsl:attribute name="parentNID"></xsl:attribute>
          </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>


    <!-- Add ATTRIBUTES CollaborationConfig ( were absent in 'objectXml_1.4' but added to  'objectXml_1.5') -->
    
    <!--  xsd was different from objectxml -->
    <!--  
    <xsl:template mode="add-atts" match="/CollaborationConfig/ConnectionList/Connection">
        <xsl:choose>
            <xsl:when test="@streamType">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="streamType"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@gid">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="gid"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@type">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="type"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template mode="add-atts" match="/CollaborationConfig/EntityList/Entity">
        <xsl:choose>
            <xsl:when test="@objectType">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="objectType"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@streamType">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="streamType"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@presence">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="presence"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@maxInstanceCount">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="maxInstanceCount"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@entityType">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="entityType"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@title">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="title"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    -->


    <!-- DELETE ELEMENTS CollaborationConfig ( were present in 'objectXml_1.4' but removed from 'objectXml_1.5') -->

    <xsl:template match="/CollaborationConfig/EntityList/Entity/MediaDestinationRoleData/destNID"/>
    <xsl:template match="/CollaborationConfig/EntityList/Entity/MediaSourceRoleData/sourceNID"/>


    <!-- Add ELEMENTS CollaborationConfig ( were absent in 'objectXml_1.4' but added to  'objectXml_1.5') -->

    <!--
        /CollaborationConfig/EntityList/Entity/MediaSourceRoleData/resourceNID /CollaborationConfig/EntityList/Entity/MediaSourceRoleData/playLength /CollaborationConfig/Properties
        /CollaborationConfig/PVRInfo /CollaborationConfig/RecordingInfo /CollaborationConfig/syncSourceEntityID /CollaborationConfig/EntityList/Entity/MediaDestinationRoleData/resourceNID
    -->
    <!--
        move from /CollaborationConfig/EntityList/Entity/MediaSourceRoleData/sourceNID to /CollaborationConfig/EntityList/Entity/MediaSourceRoleData/resourceNID move from
        /CollaborationConfig/EntityList/Entity/MediaSourceRoleData/sourceNID to /CollaborationConfig/EntityList/Entity/MediaDestinationRoleData/resourceNID
    -->
    <xsl:template mode="append" match="/CollaborationConfig/EntityList/Entity/MediaSourceRoleData">
        <xsl:choose>
            <xsl:when test="resourceNID">
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <xsl:when test="/CollaborationConfig/EntityList/Entity/MediaSourceRoleData/sourceNID">
                        <resourceNID>
                            <xsl:value-of select="/CollaborationConfig/EntityList/Entity/MediaSourceRoleData/sourceNID"/>
                        </resourceNID>
                    </xsl:when>
                    <xsl:otherwise>
                        <resourceNID></resourceNID>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="playLength">
            </xsl:when>
            <xsl:otherwise>
                <playLength/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template mode="append" match="/CollaborationConfig">
        <xsl:choose>
            <xsl:when test="Properties">
            </xsl:when>
            <xsl:otherwise>
                <Properties>
                    <Owner/>
                    <DateCreated/>
                    <DateModified/>

                    <Permissions>
                        <Read/>
                        <Write/>
                        <RunRecord/>
                        <RunPlayback/>
                    </Permissions>
                </Properties>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="PVRInfo">
            </xsl:when>
            <xsl:otherwise>
                <PVRInfo>
                    <Enable/>
                    <TrimLength/>
                </PVRInfo>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="RecordingInfo">
            </xsl:when>
            <xsl:otherwise>
                <RecordingInfo>
                    <MediaStreamDirResourceNID/>
                    <MediaStreamFileResourceTitle/>
                    <MediaGroupFileResourceNID/>
                    <StartOnRun>false</StartOnRun>
                </RecordingInfo>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="syncSourceEntityID">
            </xsl:when>
            <xsl:otherwise>
                <syncSourceEntityID/>
            </xsl:otherwise>
        </xsl:choose>
        <!-- Groups new from 1.3 to 1.5  -->
      <xsl:choose>
          <xsl:when test="Groups">
          </xsl:when>
          <xsl:otherwise>
              <Groups>
              <!-- <Tag Name="" Value=""/> -->
            </Groups>
          </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>
        
    </xsl:template>

    <xsl:template mode="append" match="/CollaborationConfig/EntityList/Entity/MediaDestinationRoleData">
        <xsl:choose>
            <xsl:when test="resourceNID">
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <xsl:when test="/CollaborationConfig/EntityList/Entity/MediaDestinationRoleData/destNID">
                        <resourceNID MediaStorePortResourceNID="" RecordingName="">
                            <xsl:value-of select="/CollaborationConfig/EntityList/Entity/MediaDestinationRoleData/destNID"/>
                        </resourceNID>
                    </xsl:when>
                    <xsl:otherwise>
                        <resourceNID MediaStorePortResourceNID="" RecordingName=""/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>