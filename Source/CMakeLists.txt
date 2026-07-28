add_library(AimSync SHARED dllmain.cpp)

set_target_properties(AimSync PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  POSITION_INDEPENDENT_CODE ON)

target_include_directories(AimSync PRIVATE "${CMAKE_SOURCE_DIR}/Source")
target_link_libraries(AimSync PRIVATE ${CMAKE_DL_LIBS})

if(WIN32)
  target_sources(AimSync PRIVATE
    Platform/Windows/Syscalls/WindowsSyscall.asm
    UI/Dx11/Dx11MenuStubs.cpp)
endif()

if(MSVC)
  set_target_properties(AimSync PROPERTIES MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>")
  string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  target_sources(AimSync PRIVATE $<$<CONFIG:Release>:Platform/Windows/CRTWindows.cpp>)
  # /constexpr:* is MSVC cl only — clang-cl rejects them as unused (-Werror)
  target_compile_options(AimSync PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/W4 $<$<CXX_COMPILER_ID:MSVC>:/constexpr:steps2147483647;/constexpr:depth2048> $<$<CONFIG:Release>:/sdl- /GS->>)
  target_compile_definitions(AimSync PRIVATE $<$<CONFIG:Release>:_USE_STD_VECTOR_ALGORITHMS=0>)
  target_link_options(AimSync PRIVATE $<$<CONFIG:Release>:/nodefaultlib /ENTRY:"DllMain">)
  # Match MSBuild Release deps so freestanding CRT resolves wcslen/etc from ucrt
  target_link_libraries(AimSync PRIVATE $<$<CONFIG:Release>:libcmt libvcruntime libucrt legacy_stdio_definitions>)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(AimSync PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall>)
    target_link_options(AimSync PRIVATE LINKER:--no-undefined -z noexecstack)
  endif()

  target_compile_options(AimSync PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-missing-braces>)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14.0)
  target_compile_options(AimSync PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-dangling-reference>) # false positive
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  if(UNIX)
    target_compile_options(AimSync PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables>)
    target_link_options(AimSync PRIVATE -nostdlib)
    target_link_libraries(AimSync PRIVATE c)

    if(CMAKE_STRIP)
      add_custom_command(TARGET AimSync POST_BUILD COMMAND ${CMAKE_STRIP} "$<TARGET_FILE:AimSync>")
    endif()
  endif()
endif()
