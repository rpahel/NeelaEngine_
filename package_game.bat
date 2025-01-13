xmake clean
xmake config --mode=release
xmake build AC
xmake build ACServer
xmake install -o GAME_FOLDER
PAUSE