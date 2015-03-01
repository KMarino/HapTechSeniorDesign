# HapTechSeniorDesign

#### Install dependencies

    sudo apt-get install build-essential scons cmake freeglut3-dev sfml

#### Clone submodules

    git submodule update --init

#### Auquila

    cd extern/aquila-src
    cmake -DCMAKE_INSTALL_PREFIX="../aquila"
    cd ../..

#### Build Included dependencies

    make deps

If you end up with a bunch of untracked files, you can uncheck them out (e.g. `git reset --hard HEAD`).
The binaries will still exist. If submodules keep saying 'untracked', e.g.
`modified: extern/aquila-src (untracked content)`, try

    git submodule foreach git reset --hard HEAD

### Build

    make

#### Run

    ./bin/effects_model_ui config/hardware.json config/mapping.json
