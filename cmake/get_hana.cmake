include(ExternalProject)
## Boost hana (not yet available in 1.58) (header only library)
ExternalProject_Add(hana
    PREFIX 3rd_party
    GIT_REPOSITORY https://github.com/boostorg/hana.git
    GIT_TAG v1.2.0
    GIT_SHALLOW 1
    TLS_VERIFY true
    TLS_CAINFO certs/DigiCertHighAssuranceEVRootCA.crt
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    UPDATE_COMMAND "" # Skip annoying updates for every build
    INSTALL_COMMAND ""
)
SET(HANA_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rd_party/src/hana/include)
INCLUDE_DIRECTORIES(${HANA_INCLUDE_DIR})
