
barco_ngs_setup_wizard_en = {
  invalidMinChar1: 'Must contain at least ',
  invalidMinChar2: ' character',
  invalidPwFormat: 'Password does not satisfy requirements',
  invalidPwMatch1: 'The passwords did not match, ',
  invalidPwMatch2: 'please try again',
  warningNtpBlank1: 'Warning: Leaving this field blank ',
  warningNtpBlank2: 'will cause the system clock to start at ',
  warningNtpBlank3: 'Jan 1970',
  invalidIPv4Incomp1: 'IP address or mask is incomplete ',
  invalidIPv4Incomp2: '(4 octets required, ',
  invalidIPv4Incomp3: 'e.g. ',
  invalidIPv4Incomp4: '192.168.0.1)',
  invalidIPv4Range1: ' octet',
  invalidIPv4Range2: ' outside the ',
  invalidIPv4Range3: '0 to 255 decimal range.',
  invalidMaskGeneric1: 'The subnet mask entered is invalid. ',
  invalidMaskGeneric2: 'Please enter a valid subnet mask ',
  invalidMaskGeneric3: 'like ',
  invalidMaskGeneric4: '"255.255.255.0"',
  statusWatchdog: 'The device should have rebooted by now. ' + 
    'Please check for problems. When you get the device rebooted again, ' + 
    'you may dismiss this screen by pressing ESC. ' + 
    'Then, resubmit by clicking the "Done" button again. ' + 
    'If the problem persists, please call customer support.',
  statusReboot: 'Rebooting. This may take a few minutes. Please wait...',
  statusChangeAddr: 'A static IP address was requested. ' +
    'Wait a minute or two then look for the device at this URL: ',
  errorServer1: 'The device responded with some issues:',
  errorServer2: 'After viewing the issues above, you may dismiss this screen ' +
    'by pressing ESC. You can then correct the reported problem(s) and ' +
    'resubmit by clicking on the "Done" button again. If you don\'t know ' +
    'what to do, please call customer support.',
  errorLost: 'The connection to the NGS device was lost. ' + 
    'Please check your network. When the connection is restored you may ' + 
    'dismiss this screen by pressing ESC. ' + 
    'Then, resubmit by clicking the "Done" button again. ' + 
    'If the problem persists, please call customer support.',
  statusSubmit: 'Submitting changes to your device, one moment...'
};

function barco_ngs_setup_wizard_en_plural(num) {
  return num===1 ? '' : 's';
}

function barco_ngs_setup_wizard_en_plural_is(num) {
  return num===1 ? ' is' : 's are';
}

