/**
  ******************************************************************************
  * @file    MDR32F9Qx_usb_default_handlers.h
  * @author  Milandr Application Team
  * @version V1.0.0
  * @date    18/03/2011
  * @brief   Default USB handlers definitions.
  ******************************************************************************
  */

#ifndef __MDR32F9Qx_USB_DEFAULT_HANDLERS_H
#define __MDR32F9Qx_USB_DEFAULT_HANDLERS_H

#include "MDR32FxQI_usb.h"

/** @addtogroup USB_Default_Handlers
  * @{
  */

/* === Standard Requests === */
#ifndef USB_DEVICE_HANDLE_GET_STATUS
#define USB_DEVICE_HANDLE_GET_STATUS(recipient, wIndex)   USB_DeviceDummyGetStatus(recipient, wIndex)
#endif

#ifndef USB_DEVICE_HANDLE_CLEAR_FEATURE
#define USB_DEVICE_HANDLE_CLEAR_FEATURE(recipient, wValue, wIndex)   USB_ERROR
#endif

#ifndef USB_DEVICE_HANDLE_SET_FEATURE
#define USB_DEVICE_HANDLE_SET_FEATURE(recipient, wValue, wIndex)     USB_ERROR
#endif

#ifndef USB_DEVICE_HANDLE_SET_ADDRESS
#define USB_DEVICE_HANDLE_SET_ADDRESS(wValue)                        USB_SUCCESS
#endif

#ifndef USB_DEVICE_HANDLE_GET_DESCRIPTOR
#define USB_DEVICE_HANDLE_GET_DESCRIPTOR(wValue, wIndex, wLength)    USB_DeviceDummyGetDescriptor(wValue, wIndex, wLength)
#endif

#ifndef USB_DEVICE_HANDLE_SET_DESCRIPTOR
#define USB_DEVICE_HANDLE_SET_DESCRIPTOR(wValue, wIndex, wLength)    USB_ERROR
#endif

#ifndef USB_DEVICE_HANDLE_GET_CONFIGURATION
#define USB_DEVICE_HANDLE_GET_CONFIGURATION                          1
#endif

#ifndef USB_DEVICE_HANDLE_SET_CONFIGURATION
#define USB_DEVICE_HANDLE_SET_CONFIGURATION(wValue)                  USB_SUCCESS
#endif

#ifndef USB_DEVICE_HANDLE_GET_INTERFACE
#define USB_DEVICE_HANDLE_GET_INTERFACE(wIndex)                      0
#endif

#ifndef USB_DEVICE_HANDLE_SET_INTERFACE
#define USB_DEVICE_HANDLE_SET_INTERFACE(wValue, wIndex)              USB_SUCCESS
#endif

#ifndef USB_DEVICE_HANDLE_SYNC_FRAME
#define USB_DEVICE_HANDLE_SYNC_FRAME(wIndex, data)                   USB_ERROR
#endif

/* === Class/Vendor Requests === */
#ifndef USB_DEVICE_HANDLE_CLASS_REQUEST
#define USB_DEVICE_HANDLE_CLASS_REQUEST                              USB_DeviceDummyClassRequest()
#endif

#ifndef USB_DEVICE_HANDLE_VENDOR_REQUEST
#define USB_DEVICE_HANDLE_VENDOR_REQUEST                             USB_DeviceDummyVendorRequest()
#endif

/* === Setup Packet Handler === */
#ifndef USB_DEVICE_HANDLE_SETUP
#define USB_DEVICE_HANDLE_SETUP                                      USB_DeviceSetupPacket
#endif

/* === Error Handler === */
#ifndef USB_DEVICE_HANDLE_ERROR
#define USB_DEVICE_HANDLE_ERROR(ep, sts, ts, ctrl)                   USB_SUCCESS
#endif

/* === CDC Specific Handlers === */
#ifndef USB_CDC_HANDLE_DATA_SENT
#define USB_CDC_HANDLE_DATA_SENT                                     USB_CDC_DummyDataSent()
#endif

#ifndef USB_CDC_HANDLE_DATA_RECEIVE
#define USB_CDC_HANDLE_DATA_RECEIVE(buffer, length)                  USB_CDC_DummyDataReceive(buffer, length)
#endif

#ifdef USB_CDC_ENCAPSULATION_SUPPORTED
#ifndef USB_CDC_HANDLE_SEND_ENCAPSULATED_CMD
#define USB_CDC_HANDLE_SEND_ENCAPSULATED_CMD(wIndex, wLength)        USB_CDC_DummySendEncapsulatedCMD(wIndex, wLength)
#endif
#ifndef USB_CDC_HANDLE_GET_ENCAPSULATED_RESP
#define USB_CDC_HANDLE_GET_ENCAPSULATED_RESP(wIndex, wLength)        USB_CDC_DummyGetEncapsulatedResp(wIndex, wLength)
#endif
#endif /* USB_CDC_ENCAPSULATION_SUPPORTED */

#ifdef USB_CDC_COMM_FEATURE_SUPPORTED
#ifndef USB_CDC_HANDLE_GET_COMM_FEATURE
#define USB_CDC_HANDLE_GET_COMM_FEATURE(wValue, wIndex, data)        USB_CDC_DummyGetCommFeature(wValue, wIndex, data)
#endif
#ifndef USB_CDC_HANDLE_SET_COMM_FEATURE
#define USB_CDC_HANDLE_SET_COMM_FEATURE(wValue, wIndex, data)        USB_CDC_DummySetCommFeature(wValue, wIndex, data)
#endif
#ifndef USB_CDC_HANDLE_CLEAR_COMM_FEATURE
#define USB_CDC_HANDLE_CLEAR_COMM_FEATURE(wValue, wIndex)            USB_CDC_DummyClearCommFeature(wValue, wIndex)
#endif
#endif /* USB_CDC_COMM_FEATURE_SUPPORTED */

#ifdef USB_CDC_LINE_CODING_SUPPORTED
#ifndef USB_CDC_HANDLE_GET_LINE_CODING
#define USB_CDC_HANDLE_GET_LINE_CODING(wIndex, data)                 USB_CDC_DummyGetLineCoding(wIndex, data)
#endif
#ifndef USB_CDC_HANDLE_SET_LINE_CODING
#define USB_CDC_HANDLE_SET_LINE_CODING(wIndex, data)                 USB_CDC_DummySetLineCoding(wIndex, data)
#endif
#endif /* USB_CDC_LINE_CODING_SUPPORTED */

#ifdef USB_CDC_CONTROL_LINE_STATE_SUPPORTED
#ifndef USB_CDC_HANDLE_CONTROL_LINE_STATE
#define USB_CDC_HANDLE_CONTROL_LINE_STATE(wValue, wIndex)            USB_CDC_DummyControlLineState(wValue, wIndex)
#endif
#endif /* USB_CDC_CONTROL_LINE_STATE_SUPPORTED */

#ifdef USB_CDC_LINE_BREAK_SUPPORTED
#ifndef USB_CDC_HANDLE_BREAK
#define USB_CDC_HANDLE_BREAK(wValue, wIndex)                         USB_CDC_DummySendBreak(wValue, wIndex)
#endif
#endif /* USB_CDC_LINE_BREAK_SUPPORTED */

/** @} */ /* End of group USB_Default_Handlers */

#endif /* __MDR32F9Qx_USB_DEFAULT_HANDLERS_H */

/******************* (C) COPYRIGHT 2011 Milandr *******************************/