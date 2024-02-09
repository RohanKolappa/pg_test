package com.ipvs.mediaroomservice.impl;

class TimeLine {
	long WC;
	long offset;
	long timescale;
}

class SourceInstance {
	long startWC;
	int refCount;
	
	public static SourceInstance getSourceInstance(String mediaID, long mediaOffset, long timescale) {
		// Create new if required and start it
		SourceInstance si =  new SourceInstance();
		// ss.start()
		si.refCount++;
		return si;
	}
	public static SourceInstance getSourceInstanceAt(String mediaID, long mediaOffset, long timescale, long WC) {
		// Create new if required and start it
		SourceInstance si =  new SourceInstance();
		// ss.start()
		si.refCount++;
		return si;
	}

	
	public static long releaseSourceInstance(SourceInstance si) {
		si.refCount--;
		// if(si.refCount == 0) ss.delete()
		return 0; // return ss.stop() wall clock
	}

	public static void releaseSourceInstanceAt(SourceInstance si, long mediaOffset, long WC) {
		si.refCount--;
		// if(si.refCount == 0) ss.delete()
	}

}
public class SourceEntity {

	long startOffset; // MediaOffset when mediaGroupTL.offset was 0
	TimeLine timeLine;
	SourceInstance sourceInstance; // Current sourceInstance
	boolean sync;
	String mediaType;
	String mediaID;
	String pvrMediaID;
	long initOffset;
	
	private boolean isLive(String mediaType) {
		return "LIVE".equals(mediaType);
	}
	
	private long getMediaOffset() {
		return this.startOffset + this.timeLine.offset;
	}
	
	public void handleInit(String mediaID, String mediaType, long initOffset, String pvrMediaID) {
		this.mediaID = mediaID;
		this.mediaType = mediaType;
		this.initOffset = initOffset;
		this.pvrMediaID = pvrMediaID;
	}
	
	public void handleStart(TimeLine mediaGroupTL, boolean sync) {
		this.timeLine = mediaGroupTL;
		this.sync = sync;

		if(isLive(mediaType)) { 
			initOffset = mediaGroupTL.WC;// initOffset is milli secs since unix time 0
		}
		else
			this.pvrMediaID = mediaID; // Playback media
		
		// Compute the mediaOffset when mediaGroupTL.offset was 0
		this.startOffset = initOffset - mediaGroupTL.offset;
		
		if(sync) // Start the source at WC
			this.sourceInstance = SourceInstance.getSourceInstanceAt(mediaID, 
					getMediaOffset(), this.timeLine.timescale, this.timeLine.WC);
		else {   
			// Update timeLine WC since we started it late
			this.sourceInstance = SourceInstance.getSourceInstance(mediaID, 
					getMediaOffset(), this.timeLine.timescale);
			this.timeLine.WC = this.sourceInstance.startWC;
			
			// Recompute the startOffset since we started late
			this.startOffset = this.startOffset - (this.sourceInstance.startWC - mediaGroupTL.WC);
		}
	}
	
	public void handleAddStream() {
		// Adds the output in the streaming server
	}
	
	public void stopAllStreams() {
		// stops the output in the streaming server
	}
	
	public void startAllStreams() {
		// associates the output with a queue and starts it
	}
	
	public void handlePause(TimeLine mediaGroupTL, boolean sync) {
		boolean doSyncPause = sync || this.sync;
		if(doSyncPause) {
			this.timeLine = mediaGroupTL;
			SourceInstance.releaseSourceInstanceAt(this.sourceInstance, 
					getMediaOffset(), this.timeLine.WC);
		}
		else { 
			// Update the timeLine WC and offset since stopped late 
			long ssWC = SourceInstance.releaseSourceInstance(this.sourceInstance);
			this.timeLine.offset = this.timeLine.offset + (ssWC - this.timeLine.WC) * this.timeLine.timescale;
			this.timeLine.WC = ssWC;
			this.timeLine.timescale = 0;
		}
		this.sourceInstance = null;
		this.stopAllStreams();
	}
	
	public void handleResume(TimeLine mediaGroupTL, boolean sync) {
		this.startAllStreams();
		boolean doSyncResume = sync || this.sync;
		if(doSyncResume) {
			this.timeLine = mediaGroupTL;
			this.sourceInstance = SourceInstance.getSourceInstanceAt(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale, this.timeLine.WC);			
		}
		else {
			this.timeLine.timescale = 1;
			this.sourceInstance = SourceInstance.getSourceInstance(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale);
			this.timeLine.WC = this.sourceInstance.startWC;
		}
	}

	public void handleCatchup(TimeLine mediaGroupTL, boolean sync) {
		this.handlePause(mediaGroupTL, false);
		this.startAllStreams();
		boolean doSyncResume = sync || this.sync;
		this.timeLine = mediaGroupTL;
		if(doSyncResume) {
			this.sourceInstance = SourceInstance.getSourceInstanceAt(mediaID, 
					getMediaOffset(), this.timeLine.timescale, this.timeLine.WC);			
		}
		else {
			this.timeLine.timescale = 1;
			this.sourceInstance = SourceInstance.getSourceInstance(mediaID, 
					getMediaOffset(), this.timeLine.timescale);
			this.timeLine.WC = this.sourceInstance.startWC;
		}
	}
	
	public void handleStop(TimeLine mediaGroupTL, boolean sync) {
		this.handlePause(mediaGroupTL, false);
	}
	
	public void handleSkip(TimeLine mediaGroupTL, boolean sync, long skipValue) {
		this.handlePause(mediaGroupTL, false);
		this.startAllStreams();
		boolean doSyncResume = sync || this.sync;
		if(doSyncResume) {
			this.timeLine = mediaGroupTL;
			this.timeLine.offset = this.timeLine.offset + skipValue;
			this.sourceInstance = SourceInstance.getSourceInstanceAt(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale, this.timeLine.WC);			
		}
		else {
			this.timeLine.timescale = 1;
			this.timeLine.offset = this.timeLine.offset + skipValue;
			this.sourceInstance = SourceInstance.getSourceInstance(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale);
			this.timeLine.WC = this.sourceInstance.startWC;
		}
	}
	
	public void handleGoto(TimeLine mediaGroupTL, boolean sync, long newOffset) {
		this.handlePause(mediaGroupTL, false);
		this.startAllStreams();
		boolean doSyncResume = sync || this.sync;
		if(doSyncResume) {
			this.timeLine = mediaGroupTL;
			this.timeLine.offset = newOffset;
			this.sourceInstance = SourceInstance.getSourceInstanceAt(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale, this.timeLine.WC);			
		}
		else {
			this.timeLine.timescale = 1;
			this.timeLine.offset = newOffset;
			this.sourceInstance = SourceInstance.getSourceInstance(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale);
			this.timeLine.WC = this.sourceInstance.startWC;
		}
	}
	
	public void handleRWFF(TimeLine mediaGroupTL, boolean sync, long timescale) {
		this.handlePause(mediaGroupTL, false);
		this.startAllStreams();
		boolean doSyncResume = sync || this.sync;
		if(doSyncResume) {
			this.timeLine = mediaGroupTL;
			this.sourceInstance = SourceInstance.getSourceInstanceAt(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale, this.timeLine.WC);			
		}
		else {
			this.timeLine.timescale = timescale;
			this.sourceInstance = SourceInstance.getSourceInstance(pvrMediaID, 
					getMediaOffset(), this.timeLine.timescale);
			this.timeLine.WC = this.sourceInstance.startWC;
		}		
	}

}
