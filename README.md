# Conway's Game of Life

![game_of_life](https://user-images.githubusercontent.com/52719903/145503556-76cc1712-fa6b-4179-b2a5-56bb3eea10b9.gif)

[![game_of_life](https://www.youtube.com/watch?v=Jg5F3SgtHzs)](https://www.youtube.com/watch?v=ek1j272iAmc)

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
game_of_life uses the MIT license. See [!LICENSE](https://github.com/LentilStew/game_of_life/blob/main/LICENSE) for more details.
