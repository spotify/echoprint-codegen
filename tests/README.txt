Tests using CC0 (public domain) files from:

https://www.freesound.org/people/macdaddyno1/sounds/320068/
https://www.freesound.org/people/EvolDeew/sounds/155810/
https://www.freesound.org/people/FunnyMan374/sounds/243958/

The last one was converted to mp3 using:

  ffmpeg -i 243958.wav -ac 1 243958.mp3

To test, call "py.test" in the parent directory after compiling
the project. This requires Python 2.7. To install py.test, use:

  pip install pytest
