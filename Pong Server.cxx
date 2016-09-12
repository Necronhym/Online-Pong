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
class network
	{
		//Define socket varibales:
		int sockfd, newsockfd, portno, n;
		char buffer[256];
		struct sockaddr_in serv_addr, cli_addr;
		socklen_t clilen;
		public:
		void startport(const char* p)
			{
				//Socket varables:
				//Port number:
				const char* port = p;
				//Socket Paramaters:
				sockfd = socket( AF_INET, SOCK_STREAM, 0 );
				memset( ( char* ) &serv_addr, 0, sizeof( serv_addr) );
				portno = atoi( port );
				//Server Paramaters:
				serv_addr.sin_family = AF_INET;
				serv_addr.sin_addr.s_addr = INADDR_ANY;
				serv_addr.sin_port = htons( portno );
				//Binds socket:
				bind(sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) );
				//Listens to seocket for incoming connections:
				listen( sockfd, 5 );
				//Defines size of client
				clilen = sizeof(cli_addr);
				//Accepts connection and creates a new socket:
				newsockfd = accept( sockfd, ( struct sockaddr * ) &cli_addr, &clilen );
			}
		//Handles incloming traffic:
		float gety()
			{
				float y;
				memset( &buffer, 0, sizeof buffer );
				read( newsockfd, buffer, 255 );
				std::stringstream temp;
				temp << buffer;
				temp >> y;
				return y;
			}
		//Sends traffic
		void sendy(float y, float bx, float by)
			{
				std::stringstream tempy;
				char comma;
				memset( &buffer, 0, sizeof buffer );
				tempy << y<< comma<< bx << comma << by;
				tempy >> buffer;
				write( newsockfd, buffer, strlen( buffer ) );
			}
		//Closes ports:
		void terminate()
			{
				close( newsockfd );
				close( sockfd );
			}
	};
class graphics
	{
		public:
		void draw( float x1, float y1, float x2, float y2, const char* texture )
			{
				GLint tex;
				tex = glfwLoadTexture2D ( texture, GLFW_BUILD_MIPMAPS_BIT );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glEnable( GL_BLEND);
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
class window
	{
		public:
		void create( int width, int height )
			{
				glfwInit();
				glfwSwapBuffers();
				glClear( GL_COLOR_BUFFER_BIT );
				glfwOpenWindow( width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW );
				glfwSetWindowTitle( "Server" );
			}
	};
class ball
	{
		public:
		float ballx, bally;
		void balllogic( float p1y, float p2y )
			{
				static float vx = 0.05, vy=0 ;
				if( ballx <= -0.9 && ballx >= -0.95 && bally <= p1y+0.2 && bally >= p1y-0.2 )
					{
						vx = -vx;
						vy = (bally - p1y) / 5.0;
					}
				if( ballx >= 0.9 && ballx <= 0.95 && bally <= p2y+0.2 && bally >= p2y-0.2 )
					{
						vx = -vx;
						vy = (bally - p2y) / 5.0;
					}
				if( ballx >= 1.1 )
					{
						ballx = 0.0;
					}
				if( ballx <= -1.1 )
					{
						ballx = 0.0;
					}
				if( bally >= 0.9 || bally <= -0.9 )
					{
						vy = -vy;
					}
				ballx = ballx + vx;
				bally = bally + vy;
			}
	};
int main()
	{
		float p1y;
		window window;
		input mouse;
		graphics display;
		network server;
		ball ball;
		server.startport( "2005" );
		while( !glfwGetKey( GLFW_KEY_ESC ) )
			{
				window.create( 400, 400 );
				mouse.mouse( 400, 400 );
				p1y = server.gety();
				server.sendy( mouse.mousey, ball.ballx, ball.bally );
				ball.balllogic( p1y, mouse.mousey );
				display.draw( ball.ballx+0.05, ball.bally+0.05, ball.ballx-0.05, ball.bally-0.05, "ball.tga" );			
				display.draw( -1.0, p1y + 0.2, -0.9, p1y - 0.2, "paddle.tga" );			
				display.draw( 1.0, mouse.mousey + 0.2, 0.9, mouse.mousey - 0.2, "paddle.tga" );			
			}
		server.terminate();
	}
