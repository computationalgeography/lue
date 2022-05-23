.. currentmodule:: lue.framework


Trigonometry
============
.. autosummary::
    atan
    atan2
    cos
    sin
    tan


.. py:function:: atan(tangent)

   Compute the principal value of the arc tangent

   :parm tangent: Datastructure containing tangents
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: atan2(y, x)

   Compute the arc tangent of y/x

   :parm y: Datastructure containing distances in y-direction
   :parm x: Datastructure containing distances in x-direction
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.

   The signs of the arguments is used to determine the correct quadrant.


.. py:function:: cos(angle)

   Compute the cosine

   :parm angle: Datastructure containing angles in radians
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: sin(angle)

   Compute the sine

   :parm angle: Datastructure containing angles in radians
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: tan(angle)

   Compute the tangent

   :parm angle: Datastructure containing angles in radians
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.
