gcc display.c seed.c main.c -o compiled.out -g3 

./compiled.out

ffmpeg -y -r 30 -i output/%d.pgm -pix_fmt yuv420p -vf "scale=768x768:flags=neighbor" game_of_life.mp4