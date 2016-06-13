pynxt
=====

Bluetooth control for the Lego NXT.

WARNING
-------

This is a wrapper around another project: `C_NXT
<https://github.com/llllllllll/C_NXT>`_.  That library was written when I had
access to a Lego NXT; however, I have not yet tested the bindings. I would love
for people to use this but it is currently untested.

When writing that project I hoped that it could be used to introduce programming
to kids but being in C caused too many problems. Now that I know Python, I feel
like it is a much better language for teaching and have added bindings so that
more people have access to this code.

Usage
=====

TODO: sphinx docs.

This library provides one class ``pynxt.NXT`` which represents a bluetooth
connection to a Lego NXT. The NXT must be paired with the computer, and the
constructor is given the MAC address of the NXT, for example:

.. code-block:: python

   from pynxt import NXT

   nxt = NXT('00:00:00:00:00:00')  # some MAC address


We may also use the ``NXT`` object in a context manager to automatically close
the connection when we are done.


Attributes
----------

``battery_level``
`````````````````

.. code-block::

   The charge remaining in mV.


``closed``
``````````

.. code-block::

   Is the connection to the Lego NXT closed?

``dev_id``
``````````

.. code-block::

   The device id of the connected lego NXT.

Methods
-------


``close``
`````````

.. code-block::

   Close the connection to the Lego NXT.

``drive_forward``
`````````````````

.. code-block::

   Tell the nxt to drive forward for some
   period of time at a specified power.

   Parameters
   ----------
   time : int
       The number of seconds to drive for.
   power : int
       How much power should be applied to the motors
       [-100, 100].
   left_port : int
       The port where the left motor is connected.
   right_port : int
       The port where the right motor is connected.

   Raises
   ------
   ValueError
       Raised when the left or right port is out of bounds
       or when the power is not in the range [-100, 100]
   IOError
       Raised when communication with the NXT fails.

``drive_backward``
``````````````````

.. code-block::

   Tell the nxt to drive backward for some
   period of time at a specified power.

   Parameters
   ----------
   time : int
       The number of seconds to drive for.
   power : int
       How much power should be applied to the motors
       [-100, 100].
   left_port : int
       The port where the left motor is connected.
   right_port : int
       The port where the right motor is connected.

   Raises
   ------
   ValueError
       Raised when the left or right port is out of bounds
       or when the power is not in the range [-100, 100]
   IOError
       Raised when communication with the NXT fails.

``init_button``
```````````````

.. code-block::

   Tell the NXT that there is a button plugged to a certain port.

   Parameters
   ----------
   port : int
       The port which has a button plugged in.

   Raises
   ------
   ValueError
       Raised when the port number is out of bounds.
   IOError
       Raised when communication with the NXT fails.


``init_light``
``````````````

.. code-block::

   Tell the NXT that there is a light sensor plugged to a certain
   port.

   Parameters
   ----------
   port : int
       The port which has a light plugged in.

   Raises
   ------
   ValueError
       Raised when the port number is out of bounds.
   IOError
       Raised when communication with the NXT fails.


``is_pressed``
``````````````

.. code-block::

   Check if a button is currently pressed.

   Parameters
   ----------
   port : int
       The port of the button to check.

   Returns
   -------
   is_pressed : bool
       Is the button currently being pressed?

   Raises
   ------
   ValueError
       Raised when the port number is out of bounds.
   IOError
       Raised when communication with the NXT fails.

``play_tone``
`````````````

.. code-block::

   Play a tone of a given frequency for a certain amount of time
   on the NXT.

   Parameters
   ----------
   freq : int
       The frequency to play.
   time : int
       The amount of time to play the note for in microsenconds.

   Raises
   ------
   IOError
       Raised when communication with the NXT fails.

``read_light``
``````````````

.. code-block::

  Read the value of a light sensor.

   Parameters
   ----------
   port : int
       The port of the light sensor to read.

   Returns
   -------
   value : int
       The value on a scale from 0 to 1024.

   Raises
   ------
   ValueError
       Raised when the port number is out of bounds.
   IOError
       Raised when communication with the NXT fails.

``set_motor``
`````````````

.. code-block::

   Sets the power of a motor.

   Parameters
   ----------
   port : int
       The port of the motor to set the power of.
   power : int
       The power to set the motor to: [-100, 100].

   Raises
   ------
   ValueError
       Raised if the port is out of bounds or the power is not
       in the range [-100, 100].
   IOError
       Raised when communication with the NXT fails.

``stay_alive``
``````````````

.. code-block::

   Send a message to the NXT that prevents it from turning off.

   If the NXT doesn't see this message for a couple of minutes it
   will power down to save battery.

   Raises
   ------
   IOError
       Raised when communication with the NXT fails.

``stop_all_motors``
```````````````````

.. code-block::

   Stop all of the motors.

   Raises
   ------
   IOError
       Raised when communication with the NXT fails.

``stop_motor``
``````````````

.. code-block::

   Stop a motor.

   Parameters
   ----------
   port : int
       The port of the motor to stop.

   Raises
   ------
   ValueError
       Raised if the port is out of bounds
   IOError
       Raised when communication with the NXT fails.

``turn_left``
`````````````

.. code-block::

   Tell the nxt to turn left for some
   period of time at a specified power.

   Parameters
   ----------
   time : int
       The number of seconds to turn for.
   power : int
       How much power should be applied to the motors
       [-100, 100].
   left_port : int
       The port where the left motor is connected.
   right_port : int
       The port where the right motor is connected.

   Raises
   ------
   ValueError
       Raised when the left or right port is out of bounds
       or when the power is not in the range [-100, 100]
   IOError
       Raised when communication with the NXT fails.

``turn_right``
``````````````

.. code-block::

   Tell the nxt to turn right for some
   period of time at a specified power.

   Parameters
   ----------
   time : int
       The number of seconds to turn for.
   power : int
       How much power should be applied to the motors
       [-100, 100].
   left_port : int
       The port where the left motor is connected.
   right_port : int
       The port where the right motor is connected.

   Raises
   ------
   ValueError
       Raised when the left or right port is out of bounds
       or when the power is not in the range [-100, 100]
   IOError
       Raised when communication with the NXT fails.


License
-------

``phorth`` is free software, available under the terms of the `GNU General
Public License, version 2 or later <http://gnu.org/licenses/gpl.html>`_. For
more information, see ``LICENSE``.
