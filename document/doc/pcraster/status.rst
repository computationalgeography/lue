.. _status_compared_with_pcraster:

Status of LUE
=============

File format
-----------
TODO


Modelling operations
--------------------
Currently, PCRaster contains implementations of more modelling operations than LUE. Over time
more operations will be added to LUE. Here we compare the list of PCRaster operations with the
current set of operations implemented in LUE. When planning to :ref:`port a PCRaster model to
LUE <port_pcraster_models_to_lue>`, it is a good idea to compare the information in the next
table with the operations used by the model.

Here we only mention LUE operations that implement an operation that is also in PCRaster. LUE
also contains other operations, not listed here.

If an operation used in a PCRaster model is not available in LUE yet, it is a good idea to
report it to the project team. Various factors determine the order in which operations are
added to the LUE framework. The number of model developers requesting one is one of them.

.. note::

   A LUE operation that implements an operation that is also in PCRaster may be different from
   the PCRaster operation. It may be named different, it may accept different arguments, or
   accept them in a different order. We provide a :ref:`special module <lue_pcraster_module>`
   that can be used to quickly port PCRaster model by hiding these differences as much as possible.

.. list-table:: Legend
   :widths: auto

   * - ✅
     - | LUE supports this PCRaster operation, or an operation like it.
       | It may be that the LUE operation is named differently, that it
       | supports different parameters, or that it accepts parameters in
       | a different order.
   * - ❌
     - This PCRaster operations is not considered yet. It may be in the future.
   * - 🗑
     - This PCRaster operation is considered, but skipped. See notes for reason.
   * - 1, 2, 3
     - | Guesstimate of the amount of effort required to add it:
       | 1 little, 2 somewhat, 3 serious.
       | This is only relevant for operations that have not been added yet.

.. list-table:: Status of LUE operations vs PCRaster operations
   :header-rows: 1
   :widths: auto

   * - PCRaster
     - LUE
     - Notes
   * - ``+``
     - ✅
     -
   * - ``-``
     - ✅
     -
   * - ``/`` or div
     - ✅
     -
   * - ``*``
     - ✅
     -
   * - ``**``
     - ✅
     -
   * - abs
     - ❌
     - 1
   * - | accucapacityflux
       | accucapacitystate
     - ❌
     - 1
   * - accuflux
     - ✅
     -
   * - | accufractionflux
       | accufractionstate
     - ❌
     - 1
   * - | accuthresholdflux
       | accuthresholdstate
     - ✅
     -
   * - | accutriggerflux
       | accutriggerstate
     - ❌
     - 1
   * - | accutraveltimestate
       | accutraveltimeflux
       | accutraveltimeremoved
     - ❌
     - 2
   * - | accutraveltimefractionstate
       | accutraveltimefractionflux
       | accutraveltimefractionremoved
     - ❌
     - 2
   * - acos
     - ✅
     -
   * - and, ``&``, pcrand
     - ✅
     -
   * - | argorder
       | argorderwithid
     - ❌
     - ?
   * - | argorderaddarealimited
       | argorderwithidaddarealimited
     - ❌
     - ?
   * - | argorderarealimited
       | argorderwithidarealimited
     - ❌
     - ?
   * - areaarea
     - ❌
     - 1
   * - areaaverage
     - ❌
     - 1
   * - areadiversity
     - ❌
     - 1
   * - areamajority
     - ❌
     - 1
   * - areamaximum
     - ❌
     - 1
   * - areaminimum
     - ❌
     - 1
   * - areanormal
     - ❌
     - 1
   * - areaorder
     - ❌
     - 1
   * - areatotal
     - ✅
     -
   * - areauniform
     - ❌
     - 1
   * - asin
     - ✅
     -
   * - aspect
     - ✅
     -
   * - atan
     - ✅
     -
   * - boolean
     - ✅
     -
   * - catchment
     - ❌
     - 2
   * - catchmenttotal
     - ❌
     - 1
   * - cellarea
     - ❌
     - 1
   * - celllength
     - ❌
     - 1
   * - clump
     - ❌
     - 1
   * - cos
     - ✅
     -
   * - cover
     - ❌
     - 1
   * - defined
     - ✅
     -
   * - directional
     - 🗑
     - LUE currently does not support value scales.
   * - downstream
     - ✅
     -
   * - downstreamdist
     - ✅
     -
   * - | dynwavestate
       | dynwaveflux
       | lookuppotential
       | lookupstate
     - ❌
     - 3
   * - dynamicwave
     - ❌
     - 3
   * - eq or ``==``
     - ✅
     -
   * - exp
     - ✅
     -
   * - extentofview
     - ❌
     - 2
   * - fac
     - ❌
     - 1
   * - ge or ``>=``
     - ✅
     -
   * - gt or ``>``
     - ✅
     -
   * - horizontan
     - ❌
     - 1
   * - idiv, ``//``
     - ❌
     - 1
   * - if then
     - ✅
     -
   * - if then else
     - ✅
     -
   * - influencesimplegauss
     - ❌
     - ?
   * - inversedistance
     - ❌
     - 2
   * - kinematic
     - ❌
     - ?
   * - | kinwavestate
       | kinwaveflux
     - ❌
     - ?
   * - ldd
     - 🗑
     - LUE currently does not support value scales.
   * - lddcreate
     - ✅
     - | The operation assumes the DEM passed in is sound:
       | depressions are filled and flats are handled.
   * - lddcreatedem
     - ❌
     - 3
   * - ldddist
     - ❌
     - 2
   * - lddmask
     - ❌
     - 2
   * - lddrepair
     - ❌
     - 2
   * - le or ``<=``
     - ✅
     -
   * - ln
     - ✅
     -
   * - log10
     - ❌
     - 1
   * - lookup
     - ❌
     - ?
   * - lookuplinear
     - ❌
     - ?
   * - lookupmapstack
     - ❌
     - ?
   * - lt or ``<``
     - ✅
     -
   * - maparea
     - ❌
     - 1
   * - mapmaximum
     - ✅
     -
   * - mapminimum
     - ✅
     -
   * - mapnormal
     - ❌
     - 1
   * - maptotal
     - ✅
     -
   * - mapuniform
     - ❌
     - 1
   * - | markwhilesumle
       | markwhilesumge
     - ❌
     - 2
   * - max
     - ❌
     - 1
   * - min
     - ❌
     - 1
   * - mod, ``%``
     - ❌
     - 1
   * - ne or ``!=``
     - ✅
     -
   * - nodirection
     - ❌
     - 1
   * - nominal
     - ✅
     - 
   * - normal
     - ❌
     - 1
   * - | not
       | ``~``
       | pcrnot
     - ✅
     -
   * - | or
       | ``|``
       | pcror
     - ✅
     -
   * - order
     - ❌
     - ? Idea is easy, parallel implementation seems not.
   * - ordinal
     - ✅
     -
   * - path
     - ❌
     - 2
   * - pit
     - ❌
     - 1
   * - plancurv
     - ❌
     - 2
   * - pred
     - 🗑
     - Not sure why this is useful...
   * - profcurv
     - ❌
     - 2
   * - rounddown
     - ❌
     - 1
   * - roundoff
     - ❌
     - 1
   * - roundup
     - ❌
     - 1
   * - scalar
     - ✅
     -
   * - | shift
       | shift0
     - 🗑
     - Not sure why this is useful...
   * - sin
     - ✅
     -
   * - slope
     - ✅
     -
   * - slopelength
     - ❌
     - 3
   * - spatial
     - ✅
     -
   * - spread
     - ❌
     - 3
   * - spreadldd
     - ❌
     - 3
   * - spreadlddzone
     - ❌
     - 3
   * - spreadmax
     - ❌
     - 3
   * - spreadmaxzone
     - ❌
     - 3
   * - spreadzone
     - ❌
     - 3
   * - sqr
     - ❌
     - 1
   * - sqrt
     - ✅
     -
   * - streamorder
     - ❌
     - 2
   * - subcatchment
     - ❌
     - 3
   * - succ
     - 🗑
     - Not sure why this is useful...
   * - tan
     - ✅
     -
   * - time
     - 🗑
     - Things work differently in a LUE C++/Python model.
   * - timeinput…
     - ❌
     - ?
   * - timeinput
     - ❌
     - ?
   * - timeinputmodulo
     - ❌
     - ?
   * - timeinputsparse
     - ❌
     - ?
   * - timeoutput
     - ❌
     - ?
   * - timeslice
     - ❌
     - ?
   * - transient
     - ❌
     - 2
   * - uniform
     - ✅
     -
   * - uniqueid
     - ❌
     - 1
   * - upstream
     - ✅
     -
   * - view
     - ❌
     - 3
   * - window4total
     - ❌
     - 1
   * - windowaverage
     - ❌
     - 1
   * - windowdiversity
     - ❌
     - 1
   * - windowhighpass
     - ❌
     - 1
   * - windowmajority
     - ❌
     - 1
   * - windowmaximum
     - ✅
     -
   * - windowminimum
     - ✅
     -
   * - windowtotal
     - ✅
     -
   * - xcoordinate
     - 🗑
     - Not sure why this is useful...
   * - | xor
       | ``^``
       | pcrxor
     - ❌
     - 1
   * - ycoordinate
     - 🗑
     - Not sure why this is useful...


..
   In LUE, not in PCRaster:
   - atan2.hpp
   - convolve.hpp
   - copy.hpp


..
   In LUE:
   - accu.hpp
   - accu3.hpp
   - accu_fraction.hpp
   - accu_info.hpp
   - accu_info3.hpp
   - accu_threshold.hpp
   - accu_threshold3.hpp
   - all.hpp
   - any.hpp
   - array_like.hpp
   - array_partition_id.hpp
   - close_to.hpp
   - empty.hpp
   - fill.hpp
   - flow_accumulation.hpp
   - flow_direction.hpp
   - functor_traits.hpp
   - inflow_count.hpp
   - inflow_count3.hpp
   - inter_partition_stream.hpp
   - iterate_per_element.hpp
   - kernel.hpp
   - locality_id.hpp
   - mesh_grid.hpp
   - none.hpp
   - operator.hpp
   - out_of_range.hpp
   - partition_count_unique.hpp
   - partition_operation.hpp
   - policy.hpp
   - range.hpp
   - timestamp.hpp
   - unique.hpp
   - unique_id.hpp
   - zonal_sum.hpp


Modelling framework
-------------------
TODO
