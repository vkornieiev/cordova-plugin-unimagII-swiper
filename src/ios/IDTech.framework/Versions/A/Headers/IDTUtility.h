//
//  IDTUtility.h
//  IDTech
//
//  Created by Randy Palermo on 5/9/14.
//  Copyright (c) 2014 IDTech Products. All rights reserved.
//


@interface IDTUtility : NSObject

+ (NSBundle *)frameworkBundle;

+(NSString*) dataToPrintableString:(NSData*)data;

+ (NSString*) hexToString:(NSData*)data startByte:(int)start length:(int)len;
+ (NSString*) dataToString:(NSData*)data startByte:(int)start length:(int)len;
+ (NSString *) getSubString:(NSString*)str startPosition:(int)start length:(int)len;
+ (unsigned int) hex2int:(NSString*)hex;
+ (NSData *)hexToData:(NSString*)str ;
+ (NSString*) dataToString:(NSData*)data;
+ (NSData*) extendedHextoHex:(NSData*)data;
+ (NSData*) stringToData:(NSString *)str;
+ (NSString*) hexToString:(NSString*)str;
+(NSString*) dataToHexString:(NSData*)data;
+(NSData*) encodeTags:(NSMutableDictionary*)tags;
+(NSDictionary*) processTLV:(NSData*)param;
+ (NSString *) stringByPaddingTheLeftToLength:(NSString*)orig newLen:(NSUInteger)newLength withString:(NSString *) padString;
+ (NSString*) padLeftZeros:(NSString *)orig Length:(int)len;
+ (NSString*) padRightZeros:(NSString *)orig Length:(int)len;
+(NSData*) shortToDataReverse:(short)val;
+(NSData*) shortToData:(short)val;
+(NSData*) longToData:(long)val;
+(NSData*) byteToData:(Byte)val;
+(NSData*) computeSHA1:(NSData*) data;


@end
