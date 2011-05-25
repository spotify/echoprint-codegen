/* easyzlib.h header

  easyzlib release 1.0
  Copyright (C) 2008 First Objective Software, Inc. All rights reserved
  This entire notice must be retained in this source code
  Redistributing this source code requires written permission
  This software is provided "as is", with no warranty.
  Latest fixes enhancements and documentation at www.firstobject.com
*/

#ifndef _EASYZLIB_H
#define _EASYZLIB_H

/* Return codes */
#define EZ_STREAM_ERROR  (-2)
#define EZ_DATA_ERROR    (-3)
#define EZ_MEM_ERROR     (-4)
#define EZ_BUF_ERROR     (-5)

/* Calculate maximum compressed length from uncompressed length */
#define EZ_COMPRESSMAXDESTLENGTH(n) ((n)+(((n)/1000)+1)+12)

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

int ezcompress( unsigned char* pDest, long* pnDestLen, const unsigned char* pSrc, long nSrcLen );
int ezuncompress( unsigned char* pDest, long* pnDestLen, const unsigned char* pSrc, long nSrcLen );

#ifdef __cplusplus
}

struct ezbuffer
{
	ezbuffer() { Init(); };
	ezbuffer( int n ) { Init(); Alloc(n); };
	~ezbuffer() { Release(); };
	unsigned char* Alloc( int n ) { if (nSize<n) {Release(); pBuf=new unsigned char[n]; nSize = n;} nLen=n; return pBuf; };
	void Release() { if (pBuf) { delete[] pBuf; Init(); } };
	unsigned char* pBuf;
	long nLen; /* data length */
private:
	void Init() { pBuf = NULL; nSize=0; nLen=0; };
	long nSize; /* bytes allocated */
};

#define EZ_CHECKLENGTH 8192

int ezcompress( ezbuffer& bufDest, const ezbuffer& bufSrc )
{
	if ( bufDest.nLen == 0 )
		bufDest.Alloc( EZ_CHECKLENGTH );
	int nErr = ezcompress( bufDest.pBuf, &bufDest.nLen, bufSrc.pBuf, bufSrc.nLen );
	if ( nErr == EZ_BUF_ERROR )
	{
		bufDest.Alloc( bufDest.nLen );
		nErr = ezcompress( bufDest.pBuf, &bufDest.nLen, bufSrc.pBuf, bufSrc.nLen );
	}
	return nErr;
};

int ezuncompress( ezbuffer& bufDest, const ezbuffer& bufSrc )
{
	if ( bufDest.nLen == 0 )
		bufDest.Alloc( EZ_CHECKLENGTH );
	int nErr = ezuncompress( bufDest.pBuf, &bufDest.nLen, bufSrc.pBuf, bufSrc.nLen );
	if ( nErr == EZ_BUF_ERROR )
	{
		bufDest.Alloc( bufDest.nLen );
		nErr = ezuncompress( bufDest.pBuf, &bufDest.nLen, bufSrc.pBuf, bufSrc.nLen );
	}
	return nErr;
};

#ifdef MCD_STR
/* CMarkup designated string class and macros */

int ezcompress( ezbuffer& bufDest, const MCD_STR& strSrc )
{
	int nSrcLen = MCD_STRLENGTH(strSrc) * sizeof(MCD_CHAR);
	/* alternatively: bufDest.Alloc( EZ_COMPRESSMAXDESTLENGTH(nSrcLen) ); // >.1% + 12 */
	unsigned char pTempDest[EZ_CHECKLENGTH];
	long nTempLen = EZ_CHECKLENGTH;
	int nErr = ezcompress( pTempDest, &nTempLen, (const unsigned char*)MCD_2PCSZ(strSrc), nSrcLen );
	bufDest.Alloc( nTempLen );
	nErr = ezcompress( bufDest.pBuf, &bufDest.nLen, (const unsigned char*)MCD_2PCSZ(strSrc), nSrcLen );
	return nErr;
}

int ezuncompress( MCD_STR& strDest, const ezbuffer& bufSrc )
{
	unsigned char pTempDest[EZ_CHECKLENGTH];
	long nTempLen = EZ_CHECKLENGTH;
	int nErr = ezuncompress( pTempDest, &nTempLen, bufSrc.pBuf, bufSrc.nLen );
	int nDestStrLen = nTempLen / sizeof(MCD_CHAR);
	MCD_CHAR* p = MCD_GETBUFFER(strDest,nDestStrLen);
	nErr = ezuncompress( (unsigned char*)p, &nTempLen, bufSrc.pBuf, bufSrc.nLen );
	MCD_RELEASEBUFFER(strDest,p,nDestStrLen);
	return nErr;
}

#endif /* MCD_STR */

#endif /* __cplusplus */

#endif /* _EASYZLIB_H */
