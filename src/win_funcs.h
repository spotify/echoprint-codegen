#ifndef __WIN_FUNCS_H
#define __WIN_FUNCS_H 1

#define _USE_MATH_DEFINES
#include <math.h>
#include <winsock.h>
#include <sys/timeb.h>

#define ROUND_FUNC(type,suff) inline type round##suff(type x) \
{                                                             \
    if (x >= 0.0##suff){                                      \
        type y = floor##suff(x);                              \
        if (x - y >= 0.5##suff)                               \
            y += 1.0##suff;                                   \
        return y;                                             \
    }else{                                                    \
        type y = ceil##suff(x);                               \
        if (y - x >= 0.5##suff)                               \
            y -= 1.0##suff;                                   \
      return y;                                               \
    }                                                         \
}

ROUND_FUNC(float,f)
ROUND_FUNC(double,)


inline void gettimeofday(struct timeval* t,void* timezone)
{       struct _timeb timebuffer;
        _ftime( &timebuffer );
        t->tv_sec=timebuffer.time;
        t->tv_usec=1000*timebuffer.millitm;
}

inline double rint( double x) 
// Copyright (C) 2001 Tor M. Aamodt, University of Toronto 
// Permisssion to use for all purposes commercial and otherwise granted. 
// THIS MATERIAL IS PROVIDED "AS IS" WITHOUT WARRANTY, OR ANY CONDITION OR 
// OTHER TERM OF ANY KIND INCLUDING, WITHOUT LIMITATION, ANY WARRANTY 
// OF MERCHANTABILITY, SATISFACTORY QUALITY, OR FITNESS FOR A PARTICULAR 
// PURPOSE. 
{ 
    if( x > 0 ) { 
        __int64 xint = (__int64) (x+0.5); 
        if( xint % 2 ) { 
            // then we might have an even number... 
            double diff = x - (double)xint; 
            if( diff == -0.5 ) 
                return double(xint-1); 
        } 
        return double(xint); 
    } else { 
        __int64 xint = (__int64) (x-0.5); 
        if( xint % 2 ) { 
            // then we might have an even number... 
            double diff = x - (double)xint; 
            if( diff == 0.5 ) 
                return double(xint+1); 
        } 
        return double(xint); 
    } 
}

#endif /* __WIN_FUNCS_H */
