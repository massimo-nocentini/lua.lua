
linux:
	gcc -O2 -std=gnu11 -fPIC -Wall -s -shared -o liblualua.so liblualua.c -llua

macos:
	cc -O2 -std=c11 -fPIC -Wall -dynamiclib -o liblualua.dylib liblualua.c -llua

install:
	mkdir -p /usr/local/lib/lua/5.4/	# just for ensuring that the dest dir exists
	mkdir -p /usr/local/share/lua/5.4/	# just for ensuring that the dest dir exists
	mv liblualua.so /usr/local/lib/lua/5.4/

install-macos:
	mkdir -p /usr/local/lib/lua/5.4/	# just for ensuring that the dest dir exists
	mkdir -p /usr/local/share/lua/5.4/	# just for ensuring that the dest dir exists
	mv liblualua.dylib /usr/local/lib/lua/5.4/
	#cp liblualua.lua /usr/local/share/lua/5.4/

test:
	cd ../test && lua test-liblualua.lua -v #> README
