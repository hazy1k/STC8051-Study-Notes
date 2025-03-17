
#ifndef _YMODEM_H_
#define _YMODEM_H_

#define PACKET_SEQNO_INDEX      1
#define PACKET_SEQNO_COMP_INDEX 2

#define PACKET_HEADER           3
#define PACKET_TRAILER          2
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             128
#define PACKET_1K_SIZE          1024

#define FILE_NAME_LENGTH        32
#define FILE_SIZE_LENGTH        8

#define SOH                     0x01  /* start of 128-byte data packet */
#define STX                     0x02  /* start of 1024-byte data packet */
#define EOT                     0x04  /* end of transmission */
#define ACK                     0x06  /* acknowledge */
#define NAK                     0x15  /* negative acknowledge */
#define CANCEL                  0x18  /* two of these in succession aborts transfer */
#define CRC16                   0x43  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  'A'  /* 'A' == 0x41, abort by user */
#define ABORT2                  'a'  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             10000
#define MAX_ERRORS              5


#endif

/*******************(C)COPYRIGHT 2013 STC *****END OF FILE****/
