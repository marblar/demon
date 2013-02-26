if [ -e build -o -e demon -o -e results ]; then
    rm -rf build demon results
fi
mkdir demon build results
tar xzf $1 -C demon --strip-components 1
cd build
../demon/configure
make
cd ..
echo jdemon ${@:3} > results/invocation
build/src/jdemon ${@:3} | tee results/$2
