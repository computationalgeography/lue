.. currentmodule:: lue.framework


Trigonometry
============
.. autosummary::
    acos
    asin
    atan
    atan2
    cos
    sin
    tan


.. py:function:: acos(cosine)

   Compute the principal value of the arc cosine

   :param cosine: Datastructure containing cosines
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: asin(sine)

   Compute the principal value of the arc sine

   :param sine: Datastructure containing sines
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: atan(tangent)

   Compute the principal value of the arc tangent

   :param tangent: Datastructure containing tangents
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: atan2(y, x)

   Compute the arc tangent of y/x

   :param y: Datastructure containing distances in y-direction
   :param x: Datastructure containing distances in x-direction
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.

   The signs of the arguments is used to determine the correct quadrant.


.. py:function:: cos(angle)

   Compute the cosine

   :param angle: Datastructure containing angles in radians
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: sin(angle)

   Compute the sine

   :param angle: Datastructure containing angles in radians
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.


.. py:function:: tan(angle)

   Compute the tangent

   :param angle: Datastructure containing angles in radians
   :return: New instance of the same datastructure as passed in

   This function is overloaded for arrays containing float32 and float64 elements.
