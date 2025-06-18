if(LATEX_FOUND)
    set(name logo)

    add_latex_document(${name}.tex
        TARGET_NAME
            lue.figure.${name}
    )

    if(ImageMagick_FOUND)
        # The osgeo version can be used for the OSGEO LUE project page, which requires the logo to be of size
        # 740x412 pixels
        add_custom_command(
            COMMENT "${name}.pdf → ${name}-{250,500,osgeo}.png"
            TARGET lue.figure.${name}_pdf
            POST_BUILD
            BYPRODUCTS
                ${name}-250.png
                ${name}-500.png
                ${name}-osgeo.png
            COMMAND ${ImageMagick_convert_EXECUTABLE} -density 2400 -resize 250x250 ${name}.pdf ${name}-250.png
            COMMAND ${ImageMagick_convert_EXECUTABLE} -density 2400 -resize 500x500 ${name}.pdf ${name}-500.png
            COMMAND ${ImageMagick_convert_EXECUTABLE}
                -density 2400 -resize 700x700 -gravity center -extent 740x412 ${name}.pdf ${name}-osgeo.png
            VERBATIM
        )
    endif()
endif()
