# Solition1_Dashboard

THis Qt5 program aims to build a display for the Solition1 motor controller



// The following readme was taken from the Logger.c distribute with the Solition1 Firmware.

Note!

This document contains a specification of the logger data the
controller broadcasts over the network. It's aimed at software
engineers that want to do their own logging software, dash board
gauges or similar that will be compatible with the Soliton series of
controllers.

Anyone else can safely ignore this document since it only contains a
lot of technical mumbo-jumbo anyway. ;-)

______________________________________________________________________


The data traffic is sent over UDP, source address is the controller's
IP-address (169.254.0.1), destination address is the global broadcast
address (255.255.255.255) and the port is 48879 (or 0xBEEF in hex,
check Wikipedia for "magic number" if you didn't get it and are
curious of what cultural reference you just missed).

The controller might broadcast an UDP-packet every 10:th ms but will
only do so if there's CPU time enough and the web server isn't
accessed at the same time. The best tactic to stay in sync is simply
to read as fast as possible so newer packets never have time to be
appended to the previous one(s).

The UDP-packets used to contain 14 16-bit words (most of them
unsigned) in Soliton softare version 1.0 but in 1.1 it was extended to
16 words. 1.2 and later have kept that size but one has a slightly
altered function. Since you can't be sure of the length (it could even
be increased in the future), keeping in sync by reading faster than
the controller can transmit is a good tactic.

This is the complete format:

   0: 2 NUL-bytes (ie 0x0000)
   1: Running time (0-59999 ms)
   2: Running time (0-65535 s)
   3: 12 Volt supply (see comment below)
   4: Pack voltage (V)
   5: Motor current (A)
   6: Tempreature (C * 10, signed)
   7: Input 3 (see comment below)
   8: Input 2 (-"-)
   9: Input 1 (-"-)
  10: Throttle (A)
  11: CPU-load (divide by 128 for percent)
  12: Pulse width (divide by 10 for percent)
  13: RPM (not that it works very well in 1.0, but it's reported anyway)
  14: Debug word (ie ignore, introduced in 1.1)
  15: Mode (see comment below, introduced in 1.1)

Some of these values are dumped raw from the internal format rather
than converted while others are converted to more or less readable
format. The actual 12 Volt supply voltage can be calculated by taking
the 4:th word, multiply by 0.00025939941 and adding 1. The actual
input voltages can be calculated by multiplying by 0.000080645.

The mode word has changed format between 1.1 and 1.2. In 1.1 it only
reported errors as a signed byte (high byte was always 0) where
negative values indicated errors. In 1.2 the word has been changed to
a bit field with the following functions (the function of bit 13 has
changed in 1.4):

  15: High motor voltage
  14: High motor power
  13: Throttle limit input (any of 1-3)
      (Before 1.4: High motor temp (snap switch input))
  12: Over REV
  11: Low pack voltage (or LVC BMS input active)
  10: High pack current
   9: High voltage high current (ie if pack voltage is above 310 Volt
      AND motor current hit 900 Ampere)
   8: High controller temp (this will only be active if the current is
      detoriated due to high temperature and not if only the
      temperature itself is high)
   7: Slewrate active
   6: Throttle blocked (for example when shifting reverse-forward)
   5: Brake input active
   4-0: Status/error codes

The status/error codes are:

     0: Starting up (software initializing etc)
     1: Precharge phase (precharge relay active)
     2: Engaging contactors (the two contactors are activated
        sequencially to avoid a too big current rush on 12 Volt)
     3: Waiting for start signal (if that's enabled as an input)
     4: Throttle not in zero position
     5: Running
  6-18: (not used)
    19: ADC out of range (new in 1.3)
    20: Current sensor failure (new in 1.3)
    21: Zero voltage after precharge
    22: Pack voltage too low after precharge
    23: Faulty throttle signal
    24: 12 Volt too high
    25: 12 Volt too low
    26: Pack voltage too high (ie above 342 Volt)
    27: Pack voltage too low (will probably only show if you blow a
    	fuse or have a loose connection)
    28: IGBT desaturation (changed name in 1.3 but same function)
    29: Out of memory
    30: Software error
    31: Controller shut down by user (either in web interface or by
    	turning off ignition)

If you mask the status/error word with 0x1f (or 31 in decimal) the
status/error codes will be the same in both 1.1 and 1.2.

Please note that there's also an extra debug feature in the log
format! If the first byte isn't NUL the controller hasn't sent a log
packet but a clear text NUL-terminated ASCII-string of arbitrary
length! Most (all?) of these are removed in 1.2 since the mode bit
field satisfy the same need in a more elegant fashion, but it's
probably still a good thing to make sure the software can handle them
if they appear since older versions of the software does transmit them
every now and then.

/Martin

[eof]
