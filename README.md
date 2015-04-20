# HapTechSeniorDesign

#### Install dependencies

        sudo apt-get install build-essential scons cmake freeglut3-dev 

	We now depend on SFML 2.2 which is not in the normal repo.
	Download as source from the github and build/install from there
	mkdir SFML
	mkdir SFML_build (builds with cmake like aquila)
	cd SFML
	git clone https://github.com/LaurentGomila/SFML.git
	cd ../SFML_build
	cmake ../SFML
	(install dependencies until cmake is happy full list here 
	http://www.sfml-dev.org/tutorials/2.2/compile-with-cmake.php)
	make
	sudo make install
	ldconfig (this will refresh the linker cache so it links correctly later)
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
