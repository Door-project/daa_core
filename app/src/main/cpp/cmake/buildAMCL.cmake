include(ExternalProject)

externalproject_add(AMCL
        URL https://github.com/apache/incubator-milagro-crypto-c/archive/refs/tags/2.0.1.tar.gz
        URL_MD5 a4b36a6a967bd6e5513527eacbf20ca7
        BUILD_IN_SOURCE 0
        PREFIX ${CMAKE_BINARY_DIR}/deps/AMCL
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
        cd <SOURCE_DIR> &&
        mkdir -p build &&
        cd build && rm -f CMakeCache.txt && # clean if already existing
        cmake -DBUILD_BENCHMARKS=OFF -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF -DAMCL_CURVE=FP256BN .. &&
        make
        INSTALL_COMMAND ""
        )

set(AMCL_INCLUDE_PATH ${CMAKE_BINARY_DIR}/deps/AMCL/src/AMCL/build/include)
set(AMCL_LIB_PATH ${CMAKE_BINARY_DIR}/deps/AMCL/src/AMCL/build/lib)
