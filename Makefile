all: ./build ./server

./deps/gyp:
	git clone --depth 1 https://chromium.googlesource.com/external/gyp.git ./deps/gyp

./deps/nodeuv-uri:
	git clone --depth 1 http://github.com/hij1nx/nodeuv-uri ./deps/nodeuv-uri

./deps/nodeuv-http:
	git clone --depth 1 git://github.com/hij1nx/nodeuv-http.git ./deps/nodeuv-http
	cd ./deps/nodeuv-http && make && cp build/out/Release/libnodeuv-http.a /usr/local/lib/ 
	cd ..

./deps/debug:
	git clone --depth 1 git://github.com/hij1nx/debug.git ./deps/debug

./deps/route:
	git clone --depth 1 git://github.com/hij1nx/route.git ./deps/route
	cd ./deps/route && make && make install

./deps/leveldb:
	git clone https://code.google.com/p/leveldb/ ./deps/leveldb
	cd ./deps/leveldb && make && cp ./libleveldb.a /usr/local/lib

./server: server.cc
	make -C ./build/ server
	cp ./build/out/Release/server ./server

./build: ./deps/gyp ./deps/nodeuv-http ./deps/nodeuv-uri ./deps/debug ./deps/route
	deps/gyp/gyp --depth=. -Goutput_dir=./out --generator-output=./build -f make -debug 

distclean:
	make clean
	rm -rf ./build

test:
	./build/out/Release/server && killall server

clean:
	rm -rf ./build/out/Release/obj.target/server/
	rm -f ./build/out/Release/server
	rm -f ./server

.PHONY: test
