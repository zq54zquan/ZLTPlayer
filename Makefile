player : ZLTPlayer.cpp
	gcc -o player ZLTPlayer.cpp -Xlinker -export_dynamic -Xlinker -no_deduplicate -liconv -framework coreMedia -framework VideoDecodeAcceleration -framework Security -lbz2 -framework CoreFoundation -lz -framework CoreVideo -framework AudioToolbox -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -framework VideoToolbox -lstdc++  -I /usr/local/include -L /usr/local/lib -framework SDL  -F /usr/local/Frameworks

