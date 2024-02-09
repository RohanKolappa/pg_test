#ifndef __SERIAL_CONFIG_H_
#define __SERIAL_CONFIG_H_

struct serial_config {
    int                 serial_enable    ; /* Enable */
    int                 baudrate         ; /* Serial port baud rate*/
    int                 databits         ; /* Serial port data bits */
    int                 stopbits         ; /* Serial port stop bits */
    unsigned char       parity           ; /* Serial port parity */
    unsigned char       flowcontrol      ; /* Serial port flow control */
    int                 serialwait       ; /* Max waiting time before xmit */
};

#ifdef __cplusplus
extern "C" {
#endif

extern int get_tx_serial_config(HAL *hp, struct serial_config *serial_cfg);
extern int get_rx_serial_config(HAL *hp, struct serial_config *serial_cfg);

#ifdef __cplusplus
}
#endif
#endif                          /* __SERIAL_CONFIG_H_ */
