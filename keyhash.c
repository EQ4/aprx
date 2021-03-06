/********************************************************************
 *  APRX -- 2nd generation APRS iGate and digi with                 *
 *          minimal requirement of esoteric facilities or           *
 *          libraries of any kind beyond UNIX system libc.          *
 *                                                                  *
 * (c) Matti Aarnio - OH2MQK,  2007-2014                            *
 *                                                                  *
 ********************************************************************/

/*
 * Keyhash routines for the system.
 *
 * What is needed is _fast_ hash function.  Preferrably arithmethic one,
 * which does not need table lookups, and can work with aligned 32 bit
 * data -- but also on unaligned, and on any byte counts...
 *
 * Contenders:
 *   http://burtleburtle.net/bob/c/lookup3.c
 *   http://www.ibiblio.org/pub/Linux/devel/lang/c/mph-1.2.tar.gz
 *   http://www.concentric.net/~Ttwang/tech/inthash.htm
 *   http://isthe.com/chongo/tech/comp/fnv/
 *
 * Currently using FNV-1a
 *
 */

/*
//  FNV-1a  hash from   http://isthe.com/chongo/tech/comp/fnv/
//
//  It is algorithmic hash without memory lookups.
//  Compiler seems to prefer actual multiplication over a bunch of
//  fixed shifts and additions.
*/

#include <stdint.h>
#include <sys/types.h>

#include "keyhash.h"

void keyhash_init(void) { }

uint32_t __attribute__((pure)) keyhash(const void const *p, int len, uint32_t hash)
{
	const uint8_t *u = p;
	int i;
#define FNV_32_PRIME     16777619U
#define FNV_32_OFFSET  2166136261U

	if (hash == 0)
        	hash = (uint32_t)FNV_32_OFFSET;

	for (i = 0; i < len; ++i, ++u) {
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hash *= FNV_32_PRIME;
#else
		hash += (hash<<1) + (hash<<4) + (hash<<7) +
		        (hash<<8) + (hash<<24);
#endif
		hash ^= (uint32_t) *u;
	}
	return hash;
}

/* The data material is known to contain ASCII, and if any value in there
 * is a lower case letter, it is first converted to upper case one.
*/
uint32_t __attribute__((pure)) keyhashuc(const void const *p, int len, uint32_t hash)
{
	const uint8_t *u = p;
	int i;

	if (hash == 0)
        	hash = (uint32_t)FNV_32_OFFSET;

	for (i = 0; i < len; ++i, ++u) {
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hash *= FNV_32_PRIME;
#else
		hash += (hash<<1) + (hash<<4) + (hash<<7) +
		        (hash<<8) + (hash<<24);
#endif
		uint32_t c = *u;
		// Is it lower case ASCII letter ?
		if ('a' <= c && c <= 'z') {
			// convert to upper case.
			c -= ('a' - 'A');
		}
		hash ^= c;
	}
	return hash;
}
