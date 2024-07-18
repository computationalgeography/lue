#!/usr/bin/env bash
set -e


# Find all .lue files in/below the current directory, and convert them to
# .svg files in the current directory.
for lue_pathname in `find . -name "*.lue"`; do
    basename=${lue_pathname##*/}
    name=${basename%.*}
    dot_filename=${name}.dot
    svg_filename=${name}.svg

    echo "$lue_pathname → $svg_filename"
    if [ $name != "string_attribute" ]; then
        $LUE_OBJECTS/bin/lue_translate export \
            --meta $LUE/document/lue_translate/dot_properties.json \
            $lue_pathname $dot_filename
        dot -Tsvg -o $svg_filename $dot_filename
    fi
done
