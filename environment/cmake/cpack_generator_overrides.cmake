# We like the name of the top level directory in the archive file to be the name of the package,
# without the file extension. This is the default for archive generators for non-component
installs. Multi-component packages don't have a top level directory by default. Let's change that.
if(CPACK_GENERATOR MATCHES "^(7Z|TBZ2|TGZ|TXZ|TZ|TZST|ZIP)$")
    set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY YES)
endif()
