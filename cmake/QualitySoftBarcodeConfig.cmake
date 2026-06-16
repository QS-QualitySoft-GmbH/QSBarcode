include(CMakeFindDependencyMacro)

add_library(QualitySoftBarcode::headers INTERFACE IMPORTED)
target_include_directories(QualitySoftBarcode::headers INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../../../include")

function(qsbarcode_configure_windows_stack target)
  if(WIN32)
    if(MSVC)
      set_property(TARGET "${target}" APPEND_STRING PROPERTY LINK_FLAGS " /STACK:16777216")
    else()
      set_property(TARGET "${target}" APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--stack,16777216")
    endif()
  endif()
endfunction()
