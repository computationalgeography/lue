if(LUE_BUILD_DOCUMENTATION AND LATEX_FOUND)
    add_latex_document(land_use.tex
        BIBFILES
            bibliography.bib
    )
endif()
