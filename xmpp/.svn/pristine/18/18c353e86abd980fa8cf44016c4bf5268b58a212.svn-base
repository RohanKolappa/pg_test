package org.jivesoftware.openfire.auth;

import org.jivesoftware.openfire.XMPPServer;
import org.jivesoftware.openfire.user.IpvsUserProvider;
import org.jivesoftware.openfire.user.MRDebug;
import org.jivesoftware.openfire.user.UserNotFoundException;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsAuthProvider implements AuthProvider {
    private IpvsUserProvider provider = null;

    public IpvsAuthProvider() {
    }

    public void authenticate(String username, String password) throws UnauthorizedException {
        if (username == null || password == null) {
            throw new UnauthorizedException();
        }
        username = username.trim().toLowerCase();
        if (username.contains("@")) {
            // Check that the specified domain matches the server's domain
            int index = username.indexOf("@");
            String domain = username.substring(index + 1);
            if (domain.equals(XMPPServer.getInstance().getServerInfo().getXMPPDomain())) {
                username = username.substring(0, index);
            } else {
                // Unknown domain. Return authentication failed.
                throw new UnauthorizedException();
            }
        }
        try {
            if (!password.equals(getPassword(username))) {
                throw new UnauthorizedException();
            }
        } catch (UserNotFoundException unfe) {
            throw new UnauthorizedException();
        }
        // Got this far, so the user must be authorized.
    }

    public void authenticate(String username, String token, String digest) throws UnauthorizedException {
        if (username == null || token == null || digest == null) {
            throw new UnauthorizedException();
        }
        username = username.trim().toLowerCase();
        if (username.contains("@")) {
            // Check that the specified domain matches the server's domain
            int index = username.indexOf("@");
            String domain = username.substring(index + 1);
            if (domain.equals(XMPPServer.getInstance().getServerInfo().getXMPPDomain())) {
                username = username.substring(0, index);
            } else {
                // Unknown domain. Return authentication failed.
                throw new UnauthorizedException();
            }
        }
        try {
            String password = getPassword(username);
            String anticipatedDigest = AuthFactory.createDigest(token, password);
            if (!digest.equalsIgnoreCase(anticipatedDigest)) {
                throw new UnauthorizedException();
            }
        } catch (UserNotFoundException unfe) {
            throw new UnauthorizedException();
        }
        // Got this far, so the user must be authorized.
    }

    public boolean isPlainSupported() {
        return true;
    }

    public boolean isDigestSupported() {
        return true;
    }

    public String getPassword(String username) throws UserNotFoundException {
        if (!supportsPasswordRetrieval()) {
            // Reject the operation since the provider is read-only
            throw new UnsupportedOperationException();
        }
        //MRDebug.log("");
        String plainText =null;
        //Connection con = null;
        //PreparedStatement pstmt = null;
        if (username.contains("@")) {
            // Check that the specified domain matches the server's domain
            int index = username.indexOf("@");
            String domain = username.substring(index + 1);
            if (domain.equals(XMPPServer.getInstance().getServerInfo().getXMPPDomain())) {
                username = username.substring(0, index);
            } else {
                // Unknown domain.
                throw new UserNotFoundException();
            }
        }
        try {
            if (provider == null) {
                provider = IpvsUserProvider.getInstance();
                if (provider == null) {
                    provider = new IpvsUserProvider();
                }
            }
            String query = "//User[Username='" + username + "']";
            plainText = provider.getPassword_ipvs(username, query);
            return plainText;
        } catch (Exception sqle) {
            throw new UserNotFoundException(sqle);
        } finally {
            if(plainText==null || plainText.length()==0){
                MRDebug.log("FAILED: NOT FOUND password for '" + username + "' (len zero)");
            }else{
                MRDebug.log("FOUND password for '" + username + "' (len non zero ? " +( plainText.length() > 0) +")");
            }
        }
    }

    public void setPassword(String username, String password) throws UserNotFoundException {
        //donothing
    }

    public boolean supportsPasswordRetrieval() {
        return true;
    }
}
