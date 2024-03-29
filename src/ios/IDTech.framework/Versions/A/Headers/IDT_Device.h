//
//  IDT_Device.h
//  
//  IDT_Device SDK
//  V1.01.006
//  ceated by Xinhu Li on 14-1-9. IDTECH. 
//


#ifdef __x86_64__
    #import <Cocoa/Cocoa.h>
    #import <IOBluetooth/objc/IOBluetoothDevice.h>
    #import <IOBluetooth/objc/IOBluetoothSDPUUID.h>
    #import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>
    #import <IOBluetoothUI/objc/IOBluetoothDeviceSelectorController.h>

#else
    #import <ExternalAccessory/ExternalAccessory.h>
    #import <UIKit/UIKit.h>
#ifndef __i386__
#import "UniPay.h"
#import "uniMag.h"
#endif


#endif


#import "IDT_Device.h"
#import "IDTMSRData.h"
#import "IDTCommon.h"
#import "APDUResponse.h"
#import "IDTEMVData.h"

#import "CTLSResponse.h"




/** Protocol methods established for IDT_Device class  **/
@protocol IDT_Device_Delegate <NSObject>

@optional

-(void) deviceConnected; //!<Fires when device connects.  If a connection is established before the delegate is established (no delegate to send initial connection notification to), this method will fire upon establishing the delegate.
-(void) deviceDisconnected; //!<Fires when device disconnects.
- (void) plugStatusChange:(BOOL)deviceInserted; //!<Monitors the headphone jack for device insertion/removal.
//!< @param deviceInserted TRUE = device inserted, FALSE = device removed
- (void) dataInOutMonitor:(NSData*)data  incoming:(BOOL)isIncoming; //!<All incoming/outgoing data going to the device can be monitored through this delegate.
//!< @param data The serial data represented as a NSData object
//!< @param isIncoming The direction of the data
//!<- <c>TRUE</c> specifies data being received from the device,
//!<- <c>FALSE</c> indicates data being sent to the device.

- (void) swipeMSRData:(IDTMSRData*)cardData;//!<Receives card data from MSR swipe.
//!< @param cardData Captured card data from MSR swipe

- (void) deviceMessage:(NSString*)message;//!<Receives messages from the framework
//!< @param message String message transmitted by framework


/**
 UniPay ICC Event
 This function will be called when an ICC is attached or detached from reader. Applies to UniPay only
 
 @param nICC_Attached Can be one of the following values:
 - 0x01: ICC attached while reader is idle
 - 0x00: ICC detached while reader is idle
 - 0x11: ICC attached while reader is in MSR mode
 - 0x10: After ICC Powered On, ICC Card Removal,Power off ICC
 
 @code
 -(void) UniPay_EventFunctionICC: (Byte) nICC_Attached
 {
    switch (nICC_Attached) {
        case 0x01:
        case 0x11:
        {
            LOGI(@"ICC event: ICC attached.");
        }
        break;
 
        case 0x00:
        case 0x10:
        {
            LOGI(@"ICC event: ICC detached.");
        }
        break;
    }
 }
 @endcode
 */
-(void) UniPay_EventFunctionICC: (Byte) nICC_Attached;

/**
 EMV Transaction Data
 
 This protocol will receive results from IDT_Device::startEMVTransaction:otherAmount:timeout:cashback:additionalTags:()
 
 
 @param emvData EMV Results Data.  Result code, card type, encryption type, masked tags, encrypted tags, unencrypted tags and KSN

 @param error The error code as defined in the errors.h file
 
 
 */
- (void) emvTransactionData:(IDTEMVData*)emvData errorCode:(int)error;


/**
 CTLS Transaction Data
 
 This protocol will receive results from IDT_Device::ActivateTransaction:timeout:()
 
 
 @param ctlsData CTLS Results Data.  MSR track data, clearning record, Result code, card type, encryption type, masked tags, encrypted tags, unencrypted tags and KSN
 
 @param status The status code
 
 
 */
- (void) ctlsTransactionData:(CTLSResponse*)ctlsData statusCode:(Byte)status;



/**
 Pinpad data delegate protocol
 
 Receives data from pinpad methods
 
  @param value encrypted data returned from IDT_Device::getEncryptedData:minLength:maxLength:messageID:language:(), or encrypted account number returned from IDT_Device::getCardAccount:max:line1:line2:(). String value returned from IDT_Device::getAmount:maxLength:messageID:language:() or IDT_Device::getNumeric:minLength:maxLength:messageID:language:(). PINblock returned from IDT_Device::getEncryptedPIN:keyType:line1:line2:line3:()
  @param KSN Key Serial Number returned from IDT_Device::getEncryptedPIN:keyType:line1:line2:line3:(), IDT_Device::getCardAccount:max:line1:line2:() or IDT_Device::getEncryptedData:minLength:maxLength:messageID:language:()
  @param event EVENT_PINPAD_Types PINpad event that solicited the data capture
 
 @code
 typedef enum{
    EVENT_PINPAD_UNKNOWN = 11,
    EVENT_PINPAD_ENCRYPTED_PIN,
    EVENT_PINPAD_NUMERIC,
    EVENT_PINPAD_AMOUNT,
    EVENT_PINPAD_ACCOUNT,
    EVENT_PINPAD_ENCRYPTED_DATA,
    EVENT_PINPAD_CANCEL,
    EVENT_PINPAD_TIMEOUT,
    EVENT_PINPAD_FUNCTION_KEY,
    EVENT_PINPAD_DATA_ERROR
 }EVENT_PINPAD_Types;
 @endcode
 */
- (void) pinpadData:(NSData*)value keySN:(NSData*)KSN event:(EVENT_PINPAD_Types)event;


@end
/**
 Class to drive the IDTech device
 */

#ifdef __x86_64__

@interface IDT_Device : NSObject <NSStreamDelegate>{
    id<IDT_Device_Delegate> delegate;
    IOBluetoothDevice *mBluetoothDevice;
}
#else
#ifdef __i386__
@interface IDT_Device : NSObject<EAAccessoryDelegate, NSStreamDelegate>{
    id<IDT_Device_Delegate> delegate;
    id<IDT_Device_Delegate> delegate2;
}
#else
@interface IDT_Device : NSObject<EAAccessoryDelegate, NSStreamDelegate,UniPay_Delegate>{
    id<IDT_Device_Delegate> delegate;
    id<IDT_Device_Delegate> delegate2;
}
#endif

#endif


@property(strong) id<IDT_Device_Delegate> delegate;  //!<- Reference to IDT_Device_Delegate.
@property(strong) id<IDT_Device_Delegate> delegate2;  //!<- Reference to IDT_Device_Delegate.



/**
 * SDK Version
 - All Devices
 *
 Returns the current version of IDTech.framework

 @retval  Framework version
 */
+(NSString*) SDK_version;

/**
 * Singleton Instance
 - All Devices
 *
 Establishes an singleton instance of IDT_Device class.
 
 @retval  Instance of IDT_Device
 */
+(IDT_Device*) sharedController;

/**
 * Sets the IDTech Device Type
 - All Devices
 *
 Tells the framework what device type to configure for, in addition to connection type (BlueTooth or USB-HID) when applicable.  Default is BTPay 200 over iOS
 
 @param deviceType  Select from the IDT_DEVICE_Types enumeration
 
 @code
 typedef enum{
    IDT_DEVICE_BTPAY_IOS = 0,
    IDT_DEVICE_BTPAY_OSX_BT,
    IDT_DEVICE_BTPAY_OSX_USB,
    IDT_DEVICE_UNIPAY_IOS,
    IDT_DEVICE_UNIPAY_OSX_USB,
    IDT_DEVICE_UNIPAYII_IOS,
    IDT_DEVICE_UNIPAYII_OSX_USB
 }IDT_DEVICE_Types;

 @endcode
 

 */
+(void) setDeviceType:(IDT_DEVICE_Types)deviceType;

/**
 Initialize class.
  - All Devices
 
 DEPRECATED: Use singleton instance [IDT_Device sharedConroller];
 */
-(id)init;

/**
 Opens connection to device.
  - All Devices
 
 DEPRECATED: Use singleton instance [IDT_Device sharedConroller] to automatically open/close device
  */
-(BOOL)open;

 /**
  Closes connection to device. 
   - All Devices
  
  DEPRECATED: Use singleton instance [IDT_Device sharedConroller] to automatically open/close device
   */
 -(void)close;

 /**
  Destroys connection to device.
   - All Devices
  
  DEPRECATED: Use singleton instance [IDT_Device sharedConroller] to automatically open/close device
   */
 -(void)destroy;



/**
 * Polls device for EMV L1 Version
 - UniPayII
 *
 * @param response Response returned of Level 1 Version
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getEMVL1Version:(NSString**)response;

/**
 * Polls device for EMV L2 Version
 - UniPayII
 *
 * @param response Response returned of Level 2 Version
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getEMVL2Version:(NSString**)response;

/**
 * Polls device for EMV Kernel Version
  - BTPay 200
 *
 * @param response Response returned of Kernel Version
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()

 *
 */
-(RETURN_CODE) getEMVKernelVersion:(NSString**)response;



/**
 * Activate Transaction
 *
 - Vendi Mobile
 Initiates a CTLS transaction
 
 Use this command when the ctls reader is in “Poll on Demand” mode to begin an EMV or contactless MagStripe Card transaction. When the reader is in “Poll on Demand” mode, the RF is turned on only after receiving an Activate Transaction command. When a valid Activate Transaction command is sent to the ctls reader, it starts polling for cards.
 
 If the ctls reader does not find a supported card (an AID that matches one of the configured AIDs in the reader) for the specified time duration, it times out and ends the transaction. If the ctls reader finds a card within the specified time interval, it attempts to carry out the transaction. The transaction flow between the reader and the card depends on the type of card detected.
 
 If the transaction is successful, the reader returns the data in CTLSResponse. If the transaction is not successful, yet it proceeded into the transaction state machine, the reader returns a Failed Transaction Record in the response data. The presence and format of the Clearing Record, Track Data and Failed Transaction record depends on the type of card that was detected.
 
 Note: While an Activate command is in progress, only a Cancel may be sent. Do not send other commands until Activate Transaction has completed, because the reader will interpret these as a Cancel Transaction command.
 
 Results are returned in delegate ctlsTransactionData
 

 @param tags Activate TLV tags
 @param timeout Timeout value in seconds.

 
 Activate TVL Tag | Description | Format | Length
  ------ | ------ | ------ | ------
 9A | Transaction Date | n6 (YYMMDD) | 3
 9C | Transction Type | n2 | 2
 5F2A | Transaction Currency Code | n2 | 2
 5F36 | Transaction Currency Exponent | n1 | 1
 9F02 | Amount, Authorized | n12 | 6
 9F03 | Amount Other | n12 | 6
 9F1A | Terminal Country Code |  n3| 2
 9F21 | Transaction Time | n6 (HHMMSS} | 3
 9F5A | Terminal Transaction Type | b | 1
 
 Transaction Types: 0x00 = Purchase Goods/Services, 0x20 = Refund
 Terminal Transaction Type (Interac)  0x00 = Purchase, 0x01 = Refund
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0xFF00: Accept the online transaction RETURN_CODE_EMV_APPROVED
 - 0xFF01: Decline the online transaction RETURN_CODE_EMV_DECLINED
 - 0xFF02: Request to go online RETURN_CODE_EMV_GO_ONLINE
 - 0xFF03: Transaction is terminated RETURN_CODE_EMV_FAILED
 - 0xFF05: ICC format error or ICC missing data error RETURN_CODE_EMV_SYSTEM_ERROR
 - 0xFF07: ICC didn't accept transaction RETURN_CODE_EMV_NOT_ACCEPTED
 - 0xFF0A: Application may fallback to magstripe technology RETURN_CODE_EMV_FALLBACK
 - 0xFF0C: Transaction was cancelled RETURN_CODE_EMV_CANCEL
 - 0xFF0D: Timeout RETURN_CODE_EMV_TIMEOUT
 - 0xFF0F: Other EMV Error RETURN_CODE_EMV_OTHER_ERROR
 - 0xFF10: Accept the offline transaction RETURN_CODE_EMV_OFFLINE_APPROVED
 - 0xFF11: Decline the offline transaction RETURN_CODE_EMV_OFFLINE_DECLINED
 
 
 
 \par Converting TLV to NSMutableDictionary
 
 EMV data is  received in TLV (Tag, Length, value) format:
 `950500000080009B02E8009F2701018A025A339F26080C552B9364D55CE5`
 
 This data contains the following EMV tags/values:
 
 Tag | Length | Value
 ----- | ----- | -----
 9502 | 06 | 000000001995
 9A | 03 | 140530
 9C | 01 | 00
 
 An example how to create an NSMutableDictionary with these values follows.
 
 @code
 -(NSMutableDictionary*) createTLVDict{
 
 NSMutableDictionary *emvTags = [[NSMutableDictionary alloc] initWithCapacity:0];
 
 [emvTags setObject:@"000000001995" forKey:@"9502"];
 [emvTags setObject:@"140530" forKey:@"9A"];
 [emvTags setObject:@"00" forKey:@"9C"];
 
 return emvTags;
 
 }
 @endcode
 
 */




-(RETURN_CODE) activateTransaction:(NSMutableDictionary *)tags timeout:(int)timeout;

/**
 * Cancel the Activate Transaction
 * - Vendi Mobile
 
 Cancels the CTLS transaction.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0xFF00: Accept the online transaction RETURN_CODE_EMV_APPROVED
 - 0xFF01: Decline the online transaction RETURN_CODE_EMV_DECLINED
 - 0xFF02: Request to go online RETURN_CODE_EMV_GO_ONLINE
 - 0xFF03: Transaction is terminated RETURN_CODE_EMV_FAILED
 - 0xFF05: ICC format error or ICC missing data error RETURN_CODE_EMV_SYSTEM_ERROR
 - 0xFF07: ICC didn't accept transaction RETURN_CODE_EMV_NOT_ACCEPTED
 - 0xFF0A: Application may fallback to magstripe technology RETURN_CODE_EMV_FALLBACK
 - 0xFF0C: Transaction was cancelled RETURN_CODE_EMV_CANCEL
 - 0xFF0D: Timeout RETURN_CODE_EMV_TIMEOUT
 - 0xFF0F: Other EMV Error RETURN_CODE_EMV_OTHER_ERROR
 - 0xFF10: Accept the offline transaction RETURN_CODE_EMV_OFFLINE_APPROVED
 - 0xFF11: Decline the offline transaction RETURN_CODE_EMV_OFFLINE_DECLINED
 */
-(RETURN_CODE) cancelCTLSTransaction;

 /**
 * Start EMV Transaction Request
 *
 - BTPay 200
 Authorizes the EMV transaction  for an ICC card
 
 The tags will be returned in the emvTransactionData delegate protocol.
 
  @param amount Transaction amount value  (tag value 9F02)
  @param amtOther Other amount value, if any  (tag value 9F03)
  @param timeout Timeout value in seconds.
  @param type Transaction type (tag value 9C).
  @param tags Any other optional tags to be included in the request.  Passed as a mutable dictionary.


 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0xFF00: Accept the online transaction RETURN_CODE_EMV_APPROVED
 - 0xFF01: Decline the online transaction RETURN_CODE_EMV_DECLINED
 - 0xFF02: Request to go online RETURN_CODE_EMV_GO_ONLINE
 - 0xFF03: Transaction is terminated RETURN_CODE_EMV_FAILED
 - 0xFF05: ICC format error or ICC missing data error RETURN_CODE_EMV_SYSTEM_ERROR
 - 0xFF07: ICC didn't accept transaction RETURN_CODE_EMV_NOT_ACCEPTED
 - 0xFF0A: Application may fallback to magstripe technology RETURN_CODE_EMV_FALLBACK
 - 0xFF0C: Transaction was cancelled RETURN_CODE_EMV_CANCEL
 - 0xFF0D: Timeout RETURN_CODE_EMV_TIMEOUT
 - 0xFF0F: Other EMV Error RETURN_CODE_EMV_OTHER_ERROR
 - 0xFF10: Accept the offline transaction RETURN_CODE_EMV_OFFLINE_APPROVED
 - 0xFF11: Decline the offline transaction RETURN_CODE_EMV_OFFLINE_DECLINED
 
 
 
 \par Converting TLV to NSMutableDictionary
 
 EMV data is  received in TLV (Tag, Length, value) format:
 `950500000080009B02E8009F2701018A025A339F26080C552B9364D55CE5`
 
 This data contains the following EMV tags/values:
 
 Tag | Length | Value
 ----- | ----- | -----
 95 | 05 | 0000008000
 9B | 02 | E800
 9F27 | 01 | 01
 8A | 02 | 5A33
 9F26 | 08 | 0C552B9364D55CE5
 
 An example how to create an NSMutableDictionary with these values follows.
 
 @code
 -(NSMutableDictionary*) createTLVDict{
 
 NSMutableDictionary *emvTags = [[NSMutableDictionary alloc] initWithCapacity:0];
 
 [emvTags setObject:@"0000008000" forKey:@"95"];
 [emvTags setObject:@"E800" forKey:@"9B"];
 [emvTags setObject:@"01" forKey:@"9F27"];
 [emvTags setObject:@"5A33" forKey:@"8A"];
 [emvTags setObject:@"0C552B9364D55CE5" forKey:@"9F26"];
 
 return emvTags;
 
 }
 @endcode
 
 */




-(RETURN_CODE) startEMVTransaction:(float)amount otherAmount:(float)amtOther timeout:(int)timeout transactionType:(unsigned char)type additionalTags:(NSMutableDictionary *)tags;

/**
 * Start  Transaction Request
 *
 - UniPayII
 Authorizes the EMV transaction  for an ICC card
 
 The tags will be returned in the emvTransactionData delegate protocol.
 
 By default, auto authorize is ENABLED.  If auto authorize is DISABLED, this function will complete with a return of EMV_RESULT_CODE_START_TRANSACTION_SUCCESS to emvTransactionData delegate protocol, and then IDT_Device::authenticateTransaction() must be executed.  If auto authorize is ENABLED (default), IDT_Device:authentiateTransaction() will automatically be executed after receiving the result EMV_RESULT_CODE_START_TRANSACTION_SUCCESS.  The auto authorize can be enabled/disabled with IDT_Device::disableAutoAuthenticateTransaction:()
 
 @param tags Transaction Tags
 @param fallback TRUE = support fallback
 @param timeout Timeout value in seconds.
 @param forceOnline Forces EMV transaction online

 Tags for transaction data
  Tag | Description
  ===== | =====
 9F02 | Amount, Authorised(Numeric)
 9C | Transaction Type
 5F2A | Transaction Currency Code
 9A | Transaction Date
 9F21 | Transaction Time
 9F03 | Amount, Other(Numeric)
 
 
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0xFF00: Accept the online transaction RETURN_CODE_EMV_APPROVED
 - 0xFF01: Decline the online transaction RETURN_CODE_EMV_DECLINED
 - 0xFF02: Request to go online RETURN_CODE_EMV_GO_ONLINE
 - 0xFF03: Transaction is terminated RETURN_CODE_EMV_FAILED
 - 0xFF05: ICC format error or ICC missing data error RETURN_CODE_EMV_SYSTEM_ERROR
 - 0xFF07: ICC didn't accept transaction RETURN_CODE_EMV_NOT_ACCEPTED
 - 0xFF0A: Application may fallback to magstripe technology RETURN_CODE_EMV_FALLBACK
 - 0xFF0C: Transaction was cancelled RETURN_CODE_EMV_CANCEL
 - 0xFF0D: Timeout RETURN_CODE_EMV_TIMEOUT
 - 0xFF0F: Other EMV Error RETURN_CODE_EMV_OTHER_ERROR
 - 0xFF10: Accept the offline transaction RETURN_CODE_EMV_OFFLINE_APPROVED
 - 0xFF11: Decline the offline transaction RETURN_CODE_EMV_OFFLINE_DECLINED
 
 
 
 \par Converting TLV to NSMutableDictionary
 
 EMV data is  received in TLV (Tag, Length, value) format:
 `950500000080009B02E8009F2701018A025A339F26080C552B9364D55CE5`
 
 This data contains the following EMV tags/values:
 
 Tag | Length | Value
 ----- | ----- | -----
 95 | 05 | 0000008000
 9B | 02 | E800
 9F27 | 01 | 01
 8A | 02 | 5A33
 9F26 | 08 | 0C552B9364D55CE5
 
 An example how to create an NSMutableDictionary with these values follows.
 
 @code
 -(NSMutableDictionary*) createTLVDict{
 
 NSMutableDictionary *emvTags = [[NSMutableDictionary alloc] initWithCapacity:0];
 
 [emvTags setObject:@"0000008000" forKey:@"95"];
 [emvTags setObject:@"E800" forKey:@"9B"];
 [emvTags setObject:@"01" forKey:@"9F27"];
 [emvTags setObject:@"5A33" forKey:@"8A"];
 [emvTags setObject:@"0C552B9364D55CE5" forKey:@"9F26"];
 
 return emvTags;
 
 }
 @endcode
 
 */

-(RETURN_CODE) startTransaction:(NSMutableDictionary *)tags allowFallback:(BOOL)fallback timeout:(int)timeout forceOnline:(BOOL)forceOnline;

/**
 * Complete EMV Transaction Online Request
  - BTPay 200
 *
 Completes an online EMV transaction request by the card
 
The tags will be returned in the response parameter.
 
 @param result Determines if connection to host was successful.
 @param tags Host response tag
 @param response returns the response tags
 
 Tag | Length | Description
 ----- | ----- | -----
 8A | 2 | Data element Authorization Response Code. Mandatory
 91 | 8-16 | Issuer Authentication Data. Optional
 71 | 0-256 | Issuer Scripts. Optional
 72 | 0-256 | Issuer Scripts. Optional
 

 
 
 *  @retval RETURN_CODE:
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0xFE00: Authorization Accepted - RETURN_CODE_EMV_AUTHORIZATION_ACCEPTED
 - 0xFE01: Online Failure - RETURN_CODE_EMV_AUTHORIZATION_UNABLE_TO_GO_ONLINE
 - 0xFE02: Technical Issue - RETURN_CODE_EMV_AUTHORIZATION_TECHNICAL_ISSUE
 - 0xFE03: Declined - RETURN_CODE_EMV_AUTHORIZATION_DECLINED
 - 0xFE04: Issuer Referral - RETURN_CODE_EMV_AUTHORIZATION_ISSUER_REFERRAL
 
 
 
 \par Converting TLV to NSMutableDictionary
 
 EMV data is  received in TLV (Tag, Length, value) format:
 `950500000080009B02E8009F2701018A025A339F26080C552B9364D55CE5`
 
 This data contains the following EMV tags/values:
 
 Tag | Length | Value
 ----- | ----- | -----
 95 | 05 | 0000008000
 9B | 02 | E800
 9F27 | 01 | 01
 8A | 02 | 5A33
 9F26 | 08 | 0C552B9364D55CE5
 
 An example how to create an NSMutableDictionary with these values follows.
 
 @code
 -(NSMutableDictionary*) createTLVDict{
 
 NSMutableDictionary *emvTags = [[NSMutableDictionary alloc] initWithCapacity:0];
 
 [emvTags setObject:@"0000008000" forKey:@"95"];
 [emvTags setObject:@"E800" forKey:@"9B"];
 [emvTags setObject:@"01" forKey:@"9F27"];
 [emvTags setObject:@"5A33" forKey:@"8A"];
 [emvTags setObject:@"0C552B9364D55CE5" forKey:@"9F26"];
 
 return emvTags;
 
 }
 @endcode
 
 */
-(RETURN_CODE) completeOnlineEMVTransaction:(EMV_AUTHORIZATION_RESULT)result hostResponseTags:(NSMutableDictionary *)tags responseTags:(IDTEMVData**)response;



/**
 * Set Certificate Revocation List Entry
  - BTPay 200
 *
 Sets the CRL entry as specified by the CRLEntry structure
 
 @param key CRLEntry containing the RID, Index, and serial number to set
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setCRL:(CRLEntry)key;

/**
 * Remove Certificate Revocation List RID
  - BTPay 200
 *
 Removes all CRLEntry as specified by the RID and Index passed as a parameter in the CRLEntry structure
 
 @param key CRLEntry containing the RID and Index  of the of the entries to remove
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeCRL:(CRLEntry)key;

/**
 * Remove Certificate Revocation List unit
  - BTPay 200
 *
 Removes a single CRLEntry as specified by the RID/Index/Serial Number passed as a parameter in the CRLEntry structure
 
 @param key CRLEntry containing the RID, Index and serial number of the of the entry to remove
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeCRLUnit:(CRLEntry)key;

/**
 * Retrieve the Certificate Revocation List specific to RID and index
  - BTPay 200
 *
 Returns all the serial numbers for a specific RID/Index on the CRL.
 
  @param rid RID of the certificate to search for
  @param response Response returned as an NSArray of NSData* objects for each CRLEntry:
            5 bytes: AID
            1 byte: Index
            3 bytes: Serial Number
 
 The following code can map the NSData entries into crlEntries
 @code
 NSArray* returnArray;
 [[IDT_Device sharedController] retrieveCRLForRID:@"a000000003" response:&returnArray];
 for (NSData* obj in returnArray) {
    Byte *keyByte = (Byte*) obj.bytes;
    CRLEntry* crl = (CRLEntry*)keyByte;
 }
 @endcode
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) retrieveCRLForRID:(NSString*)rid response:(NSArray**)response;

/**
 * Retrieve the Certificate Revocation List
  - BTPay 200
  - UniPay II
 *
 Returns all the RID in the CRL.
 @param response Response returned as an NSArray of NSData objects (either 5 or 9 bytes each):
  - On BTPay, 5-byte objects for each RID
  - On UniMagII, 9-bytes objects:  5-bytes RID, 1-byte Index, 3-byte Serial Number
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
  */
-(RETURN_CODE) retrieveCRLList:(NSMutableArray**)response;

/**
 * Remove Certificate Revocation List
 *
 - UniPay II
 Removes all CRLEntry entries
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_BTPay::device_getResponseCodeString:()
 
 */
-(RETURN_CODE) removeCRLList;

/**
 * Retrieve Certificate Authority Public Key
  - BTPay 200
 *
 Retrieves the CAPK as specified by the RID/Index  passed as a parameter in the CAKey structure.  The CAPK will be in the response parameter
 
 @param rid The RID of the key to retrieve
 @param index The Index of the key to retrieve
 @param response Response returned as a CAKey
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 

 */
-(RETURN_CODE) retrieveCAPK:(NSString*)rid index:(NSString*)index response:(CAKey**)response;

/**
 * Remove Certificate Authority Public Key
  - BTPay 200
 *
 Removes the CAPK as specified by the RID/Index passed as a parameter in the CAKey structure
 
 @param rid RID of the key to remove
 @param index Index of the key to remove
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeCAPK:(NSString*)rid index:(NSString*)index ;

/**
 * Set Certificate Authority Public Key
  - BTPay 200
 *
 Sets the CAPK as specified by the CAKey structure
 
 @param key CAKey containing the RID, Index, and key data to set
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setCAPK:(CAKey)key;

/**
 * Retrieve the Certificate Authority Public Key list
  - BTPay 200
 *
 Returns all the CAPK RID and Index.  Populates response parameter with an array of NSString items, 12 characters each, characters 1-10 RID, characters 11-12 index.
 
 @param response Response returned contains an NSArray of NSString items, 12 characters each, characters 1-10 RID, characters 11-12 index.  Example "a00000000357" = RID a00000003, Index 57
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
*/
-(RETURN_CODE) retrieveCAPKList:(NSArray**)response;



/**
 * Retrieve Terminal Data
  - BTPay 200
 *
 Retrieves the Terminal Data.  The data will be in the response parameter
 
 @param response Response returned as a TerminalData
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) retrieveTerminalData:(TerminalData**)response;

/**
 * Remove Terminal Data
  - BTPay 200
 *
 Removes the Terminal Data
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeTerminalData;

/**
 * Set Terminal Data
  - BTPay 200
 *
 Sets the Terminal Data as specified by the TerminalData structure passed as a parameter
 
 @param data TerminalData configuration file
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setTerminalData:(TerminalData)data;

/**
 * Retrieve Terminal Data
 - UniPay II
 *
 Retrieves the Terminal Data .  The Terminal Data will be in the response parameter responseData
 
 @param responseData  The response returned from the method as a dictionary with Key/Object to match TagValues as follows:
 
 9F1A | Terminal Country Code
 9F35 | Terminal Type
 9F33 | Terminal Capability
 9F40 | Additional Terminal Capability
 9F1E | IFD Serial Number
 9F15 | Merchant Category Code
 9F16 | Merchant Identifier
 9F1C | Terminal Identification
 9F4E | Merchant Name and Location
 DF10 | LANGUAGE
 DF11 | Use Trans Log
 DF13 | TAC-Default
 DF14 | TAC-Denial
 DF15 | TAC-Online
 DF17 | Threshold Value for Biased Random Selection
 DF18 | Target Percentage For Random Transaction Selection
 DF19 | Maximum Target Percentage For Random Transaction Selection
 DF20 | Trace
 DF22 | Merchant Forced Transaction Online
 DF25 | Default DDOL
 DF26 | Use Revocation list
 DF27 | Use Exception  list
 DF28 | TDOL
 DF30 | Online DOL
 DF62 | Application Selection Flag
 DF63 | Transaction Reference Currency
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 
 
 */
-(RETURN_CODE) retrieveTerminalDataUniPay:(NSDictionary**)responseData;

/**
 * Retrieve Application Data by AID
 - UniPay II
 *
 Retrieves the Application Data as specified by the AID name passed as a parameter.  The AID will be in the response parameter responseAID
 
 @param AID Name of ApplicationID in ASCII, example "A0000000031020".  Must be between 5 and 16 characters
 @param responseAID  The response returned from the method as a dictionary with Key/Object to match TagValues as follows:
 
 Tag | Description
 ===== | =====
 5F57 | Account Type
 9F01 | Acquirer Identifier
 9F09 | Terminal application version number
 5F36 | Transaction Currency Exponent
 9F1B | Terminal Floor Limit
 9F49 | Dynamic Data Authentication Data Object List(DDOL)
 97 | Transaction Certificate Data Object List(TDOL)
 9F39 | POS Entry Mode
 9F3C | Transaction Reference Currency Code
 9F3D | Transaction Reference Currency Exponent
 99 | PIN Block
 DF10 | LANGUAGE
 DF11 | Use Trans Log
 DF13 | TAC-Default
 DF14 | TAC-Denial
 DF15 | TAC-Online
 DF17 | Threshold Value for Biased Random Selection
 DF18 | Target Percentage For Random Transaction Selection
 DF19 | Maximum Target Percentage For Random Transaction Selection
 DF20 | Trace
 DF22 | Merchant Forced Transaction Online
 DF25 | Default DDOL
 DF26 | Use Revocation list
 DF27 | Use Exception  list
 DF28 | TDOL
 DF30 | Online DOL
 DF62 | Application Selection Flag
 DF63 | Transaction Reference Currency Conversion
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()

 
 
 
 */
-(RETURN_CODE) retrieveApplicationDataUniPay:(NSString*)AID response:(NSDictionary**)responseAID;

/**
 * Retrieve Application Data by AID
  - BTPay 200
 *
 Retrieves the Application Data as specified by the AID name passed as a parameter.  The AID will be in the response parameter responseAID
 
 @param AID Name of ApplicationID in ASCII, example "A0000000031020".  Must be between 5 and 16 characters
 @param responseAID  The response returned from the method
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 

 
 */
-(RETURN_CODE) retrieveApplicationData:(NSString*)AID response:(ApplicationID**)responseAID;

/**
 * Remove Application Data by AID
  - BTPay 200
  - UniPay II
 *
 Removes the Application Data as specified by the AID name passed as a parameter
 
 @param AID Name of ApplicationID in ASCII, example "A0000000031020".  Must be between 5 and 16 characters
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeApplicationData:(NSString*)AID;

/**
 * Set Application Data by AID
 - UniPay II
 *
 Sets the Application Data as specified by the dictionary containing Tags (dictionary keys) and Values (dictionary objects) according to the following table
 
 Tag | Description
 ===== | =====
 5F57 | Account Type
 9F01 | Acquirer Identifier
 9F09 | Terminal application version number
 5F36 | Transaction Currency Exponent
 9F1B | Terminal Floor Limit
 9F49 | Dynamic Data Authentication Data Object List(DDOL)
 97 | Transaction Certificate Data Object List(TDOL)
 9F39 | POS Entry Mode
 9F3C | Transaction Reference Currency Code
 9F3D | Transaction Reference Currency Exponent
 99 | PIN Block
 DF10 | LANGUAGE
 DF11 | Use Trans Log
 DF13 | TAC-Default
 DF14 | TAC-Denial
 DF15 | TAC-Online
 DF17 | Threshold Value for Biased Random Selection
 DF18 | Target Percentage For Random Transaction Selection
 DF19 | Maximum Target Percentage For Random Transaction Selection
 DF20 | Trace
 DF22 | Merchant Forced Transaction Online
 DF25 | Default DDOL
 DF26 | Use Revocation list
 DF27 | Use Exception  list
 DF28 | TDOL
 DF30 | Online DOL
 DF62 | Application Selection Flag
 DF63 | Transaction Reference Currency Conversion
 
 @param aidName aidName AID name.  Example "a0000000031010"
 @param data NSDictionary with Tags/Values for the AID configuration file
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setApplicationDataUniPay:(NSString*)aidName configData:(NSMutableDictionary*)data;

/**
 * Set Terminal Data
 - UniPay II
 *
 Sets the Terminal Data as specified by the dictionary containing Tags (dictionary keys) and Values (dictionary objects) according to the following table
 
 Tag | Description
 ===== | =====
 9F1A | Terminal Country Code
 9F35 | Terminal Type
 9F33 | Terminal Capability
 9F40 | Additional Terminal Capability
 9F1E | IFD Serial Number
 9F15 | Merchant Category Code
 9F16 | Merchant Identifier
 9F1C | Terminal Identification
 9F4E | Merchant Name and Location
 DF10 | LANGUAGE
 DF11 | Use Trans Log
 DF13 | TAC-Default
 DF14 | TAC-Denial
 DF15 | TAC-Online
 DF17 | Threshold Value for Biased Random Selection
 DF18 | Target Percentage For Random Transaction Selection
 DF19 | Maximum Target Percentage For Random Transaction Selection
 DF20 | Trace
 DF22 | Merchant Forced Transaction Online
 DF25 | Default DDOL
 DF26 | Use Revocation list
 DF27 | Use Exception  list
 DF28 | TDOL
 DF30 | Online DOL
 DF62 | Application Selection Flag
 DF63 | Transaction Reference Currency
 
 @param data NSDictionary with Tags/Values for the Terminal configuration file
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setTerminalDataUniPay:(NSMutableDictionary*)data;

/**
 * Set Application Data by AID
  - BTPay 200
 *
 Sets the Application Data as specified by the ApplicationID structure passed as a parameter
 
 @param data ApplicationID configuration file
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setApplicationData:(ApplicationID)data;

/**
 * Retrieve AID list
  - BTPay 200
  - UniPay II
 *
 Returns all the AID names supported by the terminal.
 
 @param response Returns a NSArray of NSString of AID Names
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) retrieveAIDList:(NSArray**)response;



/**
 * Keep Accessory Connected
  - BTPay 200
 *
 * Framework attempts to disconnect from accessory whever application goes to backround.  Setting this value to TRUE will disable framework disconnect attempts.  NOTE:  ExternalAccessory may still disconnect device when going to background by default.  If you want to stay connected to device, you must also set the .pist "Required Background Modes" to "App communicates using CoreBluetooth", "App communicates with an accessory", and "App shares data using CoreBluetooth"
 *
 * @param stayConnected  TRUE = stay connected while in background (assuming .plist is properly configured)
 */
-(void)stayConnected:(BOOL)stayConnected;


/**
 * Exchange Encrypted APDU
- UniPay
 *
 * Sends an encrypted APDU packet to the ICC.  If successful, response is returned in APDUResult class instance in response parameter.
 
 @param dataAPDU  APDU data packet
 @param response encrypted parsed APDU response
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */

-(RETURN_CODE) exchangeEncryptedAPDU:(NSData*)dataAPDU response:(APDUResponse**)response;

/**
 * Exchange APDU
  - BTPay 200
  - UniPay
 *
 * Sends an APDU packet to the ICC.  If successful, response is returned in APDUResult class instance in response parameter. If encrypted, a KSN must be initially provided.  If encrypted and no KSN provided, the last provided KSN will be utilized
 
 @param dataAPDU  APDU data packet
 @param encrypted  Send data encrypted
 @param ksn For encrypted APDU, 10-byte KSN value, or nil if unencrypted or encrypted and use previous KSN
 @param response Unencrypted/encrypted parsed APDU response
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */

-(RETURN_CODE) exchangeAPDU:(NSData*)dataAPDU encrypted:(BOOL)encrypted ksn:(NSData*)ksn response:(APDUResponse**)response;

/**
 * Get APDU KSN
 - BTPay 200
 
 *
 * Retrieves the KSN used in ICC Encypted APDU usage
 
 * @param ksn Returns the encrypted APDU packet KSN
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getAPDU_KSN:(NSData**)ksn;

/**
 * Power Off ICC
 - BTPay 200
 - UniPay

 *
 * Powers down the ICC
 
 * @param error Returns the error, if any
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 If Success, empty
 If Failure, ASCII encoded data of error string
 */
-(RETURN_CODE) powerOffICC:(NSString**)error;


/**
 * Power On ICC
 - BTPay 200
 - UniPay

 *
 * Power up the currently selected microprocessor card in the ICC reader
 *
 * @param response Response returned. If Success, ATR String. If Failure, ASCII encoded data of error string
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 
 
 */
-(RETURN_CODE) powerOnICC:(NSData**)response;

/**
 * Power On ICC with Options
 - BTPay 200

 *
 * Power up the currently selected microprocessor card in the ICC reader, specifying IFS/pps options.
 
 @param options ATR Options
* @param response Response returned. If Success, ATR String. If Failure, ASCII encoded data of error string
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 
 */
-(RETURN_CODE) powerOnICC:(PowerOnStructure)options response:(NSData**)response;


/**
 * Command Acknowledgement Timout
 *
 * Sets the amount of seconds to wait for an {ACK} to a command before a timeout.  Responses should normally be received under one second.  Default is 3 seconds.
 *
 * @param nSecond  Timout value.  Valid range 1 - 60 seconds
 
 * @retval Success flag.  Determines if value was set and in range.
 */
-(BOOL) setCmdTimeOutDuration: (int) nSecond;


/**
 * Send a NSData object to device
 - All Devices
 *
 * Sends a command represented by the provide NSData object to the device through the accessory protocol.
 *
 * @param cmd NSData representation of command to execute
 * @param lrc If <c>TRUE</c>, this will wrap command with start/length/lrc/sum/end:  '{STX}{Len_Low}{Len_High} data {CheckLRC} {CheckSUM} {ETX}'
   @param response Response data
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 */
-(RETURN_CODE) sendDataCommand:(NSData*)cmd calcLRC:(BOOL)lrc response:(NSData**)response;


/**
 * Get Reader Status
 - BTPay 200
 - UniPay
 *
 Returns the reader status
 
 @param readerStatus Pointer that will return with the ICCReaderStatus results.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
@code
    ICCReaderStatus readerStatus;
    RETURN_CODE rt = [[IDT_Device sharedController] getICCReaderStatus:&readerStatus];
    if(RETURN_CODE_DO_SUCCESS != rt){
        LOGI(@"Fail");
    }
    else{
        NSString *sta;
        if(readerStatus.iccPower)
            sta =@"[ICC Powered]";
        else
            sta = @"[ICC Power not Ready]";
        if(readerStatus.cardSeated)
            sta =[NSString stringWithFormat:@"%@,[Card Seated]", sta];
        else
            sta =[NSString stringWithFormat:@"%@,[Card not Seated]", sta];
 
        LOGI(@"Card Status = %@",sta);
 }
@endcode
*/

-(RETURN_CODE) getICCReaderStatus:(ICCReaderStatus**)readerStatus;



/**
 * Set Bluetooth Address
 - BTPay 200

 *
 Sets the Bluetooth address of the device. 6 bytes, example F0DE07CCA03F.
 
 @param address 6 Byte address represented by a 12-character HEX string
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */

-(RETURN_CODE) setBluetoothAddress:(NSString*)address;


/**
 * Reboot Device
 - BTPay 200
 - UniPay

 *
 Executes a command to restart the device.
 *
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */

-(RETURN_CODE) rebootDevice;

/**
 * Clear Display
 - UniPay II
 
 *
 Clears the display of the UniPay II.
 *
 
 @param option 0=Clear First Line, 1 = Clear Second Line, 2= Clear Both Lines
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */

-(RETURN_CODE) clearDisplay:(int)option;

/**
 * Backlight Control
 - UniPay II
 
 *
 Turns on/off the backlight of the UniPay II.
 *
 
 @param turnON TRUE = Turn On, FALSE = Turn Off
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */

-(RETURN_CODE) backlightControl:(BOOL)turnON;

/**
 * Restore Default PINpad Settings
 - UniPay II
 
 *
 Executes a command to restore default PINpad settings.  PIN Length 4-12, Numeric Len 1-16;
 *
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) defaultSettingsPinPadUniPay;


/**
 * Sets the Beep Value
 - UniPayII
 
 *
 Sets a beep value on the UniPayII.
 *
 @param frequency Frequence of the beep.  Valid range 1000-20000.
 @param duration Duration in milliseconds.  Valid range 16-65535.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) sendBeepUniPayII:(int)frequency duration:(int)duration;

/**
 * Sends a Beep Value
 - BTPay 200

 *
 Executes a beep on the BT200.  The complete beep may be defined as a multiple of single beep tones.
 *
 @param beep Unsigned short array containing freq1,dur1,freq2,dur2,. . . freq#,dur#.  Frequency is in Hz and must be in the range 2000-4000. Duration is in milliseconds.
 @param num Number of tones in the beep array.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 Example Code-
 @code
 unsigned short beep[] = {0xb00,0x400,0x800,0x300};
 RETURN_CODE rt = [[IDT_Device sharedController] sendBeep:beep numberOfTones:2];
 LOGI(@"\nControl Beep Return Status Code %i ",  rts);
 @endcode
 
 */
-(RETURN_CODE) sendBeep:(unsigned short*)beep numberOfTones:(int)num;

/**
 * UniPay 200
 
 *
 Display a message on either line 1 or line 2  in the UniPayII LCD.
 
 @param message Display message, up to 16 characters
 @param line 0 = First Line, 1 = Second Line
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) displayMessageUniPayII:(NSString*)message lineNumber:(int)line;

/**
 * UniPay 200
 
 *
 Saves a prompt into UniPay memory.
 
 @param message Prompt message, up to 16 characters
 @param location Memory location 0-9
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) savePromptUniPayII:(NSString*)message location:(int)location;


/**
 - UniPay II
 
 Display a Prompt stored in UniPay II.
 
 @param prompt Prompt number, 0-9
 @param line 0 = First Line, 1 = Second Line
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) displayPromptUniPayII:(int)prompt lineNumber:(int)line;
/**
 * DisplayMessage
 - BTPay 200

 *
 Display up to 4 lines of text in the device LCD.
 
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 @param line3 Display line 3, up to 16 characters
 @param line4 Display line 4, up to 16 characters
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) displayMessage:(NSString*)line1 line2:(NSString*)line2 line3:(NSString*)line3 line4:(NSString*)line4;

/**
 * Set Enter Sleep Mode Time
 - BTPay 200

 *
Sets seconds of idle that must pass before entering sleep mode
 
 @param seconds  Amount of time (in seconds) that must pass during idle before unit goes to sleep
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setIdleTimeForSleep: (int) seconds;

/**
 * Put device to sleep
 - BTPay 200

 *
 Set device to enter  sleep mode. In sleep mode, LCD display and backlight is off. It can be waked up by key press or sending commands
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) enterSleepMode;

/**
 * Polls device for Firmware Version
 - BTPay 200
 - UniPay
 *
 * @param response Response returned of Firmware Version
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
  *
 */
-(RETURN_CODE) getFirmwareVersion:(NSString**)response;

/**
 * Polls device for Model Number
 - BTPay 200
 - UniPay
 *
 * @param response  Returns Model Number
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 *
 */
-(RETURN_CODE) getModelNumber:(NSString**)response;

/**
 * Polls device for status of the keys
 - UniPay II
 *
 * @param response  Returns Key Status.  NSData byte stream following following format: 
    PIN DUKPT Status + PIN Master Key Status + PIN Session Key Status + Account DUKPT Key Status + AccountDUKPT Key Status + Admin DUKPT Key
 PIN DUKPT Key:
 - 0: None.
 - 1: Exist
 - 0xFF: STOP
 
 PIN Master Key:
 - 0: None
 - 1: At least Exist a Master Key
 
 PIN Session Key:
 - 0: None.
 - 1: Exist
 
 Account DUKPT Key:
 - 0: None.
 - 1: Exist
 - 0xFF: STOP
 
 Account DUKPT Key:
 - 0: None.
 - 1: Exist
 - 0xFF: STOP
 
 Admin DUKPT Key:
 - 0: None.
 - 1: Exist
 - 0xFF: STOP
 
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 *
 */
-(RETURN_CODE) getKeyStatus:(NSData**)response;

/**

 * Polls device for Battery Voltage
 - UniPay
 *
 * @param response  Returns Battery Voltage as 4-chararacter string * 100.  Example: "0186" = 1.86v. "1172" = 11.72v.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getBatteryVoltage:(NSString**)response;

/**
 * Polls device for Serial Number
 - BTPay 200
 - UniPay
 *
 * @param response  Returns Serial Number
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getSerialNumber:(NSString**)response;

/**
 * Set Serial Number
 - BTPay 200
 - UniPay
 *
 Set device's serial number and Bluetooth name, then reboots device. Bluetooth name will be set as IDT_Device + Space + Serial number
 *
 @param strSN Device serial number
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setSerialNumber:(NSString*)strSN;

/**
 * Get interface device's serial number

 *
 * @param response  Returns Serial Number
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getInterfaceDeviceSN:(NSString**)response;

/**
 * Set Interface Device serial number.
 *
 EMV serial number can be set only once
 *
 @param sn Device serial number
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setInterfaceDeviceSN:(NSString*)sn;

/**
 * Get terminal identification
 *
 * @param response  Returns device terminal identification
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getTerminalIdentification:(NSString**)response;

/**
 * Set terminal identifcation
 *
 Set device's serial number and Bluetooth name, then reboots device. Bluetooth name will be set as IDT_Device + Space + Serial number
 *
 @param sn Device terminal identification
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setTerminalIdentification:(NSString*)sn;

/**
 * Polls device for current Date/Time
 - BTPay 200
 *
 * @param response Response returned as ASCII Data of Date  YYMMDDhhmmss. Example 140215171628 = Feb. 15, 2014, 28 seconds into 5:16pm.
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 

 
 @code
 NSString* response;
 RETURN_CODE rt = [[IDT_Device sharedController] getDateTime:&response];
    if (RETURN_CODE_DO_SUCCESS == rt)
    {
        LOGI* (@"Date Time (YYMMDDhhmmss) = %@",response);
    }
 @endcode
 *
 */
-(RETURN_CODE) getDateTime:(NSString**)response;


/**
 * Set device device Date/Time
 - BTPay 200
 *
 Set device's date/time
 *
 @param date Device date represented by a YYMMDDhhmmss.  Example March 12, 2014, 6:30pm (and 12 seconds) = 140312183012
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setDateTime:(NSString*)date;


/**
 * Cancel PIN Command
 - BTPay 200
 *
 
 This command can cancel IDT_Device:getEncryptedPIN:keyType:line1:line2:line3:() and IDT_Device::getNumeric:minLength:maxLength:messageID:language:() and IDT_Device::getAmount:maxLength:messageID:language:() and IDT_Device::getCardAccount:max:line1:line2:() and 
     IDT_Device::pin_getFunctionKey() and IDT_Device::getEncryptedData:minLength:maxLength:messageID:language:() */
-(RETURN_CODE) cancelPin;


/**
 * Set Numeric Length
 - UniPay II
 *
 Sets the Numeric length.
 
 @param minLength Minimum Numeric length at least 1
 @param maxLength Maximum Numeric length not to exceed 16
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setNumericLength:(int)minLength maxLength:(int)maxLength;

/**
 * Set PIN Length
 - BTPay 200
 - UniPayII
 *
 Sets the encrypted PIN length.
 
 @param minLength Minimum PIN length at least 4
 @param maxLength Maximum PIN length not to exceed 12
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setPinLength:(int)minLength maxLength:(int)maxLength;

/**
 Get Numeric Length
 - UniPay II
 
 * Returns Numeric Length
 *
 @param response Returned Value of Minimum/Maximum Numeric length:
 - response[0] = minimum length
 - response[1] = maximum length
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 @code
 NSData* res;
 RETURN_CODE rt = [[IDT_Device sharedController] getNumericLength:&res];
 uint8_t b[res.length];
 [data getBytes:b];
 if(RETURN_CODE_DO_SUCCESS == rt && res.length>1){
 LOGI(@"getNumericLength: min=%d max=%d", b[0], b[1]);
 }
 @endcode
 */
-(RETURN_CODE) getNumericLength:(NSData**)response;

/**
 Get PIN Length
 - BTPay 200
 
 * Returns encrypted PIN Length
 *
 @param response Returned Value of Minimum/Maximum PIN length:
  - response[0] = minimum length
  - response[1] = maximum length
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 @code
    NSData* res;
    RETURN_CODE rt = [[IDT_Device sharedController] getPinLength:&res];
    uint8_t b[res.length];
    [data getBytes:b];
    if(RETURN_CODE_DO_SUCCESS == rt && res.length>1){
    LOGI(@"GetPinLength: min=%d max=%d", b[0], b[1]);
    }
 @endcode
 */
-(RETURN_CODE) getPinLength:(NSData**)response;

/**
 Get PINPad Status
 - BTPay 200
 
 * Returns status of PINpad
 *
 @param response Pinpad status. response[0]:
 - 0x01: PINpad is inactivate.
 - 0x02: PINpad  has been activated, but Public Key is not loaded.
 - 0x03: Public key has been loaded, but Firmware Key, Numeric Key and Check Value is not loaded.
 - 0x10: PINpad  normal work status.
 - 0x30: PINpad  suspend status if password input error.
 - 0x31: PINpad  suspend status if get PIN(MKSK)120 times in one hours.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 @code
    Byte *b = (Byte*)malloc(1);
    RETURN_CODE rt = [[IDT_Device sharedController] getPINpadStatus];
    if(RETURN_CODE_DO_SUCCESS == rt){
    [self appendMessageToResults:[NSString stringWithFormat:@"PINPad Status: %d", b[0]]];
 
 }
 @endcode
 }

 */
-(RETURN_CODE) getPINpadStatus:(NSData**)response;

/**
 * Set encrypted MSR Data Output Format
 - BTPay 200

 *
 * Sets how data will be encrypted, with either Data Key or PIN key (if MSR DUKPT key loaded)
 *
 @param encryption Encryption Type
  - 00: Encrypt with Data Key
  - 01: Encrypt with PIN Key
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 
 */
-(RETURN_CODE) setEncryptMSRFormat:(int)encryption;

/**
 * Get encrypted MSR Data Output Format
 - BTPay 200

 *
 * Gets the encrypted algorightm of MSR card data and SmartCard data (if MSR DUKPT key loaded)
 *
 * @param response Response returned from method:
 - '1': 3DES (default)
 - '2': AES
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()

 */
-(RETURN_CODE) getEncryptMSRFormat:(NSString**)response;


/**
 * Set Card Data Encrypted Algorithm
 - BTPay 200
 
 *
 * Sets the encrypted algorightm of MSR card data and SmartCard data (if MSR DUKPT key loaded)
 *
 @param encryption Encryption Type
 - 01: 3DES (Default)
 - 02: AES
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 
 */
-(RETURN_CODE) setCardDataEncryptedAlgorithm:(int)encryption;

/**
 * Get Card Data Encrypted Algorithm
 - BTPay 200
 
 *
 * Sets the encrypted algorightm of MSR card data and SmartCard data (if MSR DUKPT key loaded
 *
 * @param response Response returned from method:
 - '0': Encrypted card data with Data Key if MSR DUKPT Key had been loaded.(default)
 - '1': Encrypted card data with PIN Key if MSR DUKPT Key had been loaded.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getCardDataEncryptedAlgorithm:(NSString**)response;


/**
 * Set Clear PAN Digits
 - UniPay
 *
 * Sets the amount of digits shown in the clear (not masked) at the beginning of the returned PAN value
 *
 @param digits Number of digits to show in clear.  Range 0-6.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setClearPANID:(int)digits;

/**
 * Set Clear PAN Digits
 - UniPay II
 *
 * Sets the amount of digits shown in the clear (not masked) at the beginning of the returned PAN value
 *
 @param digits Number of digits to show in clear.  Range 0-6.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setClearPANIDII:(int)digits;

/**
 * Get Clear PAN Digits
 - UniPay
 *
 * Returns the number of digits that begin the PAN that will be in the clear
 *
 * @param response Number of digits in clear.  Values are ASCII '0' - '6':
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getClearPANID:(NSString**)response;

/**
 * Get Backlight Status
 - UniPay
 *
 * Returns the backlight status of the UniPayII Display
 *
 * @param response Staus = "ON" or "OFF"
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getBacklightStatus:(NSString**)response;


/**
 * Set Expiration Masking
 - UniPay
 *
 * Sets the flag to mask the expiration date
 *
 @param masked TRUE = mask expiration
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setExpirationMask:(BOOL)masked;

/**
 * Set Expiration Masking (UniPayII)
 - UniPay
 *
 * Sets the flag to mask the expiration date
 *
 @param masked TRUE = mask expiration
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setExpirationMaskII:(BOOL)masked;

/**
 * Get Expiration Masking
 - UniPay
 *
 * Get the flag that determines if to mask the expiration date
 *
 * @param response '0' = masked, '1' = not-masked
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getExpirationMask:(NSString**)response;

/**
 * Set Swipe Data Encryption
 - UniPay
 *
 * Sets the swipe encryption method
 *
 @param encryption 1 = TDES, 2 = AES
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setSwipeEncryption:(int)encryption;

/**
 * Get Swipe Data Encryption
 - UniPay
 *
 * Returns the encryption used for sweip data
 *
 * @param response 'TDES', 'AES', 'NONE'
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getSwipeEncryption:(NSString**)response;

/**
 * Set Swipe Force Encryption
 - UniPay
 *
 * Sets the swipe force encryption options
 *
 @param track1 Force encrypt track 1
 @param track2 Force encrypt track 2
 @param track3 Force encrypt track 3
 @param track3card0 Force encrypt track 3 when card type is 0
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setSwipeForcedEncryptionOption:(BOOL)track1 track2:(BOOL)track2 track3:(BOOL)track3 track3card0:(BOOL)track3card0;

/**
 * Get Swipe Data Encryption
 - UniPay
 *
 * Sets the swipe force encryption options
 *
 * @param response A string with for flags separated by PIPE character  f1|f2|f3|f4, example "1|0|0|1" where:
 - f1 = Track 1 Force Encrypt
 - f2 = Track 2 Force Encrypt
 - f3 = Track 3 Force Encrypt
 - f4 = Track 3 Force Encrypt when card type is 0
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getSwipeForcedEncryptionOption:(NSString**)response;

/**
 * Set Swipe Mask Option
 - UniPay
 *
 * Sets the swipe mask/clear data sending option
 *
 @param track1 Mask track 1 allowed
 @param track2 Mask track 2 allowed
 @param track3 Mask track 3 allowed
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setSwipeMaskOption:(BOOL)track1 track2:(BOOL)track2 track3:(BOOL)track3;

/**
 * Get Swipe Mask Option
 - UniPay
 *
 * Gets the swipe mask/clear data sending option
 *
 * @param response A string with for flags separated by PIPE character  f1|f2|f3, example "1|0|0" where:
 - f1 = Track 1 Mask Allowed
 - f2 = Track 2 Mask Allowed
 - f3 = Track 3 Mask Allowed
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getSwipeMaskOption:(NSString**)response;


/**
 * Set Key Type for ICC DUKPT Key
  - UniPay
 *
 * Sets which key the data will be encrypted with, with either Data Key or PIN key (if DUKPT key loaded)
 *
 @param encryption Encryption Type
 - 00: Encrypt with Data Key
 - 01: Encrypt with PIN Key
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setKeyTypeForICCDUKPT:(int)encryption;

/**
 * Get Key Type for ICC DUKPT
 - UniPay
 *
 * Specifies the key type used for ICC DUKPT encryption
 *
 * @param response Response returned from method:
 - 'DATA': Encrypted card data with Data Key DUKPT Key had been loaded.(default)
 - 'PIN': Encrypted card data with PIN Key if DUKPT Key had been loaded.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()

 */
-(RETURN_CODE) getKeyTypeForICCDUKPT:(NSString**)response;

/**
 * Set Key Format for ICC DUKPT
 - UniPay
 *
 * Sets how data will be encrypted, with either TDES or AES (if DUKPT key loaded)
 *
 @param encryption Encryption Type
 - 00: Encrypt with TDES
 - 01: Encrypt with AES
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 

 
 */
-(RETURN_CODE) setKeyFormatForICCDUKPT:(int)encryption;

/**
 * Get Key Format For ICC DUKPT
  - UniPay
 *
 * Specifies how data will be encrypted with Data Key or PIN key (if DUKPT key loaded)
 *
 * @param response Response returned from method:
 - 'TDES': Encrypted card data with TDES if  DUKPT Key had been loaded.(default)
 - 'AES': Encrypted card data with AES if DUKPT Key had been loaded.
 - 'NONE': No Encryption.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getKeyFormatForICCDUKPT:(NSString**)response;

/**
 * Display Message and Get Encrypted PIN online
 - BTPay 200
 *
 Prompts the user with up to 3 lines of text. Returns pinblock/ksn of entered PIN value in deviceDelegate::pinpadData:keySN:event:() with event EVENT_PINPAD_ENCRYPTED_PIN
 
 @param account Card account number
 @param type Encryption Key Type:
 - 0x00: External Account Key PIN_KEY_TDES_MKSK_extp
 - 0x01: External Account Key PIN_KEY_TDES_DUKPT_extp
 - 0x20: Internal Account Key PIN_KEY_TDES_MKSK_intl
 - 0x21: Internal Account Key PIN_KEY_TDES_DUKPT_intl
 - 0x20: Internal Account Key PIN_KEY_TDES_MKSK2_intl
 - 0x21: Internal Account Key PIN_KEY_TDES_DUKPT2_intl
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 @param line3 Display line 3, up to 16 characters
 -
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - If there is no any enter in 20 seconds, the entered PIN key will be cleared.
 - When press Enter key , it will end this Command and response package with NGA format .
 - When press Cancel key, the entered PIN  key will be cleared and if press Cancel key again, this command terminated.
 - Cancel Command can terminate this command.
 
 */
-(RETURN_CODE) getEncryptedPIN:(NSString*)account keyType:(PIN_KEY_Types)type line1:(NSString*)line1 line2:(NSString*)line2 line3:(NSString*)line3;


/**
 * Display Message and Enable MSR Swipe
 - BTPay 200

 *
 Prompts the user with up to 3 lines of text. Enables MSR, waiting for swipe to occur. Returns IDTMSRData instance to deviceDelegate::swipeMSRData:()
 
 During waiting for swiping card, it will receive all commands except IDT_Device::getEncryptedPIN:keyType:line1:line2:line3:() and IDT_Device::getNumeric:minLength:maxLength:messageID:language:() and IDT_Device::getAmount:maxLength:messageID:language:() and IDT_Device::getCardAccount:max:line1:line2:() and IDT_Device::getEncryptedData:minLength:maxLength:messageID:language:()
 
 
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 @param line3 Display line 3, up to 16 characters
 -
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) startMSRSwipeWithDisplay:(NSString*)line1 line2:(NSString*)line2 line3:(NSString*)line3;

/**
 * Enable MSR Swipe
 - UniPay
 
 *
 Enables MSR, waiting for swipe to occur. Allows track selection. Returns IDTMSRData instance to deviceDelegate::swipeMSRData:()
 

 
 
 @param track Track Selection Option
 
 Track Selection Option	| Val
 ------------ | ------------
 Any Track | 0
 Track 1 Only | 1
 Track 2 Only | 2
 Track 1 & Track 2 | 3
 Track 3 Only | 4
 Track 1 & Track 3 | 5
 Track 2 & Track 3 | 6
 All three Tracks | 7
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) startMSRSwipe:(int)track;

/**
 * Disable MSR Swipe
 - BTPay 200
 - UniPay

 
 Cancels MSR swipe request.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 */
-(RETURN_CODE) cancelMSRSwipe;

/**
 * Get Response Code String
 - All Devices
 *
 Interpret a IDT_Device response code and return string description.
 
 @param errorCode Error code, range 0x0000 - 0xFFFF, example 0x0300

 
 * @retval Verbose error description
 
 HEX VALUE | Description
  ------- | -------
 0x0000 | No error, beginning task
 0x0001 | No response from reader
 0x0002 | Invalid response data
 0x0003 | Time out for task or CMD
 0x0004 | Wrong parameter
 0x0005 | SDK is doing MSR or ICC task
 0x0006 | SDK is doing PINPad task
 0x0007 | SDK is doing Other task
 0x0300 | Key Type(TDES) of Session Key is not same as the related Master Key.
 0x0400 | Related Key was not loaded.
 0x0500 | Key Same.
 0x0702 | PAN is Error Key.
 0x0D00 | This Key had been loaded.
 0x0E00 | Base Time was loaded.
 0x1800 | Send “Cancel Command” after send “Get Encrypted PIN” &”Get Numeric “& “Get Amount”
 0x1900 | Press “Cancel” key after send “Get Encrypted PIN” &”Get Numeric “& “Get Amount”
 0x30FF | Security Chip is not connect
 0x3000 | Security Chip is deactivation & Device is In Removal Legally State.
 0x3101 | Security Chip is activation &  Device is In Removal Legally State.
 0x5500 | No Admin DUKPT Key.
 0x5501 | Admin  DUKPT Key STOP.
 0x5502 | Admin DUKPT Key KSN is Error.
 0x5503 | Get Authentication Code1 Failed.
 0x5504 | Validate Authentication Code Error.
 0x5505 | Encrypt or Decrypt data failed.
 0x5506 | Not Support the New Key Type.
 0x5507 | New Key Index is Error.
 0x5508 | Step Error.
 0x550F | Other Error.
 0x6000 | Save or Config Failed / Or Read Config Error.
 0x6200 | No Serial Number.
 0x6900 | Invalid Command - Protocol is right, but task ID is invalid.
 0x6A00 | Unsupported Command - Protocol and task ID are right, but command is invalid.
 0x6B00 | Unknown parameter in command - Protocol task ID and command are right, but parameter is invalid.
 0x7200 | Device is suspend (MKSK suspend or press password suspend).
 0x7300 | PIN DUKPT is STOP (21 bit 1).
 0x7400 | Device is Busy.
 0xE100 | Can not enter sleep mode.
 0xE200 | File has existed.
 0xE300 | File has not existed.
 0xE400 | Open File Error.
 0xE500 | SmartCard Error.
 0xE600 | Get MSR Card data is error.
 0xE700 | Command time out.
 0xE800 | File read or write is error.
 0xE900 | Active 1850 error!
 0xEA00 | Load bootloader error.
 0xEF00 | Protocol Error- STX or ETX or check error.
 0xEB00 | Picture is not exist.
 0x2C06 | no card seated to request ATR
 0x2D01 | Card Not Supported,
 0x2D03 | Card Not Supported, wants CRC
 0x690D | Command not supported on reader without ICC support
 0x8100 | ICC error time out on power-up
 0x8200 | invalid TS character received
 0x8500 | pps confirmation error
 0x8600 | Unsupported F, D, or combination of F and D
 0x8700 | protocol not supported EMV TD1 out of range
 0x8800 | power not at proper level
 0x8900 | ATR length too long
 0x8B01 | EMV invalid TA1 byte value
 0x8B02 | EMV TB1 required
 0x8B03 | EMV Unsupported TB1 only 00 allowed
 0x8B04 | EMV Card Error, invalid BWI or CWI
 0x8B06 | EMV TB2 not allowed in ATR
 0x8B07 | EMV TC2 out of range
 0x8B08 | EMV TC2 out of range
 0x8B09 | per EMV96 TA3 must be > 0xF
 0x8B10 | ICC error on power-up
 0x8B11 | EMV T=1 then TB3 required
 0x8B12 | Card Error, invalid BWI or CWI
 0x8B13 | Card Error, invalid BWI or CWI
 0x8B17 | EMV TC1/TB3 conflict*
 0x8B20 | EMV TD2 out of range must be T=1
 0x8C00 | TCK error
 0xA304 | connector has no voltage setting
 0xA305 | ICC error on power-up invalid (SBLK(IFSD) exchange
 0xE301 | ICC error after session star
 0xFF00 | EMV: Request to go online
 0xFF01 | EMV: Accept the offline transaction
 0xFF02 | EMV: Decline the offline transaction
 0xFF03 | EMV: Accept the online transaction
 0xFF04 | EMV: Decline the online transaction
 0xFF05 | EMV: Application may fallback to magstripe technology
 0xFF06 | EMV: ICC detected that the conditions of use are not satisfied
 0xFF07 | EMV: ICC didn't accept transaction
 0xFF08 | EMV: Transaction was cancelled
 0xFF09 | EMV: Application was not selected by kernel or ICC format error or ICC missing data error
 0xFF0A | EMV: Transaction is terminated
 0xFF0B | EMV: Other EMV Error

 */
-(NSString *) getResponseCodeString: (int) errorCode;



/**
 * Display Message and Get Card Account
 - BTPay 200

 *
 Show message on LCD and get card account number from keypad, then return encrypted card account number. Returns encryptedData of entered account in deviceDelegate::pinpadData:keySN:event:() with event EVENT_PINPAD_ACCOUNT
 
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 -
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 - If  there is no any enter in 3 minutes, this command time out.
 - If  there is no any enter in 20 seconds, the entered account numbers will be cleared.
 - When press Enter key, it will end this command and respond package with NGA format.
 - When press Cancel key, the entered account numbers will be cleared and if press Cancel key again, this command terminated.
 - Cancel command can terminate this command.
 */
-(RETURN_CODE) getCardAccount:(int)minLength max:(int)maxLength line1:(NSString*)line1 line2:(NSString*)line2;


/**
 * Display Message and Get Encrypted Data
 - BTPay 200

 *
 Decrypt and display message on LCD. Prompts the user with up to 2 lines of text. Returns value of encrypted data (using MSR DUKPT key) and KSN to deviceDelegate::pinpadData:keySN:event:() with event EVENT_PINPAD_ENCRYPTED_DATA
 
 @param lastPackage Last package flag
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param mID Message ID from approved message list.
 @param lang Language file to use for message
 @code
 typedef enum{
 LANGUAGE_TYPE_ENGLISH,
 LANGUAGE_TYPE_PORTUGUESE,
 LANGUAGE_TYPE_SPANISH,
 LANGUAGE_TYPE_FRENCH
 }LANGUAGE_TYPE;
 @endcode
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 
 -  If  there is no any enter in 3 minutes, this command time out.
 -  If  there is no any enter in 20 seconds, the entered account numbers will be cleared.
 -  When press Enter key, it will end this command and respond package with NGA format.
 -  When press Cancel key, the entered account numbers will be cleared and if press Cancel key again, this command terminated.
 -  Cancel command can terminate this command.
 -  Maximum pattern number allowed is 10. If any error or invalid command is sent, input data patterns will be cleared and command fail.
 
 \par Secure Messages
 Secure messages to be used with General Prompts commands
 
 Msg Id |English Prompt | Portuguese Prompt | Spanish Prompt | French Prompt
 ---------- | ---------- | ----------  | ---------- | ----------
 1 | ENTER | ENTER | INGRESE | ENTREZ
 2 | REENTER | RE-INTRODUZIR | REINGRESE | RE-ENTREZ
 3 | ENTER YOUR | INTRODUZIR O SEU | INGRESE SU | ENTREZ VOTRE
 4 | REENTER YOUR | RE-INTRODUZIR O SEU | REINGRESE SU | RE-ENTREZ VOTRE
 5 | PLEASE ENTER | POR FAVOR DIGITE | POR FAVOR INGRESE | SVP ENTREZ
 6 | PLEASE REENTER | POR FAVO REENTRAR | POR FAVO REINGRESE | SVP RE-ENTREZ
 7 | PO NUMBER | NÚMERO PO | NUMERO PO | No COMMANDE
 8 | DRIVER ID | LICENÇA | LICENCIA | ID CONDUCTEUR
 9 | ODOMETER | ODOMETER | ODOMETRO | ODOMETRE
 10 | ID NUMBER | NÚMERO ID | NUMERO ID | No IDENT
 11 | EQUIP CODE | EQUIP CODE | CODIGO EQUIP | CODE EQUIPEMENT
 12 | DRIVERS ID | DRIVER ID | ID CONDUCTOR | ID CONDUCTEUR
 13 | JOB NUMBER | EMP NÚMERO | NUMERO EMP | No TRAVAIL
 14 | WORK ORDER | TRABALHO ORDEM | ORDEN TRABAJO | FICHE TRAVAIL
 15 | VEHICLE ID | ID VEÍCULO | ID VEHICULO | ID VEHICULE
 16 | ENTER DRIVER | ENTER DRIVER | INGRESE CONDUCTOR | ENTR CONDUCTEUR
 17 | ENTER DEPT | ENTER DEPT | INGRESE DEPT | ENTR DEPARTEMNT
 18 | ENTER PHONE | ADICIONAR PHONE | INGRESE TELEFONO | ENTR No TELEPH
 19 | ENTER ROUTE | ROUTE ADD | INGRESE RUTA | ENTREZ ROUTE
 20 | ENTER FLEET | ENTER FROTA | INGRESE FLOTA | ENTREZ PARC AUTO
 21 | ENTER JOB ID | ENTER JOB ID | INGRESE ID TRABAJO | ENTR ID TRAVAIL
 22 | ROUTE NUMBER | NÚMERO PATH | RUTA NUMERO | No ROUTE
 23 | ENTER USER ID | ENTER USER ID | INGRESE ID USUARIO | ID UTILISATEUR
 24 | FLEET NUMBER | NÚMERO DE FROTA | FLOTA NUMERO | No PARC AUTO
 25 | ENTER PRODUCT | ADICIONAR PRODUTO | INGRESE PRODUCTO | ENTREZ PRODUIT
 26 | DRIVER NUMBER | NÚMERO DRIVER | CONDUCTOR NUMERO | No CONDUCTEUR
 27 | ENTER LICENSE | ENTER LICENÇA | INGRESE LICENCIA | ENTREZ PERMIS
 28 | ENTER FLEET NO | ENTER NRO FROTA | INGRESE NRO FLOTA | ENT No PARC AUTO
 29 | ENTER CAR WASH | WASH ENTER | INGRESE LAVADO | ENTREZ LAVE-AUTO
 30 | ENTER VEHICLE | ENTER VEÍCULO | INGRESE VEHICULO | ENTREZ VEHICULE
 31 | ENTER TRAILER | TRAILER ENTER | INGRESE TRAILER | ENTREZ REMORQUE
 32 | ENTER ODOMETER | ENTER ODOMETER | INGRESE ODOMETRO | ENTREZ ODOMETRE
 33 | DRIVER LICENSE | CARTEIRA DE MOTORISTA | LICENCIA CONDUCTOR | PERMIS CONDUIRE
 34 | ENTER CUSTOMER | ENTER CLIENTE | INGRESE CLIENTE | ENTREZ CLIENT
 35 | VEHICLE NUMBER | NÚMERO DO VEÍCULO | VEHICULO NUMERO | No VEHICULE
 36 | ENTER CUST DATA | ENTER CLIENTE INFO | INGRESE INFO CLIENTE | INFO CLIENT
 37 | REENTER DRIVID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENTR ID COND
 38 | ENTER USER DATA | ENTER INFO USUÁRIO | INGRESE INFO USUARIO | INFO UTILISATEUR
 39 | ENTER CUST CODE | ENTER CODE. CLIENTE | INGRESE COD. CLIENTE | ENTR CODE CLIENT
 40 | ENTER EMPLOYEE | ENTER FUNCIONÁRIO | INGRESE EMPLEADO | ENTREZ EMPLOYE
 41 | ENTER ID NUMBER | ENTER NÚMERO ID | INGRESE NUMERO ID | ENTREZ No ID
 42 | ENTER DRIVER ID | ENTER ID DRIVER | INGRESE ID CONDUCTOR | No CONDUCTEUR
 43 | ENTER FLEET PIN | ENTER PIN FROTA | INGRESE PIN DE FLOTA | NIP PARC AUTO
 44 | ODOMETER NUMBER | NÚMERO ODOMETER | ODOMETRO NUMERO | No ODOMETRE
 45 | ENTER DRIVER LIC | ENTER DRIVER LIC | INGRESE LIC CONDUCTOR | PERMIS CONDUIRE
 46 | ENTER TRAILER NO | NRO TRAILER ENTER | INGRESE NRO TRAILER | ENT No REMORQUE
 47 | REENTER VEHICLE | REENTRAR VEÍCULO | REINGRESE VEHICULO | RE-ENTR VEHICULE
 48 | ENTER VEHICLE ID | ENTER VEÍCULO ID | INGRESE ID VEHICULO | ENTR ID VEHICULE
 49 | ENTER BIRTH DATE | INSERIR DATA NAC | INGRESE FECHA NAC | ENT DT NAISSANCE
 50 | ENTER DOB MMDDYY | ENTER FDN MMDDYY | INGRESE FDN MMDDAA | NAISSANCE MMJJAA
 51 | ENTER FLEET DATA | ENTER FROTA INFO | INGRESE INFO DE FLOTA | INFO PARC AUTO
 52 | ENTER REFERENCE | ENTER REFERÊNCIA | INGRESE REFERENCIA | ENTREZ REFERENCE
 53 | ENTER AUTH NUMBR | ENTER NÚMERO AUT | INGRESE NUMERO AUT | No AUTORISATION
 54 | ENTER HUB NUMBER | ENTER HUB NRO | INGRESE NRO HUB | ENTREZ No NOYAU
 55 | ENTER HUBOMETER | MEDIDA PARA ENTRAR HUB | INGRESE MEDIDO DE HUB | COMPTEUR NOYAU
 56 | ENTER TRAILER ID | TRAILER ENTER ID | INGRESE ID TRAILER | ENT ID REMORQUE
 57 | ODOMETER READING | QUILOMETRAGEM | LECTURA ODOMETRO | LECTURE ODOMETRE
 58 | REENTER ODOMETER | REENTRAR ODOMETER | REINGRESE ODOMETRO | RE-ENT ODOMETRE
 59 | REENTER DRIV. ID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENT ID CONDUC
 60 | ENTER CUSTOMER ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 61 | ENTER CUST. ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 62 | ENTER ROUTE NUM | ENTER NUM ROUTE | INGRESE NUM RUTA | ENT No ROUTE
 63 | ENTER FLEET NUM | FROTA ENTER NUM | INGRESE NUM FLOTA | ENT No PARC AUTO
 64 | FLEET PIN | FROTA PIN | PIN DE FLOTA | NIP PARC AUTO
 65 | DRIVER # | DRIVER # | CONDUCTOR # | CONDUCTEUR
 66 | ENTER DRIVER # | ENTER DRIVER # | INGRESE CONDUCTOR # | ENT # CONDUCTEUR
 67 | VEHICLE # | VEÍCULO # | VEHICULO # | # VEHICULE
 68 | ENTER VEHICLE # | ENTER VEÍCULO # | INGRESE VEHICULO # | ENT # VEHICULE
 69 | JOB # | TRABALHO # | TRABAJO # | # TRAVAIL
 70 | ENTER JOB # | ENTER JOB # | INGRESE TRABAJO # | ENTREZ # TRAVAIL
 71 | DEPT NUMBER | NÚMERO DEPT | NUMERO DEPTO | No DEPARTEMENT
 72 | DEPARTMENT # | DEPARTAMENTO # | DEPARTAMENTO # | DEPARTEMENT
 73 | ENTER DEPT # | ENTER DEPT # | INGRESE DEPTO # | ENT# DEPARTEMENT
 74 | LICENSE NUMBER | NÚMERO DE LICENÇA | NUMERO LICENCIA | No PERMIS
 75 | LICENSE # | LICENÇA # | LICENCIA # | # PERMIS
 76 | ENTER LICENSE # | ENTER LICENÇA # | INGRESE LICENCIA # | ENTREZ # PERMIS
 77 | DATA | INFO | INFO | INFO
 78 | ENTER DATA | ENTER INFO | INGRESE INFO | ENTREZ INFO
 79 | CUSTOMER DATA | CLIENTE INFO | INFO CLIENTE | INFO CLIENT
 80 | ID # | ID # | ID # | # ID
 81 | ENTER ID # | ENTER ID # | INGRESE ID # | ENTREZ # ID
 82 | USER ID | USER ID | ID USUARIO | ID UTILISATEUR
 83 | ROUTE # | ROUTE # | RUTA # | # ROUTE
 84 | ENTER ROUTE # | ADD ROUTE # | INGRESE RUTA # | ENTREZ # ROUTE
 85 | ENTER CARD NUM | ENTER NÚMERO DE CARTÃO | INGRESE NUM TARJETA | ENTREZ NO CARTE
 86 | EXP DATE(YYMM) | VALIDADE VAL (AAMM) | FECHA EXP (AAMM) | DATE EXPIR(AAMM)
 87 | PHONE NUMBER | TELEFONE | NUMERO TELEFONO | NO TEL
 88 | CVV START DATE | CVV DATA DE INÍCIO | CVV FECHA INICIO | CVV DATE DE DEBUT
 89 | ISSUE NUMBER | NÚMERO DE EMISSÃO | NUMERO DE EMISION | NO DEMISSION
 90 | START DATE (MMYY) | DATA DE INÍCIO (AAMM) | FECHA INICIO (AAMM) | DATE DE DEBUT-AAMM
 */
-(RETURN_CODE) getEncryptedData:(BOOL)lastPackage minLength:(int)minLength maxLength:(int)maxLength messageID:(int)mID language:(LANGUAGE_TYPE)lang;

/**
 * Display Message and Get Numeric Key(s)
 - BTPay 200

 *
 Decrypt and display message on LCD. Requires secure message data. Returns value in inputValue of deviceDelegate::pinpadData:keySN:event:() with event EVENT_PINPAD_NUMERIC
 
 @param maskInput If true, all entered data will be masked with asterik (*)
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param mID Message ID from approved message list.
 @param lang Language file to use for message
 @code
 typedef enum{
 LANGUAGE_TYPE_ENGLISH,
 LANGUAGE_TYPE_PORTUGUESE,
 LANGUAGE_TYPE_SPANISH,
 LANGUAGE_TYPE_FRENCH
 }LANGUAGE_TYPE;
 @endcode
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - If there is no any enter in 20 seconds, the entered numeric key will be cleared.
 - When press Enter key , it will end this Command and response package with NGA  format .
 - When press Cancel key, the entered numeric key will be cleared and if press Cancel key again, this command terminated.
 - Cancel Command can terminate this command.
 
 \par Secure Messages
 Secure messages to be used with General Prompts commands
 
 Msg Id |English Prompt | Portuguese Prompt | Spanish Prompt | French Prompt
 ---------- | ---------- | ----------  | ---------- | ----------
 1 | ENTER | ENTER | INGRESE | ENTREZ
 2 | REENTER | RE-INTRODUZIR | REINGRESE | RE-ENTREZ
 3 | ENTER YOUR | INTRODUZIR O SEU | INGRESE SU | ENTREZ VOTRE
 4 | REENTER YOUR | RE-INTRODUZIR O SEU | REINGRESE SU | RE-ENTREZ VOTRE
 5 | PLEASE ENTER | POR FAVOR DIGITE | POR FAVOR INGRESE | SVP ENTREZ
 6 | PLEASE REENTER | POR FAVO REENTRAR | POR FAVO REINGRESE | SVP RE-ENTREZ
 7 | PO NUMBER | NÚMERO PO | NUMERO PO | No COMMANDE
 8 | DRIVER ID | LICENÇA | LICENCIA | ID CONDUCTEUR
 9 | ODOMETER | ODOMETER | ODOMETRO | ODOMETRE
 10 | ID NUMBER | NÚMERO ID | NUMERO ID | No IDENT
 11 | EQUIP CODE | EQUIP CODE | CODIGO EQUIP | CODE EQUIPEMENT
 12 | DRIVERS ID | DRIVER ID | ID CONDUCTOR | ID CONDUCTEUR
 13 | JOB NUMBER | EMP NÚMERO | NUMERO EMP | No TRAVAIL
 14 | WORK ORDER | TRABALHO ORDEM | ORDEN TRABAJO | FICHE TRAVAIL
 15 | VEHICLE ID | ID VEÍCULO | ID VEHICULO | ID VEHICULE
 16 | ENTER DRIVER | ENTER DRIVER | INGRESE CONDUCTOR | ENTR CONDUCTEUR
 17 | ENTER DEPT | ENTER DEPT | INGRESE DEPT | ENTR DEPARTEMNT
 18 | ENTER PHONE | ADICIONAR PHONE | INGRESE TELEFONO | ENTR No TELEPH
 19 | ENTER ROUTE | ROUTE ADD | INGRESE RUTA | ENTREZ ROUTE
 20 | ENTER FLEET | ENTER FROTA | INGRESE FLOTA | ENTREZ PARC AUTO
 21 | ENTER JOB ID | ENTER JOB ID | INGRESE ID TRABAJO | ENTR ID TRAVAIL
 22 | ROUTE NUMBER | NÚMERO PATH | RUTA NUMERO | No ROUTE
 23 | ENTER USER ID | ENTER USER ID | INGRESE ID USUARIO | ID UTILISATEUR
 24 | FLEET NUMBER | NÚMERO DE FROTA | FLOTA NUMERO | No PARC AUTO
 25 | ENTER PRODUCT | ADICIONAR PRODUTO | INGRESE PRODUCTO | ENTREZ PRODUIT
 26 | DRIVER NUMBER | NÚMERO DRIVER | CONDUCTOR NUMERO | No CONDUCTEUR
 27 | ENTER LICENSE | ENTER LICENÇA | INGRESE LICENCIA | ENTREZ PERMIS
 28 | ENTER FLEET NO | ENTER NRO FROTA | INGRESE NRO FLOTA | ENT No PARC AUTO
 29 | ENTER CAR WASH | WASH ENTER | INGRESE LAVADO | ENTREZ LAVE-AUTO
 30 | ENTER VEHICLE | ENTER VEÍCULO | INGRESE VEHICULO | ENTREZ VEHICULE
 31 | ENTER TRAILER | TRAILER ENTER | INGRESE TRAILER | ENTREZ REMORQUE
 32 | ENTER ODOMETER | ENTER ODOMETER | INGRESE ODOMETRO | ENTREZ ODOMETRE
 33 | DRIVER LICENSE | CARTEIRA DE MOTORISTA | LICENCIA CONDUCTOR | PERMIS CONDUIRE
 34 | ENTER CUSTOMER | ENTER CLIENTE | INGRESE CLIENTE | ENTREZ CLIENT
 35 | VEHICLE NUMBER | NÚMERO DO VEÍCULO | VEHICULO NUMERO | No VEHICULE
 36 | ENTER CUST DATA | ENTER CLIENTE INFO | INGRESE INFO CLIENTE | INFO CLIENT
 37 | REENTER DRIVID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENTR ID COND
 38 | ENTER USER DATA | ENTER INFO USUÁRIO | INGRESE INFO USUARIO | INFO UTILISATEUR
 39 | ENTER CUST CODE | ENTER CODE. CLIENTE | INGRESE COD. CLIENTE | ENTR CODE CLIENT
 40 | ENTER EMPLOYEE | ENTER FUNCIONÁRIO | INGRESE EMPLEADO | ENTREZ EMPLOYE
 41 | ENTER ID NUMBER | ENTER NÚMERO ID | INGRESE NUMERO ID | ENTREZ No ID
 42 | ENTER DRIVER ID | ENTER ID DRIVER | INGRESE ID CONDUCTOR | No CONDUCTEUR
 43 | ENTER FLEET PIN | ENTER PIN FROTA | INGRESE PIN DE FLOTA | NIP PARC AUTO
 44 | ODOMETER NUMBER | NÚMERO ODOMETER | ODOMETRO NUMERO | No ODOMETRE
 45 | ENTER DRIVER LIC | ENTER DRIVER LIC | INGRESE LIC CONDUCTOR | PERMIS CONDUIRE
 46 | ENTER TRAILER NO | NRO TRAILER ENTER | INGRESE NRO TRAILER | ENT No REMORQUE
 47 | REENTER VEHICLE | REENTRAR VEÍCULO | REINGRESE VEHICULO | RE-ENTR VEHICULE
 48 | ENTER VEHICLE ID | ENTER VEÍCULO ID | INGRESE ID VEHICULO | ENTR ID VEHICULE
 49 | ENTER BIRTH DATE | INSERIR DATA NAC | INGRESE FECHA NAC | ENT DT NAISSANCE
 50 | ENTER DOB MMDDYY | ENTER FDN MMDDYY | INGRESE FDN MMDDAA | NAISSANCE MMJJAA
 51 | ENTER FLEET DATA | ENTER FROTA INFO | INGRESE INFO DE FLOTA | INFO PARC AUTO
 52 | ENTER REFERENCE | ENTER REFERÊNCIA | INGRESE REFERENCIA | ENTREZ REFERENCE
 53 | ENTER AUTH NUMBR | ENTER NÚMERO AUT | INGRESE NUMERO AUT | No AUTORISATION
 54 | ENTER HUB NUMBER | ENTER HUB NRO | INGRESE NRO HUB | ENTREZ No NOYAU
 55 | ENTER HUBOMETER | MEDIDA PARA ENTRAR HUB | INGRESE MEDIDO DE HUB | COMPTEUR NOYAU
 56 | ENTER TRAILER ID | TRAILER ENTER ID | INGRESE ID TRAILER | ENT ID REMORQUE
 57 | ODOMETER READING | QUILOMETRAGEM | LECTURA ODOMETRO | LECTURE ODOMETRE
 58 | REENTER ODOMETER | REENTRAR ODOMETER | REINGRESE ODOMETRO | RE-ENT ODOMETRE
 59 | REENTER DRIV. ID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENT ID CONDUC
 60 | ENTER CUSTOMER ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 61 | ENTER CUST. ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 62 | ENTER ROUTE NUM | ENTER NUM ROUTE | INGRESE NUM RUTA | ENT No ROUTE
 63 | ENTER FLEET NUM | FROTA ENTER NUM | INGRESE NUM FLOTA | ENT No PARC AUTO
 64 | FLEET PIN | FROTA PIN | PIN DE FLOTA | NIP PARC AUTO
 65 | DRIVER # | DRIVER # | CONDUCTOR # | CONDUCTEUR
 66 | ENTER DRIVER # | ENTER DRIVER # | INGRESE CONDUCTOR # | ENT # CONDUCTEUR
 67 | VEHICLE # | VEÍCULO # | VEHICULO # | # VEHICULE
 68 | ENTER VEHICLE # | ENTER VEÍCULO # | INGRESE VEHICULO # | ENT # VEHICULE
 69 | JOB # | TRABALHO # | TRABAJO # | # TRAVAIL
 70 | ENTER JOB # | ENTER JOB # | INGRESE TRABAJO # | ENTREZ # TRAVAIL
 71 | DEPT NUMBER | NÚMERO DEPT | NUMERO DEPTO | No DEPARTEMENT
 72 | DEPARTMENT # | DEPARTAMENTO # | DEPARTAMENTO # | DEPARTEMENT
 73 | ENTER DEPT # | ENTER DEPT # | INGRESE DEPTO # | ENT# DEPARTEMENT
 74 | LICENSE NUMBER | NÚMERO DE LICENÇA | NUMERO LICENCIA | No PERMIS
 75 | LICENSE # | LICENÇA # | LICENCIA # | # PERMIS
 76 | ENTER LICENSE # | ENTER LICENÇA # | INGRESE LICENCIA # | ENTREZ # PERMIS
 77 | DATA | INFO | INFO | INFO
 78 | ENTER DATA | ENTER INFO | INGRESE INFO | ENTREZ INFO
 79 | CUSTOMER DATA | CLIENTE INFO | INFO CLIENTE | INFO CLIENT
 80 | ID # | ID # | ID # | # ID
 81 | ENTER ID # | ENTER ID # | INGRESE ID # | ENTREZ # ID
 82 | USER ID | USER ID | ID USUARIO | ID UTILISATEUR
 83 | ROUTE # | ROUTE # | RUTA # | # ROUTE
 84 | ENTER ROUTE # | ADD ROUTE # | INGRESE RUTA # | ENTREZ # ROUTE
 85 | ENTER CARD NUM | ENTER NÚMERO DE CARTÃO | INGRESE NUM TARJETA | ENTREZ NO CARTE
 86 | EXP DATE(YYMM) | VALIDADE VAL (AAMM) | FECHA EXP (AAMM) | DATE EXPIR(AAMM)
 87 | PHONE NUMBER | TELEFONE | NUMERO TELEFONO | NO TEL
 88 | CVV START DATE | CVV DATA DE INÍCIO | CVV FECHA INICIO | CVV DATE DE DEBUT
 89 | ISSUE NUMBER | NÚMERO DE EMISSÃO | NUMERO DE EMISION | NO DEMISSION
 90 | START DATE (MMYY) | DATA DE INÍCIO (AAMM) | FECHA INICIO (AAMM) | DATE DE DEBUT-AAMM
 */
-(RETURN_CODE) getNumeric:(bool)maskInput minLength:(int)minLength maxLength:(int)maxLength messageID:(int)mID language:(LANGUAGE_TYPE)lang;


/**
 * Display Message and Get Numeric Key(s)
 - UniPay II
 
 *
 Decrypt and display message on LCD. Requires secure message data. Returns value in inputValue of deviceDelegate::pinpadData:keySN:event:() with event EVENT_PINPAD_NUMERIC
 
 @param maskInput If true, all entered data will be masked with asterik (*)
 @param mID Message ID from approved message list.
 @param lang Language file to use for message
 @code
 typedef enum{
 LANGUAGE_TYPE_ENGLISH,
 LANGUAGE_TYPE_PORTUGUESE,
 LANGUAGE_TYPE_SPANISH,
 LANGUAGE_TYPE_FRENCH
 }LANGUAGE_TYPE;
 @endcode
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - If there is no any enter in 20 seconds, the entered numeric key will be cleared.
 - When press Enter key , it will end this Command and response package with NGA  format .
 - When press Cancel key, the entered numeric key will be cleared and if press Cancel key again, this command terminated.
 - Cancel Command can terminate this command.
 
 \par Secure Messages
 Secure messages to be used with General Prompts commands
 
 Msg Id |English Prompt | Portuguese Prompt | Spanish Prompt | French Prompt
 ---------- | ---------- | ----------  | ---------- | ----------
 1 | ENTER | ENTER | INGRESE | ENTREZ
 2 | REENTER | RE-INTRODUZIR | REINGRESE | RE-ENTREZ
 3 | ENTER YOUR | INTRODUZIR O SEU | INGRESE SU | ENTREZ VOTRE
 4 | REENTER YOUR | RE-INTRODUZIR O SEU | REINGRESE SU | RE-ENTREZ VOTRE
 5 | PLEASE ENTER | POR FAVOR DIGITE | POR FAVOR INGRESE | SVP ENTREZ
 6 | PLEASE REENTER | POR FAVO REENTRAR | POR FAVO REINGRESE | SVP RE-ENTREZ
 7 | PO NUMBER | NÚMERO PO | NUMERO PO | No COMMANDE
 8 | DRIVER ID | LICENÇA | LICENCIA | ID CONDUCTEUR
 9 | ODOMETER | ODOMETER | ODOMETRO | ODOMETRE
 10 | ID NUMBER | NÚMERO ID | NUMERO ID | No IDENT
 11 | EQUIP CODE | EQUIP CODE | CODIGO EQUIP | CODE EQUIPEMENT
 12 | DRIVERS ID | DRIVER ID | ID CONDUCTOR | ID CONDUCTEUR
 13 | JOB NUMBER | EMP NÚMERO | NUMERO EMP | No TRAVAIL
 14 | WORK ORDER | TRABALHO ORDEM | ORDEN TRABAJO | FICHE TRAVAIL
 15 | VEHICLE ID | ID VEÍCULO | ID VEHICULO | ID VEHICULE
 16 | ENTER DRIVER | ENTER DRIVER | INGRESE CONDUCTOR | ENTR CONDUCTEUR
 17 | ENTER DEPT | ENTER DEPT | INGRESE DEPT | ENTR DEPARTEMNT
 18 | ENTER PHONE | ADICIONAR PHONE | INGRESE TELEFONO | ENTR No TELEPH
 19 | ENTER ROUTE | ROUTE ADD | INGRESE RUTA | ENTREZ ROUTE
 20 | ENTER FLEET | ENTER FROTA | INGRESE FLOTA | ENTREZ PARC AUTO
 21 | ENTER JOB ID | ENTER JOB ID | INGRESE ID TRABAJO | ENTR ID TRAVAIL
 22 | ROUTE NUMBER | NÚMERO PATH | RUTA NUMERO | No ROUTE
 23 | ENTER USER ID | ENTER USER ID | INGRESE ID USUARIO | ID UTILISATEUR
 24 | FLEET NUMBER | NÚMERO DE FROTA | FLOTA NUMERO | No PARC AUTO
 25 | ENTER PRODUCT | ADICIONAR PRODUTO | INGRESE PRODUCTO | ENTREZ PRODUIT
 26 | DRIVER NUMBER | NÚMERO DRIVER | CONDUCTOR NUMERO | No CONDUCTEUR
 27 | ENTER LICENSE | ENTER LICENÇA | INGRESE LICENCIA | ENTREZ PERMIS
 28 | ENTER FLEET NO | ENTER NRO FROTA | INGRESE NRO FLOTA | ENT No PARC AUTO
 29 | ENTER CAR WASH | WASH ENTER | INGRESE LAVADO | ENTREZ LAVE-AUTO
 30 | ENTER VEHICLE | ENTER VEÍCULO | INGRESE VEHICULO | ENTREZ VEHICULE
 31 | ENTER TRAILER | TRAILER ENTER | INGRESE TRAILER | ENTREZ REMORQUE
 32 | ENTER ODOMETER | ENTER ODOMETER | INGRESE ODOMETRO | ENTREZ ODOMETRE
 33 | DRIVER LICENSE | CARTEIRA DE MOTORISTA | LICENCIA CONDUCTOR | PERMIS CONDUIRE
 34 | ENTER CUSTOMER | ENTER CLIENTE | INGRESE CLIENTE | ENTREZ CLIENT
 35 | VEHICLE NUMBER | NÚMERO DO VEÍCULO | VEHICULO NUMERO | No VEHICULE
 36 | ENTER CUST DATA | ENTER CLIENTE INFO | INGRESE INFO CLIENTE | INFO CLIENT
 37 | REENTER DRIVID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENTR ID COND
 38 | ENTER USER DATA | ENTER INFO USUÁRIO | INGRESE INFO USUARIO | INFO UTILISATEUR
 39 | ENTER CUST CODE | ENTER CODE. CLIENTE | INGRESE COD. CLIENTE | ENTR CODE CLIENT
 40 | ENTER EMPLOYEE | ENTER FUNCIONÁRIO | INGRESE EMPLEADO | ENTREZ EMPLOYE
 41 | ENTER ID NUMBER | ENTER NÚMERO ID | INGRESE NUMERO ID | ENTREZ No ID
 42 | ENTER DRIVER ID | ENTER ID DRIVER | INGRESE ID CONDUCTOR | No CONDUCTEUR
 43 | ENTER FLEET PIN | ENTER PIN FROTA | INGRESE PIN DE FLOTA | NIP PARC AUTO
 44 | ODOMETER NUMBER | NÚMERO ODOMETER | ODOMETRO NUMERO | No ODOMETRE
 45 | ENTER DRIVER LIC | ENTER DRIVER LIC | INGRESE LIC CONDUCTOR | PERMIS CONDUIRE
 46 | ENTER TRAILER NO | NRO TRAILER ENTER | INGRESE NRO TRAILER | ENT No REMORQUE
 47 | REENTER VEHICLE | REENTRAR VEÍCULO | REINGRESE VEHICULO | RE-ENTR VEHICULE
 48 | ENTER VEHICLE ID | ENTER VEÍCULO ID | INGRESE ID VEHICULO | ENTR ID VEHICULE
 49 | ENTER BIRTH DATE | INSERIR DATA NAC | INGRESE FECHA NAC | ENT DT NAISSANCE
 50 | ENTER DOB MMDDYY | ENTER FDN MMDDYY | INGRESE FDN MMDDAA | NAISSANCE MMJJAA
 51 | ENTER FLEET DATA | ENTER FROTA INFO | INGRESE INFO DE FLOTA | INFO PARC AUTO
 52 | ENTER REFERENCE | ENTER REFERÊNCIA | INGRESE REFERENCIA | ENTREZ REFERENCE
 53 | ENTER AUTH NUMBR | ENTER NÚMERO AUT | INGRESE NUMERO AUT | No AUTORISATION
 54 | ENTER HUB NUMBER | ENTER HUB NRO | INGRESE NRO HUB | ENTREZ No NOYAU
 55 | ENTER HUBOMETER | MEDIDA PARA ENTRAR HUB | INGRESE MEDIDO DE HUB | COMPTEUR NOYAU
 56 | ENTER TRAILER ID | TRAILER ENTER ID | INGRESE ID TRAILER | ENT ID REMORQUE
 57 | ODOMETER READING | QUILOMETRAGEM | LECTURA ODOMETRO | LECTURE ODOMETRE
 58 | REENTER ODOMETER | REENTRAR ODOMETER | REINGRESE ODOMETRO | RE-ENT ODOMETRE
 59 | REENTER DRIV. ID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENT ID CONDUC
 60 | ENTER CUSTOMER ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 61 | ENTER CUST. ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 62 | ENTER ROUTE NUM | ENTER NUM ROUTE | INGRESE NUM RUTA | ENT No ROUTE
 63 | ENTER FLEET NUM | FROTA ENTER NUM | INGRESE NUM FLOTA | ENT No PARC AUTO
 64 | FLEET PIN | FROTA PIN | PIN DE FLOTA | NIP PARC AUTO
 65 | DRIVER # | DRIVER # | CONDUCTOR # | CONDUCTEUR
 66 | ENTER DRIVER # | ENTER DRIVER # | INGRESE CONDUCTOR # | ENT # CONDUCTEUR
 67 | VEHICLE # | VEÍCULO # | VEHICULO # | # VEHICULE
 68 | ENTER VEHICLE # | ENTER VEÍCULO # | INGRESE VEHICULO # | ENT # VEHICULE
 69 | JOB # | TRABALHO # | TRABAJO # | # TRAVAIL
 70 | ENTER JOB # | ENTER JOB # | INGRESE TRABAJO # | ENTREZ # TRAVAIL
 71 | DEPT NUMBER | NÚMERO DEPT | NUMERO DEPTO | No DEPARTEMENT
 72 | DEPARTMENT # | DEPARTAMENTO # | DEPARTAMENTO # | DEPARTEMENT
 73 | ENTER DEPT # | ENTER DEPT # | INGRESE DEPTO # | ENT# DEPARTEMENT
 74 | LICENSE NUMBER | NÚMERO DE LICENÇA | NUMERO LICENCIA | No PERMIS
 75 | LICENSE # | LICENÇA # | LICENCIA # | # PERMIS
 76 | ENTER LICENSE # | ENTER LICENÇA # | INGRESE LICENCIA # | ENTREZ # PERMIS
 77 | DATA | INFO | INFO | INFO
 78 | ENTER DATA | ENTER INFO | INGRESE INFO | ENTREZ INFO
 79 | CUSTOMER DATA | CLIENTE INFO | INFO CLIENTE | INFO CLIENT
 80 | ID # | ID # | ID # | # ID
 81 | ENTER ID # | ENTER ID # | INGRESE ID # | ENTREZ # ID
 82 | USER ID | USER ID | ID USUARIO | ID UTILISATEUR
 83 | ROUTE # | ROUTE # | RUTA # | # ROUTE
 84 | ENTER ROUTE # | ADD ROUTE # | INGRESE RUTA # | ENTREZ # ROUTE
 85 | ENTER CARD NUM | ENTER NÚMERO DE CARTÃO | INGRESE NUM TARJETA | ENTREZ NO CARTE
 86 | EXP DATE(YYMM) | VALIDADE VAL (AAMM) | FECHA EXP (AAMM) | DATE EXPIR(AAMM)
 87 | PHONE NUMBER | TELEFONE | NUMERO TELEFONO | NO TEL
 88 | CVV START DATE | CVV DATA DE INÍCIO | CVV FECHA INICIO | CVV DATE DE DEBUT
 89 | ISSUE NUMBER | NÚMERO DE EMISSÃO | NUMERO DE EMISION | NO DEMISSION
 90 | START DATE (MMYY) | DATA DE INÍCIO (AAMM) | FECHA INICIO (AAMM) | DATE DE DEBUT-AAMM
 */
-(RETURN_CODE) getNumericUniPayII:(bool)maskInput messageID:(int)mID language:(LANGUAGE_TYPE)lang;

/**
 * Display Message and Get Amount
 - BTPay 200

 *
 Decrypt and display message on LCD. Requires secure message data. Returns value in inputValue of deviceDelegate::pinpadData:keySN:event:() with event EVENT_PINPAD_AMOUNT
 
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param mID Message ID from approved message list.
 @param lang Language file to use for message
 @code
 typedef enum{
 LANGUAGE_TYPE_ENGLISH,
 LANGUAGE_TYPE_PORTUGUESE,
 LANGUAGE_TYPE_SPANISH,
 LANGUAGE_TYPE_FRENCH
 }LANGUAGE_TYPE;
 @endcode
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - If there is no any enter in 20 seconds, the entered namount key will be cleared.
 - When press Enter key , it will end this Command and response package with NGA  format .
 - When press Cancel key, the entered amount key will be cleared and if press Cancel key again, this command terminated.
 - Cancel Command can terminate this command.
 
 \par Secure Messages
 Secure messages to be used with General Prompts commands
 
 Msg Id |English Prompt | Portuguese Prompt | Spanish Prompt | French Prompt
 ---------- | ---------- | ----------  | ---------- | ----------
 1 | ENTER | ENTER | INGRESE | ENTREZ
 2 | REENTER | RE-INTRODUZIR | REINGRESE | RE-ENTREZ
 3 | ENTER YOUR | INTRODUZIR O SEU | INGRESE SU | ENTREZ VOTRE
 4 | REENTER YOUR | RE-INTRODUZIR O SEU | REINGRESE SU | RE-ENTREZ VOTRE
 5 | PLEASE ENTER | POR FAVOR DIGITE | POR FAVOR INGRESE | SVP ENTREZ
 6 | PLEASE REENTER | POR FAVO REENTRAR | POR FAVO REINGRESE | SVP RE-ENTREZ
 7 | PO NUMBER | NÚMERO PO | NUMERO PO | No COMMANDE
 8 | DRIVER ID | LICENÇA | LICENCIA | ID CONDUCTEUR
 9 | ODOMETER | ODOMETER | ODOMETRO | ODOMETRE
 10 | ID NUMBER | NÚMERO ID | NUMERO ID | No IDENT
 11 | EQUIP CODE | EQUIP CODE | CODIGO EQUIP | CODE EQUIPEMENT
 12 | DRIVERS ID | DRIVER ID | ID CONDUCTOR | ID CONDUCTEUR
 13 | JOB NUMBER | EMP NÚMERO | NUMERO EMP | No TRAVAIL
 14 | WORK ORDER | TRABALHO ORDEM | ORDEN TRABAJO | FICHE TRAVAIL
 15 | VEHICLE ID | ID VEÍCULO | ID VEHICULO | ID VEHICULE
 16 | ENTER DRIVER | ENTER DRIVER | INGRESE CONDUCTOR | ENTR CONDUCTEUR
 17 | ENTER DEPT | ENTER DEPT | INGRESE DEPT | ENTR DEPARTEMNT
 18 | ENTER PHONE | ADICIONAR PHONE | INGRESE TELEFONO | ENTR No TELEPH
 19 | ENTER ROUTE | ROUTE ADD | INGRESE RUTA | ENTREZ ROUTE
 20 | ENTER FLEET | ENTER FROTA | INGRESE FLOTA | ENTREZ PARC AUTO
 21 | ENTER JOB ID | ENTER JOB ID | INGRESE ID TRABAJO | ENTR ID TRAVAIL
 22 | ROUTE NUMBER | NÚMERO PATH | RUTA NUMERO | No ROUTE
 23 | ENTER USER ID | ENTER USER ID | INGRESE ID USUARIO | ID UTILISATEUR
 24 | FLEET NUMBER | NÚMERO DE FROTA | FLOTA NUMERO | No PARC AUTO
 25 | ENTER PRODUCT | ADICIONAR PRODUTO | INGRESE PRODUCTO | ENTREZ PRODUIT
 26 | DRIVER NUMBER | NÚMERO DRIVER | CONDUCTOR NUMERO | No CONDUCTEUR
 27 | ENTER LICENSE | ENTER LICENÇA | INGRESE LICENCIA | ENTREZ PERMIS
 28 | ENTER FLEET NO | ENTER NRO FROTA | INGRESE NRO FLOTA | ENT No PARC AUTO
 29 | ENTER CAR WASH | WASH ENTER | INGRESE LAVADO | ENTREZ LAVE-AUTO
 30 | ENTER VEHICLE | ENTER VEÍCULO | INGRESE VEHICULO | ENTREZ VEHICULE
 31 | ENTER TRAILER | TRAILER ENTER | INGRESE TRAILER | ENTREZ REMORQUE
 32 | ENTER ODOMETER | ENTER ODOMETER | INGRESE ODOMETRO | ENTREZ ODOMETRE
 33 | DRIVER LICENSE | CARTEIRA DE MOTORISTA | LICENCIA CONDUCTOR | PERMIS CONDUIRE
 34 | ENTER CUSTOMER | ENTER CLIENTE | INGRESE CLIENTE | ENTREZ CLIENT
 35 | VEHICLE NUMBER | NÚMERO DO VEÍCULO | VEHICULO NUMERO | No VEHICULE
 36 | ENTER CUST DATA | ENTER CLIENTE INFO | INGRESE INFO CLIENTE | INFO CLIENT
 37 | REENTER DRIVID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENTR ID COND
 38 | ENTER USER DATA | ENTER INFO USUÁRIO | INGRESE INFO USUARIO | INFO UTILISATEUR
 39 | ENTER CUST CODE | ENTER CODE. CLIENTE | INGRESE COD. CLIENTE | ENTR CODE CLIENT
 40 | ENTER EMPLOYEE | ENTER FUNCIONÁRIO | INGRESE EMPLEADO | ENTREZ EMPLOYE
 41 | ENTER ID NUMBER | ENTER NÚMERO ID | INGRESE NUMERO ID | ENTREZ No ID
 42 | ENTER DRIVER ID | ENTER ID DRIVER | INGRESE ID CONDUCTOR | No CONDUCTEUR
 43 | ENTER FLEET PIN | ENTER PIN FROTA | INGRESE PIN DE FLOTA | NIP PARC AUTO
 44 | ODOMETER NUMBER | NÚMERO ODOMETER | ODOMETRO NUMERO | No ODOMETRE
 45 | ENTER DRIVER LIC | ENTER DRIVER LIC | INGRESE LIC CONDUCTOR | PERMIS CONDUIRE
 46 | ENTER TRAILER NO | NRO TRAILER ENTER | INGRESE NRO TRAILER | ENT No REMORQUE
 47 | REENTER VEHICLE | REENTRAR VEÍCULO | REINGRESE VEHICULO | RE-ENTR VEHICULE
 48 | ENTER VEHICLE ID | ENTER VEÍCULO ID | INGRESE ID VEHICULO | ENTR ID VEHICULE
 49 | ENTER BIRTH DATE | INSERIR DATA NAC | INGRESE FECHA NAC | ENT DT NAISSANCE
 50 | ENTER DOB MMDDYY | ENTER FDN MMDDYY | INGRESE FDN MMDDAA | NAISSANCE MMJJAA
 51 | ENTER FLEET DATA | ENTER FROTA INFO | INGRESE INFO DE FLOTA | INFO PARC AUTO
 52 | ENTER REFERENCE | ENTER REFERÊNCIA | INGRESE REFERENCIA | ENTREZ REFERENCE
 53 | ENTER AUTH NUMBR | ENTER NÚMERO AUT | INGRESE NUMERO AUT | No AUTORISATION
 54 | ENTER HUB NUMBER | ENTER HUB NRO | INGRESE NRO HUB | ENTREZ No NOYAU
 55 | ENTER HUBOMETER | MEDIDA PARA ENTRAR HUB | INGRESE MEDIDO DE HUB | COMPTEUR NOYAU
 56 | ENTER TRAILER ID | TRAILER ENTER ID | INGRESE ID TRAILER | ENT ID REMORQUE
 57 | ODOMETER READING | QUILOMETRAGEM | LECTURA ODOMETRO | LECTURE ODOMETRE
 58 | REENTER ODOMETER | REENTRAR ODOMETER | REINGRESE ODOMETRO | RE-ENT ODOMETRE
 59 | REENTER DRIV. ID | REENTRAR DRIVER ID | REINGRESE ID CHOFER | RE-ENT ID CONDUC
 60 | ENTER CUSTOMER ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 61 | ENTER CUST. ID | ENTER CLIENTE ID | INGRESE ID CLIENTE | ENTREZ ID CLIENT
 62 | ENTER ROUTE NUM | ENTER NUM ROUTE | INGRESE NUM RUTA | ENT No ROUTE
 63 | ENTER FLEET NUM | FROTA ENTER NUM | INGRESE NUM FLOTA | ENT No PARC AUTO
 64 | FLEET PIN | FROTA PIN | PIN DE FLOTA | NIP PARC AUTO
 65 | DRIVER # | DRIVER # | CONDUCTOR # | CONDUCTEUR
 66 | ENTER DRIVER # | ENTER DRIVER # | INGRESE CONDUCTOR # | ENT # CONDUCTEUR
 67 | VEHICLE # | VEÍCULO # | VEHICULO # | # VEHICULE
 68 | ENTER VEHICLE # | ENTER VEÍCULO # | INGRESE VEHICULO # | ENT # VEHICULE
 69 | JOB # | TRABALHO # | TRABAJO # | # TRAVAIL
 70 | ENTER JOB # | ENTER JOB # | INGRESE TRABAJO # | ENTREZ # TRAVAIL
 71 | DEPT NUMBER | NÚMERO DEPT | NUMERO DEPTO | No DEPARTEMENT
 72 | DEPARTMENT # | DEPARTAMENTO # | DEPARTAMENTO # | DEPARTEMENT
 73 | ENTER DEPT # | ENTER DEPT # | INGRESE DEPTO # | ENT# DEPARTEMENT
 74 | LICENSE NUMBER | NÚMERO DE LICENÇA | NUMERO LICENCIA | No PERMIS
 75 | LICENSE # | LICENÇA # | LICENCIA # | # PERMIS
 76 | ENTER LICENSE # | ENTER LICENÇA # | INGRESE LICENCIA # | ENTREZ # PERMIS
 77 | DATA | INFO | INFO | INFO
 78 | ENTER DATA | ENTER INFO | INGRESE INFO | ENTREZ INFO
 79 | CUSTOMER DATA | CLIENTE INFO | INFO CLIENTE | INFO CLIENT
 80 | ID # | ID # | ID # | # ID
 81 | ENTER ID # | ENTER ID # | INGRESE ID # | ENTREZ # ID
 82 | USER ID | USER ID | ID USUARIO | ID UTILISATEUR
 83 | ROUTE # | ROUTE # | RUTA # | # ROUTE
 84 | ENTER ROUTE # | ADD ROUTE # | INGRESE RUTA # | ENTREZ # ROUTE
 85 | ENTER CARD NUM | ENTER NÚMERO DE CARTÃO | INGRESE NUM TARJETA | ENTREZ NO CARTE
 86 | EXP DATE(YYMM) | VALIDADE VAL (AAMM) | FECHA EXP (AAMM) | DATE EXPIR(AAMM)
 87 | PHONE NUMBER | TELEFONE | NUMERO TELEFONO | NO TEL
 88 | CVV START DATE | CVV DATA DE INÍCIO | CVV FECHA INICIO | CVV DATE DE DEBUT
 89 | ISSUE NUMBER | NÚMERO DE EMISSÃO | NUMERO DE EMISION | NO DEMISSION
 90 | START DATE (MMYY) | DATA DE INÍCIO (AAMM) | FECHA INICIO (AAMM) | DATE DE DEBUT-AAMM
 */
-(RETURN_CODE) getAmount:(int)minLength maxLength:(int)maxLength messageID:(int)mID language:(LANGUAGE_TYPE)lang;

/**
 * Upload JPEG to device
 - BTPay 200

 *
 Stores a picture on the device. The picture's dimensions must not exceed the display resolution of 128 x 64. The picture must be RGB JPEG.
 @param picture RGB JPEG image data
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) uploadJPEG:(NSData*)picture;

/**
 * Show stored picture on the LCD
 - BTPay 200

 *
 Show stored picture on the LCD defined by top left point [X0][Y0] and bottom right point [X1][Y1].  The values of X must be in the range 0-127, and the values of Y must be in the range of 0-63
 @param X0 Upper left X coordinate
 @param Y0 Upper left Y coordinate
 @param X1 Lower left X coordinate
 @param Y1 Lower left Y coordinate
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 
 */

-(RETURN_CODE) showJPEG:(int)X0 Y0:(int)Y0 X1:(int)X1 Y1:(int)Y1;

/**
 * Is Audio Reader Connected
 - UniPay
 
 *
 Returns value on device connection status when device is an audio-type connected to headphone plug.
 
 @retval BOOL True = Connected, False = Disconnected
 
 */

-(BOOL) isAudioReaderConnected;

/**
 * Connect To Audio Reader
 - UniPay
 
 *
 Attemps to recognize and connect to an IDTech MSR device connected via the audio port.
  * @retval RETURN_CODE
 */

-(RETURN_CODE) connectToAudioReader;

/**
 * Cancel Connect To Audio Reader
 - UniPay
 
 *
Cancels a connection attempt to an IDTech MSR device connected via the audio port.
  * @retval RETURN_CODE
 */

-(RETURN_CODE) cancelConnectToAudioReader;

/**
 * Set Volume To Audio Reader
 - UniPay
 
 *
 Set the iPhone’s volume for command communication with audio-based readers. The the range of iPhone’s volume is from 0.1 to 1.0.

 @param val Volume level from 0.1 to 1.0
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setAudioVolume:(float)val;

/**
 * Card Insertion Timeout
 - UniPay
 
 *
 During the beginning and end of an EMV transaction, if a card insertion or removal is requested, this method sets the timeout value.  Default is 30 seconds.
 
 @param sec Timeout value in seconds
 
 
 */
-(void) cardInsertTimeout:(int)sec;


/**
 * Get Tag
 - UniPay
 - BTPay
 
 *
 Retrieves an EMV tag from the inserted card.  Only available after the card has been processed after executing IDT_Device::startEMVTransaction:otherAmount:timeout:cashback:additionalTags:()
 
 @param tagName Name fo tag to retrieve
 @param data Pointer that will return location of tag data
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getTag:(NSString*)tagName tagData:(NSData**)data;


/**
 * Get All Tags
 - UniPay
 - BTPay
 
 *
 Retrieves all EMV tags from the inserted card.  Only available after the card has been processed after executing IDT_Device::startEMVTransaction:otherAmount:timeout:cashback:additionalTags:()
 
 @param data Pointer that will return location of dictionary with all tag values.  Key is NSString, Object is NSData.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getAllTags:(NSDictionary**)data;

/**
 * Get Mask and Encryption
 - BTPay
 
 *
 Retrieves the MSR Mask and Encryption settings
 
 @param data Pointer that will return location of MaskAndEncryption structure.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getMaskAndEncryption:(MaskAndEncryption**)data;


/**
 * Restore Mask and Encryption default settings
 - BTPay
 
 *
 Restores the default values for MSR Mask and Encryption settings
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) restoreMaskAndEncryptionDefaults;

/**
 * Set PAN masking character
 - BTPay
 
 *
 Sets the character for PAN masking
 
 @param maskChar Masking character.  Default value '*';
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setPANMaskingCharacter:(char)maskChar;

/**
 * Set PrePAN Clear Digits
 - BTPay
 
 *
 Sets the number of digits to show in clear text at the beginning of PAN
 
 @param clearDigits Amount of characters to display cleartext at beginning of PAN. Valid range 0-6.  Default value 4.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setPrePANClearDigits:(int)clearDigits;
/**
 * Set PostPAN Clear Digits
 - BTPay
 
 *
 Sets the number of digits to show in clear text at the ending of PAN
 
 @param clearDigits Amount of characters to display cleartext at end of PAN. Valid range 0-4.  Default value 4.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setPostPANClearDigits:(int)clearDigits;


/**
 * Set Expiration Date masking
 - BTPay
 
 *
 Sets the flag to enable Expiratin Date masking
 
 @param mask TRUE = mask expiration date.  FALSE = display expiration date in cleartext
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setExpMasking:(BOOL)mask;





/**
 * Set Base Key Type
 - UniPay II
 
 *
 Sets the base key type
 
 @param maskOption Mask Option
 Bit0: T1 mask allowed
 Bit1: T2 mask allowed
 Bit2: T3 mask allowed
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setMaskingOption:(char)maskOption;

/**
 * Set Encryption Key Type
 - UniPay II
 
 *
 Sets the encryption key type
 
 @param encOption Encryption Option
 Bit 0 : T1 force encrypt
 Bit 1 : T2 force encrypt
 Bit 2 : T3 force encrypt
 Bit 3 : T3 force encrypt when card type is 0
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setEncryptionOption:(char)encOption;

/**
 * Get Language Type
 - BTPay
 
 *
 Gets the language type
 
 @param response LANGUAGE_TYPE of the BTPay
 ENGLISH: 01
 PORTUGUESE: 02
 SPANISH: 03
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getLanguageType:(NSUInteger**)response;
/**
 * Set Language Type
 - BTPay
 
 *
 Sets the language type of BTPay prompts
 
 @param lang LANGUAGE_TYPE
 ENGLISH: 01
 PORTUGUESE: 02
 SPANISH: 03
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setLanguageType:(LANGUAGE_TYPE)lang;

/**
 * Set Pre/Post PAN Ctrl Data Length
 - UniPayII
 *
Sets the length of the PAN Pre and Post Ctrl Data
 
 @param pre  Amount of digits Pre Ctrl Data
 @param post  Amount of digits Post Ctrl Data
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setPrePostPANCtrlData:(Byte)pre post:(Byte)post;

/**
 * Get Pre/Post PAN Ctrl Data Length
 - UniPayII
 *
 Gets the length of the PAN Pre and Post Ctrl Data
 
 @param pre  Return amount of digits Pre Ctrl Data
 @param post  Return amount of digits Post Ctrl Data

 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getPrePostPANCtrlData:(Byte**)pre post:(Byte**)post;


/**
 * Get Mask Character
 - UniPayII
 *
Gets the PAN Mask Character
 
 @param response  Single character NSString with the mask character.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getASCIIMaskData:(NSString**)mask;

/**
 * Set Mask Character
  - UniPayII
 *
 Sets the PAN Mask Character
 
 @param mask  Single character NSString with the mask character.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setASCIIMaskData:(NSString*)mask;

/**
 * Get ICC Connector
 - UniPayII
 *
 Gets the ICC Connector
 
 @param response ICC Connector
 00: User Card Connector
 01: SAM Connector
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getICCConnector:(NSUInteger**)response;

/**
 * Set ICC Connector
 - UniPayII
 *
 Sets the ICC Connector
 
 @param connector ICC Connector
 00: user Car Connector (main module)
 01: SAM Connector
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setICCConnector:(int)connector;

/**
 * Get BCD Mask Data
 - UniPayII
 *
 Gets the BCD Mask Data.  Valid values are 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
 
 @param response Mask Value
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getBCDMaskData:(NSUInteger**)response;

/**
 * Set BCD Mask Data
 - UniPayII
 *
 Sets the BCD Mask Data.  Valid values are 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
 
 @param mask  Mask value, valid values are 10 - 15  (0x0A - 0x0F)
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setBCDMaskData:(int)mask;


/**
 * Set Default ICC Group Settings
 - UniPayII
 *
 Restores ICC Group Settings to defaults;
 
Function Name | Default Value
  -------- | --------
ICC Connector | User Card Connector (main module)
PrePANCtlDataLen | 4
PostPANCtlDataLen | 4
AsciiMaskData | 0x2A
BCDMaskData | 0x0C
 

 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setDefaultICCGroupSettings;

/**
 * Get Account DUKPT Key Variant
 - UniPayII
 
 *
 Gets the Key Variant for DUKPT
 
 @param response Key Variant
 DUKPT Data Key: 00
 DUKPT PIN Key: 01
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getDUKPTKeyVariant:(NSUInteger**)response;
/**
 * Set Account DUKPT Key Variant
 - UniPayII
 
 *
 Sets the response Key Variant
 
 @param type Key Variant
 DUKPT Data Key: 00
 DUKPT PIN Key: 01
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setDUKPTKeyVariant:(int)type;

/**
 * Get Account DUKPT Key Variant
 - UniPayII
 
 *
 Gets the DUKPT Key encryption and decryption modes
 
 @param response Current Encryption/Decryption Mode
 TDES: 00
 AES: 01
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getDUKPTKeyEncryption:(NSUInteger**)response;
/**
 * Get Account DUKPT Key Encryption/Decryption mode
 - UniPayII
 
 *
 Sets the DUKPT Key encryption and decryption modes
 
 @param type Encryption/Decryption Mode
 TDES: 00
 AES: 01
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setDUKPTKeyEncryption:(int)type;

/**
 * Get Function Key
 - UniPayII
 *
@param response Returns the key pressed mapped to ASCII according to the following table:
 - "C": Cancel Key
 - "B": Backspace Key
 - "E": Enter Key
 - "F1": F1 Key
 - "F2": F2 Key
 
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - Cancel Command can terminate this command.
 
 */
-(RETURN_CODE) getFunctionKeyUniPay:(NSString**)response;

/**
 * Get Function Key
 - BTPay 200
 *
 Returns function key value of pressed key in deviceDelegate::pinpadData:keySN:event:() with event EVENT_PINPAD_FUNCTION_KEY.  Value passed as NSData in pinpadData with one of the following values
  - 0x43: Cancel Key
  - 0x42: Backspace Key
  - 0x45: Enter Key
  - 0x23: # Key
  - 0x2A: * Key
 
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - Cancel Command can terminate this command.
 
 */
-(RETURN_CODE) getFunctionKey;

/**
 Is Device Connected
 
Returns the connection status of the requested device
 
 @param device Check connectivity of device type
 */
-(bool) isConnected:(IDT_DEVICE_Types)device;

/**
 * Set ICC Notifications
 *
 Determins if card insert/remove events are captured and sent to delegate UniPay_EventFunctionICC

 
 @param turnON  TRUE = monitor ICC card events, FALSE = ignore ICC card events.  Default value is FALSE/OFF.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setICCNotification:(BOOL)turnON;


/**
 * Set Minimum Wait Time
 *
 Sets the minimum wait time when waiting for a response.  Used by system when sequence of commands are being received out of sync.
 
 
 @param time  Time, in seconds, to wait for response.
 
 
 */

-(void) minWaitTime:(float)time;

/**
 * Get Current Device
 *
 returns the current device that is communicating with this class
 
 
 @param type IDT_DEVICE_Types.
 
 
 */
-(IDT_DEVICE_Types) getCurrentDevice;

/**
 * Polls device for next KSN on UniMag
 
 *
 * @param response  Returns next KSN on UniMag
 
 * @retval RETURN_CODE:  Return codes listed as typedef enum in IDTCommon:RETURN_CODE.  Values can be parsed with IDT_UniMag::device_getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getNextKSN:(NSData**)response;


/**
 * Send UniMag Command
 
 *
 * @param command  A command to execute from UNIMAG_COMMAND_Types enumeration
 
 * @retval RETURN_CODE:  Return codes listed as typedef enum in IDTCommon:RETURN_CODE.  Values can be parsed with IDT_UniMag::device_getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) sendUniMagCommand:(UNIMAG_COMMAND_Types)command;

/**
 * Device Is Connected
 *
 @retval returns value if any IDT Device is connected.
 
 
 
 */
-(BOOL) deviceIsConnected;

/**
 * UniPay Extended Firmware Version
 *
 @retval returns value of UniPay Exteneded Firmware. Internal use only.
 

 */
-(NSString*) getFirmwareVersionExtendedUnipay;


/**
 * Get Multi APDU
 *
 @retval returns value of UniPay Multi APDU request. Internal use only.
 
 
 */
-(NSArray*) getMultiAPDU;

/**
 * Disable Auto Authenticate Transaction
 *
 If auto authenticate is DISABLED, authenticateTransaction must be called after a successful startTransaction execution.
 
 @param disable  FALSE = auto authenticate ENABLED, TRUE = auto authenticate DISABLED
 
 */
-(void) disableAutoAuthenticateTransaction:(BOOL)disable;

/**
 * Authenticate Transaction
 
 Authenticated a transaction after startTransaction successfully executes.
 
 By default, auto authorize is ENABLED.  If auto authorize is DISABLED, this function must be called after a result EMV_RESULT_CODE_START_TRANSACTION_SUCCESS returned to emvTransactionData delegate protocol is received after a startTransaction call.  If auto authorize is ENABLED (default), this method will automatically be executed after receiving the result EMV_RESULT_CODE_START_TRANSACTION_SUCCESS after startTransaction.  The auto authorize can be enabled/disabled with IDT_DEVICE::disableAutoAuthenticateTransaction:()
 *
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) authenticateTransaction;

/**
 * Get Level and Baud
  *
 @param response  The Baud Rate and Audio Level.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) getLevelAndBaud:(NSString**)response;

/**
 * Assign Delegate 2
 *
 Assigns a second delegate. Internal use only.
 
 @param del  IDT_Device_Delegate delegate.
 

 
 */



-(void) assignDelegate2:(id<IDT_Device_Delegate>)del;

/**
 * Exchange Multi APDU
 *
 Sends multiple APDU commands within on command
 
 @param dataAPDU  An array of NSData APDU commands
 @param response  The combined response of the multiple APDU commands
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) exchangeMultiAPDU:(NSArray*)dataAPDU response:(NSData**)response;

/**
 * Get Resource Bundle Version
 *
 Retrieves the Resource Version
 
 @retval Version - the resource version
 
  */
+(NSString*) getResourceBundleVersion;

/**
 * Attempt connection
 *
 Reserved for system use
  */
-(void) attemptConnect;

/**
 * Loop for card insertion/removal
 *
 Reserved for system use
 */
-(void) loopCardCheck;

/**
 * Sends ping command to VivoTech reader
 *
 Reserved for system use
 */
-(RETURN_CODE) pingVivoTech;

/**
 * VendiMobile Command
 *
 Reserved for system use
 */
-(NSData*) grsiP2Command:(unsigned char)command statusCode:(unsigned char)status data:(NSData*)d1;

/**
 * Set Pass Through
 
 Sets Pass-Through mode on Vendi-Mobile series
 *
 @param enablePassThrough  TRUE = Pass through enabled
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_Device::getResponseCodeString:()
 
 */
-(RETURN_CODE) setPassThrough:(BOOL)enablePassThrough;

/**
 * Multi App Selection
 *
 Reserved for system use
 */
-(NSMutableArray*) multiAPP:(NSArray*)AIDS;

/**
 * Multi File Selection
 *
 Reserved for system use
 */
-(NSMutableArray*) multiFile:(NSArray*)apduArray;

@end
