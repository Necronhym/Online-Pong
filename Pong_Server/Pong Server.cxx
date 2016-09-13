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
class CNetwork
	{
		//Define socket varibales:
		int sockfd, newsockfd;
		char charBuffer[256];
		struct sockaddr_in serv_addr, cli_addr;
		socklen_t clilen;
		public:
		void StartPort( int intPortNumber)
			{
				//Socket varables:
				//Socket Paramaters:
				sockfd = socket( AF_INET, SOCK_STREAM, 0 );
				memset( ( char* ) &serv_addr, 0, sizeof( serv_addr) );
				//Server Paramaters:
				serv_addr.sin_family = AF_INET;
				serv_addr.sin_addr.s_addr = INADDR_ANY;
				serv_addr.sin_port = htons( intPortNumber );
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
		float Recive()
			{
				float floPlayer1Y;
				memset( &charBuffer, 0, sizeof charBuffer );
				read( newsockfd, charBuffer, 255 );
				std::stringstream strTemporary;
				strTemporary << charBuffer;
				strTemporary >> floPlayer1Y;
				return floPlayer1Y;
			}
		//Sends traffic
		void Send(float floPlayer2Y, float floBallX, float floBallY)
			{
				std::stringstream strTemporary;
				char charComma;
				memset( &charBuffer, 0, sizeof charBuffer );
				strTemporary << floPlayer2Y<< charComma<< floBallX << charComma << floBallY;
				strTemporary >>  charBuffer;
				write( newsockfd, charBuffer, strlen( charBuffer ) );
			}
		//Closes ports:
		void Terminate()
			{
				close( newsockfd );
				close( sockfd );
			}
	};
//Handles graphical display:
class CGraphics
	{
		public:
		void Draw( float floX1, float floY1, float floX2, float floY2, const char* charTexture )
			{
				GLint tex;
				tex = glfwLoadTexture2D ( charTexture, GLFW_BUILD_MIPMAPS_BIT );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glEnable( GL_BLEND);
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
//Handles mouse position:
class CInput
	{
		public:
			float floMouseY, floMouseX;
			//Gets the mouse position on screen:
			void Mouse( float floWindowHeight, float floWindowWidth )
				{
					int intTemporaryMouseX, intTemporaryMouseY;
					glfwGetMousePos( &intTemporaryMouseX, &intTemporaryMouseY );
					floMouseX = ((float(intTemporaryMouseX)/float( floWindowHeight ))*2 - 1);
					floMouseY = -((float(intTemporaryMouseY)/float( floWindowWidth ))*2 - 1);
					//Hides mouse coursor:
					glfwDisable( GLFW_MOUSE_CURSOR );
					//Dinds mouse to screen:
				        if( floMouseX >= 1 ) 
                                                {   
                                                        intTemporaryMouseX = floWindowWidth;
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
                                                        intTemporaryMouseY = floWindowWidth;
                                                }
                                        glfwSetMousePos( intTemporaryMouseX, intTemporaryMouseY );
				}
	};
//A class That handles window creating:
class CWindow
	{
		public:
		void Create( int intWindowWidth, int intWindowHeight )
			{
				glfwInit();
				glfwSwapBuffers();
				glClear( GL_COLOR_BUFFER_BIT );
				glfwOpenWindow( intWindowWidth, intWindowHeight, 0, 0, 0, 0, 0, 0, GLFW_WINDOW );
				glfwSetWindowTitle( "Server" );
			}
	};
//A class that handles ball logic:
class CBall
	{
		public:
		float floBallX, floBallY;
		void BallLogic( float floPlayer1Y, float floPlayer2Y )
			{
				static float floSpeedX = 0.05, floSpeedY=0 ;
				if( floBallX <= -0.9 && floBallX >= -0.95 && floBallY <= floPlayer1Y+0.2 && floBallY >= floPlayer1Y-0.2 )
					{
						floSpeedX = -floSpeedX;
						floSpeedY = (floBallY - floPlayer1Y) / 5.0;
					}
				if( floBallX >= 0.9 && floBallX <= 0.95 && floBallY <= floPlayer2Y+0.2 && floBallY >= floPlayer2Y-0.2 )
					{
						floSpeedX = -floSpeedX;
						floSpeedY = (floBallY - floPlayer2Y) / 5.0;
					}
				if( floBallX >= 1.1 )
					{
						floBallX = 0.0;
					}
				if( floBallX <= -1.1 )
					{
						floBallX = 0.0;
					}
				if( floBallY >= 0.9 || floBallY <= -0.9 )
					{
						floSpeedY = -floSpeedY;
					}
				floBallX = floBallX + floSpeedX;
				floBallY = floBallY + floSpeedY;
			}
	};
int main()
	{
		float floPlayer1Y;
		CWindow Window;
		CInput Mouse;
		CGraphics Display;
		CNetwork Server;
		CBall Ball;
		Server.StartPort( 2005 );
		while( !glfwGetKey( GLFW_KEY_ESC ) )
			{
				Window.Create( 400, 400 );
				Mouse.Mouse( 400, 400 );
				floPlayer1Y = Server.Recive();
				Server.Send( Mouse.floMouseY, Ball.floBallX, Ball.floBallY );
				Ball.BallLogic( floPlayer1Y, Mouse.floMouseY );
				Display.Draw( -1.0, -1.0, 1.0, 1.0, "background.tga" );			
				Display.Draw( Ball.floBallX+0.05, Ball.floBallY+0.05, Ball.floBallX-0.05, Ball.floBallY-0.05, "ball.tga" );			
				Display.Draw( -1.0, floPlayer1Y + 0.2, -0.9, floPlayer1Y - 0.2, "paddle.tga" );			
				Display.Draw( 1.0, Mouse.floMouseY + 0.2, 0.9, Mouse.floMouseY - 0.2, "paddle.tga" );			
			}
		Server.Terminate();
	}
