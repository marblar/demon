autoreconf --install
mkdir -p build
cd build
../configure
make distcheck
make dist
