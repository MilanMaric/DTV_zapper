/* 
 * File:   device_control.h
 * Author: milan
 *
 * Created on December 19, 2015, 7:21 PM
 */

#ifndef DEVICE_CONTROL_H
#define	DEVICE_CONTROL_H

typedef struct _Handles
{
    uint32_t sourceHandle;
    uint32_t playerHandle;
    uint32_t filterHandle;
    uint32_t streamHandle;
} DeviceHandle;

int deviceInit(config_parameters *parms, DeviceHandle *handle);
void deviceDeInit(DeviceHandle *handle);
uint32_t remoteCallback(uint16_t service_number);
#endif	/* DEVICE_CONTROL_H */

