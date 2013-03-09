autoreconf --install
mkdir -p build
cd build
../configure --cache-file=$HOME/demon-cache
make distcheck
make dist
