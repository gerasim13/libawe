libawe
======

libawe is a fully-software audio library written in C++11. It uses PortAudio for audio output and libsndfile for decoding sound files. The modules in this library is designed to be easy to modify, extended and/or removed depending on what your project needs. 

Installation
------------

1. Obtain and install the following libraries:
   * `PortAudio` -- audio output API
   * `libsndfile` -- audio file decoding library
2. Compile via 'Makefile'.
   
   ```bash
   $ cd ../source
   $ make
   ```
