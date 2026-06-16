cmake_minimum_required(VERSION 3.18)
project(QSBarcodePublicConsumerDemo LANGUAGES C CXX)

include(FetchContent)
include(CTest)

set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_fc" CACHE PATH "Short FetchContent base directory")

set(QSBC_PUBLIC_VERSION "6.0.0" CACHE STRING "QSBarcode public SDK version")
set(QSBC_PUBLIC_REPOSITORY "https://github.com/QS-QualitySoft-GmbH/QSBarcode.git" CACHE STRING "Public QSBarcode repository")
set(QSBC_PUBLIC_RELEASE_SHA256 "91897253d60b38776484962b77523a76b064c3a61cc18a842567a0dd52bcb74b" CACHE STRING "Expected SHA256 of the public native SDK ZIP")
option(QSBC_PUBLIC_TLS_VERIFY "Verify TLS certificates when downloading the public release ZIP" ON)
set(QSBC_DEMO_INPUT "" CACHE FILEPATH "Optional image or PDF fixture to scan")
set(QSBC_DEMO_LICENSE "" CACHE FILEPATH "Optional qsbc.lic file")

function(qsbc_detect_runtime_id out_var)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(os_part "win")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(os_part "osx")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(os_part "linux")
  else()
    message(FATAL_ERROR "Unsupported QSBarcode demo OS: ${CMAKE_SYSTEM_NAME}")
  endif()

  string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" processor)
  if(os_part STREQUAL "win" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(arch_part "x86")
  elseif(processor MATCHES "^(amd64|x86_64)$")
    set(arch_part "x64")
  elseif(processor MATCHES "^(arm64|aarch64)$")
    set(arch_part "arm64")
  elseif(processor MATCHES "^(x86|i386|i686)$")
    set(arch_part "x86")
  else()
    message(FATAL_ERROR "Unsupported QSBarcode demo architecture: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()

  set("${out_var}" "${os_part}-${arch_part}" PARENT_SCOPE)
endfunction()

qsbc_detect_runtime_id(QSBC_PUBLIC_RUNTIME_ID)

set(QSBC_PUBLIC_RELEASE_URL "https://github.com/QS-QualitySoft-GmbH/QSBarcode/releases/download/v${QSBC_PUBLIC_VERSION}/qsbarcode-native-${QSBC_PUBLIC_VERSION}.zip")
set(QSBC_PUBLIC_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/qsbarcode-public-download")
set(QSBC_PUBLIC_ZIP "${QSBC_PUBLIC_DOWNLOAD_DIR}/qsbarcode-native-${QSBC_PUBLIC_VERSION}.zip")
set(QSBC_PUBLIC_EXTRACT_DIR "${QSBC_PUBLIC_DOWNLOAD_DIR}/extracted")

file(MAKE_DIRECTORY "${QSBC_PUBLIC_DOWNLOAD_DIR}")
if(EXISTS "${QSBC_PUBLIC_ZIP}")
  file(SHA256 "${QSBC_PUBLIC_ZIP}" existing_zip_sha256)
  if(NOT existing_zip_sha256 STREQUAL QSBC_PUBLIC_RELEASE_SHA256)
    file(REMOVE "${QSBC_PUBLIC_ZIP}")
  endif()
endif()

if(NOT EXISTS "${QSBC_PUBLIC_ZIP}")
  message(STATUS "Downloading ${QSBC_PUBLIC_RELEASE_URL}")
  file(DOWNLOAD
    "${QSBC_PUBLIC_RELEASE_URL}"
    "${QSBC_PUBLIC_ZIP}"
    EXPECTED_HASH "SHA256=${QSBC_PUBLIC_RELEASE_SHA256}"
    TLS_VERIFY ${QSBC_PUBLIC_TLS_VERIFY}
    SHOW_PROGRESS
    STATUS download_status)
  list(GET download_status 0 download_status_code)
  list(GET download_status 1 download_status_message)
  if(NOT download_status_code EQUAL 0)
    file(REMOVE "${QSBC_PUBLIC_ZIP}")
    message(FATAL_ERROR "Failed to download QSBarcode SDK: ${download_status_message}")
  endif()
endif()

file(REMOVE_RECURSE "${QSBC_PUBLIC_EXTRACT_DIR}")
file(MAKE_DIRECTORY "${QSBC_PUBLIC_EXTRACT_DIR}")
execute_process(
  COMMAND "${CMAKE_COMMAND}" -E tar xzf "${QSBC_PUBLIC_ZIP}"
  WORKING_DIRECTORY "${QSBC_PUBLIC_EXTRACT_DIR}"
  RESULT_VARIABLE extract_result)
if(NOT extract_result EQUAL 0)
  message(FATAL_ERROR "Failed to extract ${QSBC_PUBLIC_ZIP}")
endif()

set(QSBC_PUBLIC_RUNTIME_DIR "${QSBC_PUBLIC_EXTRACT_DIR}/native/${QSBC_PUBLIC_RUNTIME_ID}")
if(NOT EXISTS "${QSBC_PUBLIC_RUNTIME_DIR}")
  message(FATAL_ERROR "Public SDK ZIP does not contain runtime folder: ${QSBC_PUBLIC_RUNTIME_DIR}")
endif()

set(QSBC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(QSBC_RUNTIME_DIR "${QSBC_PUBLIC_RUNTIME_DIR}" CACHE PATH "" FORCE)
FetchContent_Declare(qsbc
  GIT_REPOSITORY "${QSBC_PUBLIC_REPOSITORY}"
  GIT_TAG "v${QSBC_PUBLIC_VERSION}")
FetchContent_MakeAvailable(qsbc)

if(WIN32)
  find_file(QSBC_PUBLIC_LOADER_LIBRARY
    NAMES qs_barcode_loader_sdk.dll
    HINTS "${QSBC_PUBLIC_RUNTIME_DIR}"
    NO_DEFAULT_PATH
    REQUIRED)
else()
  find_library(QSBC_PUBLIC_LOADER_LIBRARY
    NAMES qs_barcode_loader_sdk libqs_barcode_loader_sdk
    HINTS "${QSBC_PUBLIC_RUNTIME_DIR}"
    NO_DEFAULT_PATH
    REQUIRED)
endif()

function(qsbc_add_public_demo target source)
  add_executable("${target}" "${source}")
  target_link_libraries("${target}" PRIVATE QualitySoftBarcode::headers "${QSBC_PUBLIC_LOADER_LIBRARY}")
  qsbarcode_configure_windows_stack("${target}")
  add_custom_command(TARGET "${target}" POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${QSBC_PUBLIC_RUNTIME_DIR}"
      "$<TARGET_FILE_DIR:${target}>")
endfunction()

qsbc_add_public_demo(qsbarcode_public_c_demo c/verify.c)
qsbc_add_public_demo(qsbarcode_public_cpp_demo cpp/verify.cpp)
target_compile_features(qsbarcode_public_cpp_demo PRIVATE cxx_std_17)

add_test(NAME qsbarcode_public_c_runtime COMMAND qsbarcode_public_c_demo --self-test)
add_test(NAME qsbarcode_public_cpp_runtime COMMAND qsbarcode_public_cpp_demo --self-test)

if(QSBC_DEMO_INPUT)
  set(scan_args "--input" "${QSBC_DEMO_INPUT}" "--require-hit")
  if(QSBC_DEMO_LICENSE)
    list(APPEND scan_args "--license" "${QSBC_DEMO_LICENSE}")
  endif()
  add_test(NAME qsbarcode_public_c_scan COMMAND qsbarcode_public_c_demo ${scan_args})
  add_test(NAME qsbarcode_public_cpp_scan COMMAND qsbarcode_public_cpp_demo ${scan_args})
endif()

message(STATUS "QSBarcode public runtime: ${QSBC_PUBLIC_RUNTIME_ID}")
message(STATUS "QSBarcode public runtime dir: ${QSBC_PUBLIC_RUNTIME_DIR}")
