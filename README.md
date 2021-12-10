# Conway's Game of Life

<img src="https://user-images.githubusercontent.com/52719903/145506720-8fab0170-1fa7-44ed-b9a1-ae7080690937.gif" alt="game of life" width="75%" />

<h2>Params</h2>

  -  **codec:** h264_nvenc.
  -  **output_file:** game_of_life.gif.
  -  **framerate:** 10.
  -  **frames:** 3000.
  -  **seed:** 897543123.
  -  **width:** 192.
  -  **height:** 108.
  -  **times:** 1.
  -  **bitrate:** 1000000.

<h2>Rules</h2>

+ Any live cell with two or three live neighbours survives.
+ Any dead cell with three live neighbours becomes a live cell.
+ All other live cells die in the next generation. Similarly, all other dead cells stay dead.

<h2>Seeds</h2>
To generate seeds I use a LFSR (linear-feedback shift register).

<h2>License</h2>
game_of_life uses the MIT license. See <a href="https://github.com/LentilStew/game_of_life/blob/main/LICENSE" target="_top">LICENSE</a> for more details.
