set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  IRReader
  BitReader
  BitWriter
  Core
  Support
)

add_llvm_tool(analyze_geps
  analyze_geps.cpp
)
#target_link_libraries(analyze_geps stack_deconstructor)

#add_definitions(${LLVM_DEFINITIONS})
