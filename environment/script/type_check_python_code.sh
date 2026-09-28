#!/usr/bin/env bash
set -eu

if [[ $# != 1 ]]; then
    echo "$(basename "$0") <source_prefix> <build_prefix>"
    exit 1
fi

build_prefix=$1

pushd "$build_prefix" >/dev/null
package_offsets=(lib*/python*)       # Array
package_offset=${package_offsets[0]} # One and only value
popd >/dev/null

package_prefix="$build_prefix/$package_offset"

# TODO: Get rid of excluded paths
# TODO: Get rid of skipped checks
cat >"$build_prefix/pyproject.toml" <<EOF
[tool.pyright]
extraPaths = [
    "$package_offset",
]
exclude = [
    "**/scalability/core/plot.py",
]
reportArgumentType = false
reportAttributeAccessIssue = false
reportCallIssue = false
reportIncompatibleMethodOverride = false
reportOptionalMemberAccess = false
reportOperatorIssue = false
reportOptionalOperand = false
reportOptionalSubscript = false
stubPath = "$package_offset"
EOF

# PYTHONPATH=$package_prefix pyright --createstub "lue"

find "$package_prefix" -name "*.py" -print0 | xargs -0 pyright # --skipunannotated
