mkdir -p build
mkdir -p bin
cd build || exit
cmake ../
make -j 8
mv server ../bin
cd ..