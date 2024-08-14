Goals:

- Keep the amount of code needed to get the Python package working correctly and conveniently
  as small as possible
- Keep the amount of (public) documentation needed as small as possible
- Code that is useful for language bindings, including the public C++ API, should be concentrated
  in a library (lue/framework/api/cxx). All non-C++ language binding should be based on the C++
  public API.
- The public C++ API should be documented for the end-user. This documentation must be reused
  for the non-C++ language bindings.
