[TOC]

# Notes  {#lue_notes}

This page contains random notes. These are written during development
work and need to be merged with the documentation. Some may even trigger
a reimplementation of parts of LUE.


## Domain, time, space domain
How about changing domain to time domain and treating the space domain
as any other property?

- All reasons for space domain items to be different from each other
  are the same as for values. Shape per item, shape through time, ...
- Values can change through time, as well as the space domain items. Given
  a time domain, whether or not value items and space domain items change
  through time is up to the application.

```
value/
    constant_shape
        same_shape
        different_shape
    variable_shape
        same_shape
        different_shape

time/
    omnipresent
    located
        shared
        unique
```

Start with coding all different kinds of value / time domain
combinations. Probably, space domain can be implemented in terms of
that code.
