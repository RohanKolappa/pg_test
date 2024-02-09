Format of the index file

<beginTime>-<inProgress>
<beginTimeCode>-<endTimeCode>
<gap1BeginTimeCode>-<gap1EndTimeCode>
<gap2BeginTimeCode>-<gap2EndTimeCode>
...
<gapNBeginTimeCode>-<gapNEndTimeCode>

Backwards compatibility

- The SourceClipIndex will generate an INDEX file if it doesn't find one
- The beginTimeCode is read from the frist file
- The endTimeCode is approximated based on the number of files and
  beginTimeCode

