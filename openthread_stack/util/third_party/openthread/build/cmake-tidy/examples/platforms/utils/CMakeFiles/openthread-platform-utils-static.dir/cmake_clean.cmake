file(REMOVE_RECURSE
  "libopenthread-platform-utils-static.a"
  "libopenthread-platform-utils-static.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C CXX)
  include(CMakeFiles/openthread-platform-utils-static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
