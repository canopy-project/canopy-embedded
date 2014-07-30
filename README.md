Canopy Embedded Development Kit
-------------------------------------------------------------------------------

    LIBWEBSOCKETS
        sudo apt-get install cmake
        sudo apt-get install zlib1g-dev
        sudo apt-get install libssl-dev
        sudo apt-get install rpm
        sudo apt-get install g++
        git clone git://git.libwebsockets.org/libwebsockets
        mkdir build
        cd build
        cmake ..
        make

Clone
-------------------------------------------------------------------------------

    git clone https://github.com/canopy-project/canopy-embedded.git

Build and Install
-------------------------------------------------------------------------------

    cd canopy-embedded
    make
    sudo make install

Test Installation
-------------------------------------------------------------------------------

    cano test

You should see output along the lines of:

    cano PASSED. 2 substests.
    All tests passed.

Next Steps
-------------------------------------------------------------------------------

Follow the <a href=http://www.canopy.link/api_docs/tutorial.html>Canopy
Embedded Tutorial</a>.

