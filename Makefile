player : ZLTPlayer.cpp
	gcc -o player ZLTPlayer.cpp -Xlinker -export_dynamic -Xlinker -no_deduplicate -liconv -framework VideoDecodeAcceleration -framework Security -lbz2 -framework CoreFoundation -lz -framework CoreVideo -framework AudioToolbox -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -framework VideoToolbox -lstdc++ 

