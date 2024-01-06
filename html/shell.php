<?php
header("Cross-Origin-Opener-Policy: same-origin");
header("Cross-Origin-Embedder-Policy: require-corp");
?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Asteroid</title>
    <style>
        body {
          margin: 0;
          overflow: hidden;
        }
        canvas.emscripten {
          background: black;
          position: absolute;
          top: 50%; left: 50%;
          transform: translate(-50%, -50%);
          box-shadow: 5px 5px 10px rgba(0, 0, 0, 0.5);
          border-radius: 10px;
          cursor: none;
        }
    </style>
  </head>
  <body>
    <canvas class="emscripten" id="canvas"
            oncontextmenu="event.preventDefault()"
            tabindex="-1" width="800" height="450">
    </canvas>
    <script>
      var Module = {
        canvas: (function() {
          var canvas = document.getElementById('canvas');
          return canvas;
        })()
      };
    </script>
    <script src="Asteroid.js"></script>
  </body>
</html>
