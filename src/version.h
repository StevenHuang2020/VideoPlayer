// ***********************************************************/
// version.h
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Version definition of this software.
// ***********************************************************/
// 

#ifndef __VERSION_H__
#define __VERSION_H__

#define VERSION_MAJOR  2
#define VERSION_MINOR  0
#define VERSION_MICRO  8


#define VERSION_DOT(a, b, c)   a ##.## b ##.## c

#define PLAYER_VERSION_NUMBER   VERSION_DOT(VERSION_MAJOR, \
                                    VERSION_MINOR, \
                                    VERSION_MICRO)


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PLAYER_VERSION STR(PLAYER_VERSION_NUMBER)

#endif /* end of __VERSION_H__ */
