package com.barco.devicevng.MediaStore.StoreServer;


public class MediaVersionManager {

	public static final String MEDIA_STORE_VERSION_2_DOT_4 = "0.24";
	public static final String MEDIA_STORE_VERSION_3_DOT_2 = "0.32";
	

	public static boolean is2dot4Media(String mediaVersion) {
		return MEDIA_STORE_VERSION_2_DOT_4.equals(mediaVersion);
	}

	public static boolean is3dot2Media(String mediaVersion) {
		return MEDIA_STORE_VERSION_3_DOT_2.equals(mediaVersion);
	}	

	public static boolean isMediaCurrentVersion(String mediaVersion,String currentServiceVersion) {
		return currentServiceVersion.equals(mediaVersion);
	}

	public static MediaStoreFileI getMediaStoreFileI(String mediaFileVersion) {
		if(MEDIA_STORE_VERSION_2_DOT_4.equals(mediaFileVersion)) {
			return new MediaStoreFile2Dot4Impl();
		} else  if(MEDIA_STORE_VERSION_3_DOT_2.equals(mediaFileVersion)) {
			return new MediaStoreFile3Dot2Impl();
		} else {
			return new MediaStoreFileImpl();
		}
	}
}
