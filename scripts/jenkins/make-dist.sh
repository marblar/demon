autoreconf --install
mkdir -p build
cd build
export OMPI_CXX=clang++
../configure --with-mpi
make distcheck
make dist
