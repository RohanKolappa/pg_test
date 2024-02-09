package com.ipvsserv.common.certificate;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.security.GeneralSecurityException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.Principal;
import java.security.PrivateKey;
import java.security.Provider;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.Security;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.CertificateParsingException;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.bouncycastle.asn1.ASN1Encodable;
import org.bouncycastle.asn1.ASN1InputStream;
import org.bouncycastle.asn1.DEREncodable;
import org.bouncycastle.asn1.DERObjectIdentifier;
import org.bouncycastle.asn1.DEROutputStream;
import org.bouncycastle.asn1.DERSequence;
import org.bouncycastle.asn1.DERTaggedObject;
import org.bouncycastle.asn1.DERUTF8String;
import org.bouncycastle.asn1.x509.GeneralName;
import org.bouncycastle.asn1.x509.GeneralNames;
import org.bouncycastle.asn1.x509.X509Extensions;
import org.bouncycastle.asn1.x509.X509Name;
import org.bouncycastle.jce.PKCS10CertificationRequest;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openssl.PEMReader;
import org.bouncycastle.openssl.PasswordFinder;
import org.bouncycastle.x509.X509V3CertificateGenerator;

import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class CertificateManager {
    
    private static Pattern cnPattern = Pattern.compile("(?i)(cn=)([^,]*)");
    private static Pattern valuesPattern = Pattern.compile("(?i)(=)([^,]*)");
    
    private static Provider provider = new BouncyCastleProvider();
    
    private static final int CERT_REQ_LINE_LENGTH = 76;
    
    private static List<CertificateEventListener> listeners = new CopyOnWriteArrayList<CertificateEventListener>();
    
    static {
        // Add the BC provider to the list of security providers
        Security.addProvider(provider);
    }
    
    public static X509Certificate createDSACert(KeyStore ksKeys, String keyPassword, String alias, String issuerDN, String subjectDN, String domain) throws GeneralSecurityException, IOException {
        // Generate public and private keys
        KeyPair keyPair = generateKeyPair("DSA", 1024);
        // Create X509 certificate with keys and specified domain
        X509Certificate cert = createX509V3Certificate(keyPair, 60, issuerDN, subjectDN, domain, "SHA1withDSA");
        // Store new certificate and private key in the keystore
        ksKeys.setKeyEntry(alias, keyPair.getPrivate(), keyPassword.toCharArray(), new X509Certificate[] { cert });
        // Notify listeners that a new certificate has been created
        for (CertificateEventListener listener : listeners) {
            try {
                listener.certificateCreated(ksKeys, alias, cert);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        // Return new certificate
        return cert;
    }
    
    public static X509Certificate createRSACert(KeyStore ksKeys, String keyPassword, String alias, String issuerDN, String subjectDN, String domain) throws GeneralSecurityException, IOException {
        // Generate public and private keys
        KeyPair keyPair = generateKeyPair("RSA", 1024);
        // Create X509 certificate with keys and specified domain
        X509Certificate cert = createX509V3Certificate(keyPair, 60, issuerDN, subjectDN, domain, "MD5withRSA");
        // Store new certificate and private key in the keystore
        ksKeys.setKeyEntry(alias, keyPair.getPrivate(), keyPassword.toCharArray(), new X509Certificate[] { cert });
        // Notify listeners that a new certificate has been created
        for (CertificateEventListener listener : listeners) {
            try {
                listener.certificateCreated(ksKeys, alias, cert);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        // Return new certificate
        return cert;
    }
    
    public static void deleteCertificate(KeyStore ksKeys, String alias) throws GeneralSecurityException, IOException {
        ksKeys.deleteEntry(alias);
        // Notify listeners that a new certificate has been created
        for (CertificateEventListener listener : listeners) {
            try {
                listener.certificateDeleted(ksKeys, alias);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
    }
    
    public static List<String> getPeerIdentities(X509Certificate x509Certificate) {
        // Look the identity in the subjectAltName extension if available
        List<String> names = getSubjectAlternativeNames(x509Certificate);
        if (names.isEmpty()) {
            String name = x509Certificate.getSubjectDN().getName();
            Matcher matcher = cnPattern.matcher(name);
            // Create an array with the detected identities
            names = new ArrayList<String>();
            while (matcher.find()) {
                names.add(matcher.group(2));
            }
        }
        return names;
    }
    
    private static List<String> getSubjectAlternativeNames(X509Certificate certificate) {
        List<String> identities = new ArrayList<String>();
        try {
            Collection<List<?>> altNames = certificate.getSubjectAlternativeNames();
            // Check that the certificate includes the SubjectAltName extension
            if (altNames == null) {
                return Collections.emptyList();
            }
            // Use the type OtherName to search for the certified server name
            for (List item : altNames) {
                Integer type = (Integer) item.get(0);
                if (type == 0) {
                    // Type OtherName found so return the associated value
                    try {
                        // Value is encoded using ASN.1 so decode it to get the server's identity
                        ASN1InputStream decoder = new ASN1InputStream((byte[]) item.toArray()[1]);
                        DEREncodable encoded = decoder.readObject();
                        encoded = ((DERSequence) encoded).getObjectAt(1);
                        encoded = ((DERTaggedObject) encoded).getObject();
                        encoded = ((DERTaggedObject) encoded).getObject();
                        String identity = ((DERUTF8String) encoded).getString();
                        if (!"".equals(identity)) {
                            // Add the decoded server name to the list of identities
                            identities.add(identity);
                        }
                    } catch (UnsupportedEncodingException ex) {
                        // Ignore
                    } catch (IOException ex) {
                        // Ignore
                    } catch (Exception ex) {
                        MRLog.error("CertificateManager: Error decoding subjectAltName", ex);
                    }
                }
                // Other types are not good for XMPP so ignore them
                else if (MRLog.isDebugEnabled()) {
                    MRLog.debug("CertificateManager: SubjectAltName of invalid type found: " + certificate.getSubjectDN());
                }
            }
        } catch (CertificateParsingException ex) {
            MRLog.error("CertificateManager: Error parsing SubjectAltName in certificate: " + certificate.getSubjectDN(), ex);
        }
        return identities;
    }
    
    public static boolean isRSACertificate(KeyStore ksKeys, String domain) throws KeyStoreException {
        return isCertificate(ksKeys, domain, "RSA");
    }
    
    public static boolean isDSACertificate(KeyStore ksKeys, String domain) throws KeyStoreException {
        return isCertificate(ksKeys, domain, "DSA");
    }
    
    public static boolean isDSACertificate(X509Certificate certificate) throws KeyStoreException {
        return certificate.getPublicKey().getAlgorithm().equals("DSA");
    }
    
    private static boolean isCertificate(KeyStore ksKeys, String domain, String algorithm) throws KeyStoreException {
        for (Enumeration<String> aliases = ksKeys.aliases(); aliases.hasMoreElements();) {
            X509Certificate certificate = (X509Certificate) ksKeys.getCertificate(aliases.nextElement());
            if ("*".equals(domain)) {
                // Any domain certified by the certificate is accepted
                if (certificate.getPublicKey().getAlgorithm().equals(algorithm)) {
                    return true;
                }
            } else {
                // Only accept certified domains that match the specified domain
                for (String identity : getPeerIdentities(certificate)) {
                    if (identity.endsWith(domain) && certificate.getPublicKey().getAlgorithm().equals(algorithm)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    public static boolean isSelfSignedCertificate(KeyStore keyStore, String alias) throws KeyStoreException {
        // Get certificate chain
        java.security.cert.Certificate[] certificateChain = keyStore.getCertificateChain(alias);
        // Verify that the chain is empty or was signed by himself
        return certificateChain == null || certificateChain.length == 1;
    }
    
    public static boolean isSelfSignedCertificate(KeyStore keyStore, X509Certificate certificate) throws KeyStoreException {
        String alias = keyStore.getCertificateAlias(certificate);
        if (alias == null) {
            throw new KeyStoreException("Certificate not found in store: " + certificate);
        }
        return isSelfSignedCertificate(keyStore, alias);
    }
    
    public static boolean isSigningRequestPending(KeyStore keyStore, String alias) throws KeyStoreException {
        // Verify that this is a self-signed certificate
        if (!isSelfSignedCertificate(keyStore, alias)) {
            return false;
        }
        // Verify that the issuer information has been entered
        X509Certificate certificate = (X509Certificate) keyStore.getCertificate(alias);
        Matcher matcher = valuesPattern.matcher(certificate.getIssuerDN().toString());
        return matcher.find() && matcher.find();
    }
    
    public static String createSigningRequest(X509Certificate cert, PrivateKey privKey) throws Exception {
        StringBuilder sb = new StringBuilder();
        
        String subject = cert.getSubjectDN().getName();
        X509Name xname = new X509Name(subject);
        
        PublicKey pubKey = cert.getPublicKey();
        
        String signatureAlgorithm = "DSA".equals(pubKey.getAlgorithm()) ? "SHA1withDSA" : "MD5withRSA";
        
        PKCS10CertificationRequest csr = new PKCS10CertificationRequest(signatureAlgorithm, xname, pubKey, null, privKey);
        
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DEROutputStream deros = new DEROutputStream(baos);
        deros.writeObject(csr.getDERObject());
        String sTmp = new String(org.bouncycastle.util.encoders.Base64.encode(baos.toByteArray()));
        
        // Header
        sb.append("-----BEGIN NEW CERTIFICATE REQUEST-----\n");
        
        // Add signing request content (base 64 encoded)
        for (int iCnt = 0; iCnt < sTmp.length(); iCnt += CERT_REQ_LINE_LENGTH) {
            int iLineLength;
            
            if ((iCnt + CERT_REQ_LINE_LENGTH) > sTmp.length()) {
                iLineLength = sTmp.length() - iCnt;
            } else {
                iLineLength = CERT_REQ_LINE_LENGTH;
            }
            
            sb.append(sTmp.substring(iCnt, iCnt + iLineLength)).append("\n");
        }
        
        // Footer
        sb.append("-----END NEW CERTIFICATE REQUEST-----\n");
        return sb.toString();
    }
    
    public static boolean installReply(KeyStore keyStore, KeyStore trustStore, String keyPassword, String alias, InputStream inputStream, boolean trustCACerts, boolean validateRoot) throws Exception {
        
        // Check that there is a certificate for the specified alias
        X509Certificate certificate = (X509Certificate) keyStore.getCertificate(alias);
        if (certificate == null) {
            MRLog.warn("Certificate not found for alias: " + alias);
            return false;
        }
        // Retrieve the private key of the stored certificate
        PrivateKey privKey = (PrivateKey) keyStore.getKey(alias, keyPassword.toCharArray());
        // Load certificates found in the PEM input stream
        List<X509Certificate> certs = new ArrayList<X509Certificate>();
        for (Certificate cert : CertificateFactory.getInstance("X509").generateCertificates(inputStream)) {
            certs.add((X509Certificate) cert);
        }
        if (certs.isEmpty()) {
            throw new Exception("Reply has no certificates");
        }
        List<X509Certificate> newCerts;
        if (certs.size() == 1) {
            // Reply has only one certificate
            newCerts = establishCertChain(keyStore, trustStore, null, certs.get(0), trustCACerts);
        } else {
            // Reply has a chain of certificates
            newCerts = validateReply(keyStore, trustStore, alias, null, certs, trustCACerts, validateRoot);
        }
        if (newCerts != null) {
            keyStore.setKeyEntry(alias, privKey, keyPassword.toCharArray(), newCerts.toArray(new X509Certificate[newCerts.size()]));
            
            // Notify listeners that a new certificate has been created
            for (CertificateEventListener listener : listeners) {
                try {
                    listener.certificateSigned(keyStore, alias, newCerts);
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
            
            return true;
        } else {
            return false;
        }
    }
    
    public static boolean installCert(KeyStore keyStore, KeyStore trustStore, String keyPassword, String alias, InputStream pkInputStream, final String passPhrase, InputStream inputStream, boolean trustCACerts, boolean validateRoot) throws Exception {
        // Check that there is a certificate for the specified alias
        X509Certificate certificate = (X509Certificate) keyStore.getCertificate(alias);
        if (certificate != null) {
            MRLog.warn("Certificate already exists for alias: " + alias);
            return false;
        }
        // Retrieve the private key of the stored certificate
        PasswordFinder passwordFinder = new PasswordFinder() {
            public char[] getPassword() {
                return passPhrase != null ? passPhrase.toCharArray() : new char[] {};
            }
        };
        PEMReader pemReader = new PEMReader(new InputStreamReader(pkInputStream), passwordFinder);
        KeyPair kp = (KeyPair) pemReader.readObject();
        PrivateKey privKey = kp.getPrivate();
        
        // Load certificates found in the PEM input stream
        List<X509Certificate> certs = new ArrayList<X509Certificate>();
        for (Certificate cert : CertificateFactory.getInstance("X509").generateCertificates(inputStream)) {
            certs.add((X509Certificate) cert);
        }
        if (certs.isEmpty()) {
            throw new Exception("No certificates were found");
        }
        List<X509Certificate> newCerts;
        if (certs.size() == 1) {
            // Reply has only one certificate
            newCerts = establishCertChain(keyStore, trustStore, certificate, certs.get(0), trustCACerts);
        } else {
            // Reply has a chain of certificates
            newCerts = validateReply(keyStore, trustStore, alias, certificate, certs, trustCACerts, validateRoot);
        }
        if (newCerts != null) {
            keyStore.setKeyEntry(alias, privKey, keyPassword.toCharArray(), newCerts.toArray(new X509Certificate[newCerts.size()]));
            
            // Notify listeners that a new certificate has been created (and signed)
            for (CertificateEventListener listener : listeners) {
                try {
                    listener.certificateCreated(keyStore, alias, certs.get(0));
                    if (newCerts.size() > 1) {
                        listener.certificateSigned(keyStore, alias, newCerts);
                    }
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
            
            return true;
        } else {
            return false;
        }
    }
    
    public static void addListener(CertificateEventListener listener) {
        if (listener == null) {
            throw new NullPointerException();
        }
        listeners.add(listener);
    }
    
    public static void removeListener(CertificateEventListener listener) {
        listeners.remove(listener);
    }
    
    private static List<X509Certificate> establishCertChain(KeyStore keyStore, KeyStore trustStore, X509Certificate certificate, X509Certificate certReply, boolean trustCACerts) throws Exception {
        if (certificate != null) {
            PublicKey publickey = certificate.getPublicKey();
            PublicKey publickey1 = certReply.getPublicKey();
            if (!publickey.equals(publickey1)) {
                throw new Exception("Public keys in reply and keystore don't match");
            }
            if (certReply.equals(certificate)) {
                throw new Exception("Certificate reply and certificate in keystore are identical");
            }
        }
        Map<Principal, List<X509Certificate>> knownCerts = new Hashtable<Principal, List<X509Certificate>>();
        if (keyStore.size() > 0) {
            knownCerts.putAll(getCertsByIssuer(keyStore));
        }
        if (trustCACerts && trustStore.size() > 0) {
            knownCerts.putAll(getCertsByIssuer(trustStore));
        }
        LinkedList<X509Certificate> answer = new LinkedList<X509Certificate>();
        if (buildChain(certReply, answer, knownCerts)) {
            return answer;
        } else {
            throw new Exception("Failed to establish chain from reply");
        }
    }
    
    private static boolean buildChain(X509Certificate certificate, LinkedList<X509Certificate> answer, Map<Principal, List<X509Certificate>> knownCerts) {
        Principal subject = certificate.getSubjectDN();
        Principal issuer = certificate.getIssuerDN();
        // Check if the certificate is a root certificate (i.e. was issued by the same Principal that
        // is present in the subject)
        if (subject.equals(issuer)) {
            answer.addFirst(certificate);
            return true;
        }
        // Get the list of known certificates of the certificate's issuer
        List<X509Certificate> issuerCerts = knownCerts.get(issuer);
        if (issuerCerts == null || issuerCerts.isEmpty()) {
            // No certificates were found so building of chain failed
            return false;
        }
        for (X509Certificate issuerCert : issuerCerts) {
            PublicKey publickey = issuerCert.getPublicKey();
            try {
                // Verify the certificate with the specified public key
                certificate.verify(publickey);
                // Certificate was verified successfully so build chain of issuer's certificate
                if (!buildChain(issuerCert, answer, knownCerts)) {
                    return false;
                }
            } catch (Exception exception) {
                // Failed to verify certificate
                return false;
            }
        }
        answer.addFirst(certificate);
        return true;
    }
    
    private static Map<Principal, List<X509Certificate>> getCertsByIssuer(KeyStore ks) throws Exception {
        Map<Principal, List<X509Certificate>> answer = new HashMap<Principal, List<X509Certificate>>();
        Enumeration<String> aliases = ks.aliases();
        while (aliases.hasMoreElements()) {
            String alias = aliases.nextElement();
            X509Certificate cert = (X509Certificate) ks.getCertificate(alias);
            if (cert != null) {
                Principal subjectDN = cert.getSubjectDN();
                List<X509Certificate> vec = answer.get(subjectDN);
                if (vec == null) {
                    vec = new ArrayList<X509Certificate>();
                    vec.add(cert);
                } else {
                    if (!vec.contains(cert)) {
                        vec.add(cert);
                    }
                }
                answer.put(subjectDN, vec);
            }
        }
        return answer;
    }
    
    private static List<X509Certificate> validateReply(KeyStore keyStore, KeyStore trustStore, String alias, X509Certificate userCert, List<X509Certificate> replyCerts, boolean trustCACerts, boolean verifyRoot) throws Exception {
        // order the certs in the reply (bottom-up).
        int i;
        X509Certificate tmpCert;
        if (userCert != null) {
            PublicKey userPubKey = userCert.getPublicKey();
            for (i = 0; i < replyCerts.size(); i++) {
                if (userPubKey.equals(replyCerts.get(i).getPublicKey())) {
                    break;
                }
            }
            if (i == replyCerts.size()) {
                throw new Exception("Certificate reply does not contain public key for <alias>: " + alias);
            }
            
            tmpCert = replyCerts.get(0);
            replyCerts.set(0, replyCerts.get(i));
            replyCerts.set(i, tmpCert);
        }
        
        Principal issuer = replyCerts.get(0).getIssuerDN();
        
        for (i = 1; i < replyCerts.size() - 1; i++) {
            // find a cert in the reply whose "subject" is the same as the
            // given "issuer"
            int j;
            for (j = i; j < replyCerts.size(); j++) {
                Principal subject = replyCerts.get(j).getSubjectDN();
                if (subject.equals(issuer)) {
                    tmpCert = replyCerts.get(i);
                    replyCerts.set(i, replyCerts.get(j));
                    replyCerts.set(j, tmpCert);
                    issuer = replyCerts.get(i).getIssuerDN();
                    break;
                }
            }
            if (j == replyCerts.size()) {
                throw new Exception("Incomplete certificate chain in reply");
            }
        }
        
        // now verify each cert in the ordered chain
        for (i = 0; i < replyCerts.size() - 1; i++) {
            PublicKey pubKey = replyCerts.get(i + 1).getPublicKey();
            try {
                replyCerts.get(i).verify(pubKey);
            } catch (Exception ex) {
                throw new Exception("Certificate chain in reply does not verify: " + ex.getMessage());
            }
        }
        
        if (!verifyRoot) {
            return replyCerts;
        }
        
        // do we trust the (root) cert at the top?
        X509Certificate topCert = replyCerts.get(replyCerts.size() - 1);
        boolean foundInKeyStore = keyStore.getCertificateAlias(topCert) != null;
        boolean foundInCAStore = trustCACerts && (trustStore.getCertificateAlias(topCert) != null);
        if (!foundInKeyStore && !foundInCAStore) {
            boolean verified = false;
            X509Certificate rootCert = null;
            if (trustCACerts) {
                for (Enumeration<String> aliases = trustStore.aliases(); aliases.hasMoreElements();) {
                    String name = aliases.nextElement();
                    rootCert = (X509Certificate) trustStore.getCertificate(name);
                    if (rootCert != null) {
                        try {
                            topCert.verify(rootCert.getPublicKey());
                            verified = true;
                            break;
                        } catch (Exception ex) {
                            // Ignore
                        }
                    }
                }
            }
            if (!verified) {
                return null;
            } else {
                // Check if the cert is a self-signed cert
                if (!topCert.getSubjectDN().equals(topCert.getIssuerDN())) {
                    // append the (self-signed) root CA cert to the chain
                    replyCerts.add(rootCert);
                }
            }
        }
        
        return replyCerts;
    }
    
    @SuppressWarnings("deprecation")
    private static synchronized X509Certificate createX509V3Certificate(KeyPair kp, int months, String issuerDN, String subjectDN, String domain, String signAlgoritm) throws GeneralSecurityException, IOException {
        PublicKey pubKey = kp.getPublic();
        PrivateKey privKey = kp.getPrivate();
        
        byte[] serno = new byte[8];
        SecureRandom random = SecureRandom.getInstance("SHA1PRNG");
        random.setSeed((new Date().getTime()));
        random.nextBytes(serno);
        BigInteger serial = (new java.math.BigInteger(serno)).abs();
        
        X509V3CertificateGenerator certGenerator = new X509V3CertificateGenerator();
        certGenerator.reset();
        
        certGenerator.setSerialNumber(serial);
        certGenerator.setIssuerDN(new X509Name(issuerDN));
        certGenerator.setNotBefore(new Date(System.currentTimeMillis()));
        certGenerator.setNotAfter(new Date(System.currentTimeMillis() + months * (1000L * 60 * 60 * 24 * 30)));
        certGenerator.setSubjectDN(new X509Name(subjectDN));
        certGenerator.setPublicKey(pubKey);
        certGenerator.setSignatureAlgorithm(signAlgoritm);
        
        // Generate the subject alternative name
        boolean critical = subjectDN == null || "".equals(subjectDN.trim());
        DERSequence othernameSequence = new DERSequence(new ASN1Encodable[] { new DERObjectIdentifier("1.3.6.1.5.5.7.8.5"), new DERTaggedObject(true, 0, new DERUTF8String(domain)) });
        GeneralName othernameGN = new GeneralName(GeneralName.otherName, othernameSequence);
        GeneralNames subjectAltNames = new GeneralNames(new DERSequence(new ASN1Encodable[] { othernameGN }));
        // Add subject alternative name extension
        certGenerator.addExtension(X509Extensions.SubjectAlternativeName, critical, subjectAltNames);
        
        X509Certificate cert = certGenerator.generateX509Certificate(privKey, "BC", new SecureRandom());
        cert.checkValidity(new Date());
        cert.verify(pubKey);
        
        return cert;
    }
    
    private static KeyPair generateKeyPair(String algorithm, int keysize) throws GeneralSecurityException {
        KeyPairGenerator generator;
        if (provider == null) {
            generator = KeyPairGenerator.getInstance(algorithm);
        } else {
            generator = KeyPairGenerator.getInstance(algorithm, provider);
        }
        generator.initialize(keysize, new SecureRandom());
        return generator.generateKeyPair();
    }
}
