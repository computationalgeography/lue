Objects that wrap a Vulkan type T:
- If T actually is a pointer, provide an automatic conversion from our type to T. In general,
  we are responsible for cleaning up the pointer, by calling some Vulkan cleanup function. Copies
  of T must not outlive our instance.
- If T is not a pointer, provide an automatic conversion to a pointer to T. This is useful for
  passing information into Vulkan API function. We assume that it is safe to let our instance go
  out of scope. We assume that the pointer passed into API functions is not stored, but only
  used to immediately use the information pointed to.
