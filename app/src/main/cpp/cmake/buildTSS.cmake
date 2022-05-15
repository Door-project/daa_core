include(ExternalProject)

message(WARNING ... SETTING LIB DIR ${CMAKE_BINARY_DIR})


externalproject_add(IBMTSS
        URL https://github.com/kgoldman/ibmtss/archive/refs/tags/v1.6.0.tar.gz
        URL_MD5 58e4e093a62d1e417469eb0987f0531b
        BUILD_IN_SOURCE 0
        PREFIX ${CMAKE_BINARY_DIR}/deps/ibmtss
        SOURCE_DIR ${CMAKE_BINARY_DIR}/deps/ibmtss/src/ibmtss
        CONFIGURE_COMMAND
        cd <SOURCE_DIR> &&
        autoreconf -i &&
        ./configure --disable-hwtpm
        BUILD_COMMAND
        cd <SOURCE_DIR> &&
        make
        INSTALL_COMMAND ""
        )



set(IBMTSS_INCLUDE_PATH ${CMAKE_BINARY_DIR}/deps/ibmtss/src/ibmtss/utils)
set(IBMTSS_LIB_PATH ${CMAKE_BINARY_DIR}/deps/ibmtss/src/ibmtss/utils/.libs)
message(WARNING ... SETTING LIB DIR ${CMAKE_BINARY_DIR})
