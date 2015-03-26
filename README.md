# HapTechSeniorDesign

#### Install dependencies

        sudo apt-get install build-essential scons cmake freeglut3-dev libsfml-dev

#### Auquila

Aquila must be built with `cmake`, which doesn't play nice with `make`

        cd extern
        git clone https://github.com/zsiciarz/aquila
        cd aquila
        cmake CMakeLists.txt
        make
        cd ../..

#### Build Included dependencies

This will build gtest and jsoncpp

        make deps

### Build

To make everything

        make
        
Or you can make individual parts

        make test
        make dsp
        make ui

#### Run

        ./bin/effects_model_ui config/hardware.json config/mapping.json
