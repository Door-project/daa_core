/********************************************************************************/
/*										*/
/*			ECC curve data 						*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*            $Id: CryptEccData.c 1658 2021-01-22 23:14:01Z kgoldman $		*/
/*										*/
/*  Licenses and Notices							*/
/*										*/
/*  1. Copyright Licenses:							*/
/*										*/
/*  - Trusted Computing Group (TCG) grants to the user of the source code in	*/
/*    this specification (the "Source Code") a worldwide, irrevocable, 		*/
/*    nonexclusive, royalty free, copyright license to reproduce, create 	*/
/*    derivative works, distribute, display and perform the Source Code and	*/
/*    derivative works thereof, and to grant others the rights granted herein.	*/
/*										*/
/*  - The TCG grants to the user of the other parts of the specification 	*/
/*    (other than the Source Code) the rights to reproduce, distribute, 	*/
/*    display, and perform the specification solely for the purpose of 		*/
/*    developing products based on such documents.				*/
/*										*/
/*  2. Source Code Distribution Conditions:					*/
/*										*/
/*  - Redistributions of Source Code must retain the above copyright licenses, 	*/
/*    this list of conditions and the following disclaimers.			*/
/*										*/
/*  - Redistributions in binary form must reproduce the above copyright 	*/
/*    licenses, this list of conditions	and the following disclaimers in the 	*/
/*    documentation and/or other materials provided with the distribution.	*/
/*										*/
/*  3. Disclaimers:								*/
/*										*/
/*  - THE COPYRIGHT LICENSES SET FORTH ABOVE DO NOT REPRESENT ANY FORM OF	*/
/*  LICENSE OR WAIVER, EXPRESS OR IMPLIED, BY ESTOPPEL OR OTHERWISE, WITH	*/
/*  RESPECT TO PATENT RIGHTS HELD BY TCG MEMBERS (OR OTHER THIRD PARTIES)	*/
/*  THAT MAY BE NECESSARY TO IMPLEMENT THIS SPECIFICATION OR OTHERWISE.		*/
/*  Contact TCG Administration (admin@trustedcomputinggroup.org) for 		*/
/*  information on specification licensing rights available through TCG 	*/
/*  membership agreements.							*/
/*										*/
/*  - THIS SPECIFICATION IS PROVIDED "AS IS" WITH NO EXPRESS OR IMPLIED 	*/
/*    WARRANTIES WHATSOEVER, INCLUDING ANY WARRANTY OF MERCHANTABILITY OR 	*/
/*    FITNESS FOR A PARTICULAR PURPOSE, ACCURACY, COMPLETENESS, OR 		*/
/*    NONINFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS, OR ANY WARRANTY 		*/
/*    OTHERWISE ARISING OUT OF ANY PROPOSAL, SPECIFICATION OR SAMPLE.		*/
/*										*/
/*  - Without limitation, TCG and its members and licensors disclaim all 	*/
/*    liability, including liability for infringement of any proprietary 	*/
/*    rights, relating to use of information in this specification and to the	*/
/*    implementation of this specification, and TCG disclaims all liability for	*/
/*    cost of procurement of substitute goods or services, lost profits, loss 	*/
/*    of use, loss of data or any incidental, consequential, direct, indirect, 	*/
/*    or special damages, whether under contract, tort, warranty or otherwise, 	*/
/*    arising in any way out of use or reliance upon this specification or any 	*/
/*    information herein.							*/
/*										*/
/*  (c) Copyright IBM Corp. and others, 2018 - 2021				*/
/*										*/
/********************************************************************************/

/* 10.2.8	CryptEccData.c */
#include "Tpm.h"
#include "OIDs.h"
/* This file contains the ECC curve data. The format of the data depends on the setting of
   USE_BN_ECC_DATA. If it is defined, then the TPM's BigNum() format is used. Otherwise, it is kept
   in TPM2B format. The purpose of having the data in BigNum() format is so that it does not have to
   be reformatted before being used by the crypto library. */
#if ALG_ECC
#if USE_BN_ECC_DATA
#       define TO_ECC_64                        TO_CRYPT_WORD_64
#       define TO_ECC_56(a, b, c, d, e, f, g)   TO_ECC_64(0, a, b, c, d, e, f, g)
#       define TO_ECC_48(a, b, c, d, e, f)      TO_ECC_64(0, 0, a, b, c, d, e, f)
#       define TO_ECC_40(a, b, c, d, e)         TO_ECC_64(0, 0, 0, a, b, c, d, e)
#   if RADIX_BITS > 32
#       define TO_ECC_32(a, b, c, d)            TO_ECC_64(0, 0, 0, 0, a, b, c, d)
#       define TO_ECC_24(a, b, c)               TO_ECC_64(0, 0, 0, 0, 0, a, b, c)
#       define TO_ECC_16(a, b)                  TO_ECC_64(0, 0, 0, 0, 0, 0, a, b)
#       define TO_ECC_8(a)                      TO_ECC_64(0, 0, 0, 0, 0, 0, 0, a)
#   else // RADIX_BITS == 32
#       define TO_ECC_32                        BIG_ENDIAN_BYTES_TO_UINT32
#       define TO_ECC_24(a, b, c)               TO_ECC_32(0, a, b, c)
#       define TO_ECC_16(a, b)                  TO_ECC_32(0, 0, a, b)
#       define TO_ECC_8(a)                      TO_ECC_32(0, 0, 0, a)
#   endif
#else // TPM2B_
#   define TO_ECC_64(a, b, c, d, e, f, g, h) a, b, c, d, e, f, g, h
#   define TO_ECC_56(a, b, c, d, e, f, g)    a, b, c, d, e, f, g
#   define TO_ECC_48(a, b, c, d, e, f)       a, b, c, d, e, f
#   define TO_ECC_40(a, b, c, d, e)          a, b, c, d, e
#   define TO_ECC_32(a, b, c, d)             a, b, c, d
#   define TO_ECC_24(a, b, c)                a, b, c
#   define TO_ECC_16(a, b)                   a, b
#   define TO_ECC_8(a)                       a
#endif
#if USE_BN_ECC_DATA
#define BN_MIN_ALLOC(bytes)						\
    (BYTES_TO_CRYPT_WORDS(bytes) == 0) ? 1 : BYTES_TO_CRYPT_WORDS(bytes)
# define ECC_CONST(NAME, bytes, initializer)				\
    const struct {							\
	crypt_uword_t   allocate, size, d[BN_MIN_ALLOC(bytes)];		\
    } NAME = {BN_MIN_ALLOC(bytes), BYTES_TO_CRYPT_WORDS(bytes),{initializer}}
	ECC_CONST(ECC_ZERO, 0, 0);
#else
# define ECC_CONST(NAME, bytes, initializer)				\
    const TPM2B_##bytes##_BYTE_VALUE NAME = {bytes, {initializer}}
/* Have to special case ECC_ZERO */
TPM2B_BYTE_VALUE(1);
TPM2B_1_BYTE_VALUE ECC_ZERO = {1, {0}};
#endif
ECC_CONST(ECC_ONE, 1, 1);
#if !USE_BN_ECC_DATA
TPM2B_BYTE_VALUE(24);
#define TO_ECC_192(a, b, c)  a, b, c
TPM2B_BYTE_VALUE(28);
#define TO_ECC_224(a, b, c, d)   a, b, c, d
TPM2B_BYTE_VALUE(32);
#define TO_ECC_256(a, b, c, d)   a, b, c, d
TPM2B_BYTE_VALUE(48);
#define TO_ECC_384(a, b, c, d, e, f)     a, b, c, d, e, f
TPM2B_BYTE_VALUE(66);
#define TO_ECC_528(a, b, c, d, e, f, g, h, i)    a, b, c, d, e, f, g, h, i
TPM2B_BYTE_VALUE(80);
#define TO_ECC_640(a, b, c, d, e, f, g, h, i, j)     a, b, c, d, e, f, g, h, i, j
#else
#define TO_ECC_192(a, b, c)  c, b, a
#define TO_ECC_224(a, b, c, d)   d, c, b, a
#define TO_ECC_256(a, b, c, d)   d, c, b, a
#define TO_ECC_384(a, b, c, d, e, f)     f, e, d, c, b, a
#define TO_ECC_528(a, b, c, d, e, f, g, h, i)    i, h, g, f, e, d, c, b, a
#define TO_ECC_640(a, b, c, d, e, f, g, h, i, j)     j, i, h, g, f, e, d, c, b, a
#endif // !USE_BN_ECC_DATA
#if ECC_NIST_P192
ECC_CONST(NIST_P192_p, 24, TO_ECC_192(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)));
ECC_CONST(NIST_P192_a, 24, TO_ECC_192(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC)));
ECC_CONST(NIST_P192_b, 24, TO_ECC_192(
				      TO_ECC_64(0x64, 0x21, 0x05, 0x19, 0xE5, 0x9C, 0x80, 0xE7),
				      TO_ECC_64(0x0F, 0xA7, 0xE9, 0xAB, 0x72, 0x24, 0x30, 0x49),
				      TO_ECC_64(0xFE, 0xB8, 0xDE, 0xEC, 0xC1, 0x46, 0xB9, 0xB1)));
ECC_CONST(NIST_P192_gX, 24, TO_ECC_192(
				       TO_ECC_64(0x18, 0x8D, 0xA8, 0x0E, 0xB0, 0x30, 0x90, 0xF6),
				       TO_ECC_64(0x7C, 0xBF, 0x20, 0xEB, 0x43, 0xA1, 0x88, 0x00),
				       TO_ECC_64(0xF4, 0xFF, 0x0A, 0xFD, 0x82, 0xFF, 0x10, 0x12)));
ECC_CONST(NIST_P192_gY, 24, TO_ECC_192(
				       TO_ECC_64(0x07, 0x19, 0x2B, 0x95, 0xFF, 0xC8, 0xDA, 0x78),
				       TO_ECC_64(0x63, 0x10, 0x11, 0xED, 0x6B, 0x24, 0xCD, 0xD5),
				       TO_ECC_64(0x73, 0xF9, 0x77, 0xA1, 0x1E, 0x79, 0x48, 0x11)));
ECC_CONST(NIST_P192_n, 24, TO_ECC_192(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x99, 0xDE, 0xF8, 0x36),
				      TO_ECC_64(0x14, 0x6B, 0xC9, 0xB1, 0xB4, 0xD2, 0x28, 0x31)));
#define NIST_P192_h         ECC_ONE
#define NIST_P192_gZ        ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA NIST_P192 = {
				  (bigNum)&NIST_P192_p, (bigNum)&NIST_P192_n, (bigNum)&NIST_P192_h,
				  (bigNum)&NIST_P192_a, (bigNum)&NIST_P192_b,
				  {(bigNum)&NIST_P192_gX, (bigNum)&NIST_P192_gY, (bigNum)&NIST_P192_gZ}};
#else
const ECC_CURVE_DATA NIST_P192 = {
				  &NIST_P192_p.b, &NIST_P192_n.b, &NIST_P192_h.b,
				  &NIST_P192_a.b, &NIST_P192_b.b,
				  {&NIST_P192_gX.b, &NIST_P192_gY.b, &NIST_P192_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_NIST_P192
#if ECC_NIST_P224
ECC_CONST(NIST_P224_p, 28, TO_ECC_224(
				      TO_ECC_32(0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00),
				      TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01)));
ECC_CONST(NIST_P224_a, 28, TO_ECC_224(
				      TO_ECC_32(0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE)));
ECC_CONST(NIST_P224_b, 28, TO_ECC_224(
				      TO_ECC_32(0xB4, 0x05, 0x0A, 0x85),
				      TO_ECC_64(0x0C, 0x04, 0xB3, 0xAB, 0xF5, 0x41, 0x32, 0x56),
				      TO_ECC_64(0x50, 0x44, 0xB0, 0xB7, 0xD7, 0xBF, 0xD8, 0xBA),
				      TO_ECC_64(0x27, 0x0B, 0x39, 0x43, 0x23, 0x55, 0xFF, 0xB4)));
ECC_CONST(NIST_P224_gX, 28, TO_ECC_224(
				       TO_ECC_32(0xB7, 0x0E, 0x0C, 0xBD),
				       TO_ECC_64(0x6B, 0xB4, 0xBF, 0x7F, 0x32, 0x13, 0x90, 0xB9),
				       TO_ECC_64(0x4A, 0x03, 0xC1, 0xD3, 0x56, 0xC2, 0x11, 0x22),
				       TO_ECC_64(0x34, 0x32, 0x80, 0xD6, 0x11, 0x5C, 0x1D, 0x21)));
ECC_CONST(NIST_P224_gY, 28, TO_ECC_224(
				       TO_ECC_32(0xBD, 0x37, 0x63, 0x88),
				       TO_ECC_64(0xB5, 0xF7, 0x23, 0xFB, 0x4C, 0x22, 0xDF, 0xE6),
				       TO_ECC_64(0xCD, 0x43, 0x75, 0xA0, 0x5A, 0x07, 0x47, 0x64),
				       TO_ECC_64(0x44, 0xD5, 0x81, 0x99, 0x85, 0x00, 0x7E, 0x34)));
ECC_CONST(NIST_P224_n, 28, TO_ECC_224(
				      TO_ECC_32(0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0x16, 0xA2, 0xE0, 0xB8, 0xF0, 0x3E),
				      TO_ECC_64(0x13, 0xDD, 0x29, 0x45, 0x5C, 0x5C, 0x2A, 0x3D)));
#define NIST_P224_h         ECC_ONE
#define NIST_P224_gZ        ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA NIST_P224 = {
				  (bigNum)&NIST_P224_p, (bigNum)&NIST_P224_n, (bigNum)&NIST_P224_h,
				  (bigNum)&NIST_P224_a, (bigNum)&NIST_P224_b,
				  {(bigNum)&NIST_P224_gX, (bigNum)&NIST_P224_gY, (bigNum)&NIST_P224_gZ}};
#else
const ECC_CURVE_DATA NIST_P224 = {
				  &NIST_P224_p.b, &NIST_P224_n.b, &NIST_P224_h.b,
				  &NIST_P224_a.b, &NIST_P224_b.b,
				  {&NIST_P224_gX.b, &NIST_P224_gY.b, &NIST_P224_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_NIST_P224
#if ECC_NIST_P256
ECC_CONST(NIST_P256_p, 32, TO_ECC_256(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01),
				      TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
				      TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)));
ECC_CONST(NIST_P256_a, 32, TO_ECC_256(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01),
				      TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
				      TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC)));
ECC_CONST(NIST_P256_b, 32, TO_ECC_256(
				      TO_ECC_64(0x5A, 0xC6, 0x35, 0xD8, 0xAA, 0x3A, 0x93, 0xE7),
				      TO_ECC_64(0xB3, 0xEB, 0xBD, 0x55, 0x76, 0x98, 0x86, 0xBC),
				      TO_ECC_64(0x65, 0x1D, 0x06, 0xB0, 0xCC, 0x53, 0xB0, 0xF6),
				      TO_ECC_64(0x3B, 0xCE, 0x3C, 0x3E, 0x27, 0xD2, 0x60, 0x4B)));
ECC_CONST(NIST_P256_gX, 32, TO_ECC_256(
				       TO_ECC_64(0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47),
				       TO_ECC_64(0xF8, 0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40, 0xF2),
				       TO_ECC_64(0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0),
				       TO_ECC_64(0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96)));
ECC_CONST(NIST_P256_gY, 32, TO_ECC_256(
				       TO_ECC_64(0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B),
				       TO_ECC_64(0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E, 0x16),
				       TO_ECC_64(0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE),
				       TO_ECC_64(0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5)));
ECC_CONST(NIST_P256_n, 32, TO_ECC_256(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84),
				      TO_ECC_64(0xF3, 0xB9, 0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51)));
#define NIST_P256_h         ECC_ONE
#define NIST_P256_gZ        ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA NIST_P256 = {
				  (bigNum)&NIST_P256_p, (bigNum)&NIST_P256_n, (bigNum)&NIST_P256_h,
				  (bigNum)&NIST_P256_a, (bigNum)&NIST_P256_b,
				  {(bigNum)&NIST_P256_gX, (bigNum)&NIST_P256_gY, (bigNum)&NIST_P256_gZ}};
#else
const ECC_CURVE_DATA NIST_P256 = {
				  &NIST_P256_p.b, &NIST_P256_n.b, &NIST_P256_h.b,
				  &NIST_P256_a.b, &NIST_P256_b.b,
				  {&NIST_P256_gX.b, &NIST_P256_gY.b, &NIST_P256_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_NIST_P256
#if ECC_NIST_P384
ECC_CONST(NIST_P384_p, 48, TO_ECC_384(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00),
				      TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF)));
ECC_CONST(NIST_P384_a, 48, TO_ECC_384(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00),
				      TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFC)));
ECC_CONST(NIST_P384_b, 48, TO_ECC_384(
				      TO_ECC_64(0xB3, 0x31, 0x2F, 0xA7, 0xE2, 0x3E, 0xE7, 0xE4),
				      TO_ECC_64(0x98, 0x8E, 0x05, 0x6B, 0xE3, 0xF8, 0x2D, 0x19),
				      TO_ECC_64(0x18, 0x1D, 0x9C, 0x6E, 0xFE, 0x81, 0x41, 0x12),
				      TO_ECC_64(0x03, 0x14, 0x08, 0x8F, 0x50, 0x13, 0x87, 0x5A),
				      TO_ECC_64(0xC6, 0x56, 0x39, 0x8D, 0x8A, 0x2E, 0xD1, 0x9D),
				      TO_ECC_64(0x2A, 0x85, 0xC8, 0xED, 0xD3, 0xEC, 0x2A, 0xEF)));
ECC_CONST(NIST_P384_gX, 48, TO_ECC_384(
				       TO_ECC_64(0xAA, 0x87, 0xCA, 0x22, 0xBE, 0x8B, 0x05, 0x37),
				       TO_ECC_64(0x8E, 0xB1, 0xC7, 0x1E, 0xF3, 0x20, 0xAD, 0x74),
				       TO_ECC_64(0x6E, 0x1D, 0x3B, 0x62, 0x8B, 0xA7, 0x9B, 0x98),
				       TO_ECC_64(0x59, 0xF7, 0x41, 0xE0, 0x82, 0x54, 0x2A, 0x38),
				       TO_ECC_64(0x55, 0x02, 0xF2, 0x5D, 0xBF, 0x55, 0x29, 0x6C),
				       TO_ECC_64(0x3A, 0x54, 0x5E, 0x38, 0x72, 0x76, 0x0A, 0xB7)));
ECC_CONST(NIST_P384_gY, 48, TO_ECC_384(
				       TO_ECC_64(0x36, 0x17, 0xDE, 0x4A, 0x96, 0x26, 0x2C, 0x6F),
				       TO_ECC_64(0x5D, 0x9E, 0x98, 0xBF, 0x92, 0x92, 0xDC, 0x29),
				       TO_ECC_64(0xF8, 0xF4, 0x1D, 0xBD, 0x28, 0x9A, 0x14, 0x7C),
				       TO_ECC_64(0xE9, 0xDA, 0x31, 0x13, 0xB5, 0xF0, 0xB8, 0xC0),
				       TO_ECC_64(0x0A, 0x60, 0xB1, 0xCE, 0x1D, 0x7E, 0x81, 0x9D),
				       TO_ECC_64(0x7A, 0x43, 0x1D, 0x7C, 0x90, 0xEA, 0x0E, 0x5F)));
ECC_CONST(NIST_P384_n, 48, TO_ECC_384(
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xC7, 0x63, 0x4D, 0x81, 0xF4, 0x37, 0x2D, 0xDF),
				      TO_ECC_64(0x58, 0x1A, 0x0D, 0xB2, 0x48, 0xB0, 0xA7, 0x7A),
				      TO_ECC_64(0xEC, 0xEC, 0x19, 0x6A, 0xCC, 0xC5, 0x29, 0x73)));
#define NIST_P384_h         ECC_ONE
#define NIST_P384_gZ        ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA NIST_P384 = {
				  (bigNum)&NIST_P384_p, (bigNum)&NIST_P384_n, (bigNum)&NIST_P384_h,
				  (bigNum)&NIST_P384_a, (bigNum)&NIST_P384_b,
				  {(bigNum)&NIST_P384_gX, (bigNum)&NIST_P384_gY, (bigNum)&NIST_P384_gZ}};
#else
const ECC_CURVE_DATA NIST_P384 = {
				  &NIST_P384_p.b, &NIST_P384_n.b, &NIST_P384_h.b,
				  &NIST_P384_a.b, &NIST_P384_b.b,
				  {&NIST_P384_gX.b, &NIST_P384_gY.b, &NIST_P384_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_NIST_P384
#if ECC_NIST_P521
ECC_CONST(NIST_P521_p, 66, TO_ECC_528(
				      TO_ECC_16(0x01, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)));
ECC_CONST(NIST_P521_a, 66, TO_ECC_528(
				      TO_ECC_16(0x01, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC)));
ECC_CONST(NIST_P521_b, 66, TO_ECC_528(
				      TO_ECC_16(0x00, 0x51),
				      TO_ECC_64(0x95, 0x3E, 0xB9, 0x61, 0x8E, 0x1C, 0x9A, 0x1F),
				      TO_ECC_64(0x92, 0x9A, 0x21, 0xA0, 0xB6, 0x85, 0x40, 0xEE),
				      TO_ECC_64(0xA2, 0xDA, 0x72, 0x5B, 0x99, 0xB3, 0x15, 0xF3),
				      TO_ECC_64(0xB8, 0xB4, 0x89, 0x91, 0x8E, 0xF1, 0x09, 0xE1),
				      TO_ECC_64(0x56, 0x19, 0x39, 0x51, 0xEC, 0x7E, 0x93, 0x7B),
				      TO_ECC_64(0x16, 0x52, 0xC0, 0xBD, 0x3B, 0xB1, 0xBF, 0x07),
				      TO_ECC_64(0x35, 0x73, 0xDF, 0x88, 0x3D, 0x2C, 0x34, 0xF1),
				      TO_ECC_64(0xEF, 0x45, 0x1F, 0xD4, 0x6B, 0x50, 0x3F, 0x00)));
ECC_CONST(NIST_P521_gX, 66, TO_ECC_528(
				       TO_ECC_16(0x00, 0xC6),
				       TO_ECC_64(0x85, 0x8E, 0x06, 0xB7, 0x04, 0x04, 0xE9, 0xCD),
				       TO_ECC_64(0x9E, 0x3E, 0xCB, 0x66, 0x23, 0x95, 0xB4, 0x42),
				       TO_ECC_64(0x9C, 0x64, 0x81, 0x39, 0x05, 0x3F, 0xB5, 0x21),
				       TO_ECC_64(0xF8, 0x28, 0xAF, 0x60, 0x6B, 0x4D, 0x3D, 0xBA),
				       TO_ECC_64(0xA1, 0x4B, 0x5E, 0x77, 0xEF, 0xE7, 0x59, 0x28),
				       TO_ECC_64(0xFE, 0x1D, 0xC1, 0x27, 0xA2, 0xFF, 0xA8, 0xDE),
				       TO_ECC_64(0x33, 0x48, 0xB3, 0xC1, 0x85, 0x6A, 0x42, 0x9B),
				       TO_ECC_64(0xF9, 0x7E, 0x7E, 0x31, 0xC2, 0xE5, 0xBD, 0x66)));
ECC_CONST(NIST_P521_gY, 66, TO_ECC_528(
				       TO_ECC_16(0x01, 0x18),
				       TO_ECC_64(0x39, 0x29, 0x6A, 0x78, 0x9A, 0x3B, 0xC0, 0x04),
				       TO_ECC_64(0x5C, 0x8A, 0x5F, 0xB4, 0x2C, 0x7D, 0x1B, 0xD9),
				       TO_ECC_64(0x98, 0xF5, 0x44, 0x49, 0x57, 0x9B, 0x44, 0x68),
				       TO_ECC_64(0x17, 0xAF, 0xBD, 0x17, 0x27, 0x3E, 0x66, 0x2C),
				       TO_ECC_64(0x97, 0xEE, 0x72, 0x99, 0x5E, 0xF4, 0x26, 0x40),
				       TO_ECC_64(0xC5, 0x50, 0xB9, 0x01, 0x3F, 0xAD, 0x07, 0x61),
				       TO_ECC_64(0x35, 0x3C, 0x70, 0x86, 0xA2, 0x72, 0xC2, 0x40),
				       TO_ECC_64(0x88, 0xBE, 0x94, 0x76, 0x9F, 0xD1, 0x66, 0x50)));
ECC_CONST(NIST_P521_n, 66, TO_ECC_528(
				      TO_ECC_16(0x01, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				      TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA),
				      TO_ECC_64(0x51, 0x86, 0x87, 0x83, 0xBF, 0x2F, 0x96, 0x6B),
				      TO_ECC_64(0x7F, 0xCC, 0x01, 0x48, 0xF7, 0x09, 0xA5, 0xD0),
				      TO_ECC_64(0x3B, 0xB5, 0xC9, 0xB8, 0x89, 0x9C, 0x47, 0xAE),
				      TO_ECC_64(0xBB, 0x6F, 0xB7, 0x1E, 0x91, 0x38, 0x64, 0x09)));
#define NIST_P521_h         ECC_ONE
#define NIST_P521_gZ        ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA NIST_P521 = {
				  (bigNum)&NIST_P521_p, (bigNum)&NIST_P521_n, (bigNum)&NIST_P521_h,
				  (bigNum)&NIST_P521_a, (bigNum)&NIST_P521_b,
				  {(bigNum)&NIST_P521_gX, (bigNum)&NIST_P521_gY, (bigNum)&NIST_P521_gZ}};
#else
const ECC_CURVE_DATA NIST_P521 = {
				  &NIST_P521_p.b, &NIST_P521_n.b, &NIST_P521_h.b,
				  &NIST_P521_a.b, &NIST_P521_b.b,
				  {&NIST_P521_gX.b, &NIST_P521_gY.b, &NIST_P521_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_NIST_P521
#if ECC_BN_P256
ECC_CONST(BN_P256_p, 32, TO_ECC_256(
				    TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xCD),
				    TO_ECC_64(0x46, 0xE5, 0xF2, 0x5E, 0xEE, 0x71, 0xA4, 0x9F),
				    TO_ECC_64(0x0C, 0xDC, 0x65, 0xFB, 0x12, 0x98, 0x0A, 0x82),
				    TO_ECC_64(0xD3, 0x29, 0x2D, 0xDB, 0xAE, 0xD3, 0x30, 0x13)));
#define BN_P256_a           ECC_ZERO
ECC_CONST(BN_P256_b, 1, TO_ECC_8(3));
#define BN_P256_gX          ECC_ONE
ECC_CONST(BN_P256_gY, 1, TO_ECC_8(2));
ECC_CONST(BN_P256_n, 32, TO_ECC_256(
				    TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xCD),
				    TO_ECC_64(0x46, 0xE5, 0xF2, 0x5E, 0xEE, 0x71, 0xA4, 0x9E),
				    TO_ECC_64(0x0C, 0xDC, 0x65, 0xFB, 0x12, 0x99, 0x92, 0x1A),
				    TO_ECC_64(0xF6, 0x2D, 0x53, 0x6C, 0xD1, 0x0B, 0x50, 0x0D)));
#define BN_P256_h           ECC_ONE
#define BN_P256_gZ          ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA BN_P256 = {
				(bigNum)&BN_P256_p, (bigNum)&BN_P256_n, (bigNum)&BN_P256_h,
				(bigNum)&BN_P256_a, (bigNum)&BN_P256_b,
				{(bigNum)&BN_P256_gX, (bigNum)&BN_P256_gY, (bigNum)&BN_P256_gZ}};
#else
const ECC_CURVE_DATA BN_P256 = {
				&BN_P256_p.b, &BN_P256_n.b, &BN_P256_h.b,
				&BN_P256_a.b, &BN_P256_b.b,
				{&BN_P256_gX.b, &BN_P256_gY.b, &BN_P256_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_BN_P256
#if ECC_BN_P638
ECC_CONST(BN_P638_p, 80, TO_ECC_640(
				    TO_ECC_64(0x23, 0xFF, 0xFF, 0xFD, 0xC0, 0x00, 0x00, 0x0D),
				    TO_ECC_64(0x7F, 0xFF, 0xFF, 0xB8, 0x00, 0x00, 0x01, 0xD3),
				    TO_ECC_64(0xFF, 0xFF, 0xF9, 0x42, 0xD0, 0x00, 0x16, 0x5E),
				    TO_ECC_64(0x3F, 0xFF, 0x94, 0x87, 0x00, 0x00, 0xD5, 0x2F),
				    TO_ECC_64(0xFF, 0xFD, 0xD0, 0xE0, 0x00, 0x08, 0xDE, 0x55),
				    TO_ECC_64(0xC0, 0x00, 0x86, 0x52, 0x00, 0x21, 0xE5, 0x5B),
				    TO_ECC_64(0xFF, 0xFF, 0xF5, 0x1F, 0xFF, 0xF4, 0xEB, 0x80),
				    TO_ECC_64(0x00, 0x00, 0x00, 0x4C, 0x80, 0x01, 0x5A, 0xCD),
				    TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEC, 0xE0),
				    TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67)));
#define BN_P638_a           ECC_ZERO
ECC_CONST(BN_P638_b, 2, TO_ECC_16(0x01,0x01));
ECC_CONST(BN_P638_gX, 80, TO_ECC_640(
				     TO_ECC_64(0x23, 0xFF, 0xFF, 0xFD, 0xC0, 0x00, 0x00, 0x0D),
				     TO_ECC_64(0x7F, 0xFF, 0xFF, 0xB8, 0x00, 0x00, 0x01, 0xD3),
				     TO_ECC_64(0xFF, 0xFF, 0xF9, 0x42, 0xD0, 0x00, 0x16, 0x5E),
				     TO_ECC_64(0x3F, 0xFF, 0x94, 0x87, 0x00, 0x00, 0xD5, 0x2F),
				     TO_ECC_64(0xFF, 0xFD, 0xD0, 0xE0, 0x00, 0x08, 0xDE, 0x55),
				     TO_ECC_64(0xC0, 0x00, 0x86, 0x52, 0x00, 0x21, 0xE5, 0x5B),
				     TO_ECC_64(0xFF, 0xFF, 0xF5, 0x1F, 0xFF, 0xF4, 0xEB, 0x80),
				     TO_ECC_64(0x00, 0x00, 0x00, 0x4C, 0x80, 0x01, 0x5A, 0xCD),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEC, 0xE0),
				     TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66)));
ECC_CONST(BN_P638_gY, 1, TO_ECC_8(0x10));
ECC_CONST(BN_P638_n, 80, TO_ECC_640(
				    TO_ECC_64(0x23, 0xFF, 0xFF, 0xFD, 0xC0, 0x00, 0x00, 0x0D),
				    TO_ECC_64(0x7F, 0xFF, 0xFF, 0xB8, 0x00, 0x00, 0x01, 0xD3),
				    TO_ECC_64(0xFF, 0xFF, 0xF9, 0x42, 0xD0, 0x00, 0x16, 0x5E),
				    TO_ECC_64(0x3F, 0xFF, 0x94, 0x87, 0x00, 0x00, 0xD5, 0x2F),
				    TO_ECC_64(0xFF, 0xFD, 0xD0, 0xE0, 0x00, 0x08, 0xDE, 0x55),
				    TO_ECC_64(0x60, 0x00, 0x86, 0x55, 0x00, 0x21, 0xE5, 0x55),
				    TO_ECC_64(0xFF, 0xFF, 0xF5, 0x4F, 0xFF, 0xF4, 0xEA, 0xC0),
				    TO_ECC_64(0x00, 0x00, 0x00, 0x49, 0x80, 0x01, 0x54, 0xD9),
				    TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xED, 0xA0),
				    TO_ECC_64(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61)));
#define BN_P638_h           ECC_ONE
#define BN_P638_gZ          ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA BN_P638 = {
				(bigNum)&BN_P638_p, (bigNum)&BN_P638_n, (bigNum)&BN_P638_h,
				(bigNum)&BN_P638_a, (bigNum)&BN_P638_b,
				{(bigNum)&BN_P638_gX, (bigNum)&BN_P638_gY, (bigNum)&BN_P638_gZ}};
#else
const ECC_CURVE_DATA BN_P638 = {
				&BN_P638_p.b, &BN_P638_n.b, &BN_P638_h.b,
				&BN_P638_a.b, &BN_P638_b.b,
				{&BN_P638_gX.b, &BN_P638_gY.b, &BN_P638_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_BN_P638
#if ECC_SM2_P256
ECC_CONST(SM2_P256_p, 32, TO_ECC_256(
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)));
ECC_CONST(SM2_P256_a, 32, TO_ECC_256(
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC)));
ECC_CONST(SM2_P256_b, 32, TO_ECC_256(
				     TO_ECC_64(0x28, 0xE9, 0xFA, 0x9E, 0x9D, 0x9F, 0x5E, 0x34),
				     TO_ECC_64(0x4D, 0x5A, 0x9E, 0x4B, 0xCF, 0x65, 0x09, 0xA7),
				     TO_ECC_64(0xF3, 0x97, 0x89, 0xF5, 0x15, 0xAB, 0x8F, 0x92),
				     TO_ECC_64(0xDD, 0xBC, 0xBD, 0x41, 0x4D, 0x94, 0x0E, 0x93)));
ECC_CONST(SM2_P256_gX, 32, TO_ECC_256(
				      TO_ECC_64(0x32, 0xC4, 0xAE, 0x2C, 0x1F, 0x19, 0x81, 0x19),
				      TO_ECC_64(0x5F, 0x99, 0x04, 0x46, 0x6A, 0x39, 0xC9, 0x94),
				      TO_ECC_64(0x8F, 0xE3, 0x0B, 0xBF, 0xF2, 0x66, 0x0B, 0xE1),
				      TO_ECC_64(0x71, 0x5A, 0x45, 0x89, 0x33, 0x4C, 0x74, 0xC7)));
ECC_CONST(SM2_P256_gY, 32, TO_ECC_256(
				      TO_ECC_64(0xBC, 0x37, 0x36, 0xA2, 0xF4, 0xF6, 0x77, 0x9C),
				      TO_ECC_64(0x59, 0xBD, 0xCE, 0xE3, 0x6B, 0x69, 0x21, 0x53),
				      TO_ECC_64(0xD0, 0xA9, 0x87, 0x7C, 0xC6, 0x2A, 0x47, 0x40),
				      TO_ECC_64(0x02, 0xDF, 0x32, 0xE5, 0x21, 0x39, 0xF0, 0xA0)));
ECC_CONST(SM2_P256_n, 32, TO_ECC_256(
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF),
				     TO_ECC_64(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF),
				     TO_ECC_64(0x72, 0x03, 0xDF, 0x6B, 0x21, 0xC6, 0x05, 0x2B),
				     TO_ECC_64(0x53, 0xBB, 0xF4, 0x09, 0x39, 0xD5, 0x41, 0x23)));
#define SM2_P256_h          ECC_ONE
#define SM2_P256_gZ         ECC_ONE
#if USE_BN_ECC_DATA
const ECC_CURVE_DATA SM2_P256 = {
				 (bigNum)&SM2_P256_p, (bigNum)&SM2_P256_n, (bigNum)&SM2_P256_h,
				 (bigNum)&SM2_P256_a, (bigNum)&SM2_P256_b,
				 {(bigNum)&SM2_P256_gX, (bigNum)&SM2_P256_gY, (bigNum)&SM2_P256_gZ}};
#else
const ECC_CURVE_DATA SM2_P256 = {
				 &SM2_P256_p.b, &SM2_P256_n.b, &SM2_P256_h.b,
				 &SM2_P256_a.b, &SM2_P256_b.b,
				 {&SM2_P256_gX.b, &SM2_P256_gY.b, &SM2_P256_gZ.b}};
#endif // USE_BN_ECC_DATA
#endif // ECC_SM2_P256
#define comma
const ECC_CURVE   eccCurves[] = {
#if ECC_NIST_P192
				 comma
				 {TPM_ECC_NIST_P192,
				  192,
				  {TPM_ALG_KDF1_SP800_56A, {{TPM_ALG_SHA256}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &NIST_P192,
				  OID_ECC_NIST_P192
				  CURVE_NAME("NIST_P192")}
#   undef comma
#   define comma ,
#endif // ECC_NIST_P192
#if ECC_NIST_P224
				 comma
				 {TPM_ECC_NIST_P224,
				  224,
				  {TPM_ALG_KDF1_SP800_56A_, {{TPM_ALG_SHA256}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &NIST_P224,
				  OID_ECC_NIST_P224
				  CURVE_NAME("NIST_P224")}
#   undef comma
#   define comma ,
#endif // ECC_NIST_P224
#if ECC_NIST_P256
				 comma
				 {TPM_ECC_NIST_P256,
				  256,
				  {TPM_ALG_KDF1_SP800_56A, {{TPM_ALG_SHA256}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &NIST_P256,
				  OID_ECC_NIST_P256
				  CURVE_NAME("NIST_P256")}
#   undef comma
#   define comma ,
#endif // ECC_NIST_P256
#if ECC_NIST_P384
				 comma
				 {TPM_ECC_NIST_P384,
				  384,
				  {TPM_ALG_KDF1_SP800_56A, {{TPM_ALG_SHA384}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &NIST_P384,
				  OID_ECC_NIST_P384
				  CURVE_NAME("NIST_P384")}
#   undef comma
#   define comma ,
#endif // ECC_NIST_P384
#if ECC_NIST_P521
				 comma
				 {TPM_ECC_NIST_P521,
				  521,
				  {TPM_ALG_KDF1_SP800_56A, {{TPM_ALG_SHA512}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &NIST_P521,
				  OID_ECC_NIST_P521
				  CURVE_NAME("NIST_P521")}
#   undef comma
#   define comma ,
#endif // ECC_NIST_P521
#if ECC_BN_P256
				 comma
				 {TPM_ECC_BN_P256,
				  256,
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &BN_P256,
				  OID_ECC_BN_P256
				  CURVE_NAME("BN_P256")}
#   undef comma
#   define comma ,
#endif // ECC_BN_P256
#if ECC_BN_P638
				 comma
				 {TPM_ECC_BN_P638,
				  638,
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &BN_P638,
				  OID_ECC_BN_P638
				  CURVE_NAME("BN_P638")}
#   undef comma
#   define comma ,
#endif // ECC_BN_P638
#if ECC_SM2_P256
				 comma
				 {TPM_ECC_SM2_P256,
				  256,
				  {TPM_ALG_KDF1_SP800_56A, {{TPM_ALG_SM3_256}}},
				  {TPM_ALG_NULL, {{TPM_ALG_NULL}}},
				  &SM2_P256,
				  OID_ECC_SM2_P256
				  CURVE_NAME("SM2_P256")}
#   undef comma
#   define comma ,
#endif // ECC_SM2_P256
};
#endif // TPM_ALG_ECC
