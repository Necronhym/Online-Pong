# Online-Pong

This is a simple game of Pong that can be played over the Internet or on a local network.

It's a simple server/client architecture. The server side holds the ball logic while the client supplies the paddle location. They both handle the graphics that is done vie OpenGL and GLFW( 2.6 ).
The game uses port 2005 so if you plan to host a game makes sure you have the port open.
The client takes one argument, the server ip address, while the server takes no arguments.
The game was built and tested on GNU/Linux Debian 8.5 Jessie
The library required to run are OpenGl and GLFW  
The game currently has no score, sound or graphical artwork. These might me added later.
