gcc -pthread video.c game_utils.c main.c -o compiled.out -g3 \
`pkg-config --libs libavformat libavfilter libavutil libavcodec libswscale libavdevice libavutil` 

./compiled.out

