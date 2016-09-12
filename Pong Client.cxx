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
class network
	{
		//Defines buffer (ands size) that gets sent over socket
		char buffer[ 256 ];
		//Variables for socket info and port nubmer
		int sockfd, portno;
		//Structs for server inforamtion:
		struct sockaddr_in serv_addr;
		struct hostent *server;
		public:
		//Paddle and ball position:
		float p2y,bally, ballx;
		//Connects to a server:
		void pongconnect( const char* serveraddress )
			{
				portno = atoi( "2005" );
				sockfd = socket( AF_INET, SOCK_STREAM, 0 );
				server = gethostbyname( serveraddress );
				memset( (char*) &serv_addr, 0, sizeof( serv_addr ) );
				serv_addr.sin_family = AF_INET;
				bcopy( ( char* ) server -> h_addr, ( char* ) &serv_addr.sin_addr.s_addr, server -> h_length );
				serv_addr.sin_port = htons( portno );
				connect( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) );
			}
		//Sernds paddle y coordiante over socket:
		void sendy( float y )
			{
				std::stringstream tempy;
				memset( buffer, 0, sizeof buffer );
				tempy << y;
				tempy >> buffer;
				write( sockfd, buffer, strlen( buffer ));
			}
		//Recives paddle y coordiante over socket:
		void gety()
			{
				std::stringstream tempy;
				char comma;
				memset( buffer, 0, sizeof buffer );
				read( sockfd, buffer, 255);
				tempy << buffer;
				tempy >> p2y >> comma >> ballx >> comma >> bally;
			}
		//Closes socket:
		void terminate()
			{
				close( sockfd );
			}
	};
//Handles display:
class graphics
	{
		public:
		void draw( float x1, float y1, float x2, float y2, const char* texture )
			{
				GLint tex;
				tex = glfwLoadTexture2D ( texture, GLFW_BUILD_MIPMAPS_BIT );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glEnable( GL_BLEND );
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, tex );
				glBegin( GL_POLYGON );
				glColor3f( 1, 1, 1 );
				glTexCoord2f( 0, 0 );
				glVertex2f( x1, y1);
				glTexCoord2f( 1, 0 );
				glVertex2f( x2, y1 );
				glTexCoord2f( 1, 1 );
				glVertex2f( x2, y2 );
				glTexCoord2f( 0, 1);
				glVertex2f( x1, y2 );
				glDisable( GL_TEXTURE_2D );
				glDisable( GL_BLEND );
				glEnd();
				glFlush();	
			}

	};
//Handles mouse input and positon:
class input
	{
		public:
			float mousey, mousex;
			//Gets the mouse position on screen:
			void mouse( float wh, float ww )
				{
					int tmousex, tmousey;
					float x,y;
					glfwGetMousePos( &tmousex, &tmousey );
					x = ((float(tmousex)/float( wh ))*2 - 1);
					y = -((float(tmousey)/float( ww ))*2 - 1);
					mousey = y;
					mousex = x;	
					//Hides mouse coursor:
					glfwDisable( GLFW_MOUSE_CURSOR );
					//Dinds mouse to screen:
				        if( mousex >= 1 ) 
                                                {   
                                                        tmousex = ww;
                                                }   
                                        if( mousex <= -1 )
                                                {
                                                        tmousex = 0;
                                                }
                                        if( mousey >= 1 )
                                                {
                                                        tmousey = 0;
                                                }
                                        if( mousey <= -1 )
                                                {
                                                        tmousey = wh;
                                                }
                                        glfwSetMousePos( tmousex, tmousey );
				}
	};
//Creates a game window:
class window
	{
		public:
		void create( int width, int height )
			{
				glfwInit();
				glfwSwapBuffers();
				glClear( GL_COLOR_BUFFER_BIT );
				glfwOpenWindow( width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW );
				glfwSetWindowTitle( "Clinet" );
			}
	};
int main( int argc, char *argv[] )
	{
		float p2y;
		window window;
		input mouse;
		graphics display;
		network network;
		network.pongconnect( argv[1] );
		while( !glfwGetKey( GLFW_KEY_ESC ) )
			{
				window.create( 400, 400 );
				mouse.mouse(400,400);
				network.sendy( mouse.mousey );
				network.gety();
				display.draw( network.ballx+0.05, network.bally+0.05, network.ballx-0.05, network.bally-0.05 , "ball.tga");
				display.draw( -1.0, mouse.mousey + 0.2, -0.9, mouse.mousey-0.2, "paddle.tga");
				display.draw(  1.0, network.p2y + 0.2,  0.9, network.p2y-0.2, "paddle.tga");
			}
		network.terminate();
	}
