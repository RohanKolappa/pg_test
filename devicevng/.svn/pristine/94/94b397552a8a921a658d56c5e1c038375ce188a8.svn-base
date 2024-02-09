Call Chain:

+ MediaAgent.startStream
  + MediaAgent.getSourceTableInstance
    + MediaAgent.createSourceTableInstance
      + MediaAgent.createInputElement
      	+ MediaAgentExt.updateInput *
      + SourceTable.addSourceInstance
      + MediaAgentEx.setupExternalInput *
  + MediaAgent.getMediaSourceElement
  + MediaAgent.createOutputElement
    + MediaAgentExt.updateOutput *
  + SourceTable.addOutput

+ MediaAgent.stopStream
  + SourceTable.deleteOutput
  + MediaAgent.releaseMediaSourceElement
    + MediaAgent.deleteSourceTableInstance
      + SourceTable.deleteSourceInstance
      + MediaAgentExt.teardownExternalInput *