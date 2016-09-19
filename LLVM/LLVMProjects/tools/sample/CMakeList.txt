set(LLVM_LINK_COMPONENTS
  Core
  ${LLVM_TARGETS_TO_BUILD}
)

add_llvm_tool(sample
  main.c
)
target_link_libraries(sample liball)

add_definitions(${LLVM_DEFINITIONS})
