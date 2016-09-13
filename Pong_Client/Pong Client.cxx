#include <GL/glfw.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//A class that handles networc comunitacion:
class CNetwork
	{
		//Defines buffer (ands size) that gets sent over socket
		char charBuffer[ 256 ];
		//Variables for socket info and port nubmer
		int sockfd;
		//Structs for server inforamtion:
		struct sockaddr_in serv_addr;
		struct hostent *server;
		public:
		//Paddle and ball position:
		float floPlayer2Y,floBallY, floBallX;
		//Connects to a server:
		bool PongConnect( const char* charServerAddress, int intPortNumber )
			{
				sockfd = socket( AF_INET, SOCK_STREAM, 0 );
				server = gethostbyname( charServerAddress );
				memset( (char*) &serv_addr, 0, sizeof( serv_addr ) );
				serv_addr.sin_family = AF_INET;
				bcopy( ( char* ) server -> h_addr, ( char* ) &serv_addr.sin_addr.s_addr, server -> h_length );
				serv_addr.sin_port = htons( intPortNumber );
				connect( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) );
			}
		//Sernds paddle y coordiante over socket:
		void Send( float floY )
			{
				std::stringstream strTemporary;
				memset( charBuffer, 0, sizeof charBuffer );
				strTemporary << floY;
				strTemporary >> charBuffer;
				write( sockfd, charBuffer, strlen( charBuffer ));
			}
		//Recives paddle y coordiante over socket:
		void Recive()
			{
				std::stringstream strTemporary;
				char charComma;
				memset( charBuffer, 0, sizeof charBuffer );
				read( sockfd, charBuffer, 255);
				strTemporary << charBuffer;
				strTemporary >> floPlayer2Y >> charComma >> floBallX >> charComma >> floBallY;
			}
		//Closes socket:
		void Terminate()
			{
				close( sockfd );
			}
	};
//Handles display:
class CGraphics
	{
		public:
		//Draws a rectange with given parameters, can be textured:
		void Draw( float floX1, float floY1, float floX2, float floY2, const char* charTexture )
			{
				GLint tex;
				tex = glfwLoadTexture2D ( charTexture, GLFW_BUILD_MIPMAPS_BIT );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glEnable( GL_BLEND );
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, tex );
				glBegin( GL_POLYGON );
				glColor3f( 1, 1, 1 );
				glTexCoord2f( 0, 0 );
				glVertex2f( floX1, floY1);
				glTexCoord2f( 1, 0 );
				glVertex2f( floX2, floY1 );
				glTexCoord2f( 1, 1 );
				glVertex2f( floX2, floY2 );
				glTexCoord2f( 0, 1);
				glVertex2f( floX1, floY2 );
				glDisable( GL_TEXTURE_2D );
				glDisable( GL_BLEND );
				glEnd();
				glFlush();	
			}

	};
//Handles mouse input and positon:
class CInput
	{
		public:
			float floMouseY, floMouseX;
			//Gets the mouse position on screen:
			void Mouse( float intWindowHeight, float intWindowWidth )
				{
					int intTemporaryMouseX, intTemporaryMouseY;
					glfwGetMousePos( &intTemporaryMouseX, &intTemporaryMouseY );
					floMouseX = ((float(intTemporaryMouseX)/float( intWindowHeight ))*2 - 1);
					floMouseY = -((float(intTemporaryMouseY)/float( intWindowWidth ))*2 - 1);
					//Hides mouse coursor:
					glfwDisable( GLFW_MOUSE_CURSOR );
					//Dinds mouse to screen:
				        if( floMouseX >= 1 ) 
                                                {   
                                                        intTemporaryMouseX = intWindowWidth;
                                                }   
                                        if( floMouseX <= -1 )
                                                {
                                                        intTemporaryMouseX = 0;
                                                }
                                        if( floMouseY >= 1 )
                                                {
                                                        intTemporaryMouseY = 0;
                                                }
                                        if( floMouseY <= -1 )
                                                {
                                                        intTemporaryMouseY = intWindowWidth;
                                                }
                                        glfwSetMousePos( intTemporaryMouseX, intTemporaryMouseY );
				}
	};
//Creates a game window:
class CWindow
	{
		public:
		void Create( int intWindowWidth, int intWindowHeight )
			{
				glfwInit();
				glfwSwapBuffers();
				glClear( GL_COLOR_BUFFER_BIT );
				glfwOpenWindow( intWindowWidth, intWindowHeight, 0, 0, 0, 0, 0, 0, GLFW_WINDOW );
				glfwSetWindowTitle( "Clinet" );
			}
	};
int main( int argc, char *argv[] )
	{
		CWindow Window;
		CInput Mouse;
		CGraphics Display;
		CNetwork Network;
		Network.PongConnect( argv[1], 2005 );
		while( !glfwGetKey( GLFW_KEY_ESC ) )
			{
				Window.Create( 400, 400 );
				Mouse.Mouse( 400, 400 );
				Network.Send( Mouse.floMouseY );
				Network.Recive();
				Display.Draw(  -1.0, -1.0, 1.0, 1.0, "background.tga");
				Display.Draw( Network.floBallX +0.05, Network.floBallY +0.05, Network.floBallX -0.05, Network.floBallY -0.05 , "ball.tga");
				Display.Draw( -1.0, Mouse.floMouseY +0.2, -0.9, Mouse.floMouseY -0.2, "paddle.tga");
				Display.Draw(  1.0, Network.floPlayer2Y +0.2, 0.9, Network.floPlayer2Y -0.2, "paddle.tga");
			}
		Network.Terminate();
	}
