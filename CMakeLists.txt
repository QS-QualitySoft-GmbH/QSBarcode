cmake_minimum_required(VERSION 3.18)
project(QualitySoftBarcodeNative
  VERSION 6.0.0
  HOMEPAGE_URL "https://github.com/QS-QualitySoft-GmbH/QSBarcode"
  LANGUAGES C CXX)

option(QSBC_BUILD_TESTS "Build QS Barcode native tests" ON)
set(QSBC_RUNTIME_DIR "" CACHE PATH "Directory containing QS Barcode native runtime assets")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(QSBC_OS_PART "win")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(QSBC_OS_PART "osx")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(QSBC_OS_PART "linux")
else()
  set(QSBC_OS_PART "")
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" QSBC_PROCESSOR)
if(QSBC_PROCESSOR MATCHES "^(amd64|x86_64)$")
  set(QSBC_ARCH_PART "x64")
elseif(QSBC_PROCESSOR MATCHES "^(x86|i386|i686)$")
  set(QSBC_ARCH_PART "x86")
elseif(QSBC_PROCESSOR MATCHES "^(arm64|aarch64)$")
  set(QSBC_ARCH_PART "arm64")
else()
  set(QSBC_ARCH_PART "")
endif()

if(QSBC_OS_PART AND QSBC_ARCH_PART)
  set(QSBC_RUNTIME_ID "${QSBC_OS_PART}-${QSBC_ARCH_PART}")
else()
  set(QSBC_RUNTIME_ID "")
endif()

add_library(QualitySoftBarcode::headers INTERFACE IMPORTED GLOBAL)
target_include_directories(QualitySoftBarcode::headers INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")

find_file(QSBC_LOADER_LIBRARY
  NAMES qs_barcode_loader_sdk.dll libqs_barcode_loader_sdk.so libqs_barcode_loader_sdk.dylib
  HINTS
    "${QSBC_RUNTIME_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/native/${QSBC_RUNTIME_ID}"
  DOC "Path to the QS Barcode native runtime library")

if(QSBC_LOADER_LIBRARY)
  get_filename_component(QSBC_LOADER_DIRECTORY "${QSBC_LOADER_LIBRARY}" DIRECTORY)
  add_library(QualitySoftBarcode::loader SHARED IMPORTED GLOBAL)
  set_target_properties(QualitySoftBarcode::loader PROPERTIES IMPORTED_LOCATION "${QSBC_LOADER_LIBRARY}")
endif()

function(qsbarcode_configure_windows_stack target)
  if(WIN32)
    if(MSVC)
      set_property(TARGET "${target}" APPEND_STRING PROPERTY LINK_FLAGS " /STACK:16777216")
    else()
      set_property(TARGET "${target}" APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--stack,16777216")
    endif()
  endif()
endfunction()

if(QSBC_BUILD_TESTS)
  enable_testing()
  add_executable(qsbc_header_compile tests/header_compile.cpp)
  target_compile_features(qsbc_header_compile PRIVATE cxx_std_17)
  target_link_libraries(qsbc_header_compile PRIVATE QualitySoftBarcode::headers)
  qsbarcode_configure_windows_stack(qsbc_header_compile)
  add_test(NAME qsbc_header_compile COMMAND qsbc_header_compile)

  if(QSBC_LOADER_LIBRARY)
    add_executable(qsbc_native_smoke tests/smoke.cpp)
    target_compile_features(qsbc_native_smoke PRIVATE cxx_std_17)
    target_link_libraries(qsbc_native_smoke PRIVATE QualitySoftBarcode::headers)
    target_link_libraries(qsbc_native_smoke PRIVATE "${QSBC_LOADER_LIBRARY}")
    qsbarcode_configure_windows_stack(qsbc_native_smoke)
    add_custom_command(TARGET qsbc_native_smoke POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${QSBC_LOADER_DIRECTORY}"
        "$<TARGET_FILE_DIR:qsbc_native_smoke>")
    add_test(NAME qsbc_native_smoke COMMAND qsbc_native_smoke)
  endif()
endif()

install(DIRECTORY include/ DESTINATION include)
install(FILES cmake/QualitySoftBarcodeConfig.cmake DESTINATION lib/cmake/QualitySoftBarcode)
