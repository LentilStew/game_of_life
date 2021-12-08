gcc -pthread save_video.c display.c seed.c main.c -o compiled.out -g3 \
`pkg-config --libs libavformat libavfilter libavutil libavcodec libswscale libavdevice libavutil` 

./compiled.out
