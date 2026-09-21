if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(etudes PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=thread -fno-omit-frame-pointer -g>
  )
  target_link_options(etudes PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=thread>
  )
endif()