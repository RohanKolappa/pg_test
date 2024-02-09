# To mirror download.eclipse.org, install an eclipse in /usr/local/eclipse-4.3.1, then run the script below, and expose /home/barco/download.eclipse.org in a webserver
# See also: http://help.eclipse.org/indigo/index.jsp?topic=/org.eclipse.platform.doc.isv/guide/p2_mirror.html

/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.metadata.repository.mirrorApplication -source http://download.eclipse.org/releases/kepler -destination file:///home/barco/download.eclipse.org/releases/kepler
/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.artifact.repository.mirrorApplication -source http://download.eclipse.org/releases/kepler -destination file:///home/barco/download.eclipse.org/releases/kepler

/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.metadata.repository.mirrorApplication -source http://download.eclipse.org/eclipse/updates/4.3 -destination file:///home/barco/download.eclipse.org/eclipse/updates/4.3
/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.artifact.repository.mirrorApplication -source http://download.eclipse.org/eclipse/updates/4.3 -destination file:///home/barco/download.eclipse.org/eclipse/updates/4.3

/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.metadata.repository.mirrorApplication -source http://download.eclipse.org/gemini/updates/jpa/1.1.0 -destination file:///home/barco/download.eclipse.org/gemini/updates/jpa/1.1.0
/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.artifact.repository.mirrorApplication -source http://download.eclipse.org/gemini/updates/jpa/1.1.0 -destination file:///home/barco/download.eclipse.org/gemini/updates/jpa/1.1.0

/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.metadata.repository.mirrorApplication -source http://download.eclipse.org/rt/eclipselink/updates -destination file:///home/barco/download.eclipse.org/rt/eclipselink/updates
/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.artifact.repository.mirrorApplication -source http://download.eclipse.org/rt/eclipselink/updates -destination file:///home/barco/download.eclipse.org/rt/eclipselink/updates

/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.metadata.repository.mirrorApplication -source http://download.eclipse.org/gemini/jpa/updates -destination file:///home/barco/download.eclipse.org/gemini/jpa/updates
/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.artifact.repository.mirrorApplication -source http://download.eclipse.org/gemini/jpa/updates -destination file:///home/barco/download.eclipse.org/gemini/jpa/updates

