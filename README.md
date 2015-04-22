# HapTechSeniorDesign

#### Install dependencies

        sudo apt-get install build-essential scons cmake freeglut3-dev 

#### Rtaudio
First clone Rtaudio to extern
	cd extern/
	git clone https://github.com/thestk/rtaudio.git
	cd rtaudio
	autoconf
	./configure
	make
#### Aquila

First clone Aqulia to extern

	cd extern/
	git clone git://github.com/zsiciarz/aquila.git aquila-src
	cd ..

Make a build directory for Aquila
Aquila must be built with `cmake`, which doesn't play nice with `make`

        mkdir extern/aquila
	cd aquila
        cmake ../aquila-src
 	
	make
	sudo make install

#### Build Included dependencies

This will build gtest, jsoncpp, and aquila

### Build

        make

#### Run

        ./bin/effects_model_ui config/hardware.json config/mapping.json
