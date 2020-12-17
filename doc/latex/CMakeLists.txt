cmake_minimum_required (VERSION 3.9.6)

add_latex_document(radcalc_uguide.tex
  EXCLUDE_FROM_ALL
  USE_INDEX
  BIBFILES references.bib
)

