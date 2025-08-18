# TODO: Upload all graphics to lue.computationalgeography.org/download/logo

function(outer_circle_radius extent output)
    # Compute radius of circle around a square of extent x extent
    execute_process(
        COMMAND
            ${Python_EXECUTABLE}
                -c "import math, sys; sys.stdout.write(str(math.ceil(math.sqrt(2 * (0.5 * ${extent})**2) - 0.5 * ${extent})))"
        OUTPUT_VARIABLE
            __output
        COMMAND_ERROR_IS_FATAL ANY
    )
    set(${output} ${__output} PARENT_SCOPE)
endfunction()


function(inner_circle_radius extent output)
    # Compute extent of square within a circle within a square of extent x extent
    execute_process(
        COMMAND
            ${Python_EXECUTABLE}
                -c "import math, sys; aa = 0.5 * ${extent}; bb = aa; cc = math.sqrt(aa**2 + bb**2); c = 0.5 * ${extent}; a = aa * c / cc; sys.stdout.write(str(math.floor(a)))"
        OUTPUT_VARIABLE
            __output
        COMMAND_ERROR_IS_FATAL ANY
    )
    set(${output} ${__output} PARENT_SCOPE)
endfunction()


function(icon_border outer_icon_extent inner_icon_extent output)
    # Compute the border around an inner icon so the extent becomes outer_icon_extent
    execute_process(
        COMMAND
            ${Python_EXECUTABLE}
                -c "import math, sys; sys.stdout.write(str(math.ceil(${outer_icon_extent} - ${inner_icon_extent}) / 2.0))"
        OUTPUT_VARIABLE
            __output
        COMMAND_ERROR_IS_FATAL ANY
    )
    set(${output} ${__output} PARENT_SCOPE)
endfunction()


if(LATEX_FOUND)
    foreach(style bw colour grey)
        set(figure icon)
        set(target_name lue.figure.${figure}.${style})

        configure_file(${figure}.tex.in ${figure}-${style}.tex)

        add_latex_document(${figure}-${style}.tex
            TARGET_NAME
                ${target_name}
            INPUTS
                ${figure}.tex
                ${figure}-style.tex
                ${style}.tex
                style.tex
        )

        if(ImageMagick_FOUND)
            # TODO: requires potrace
            # add_custom_command(
            #     TARGET ${target_name}
            #     POST_BUILD
            #     COMMENT "${figure}-${style}.pdf → ${figure}-${style}.svg"
            #     BYPRODUCTS "${figure}-${style}.svg"
            #     COMMAND ${ImageMagick_convert_EXECUTABLE}
            #         ${figure}-${style}.pdf ${figure}-${style}.svg
            #     VERBATIM
            # )

            foreach(outer_icon_extent IN ITEMS 400 500 750 1000)
                # Icon to use for square spaces
                set(output_name "${figure}-${style}-square-${outer_icon_extent}")
                add_custom_command(
                    TARGET ${target_name}
                    POST_BUILD
                    COMMENT "${figure}-${style}.pdf → ${output_name}.png"
                    BYPRODUCTS "${output_name}.png"
                    COMMAND ${ImageMagick_convert_EXECUTABLE}
                        -density 2400 -resize "${outer_icon_extent}x${outer_icon_extent}"
                        ${figure}-${style}.pdf ${output_name}.png
                    VERBATIM
                )

                # Icon to use for round spaces. Some padding is added.
                inner_circle_radius(${outer_icon_extent} inner_circle_radius)
                math(EXPR inner_icon_extent "2 * ${inner_circle_radius}")
                icon_border(${outer_icon_extent} ${inner_icon_extent} icon_border)

                set(output_name "${figure}-${style}-round-${outer_icon_extent}")
                add_custom_command(
                    TARGET ${target_name}
                    POST_BUILD
                    COMMENT "${figure}-${style}.pdf → ${output_name}.png"
                    BYPRODUCTS "${output_name}.png"
                    COMMAND ${ImageMagick_convert_EXECUTABLE}
                        -density 2400 -resize "${inner_icon_extent}x${inner_icon_extent}" -bordercolor none -border ${icon_border}
                        ${figure}-${style}.pdf ${output_name}.png
                    VERBATIM
                )
            endforeach()
        endif()

        set(figure logo)
        set(target_name lue.figure.${figure}.${style})

        configure_file(${figure}.tex.in ${figure}-${style}.tex)

        add_latex_document(${figure}-${style}.tex
            TARGET_NAME
                ${target_name}
            INPUTS
                ${figure}.tex
                ${figure}-style.tex
                ${style}.tex
                style.tex
        )

        if(ImageMagick_FOUND)
            # TODO: requires potrace
            # add_custom_command(
            #     TARGET ${target_name}
            #     POST_BUILD
            #     COMMENT "${figure}-${style}.pdf → ${figure}-${style}.svg"
            #     BYPRODUCTS "${figure}-${style}.svg"
            #     COMMAND ${ImageMagick_convert_EXECUTABLE}
            #         ${figure}-${style}.pdf ${figure}-${style}.svg
            #     VERBATIM
            # )

            foreach(icon_extent IN ITEMS 400 500 750 1000)
                add_custom_command(
                    TARGET ${target_name}
                    POST_BUILD
                    COMMENT "${figure}-${style}.pdf → ${figure}-${style}-${icon_extent}.png"
                    BYPRODUCTS
                        ${figure}-${style}-${icon_extent}.png
                    COMMAND ${ImageMagick_convert_EXECUTABLE}
                        -density 2400 -resize "${icon_extent}x${icon_extent}"
                        ${figure}-${style}.pdf ${figure}-${style}-${icon_extent}.png
                    VERBATIM
                )
            endforeach()

            # The osgeo version can be used for the OSGEO LUE project page, which requires the logo to be of
            # size 740x412 pixels
            add_custom_command(
                TARGET ${target_name}
                POST_BUILD
                COMMENT "${figure}-${style}.pdf → ${figure}-${style}-osgeo.png"
                BYPRODUCTS
                    ${figure}-${style}-osgeo.png
                COMMAND ${ImageMagick_convert_EXECUTABLE}
                    -density 2400 -resize 700x700 -gravity center -extent 740x412
                    ${figure}-${style}.pdf ${figure}-${style}-osgeo.png
                VERBATIM
            )
        endif()
    endforeach()

    if(ImageMagick_FOUND)
        set(style colour)
        set(figure icon)
        set(target_name lue.figure.${figure}.${style})

        # .ico file for use in websites
        add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMENT "${figure}-${style}.pdf → favicon.ico"
            BYPRODUCTS "favicon.ico"
            COMMAND ${ImageMagick_convert_EXECUTABLE}
                -define icon:auto-resize=16,24,32,48,64,72,96,128,256 ${figure}-${style}.pdf "favicon.ico"
            VERBATIM
        )
    endif()
endif()
