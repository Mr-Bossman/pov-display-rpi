./make.sh
./main
git clean -f -d 

g++ -o main tlc59711.o spi.o main.o
g++ -o spi.o  src/spi.c -Iinclude -c
g++ -o tlc59711.o  src/tlc59711.c -Iinclude -c

g++ -o main.o  src/main.cpp -Iinclude -c 
g++ src/spi.c src/main.cpp src/tlc59711.c src/GPIO.c src/display.c src/render.c  -lpthread -Wl,--no-as-needed -Wall -std=c++11 -Iinclude  `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -o main -O3

sudo apt-get update && sudo apt-get -y upgrade && sudo apt-get install -y libjpeg-dev libtiff5-dev libjasper-dev libpng-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libfontconfig1-dev libcairo2-dev libgdk-pixbuf2.0-dev libpango1.0-dev libgtk2.0-dev libgtk-3-dev libatlas-base-dev gfortran libqtgui4 libqtwebkit4 libqt4-test python3-pyqt5 libhdf5-dev libhdf5-serial-dev libhdf5-103 python3-dev python3-pip

cd ~
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.1.1.zip
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.1.1.zip
unzip opencv.zip
unzip opencv_contrib.zip
mv opencv-4.1.1 opencv
mv opencv_contrib-4.1.1 opencv_contrib
pip install numpy
cd opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \
    -D ENABLE_NEON=ON \
    -D ENABLE_VFPV3=ON \
    -D BUILD_TESTS=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D OPENCV_ENABLE_NONFREE=ON \
    -D CMAKE_SHARED_LINKER_FLAGS=-latomic \
    -D BUILD_EXAMPLES=OFF ..
make -j4
sudo make install
sudo ldconfig