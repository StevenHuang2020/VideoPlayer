//
// version.h
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Defines the version of this software.
//


#ifndef _H_VERSION_H
#define _H_VERSION_H

#define VERSION_MAJOR  1
#define VERSION_MINOR  1
#define VERSION_MICRO  4


#define VERSION_DOT(a, b, c)   a ##.## b ##.## c

#define PLAYER_VERSION_NUMBER   VERSION_DOT(VERSION_MAJOR, \
                                    VERSION_MINOR, \
                                    VERSION_MICRO)


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PLAYER_VERSION STR(PLAYER_VERSION_NUMBER)

#endif /* _H_VERSION_H */
