# HapTechSeniorDesign

#### Install dependencies

        sudo apt-get install build-essential scons cmake freeglut3-dev libsfml-dev


#### Build Included dependencies

This will build gtest, aquila, and jsoncpp

        make get-deps
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
