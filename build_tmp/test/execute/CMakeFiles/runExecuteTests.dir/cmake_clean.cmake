file(REMOVE_RECURSE
  "runExecuteTests.pdb"
  "runExecuteTests"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/runExecuteTests.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
