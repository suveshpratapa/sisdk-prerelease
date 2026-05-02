# Install script for directory: /Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/aes.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/aria.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/asn1.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/asn1write.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/base64.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/bignum.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/block_cipher.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/build_info.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/camellia.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ccm.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/chacha20.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/chachapoly.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/check_config.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/cipher.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/cmac.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/compat-2.x.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/config_adjust_legacy_crypto.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/config_adjust_legacy_from_psa.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/config_adjust_psa_from_legacy.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/config_adjust_psa_superset_legacy.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/config_adjust_ssl.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/config_adjust_x509.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/config_psa.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/constant_time.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ctr_drbg.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/debug.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/des.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/dhm.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ecdh.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ecdsa.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ecjpake.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ecp.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/entropy.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/error.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/gcm.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/hkdf.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/hmac_drbg.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/lms.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/mbedtls_config.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/md.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/md5.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/memory_buffer_alloc.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/net_sockets.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/nist_kw.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/oid.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/pem.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/pk.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/pkcs12.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/pkcs5.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/pkcs7.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/platform.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/platform_time.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/platform_util.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/poly1305.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/private_access.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/psa_util.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ripemd160.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/rsa.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/sha1.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/sha256.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/sha3.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/sha512.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ssl.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ssl_cache.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ssl_ciphersuites.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ssl_cookie.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/ssl_ticket.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/threading.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/timing.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/version.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/x509.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/x509_crl.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/x509_crt.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/build_info.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_adjust_auto_enabled.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_adjust_config_key_pair_types.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_adjust_config_synonyms.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_builtin_composites.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_builtin_key_derivation.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_builtin_primitives.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_compat.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_config.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_driver_common.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_driver_contexts_composites.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_driver_contexts_key_derivation.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_driver_contexts_primitives.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_extra.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_legacy.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_platform.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_se_driver.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_sizes.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_struct.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_types.h"
    "/Users/supratap/silabs-stash/silabs-thread/util/third_party/openthread/third_party/mbedtls/repo/include/psa/crypto_values.h"
    )
endif()

