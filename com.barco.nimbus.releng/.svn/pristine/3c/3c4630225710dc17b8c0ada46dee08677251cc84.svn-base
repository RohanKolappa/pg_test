# To mirror download.eclipse.org, install an eclipse in /usr/local/eclipse-4.3.1, then run the script below, and expose /home/barco/download.eclipse.org in a webserver
# See also: http://help.eclipse.org/indigo/index.jsp?topic=/org.eclipse.platform.doc.isv/guide/p2_mirror.html

/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.metadata.repository.mirrorApplication -source http://hstaudacher.github.io/osgi-jax-rs-connector -destination file:///home/barco/hstaudacher.github.io/osgi-jax-rs-connector
/usr/local/eclipse-4.3.1/eclipse/eclipse -nosplash -verbose -consoleLog -application org.eclipse.equinox.p2.artifact.repository.mirrorApplication -source http://hstaudacher.github.io/osgi-jax-rs-connector -destination file:///home/barco/hstaudacher.github.io/osgi-jax-rs-connector

