cmake_minimum_required (VERSION 3.9.6)

project (RADCALC_DOCS NONE)

add_subdirectory(latex)

message(STATUS ${RADCALC_DOCS_BINARY_DIR}/latex/radcalc_uguide.pdf)

add_custom_target(radcalc_docs DEPENDS radcalc_uguide_pdf)
add_custom_command(
  TARGET radcalc_docs
  COMMAND ${CMAKE_COMMAND} -E rename
  ${RADCALC_DOCS_BINARY_DIR}/latex/radcalc_uguide.pdf
  ${RADCALC_DOCS_BINARY_DIR}/radcalc_uguide.pdf
)
