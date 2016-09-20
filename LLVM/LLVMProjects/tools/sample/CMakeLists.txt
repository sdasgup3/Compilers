#set(LLVM_LINK_COMPONENTS
#  Core
#  ${LLVM_TARGETS_TO_BUILD}
#)

add_llvm_tool(sample_tool
  main.c
)
target_link_libraries(sample_tool sample)

#add_definitions(${LLVM_DEFINITIONS})
