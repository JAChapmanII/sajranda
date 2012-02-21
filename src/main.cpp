/* {{{
 * Copyright 2010 Jeff Chapman
 *
 * This file is a part of sajranda
 *
 * sajranda is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sajranda is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sajranda.  If not, see <http://www.gnu.org/licenses/>.
 *
 */ //}}}
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;

#include <fstream>
using std::ofstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <SFML/Graphics.hpp>
using sf::RenderWindow;
using sf::VideoMode;
using sf::Event;
using sf::Color;
using sf::Shape;
using sf::Vector2f;
using sf::Vector2i;
using sf::Clock;

using sf::Keyboard;
using sf::Mouse;

#include "model.hpp"

#include "util.hpp"
using util::fileExists;

#include <GL/gl.h>
#include <cmath>

static const long double T_PI = 6.2831853071796;

Model* createModel( string fileName );

int main( int argc, char** argv )
{ //{{{
	static const float gWidth = 960;
	static const float gHeight = 540;
	RenderWindow* mGame =
		new RenderWindow(
			VideoMode( gWidth, gHeight ), "sajranda", sf::Style::Close );
	mGame->SetFramerateLimit( 60 );
	Clock* mClock = new Clock();

	Event* mEvent = new Event;
	vector< Model* > models;
	models.push_back( createModel( "dat/001" ) );
	models.push_back( createModel( "dat/002" ) );
	models.push_back( createModel( "dat/003" ) );
	for( unsigned int i = 0; i < models.size(); ++i )
	{
		if( (models[ i ]) == NULL )
			continue;
		(models[ i ])->setCenter( Model::RectangularPoint(
			225.0f * cos( i * T_PI / models.size() ),
			225.0f * sin( i * T_PI / models.size() ) ) );
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(
		-(gWidth / 2.0f), (gWidth / 2.0f),
		(gHeight / 2.0f), -(gHeight / 2.0f),
		-1, 1 );
	glMatrixMode( GL_MODELVIEW );

	bool isSelecting = false;
	Vector2f selectionStart, selectionEnd;
	float selectionTimeStart = 0;
	while( mGame->IsOpen() )
	{
		while( mGame->PollEvent( *mEvent ) )
		{
			if( mEvent->Type == Event::Closed )
				mGame->Close();

			if( mEvent->Type == Event::KeyPressed )
			{
				if( mEvent->Key.Code == Keyboard::Escape )
				{
					mGame->Close();
				}
			}
		}
		if( !mGame->IsOpen() )
			break;

		mGame->Clear( Color::White );
		Vector2i mousePosition = Mouse::GetPosition( *mGame );

		if( isSelecting )
		{
			if( ! Mouse::IsButtonPressed( sf::Mouse::Left ) )
			// selecting is done, mark selected/non
			{ //{{{
				long double x = 0, y = 0;
				Vector2f offset;
				for( auto i = models.begin(); i != models.end(); ++i )
				{
					if( (*i) == NULL )
						continue;
					x = (*i)->centerX();
					y = (*i)->centerY();
					Vector2f pointDifference = selectionStart - selectionEnd;
					long double pointDistance = sqrt(
							pointDifference.x*pointDifference.x +
							pointDifference.y*pointDifference.y );
					float secondsNow = mClock->GetElapsedTime().AsSeconds();
					if(( secondsNow - selectionTimeStart > 0.250f ) ||
						( pointDistance > 15.0f ))
					{
						// complicated test in case selectionStart and End are reversed
						if((( selectionStart.x < x && x <= selectionEnd.x ) ||
					 		( selectionEnd.x < x && x <= selectionStart.x )) &&
							(( selectionStart.y < y && y <= selectionEnd.y ) ||
							( selectionEnd.y < y && y <= selectionStart.y )))
						{
							if( Keyboard::IsKeyPressed( Keyboard::LControl ) )
							{
								(*i)->unselect();
							}
							else
							{
								(*i)->select();
							}
						}
						else
						{
							if(!(Keyboard::IsKeyPressed( Keyboard::LShift ) ||
								Keyboard::IsKeyPressed( Keyboard::LControl )))
							{
								(*i)->unselect();
							}
						}
					}
					else
					{
						offset.x = selectionStart.x - x;
						offset.y = selectionStart.y - y;
						long double distance =
							sqrt( offset.x*offset.x + offset.y*offset.y );
						if( distance < (*i)->getMinimumRadius() )
						{
							if( Keyboard::IsKeyPressed( Keyboard::LControl ) )
							{
								(*i)->unselect();
							}
							else if( Keyboard::IsKeyPressed( Keyboard::LShift ) )
							{
								(*i)->select();
							}
							else
							{
								for( auto j = models.begin(); j != models.end(); ++j )
								{
									if( (*j) == NULL )
										continue;
									(*j)->unselect();
								}
								(*i)->select();
							}
							break;
						}
					}
				}
				isSelecting = false;
			} //}}}
			else
			// update end point, draw selection rectangle
			{ //{{{
				selectionEnd.x = mousePosition.x - (gWidth / 2.0f);
				selectionEnd.y = mousePosition.y - (gHeight / 2.0f);

				glColor3f( 0.9f, 0.9f, 1.0f );
				glBegin( GL_QUADS );
					glVertex2f( selectionStart.x, selectionStart.y );
					glVertex2f( selectionStart.x, selectionEnd.y );
					glVertex2f( selectionEnd.x, selectionEnd.y );
					glVertex2f( selectionEnd.x, selectionStart.y );
				glEnd();

				glLineWidth( 3.0f );
				glColor3f( 0.0f, 0.0f, 0.6f );
				glBegin( GL_LINE_STRIP );
					glVertex2f( selectionStart.x, selectionStart.y );
					glVertex2f( selectionStart.x, selectionEnd.y );
					glVertex2f( selectionEnd.x, selectionEnd.y );
					glVertex2f( selectionEnd.x, selectionStart.y );
					glVertex2f( selectionStart.x, selectionStart.y );
				glEnd();
			} //}}}
		}
		else
		{
			if( Mouse::IsButtonPressed( sf::Mouse::Left ) )
			{
				selectionStart.x = mousePosition.x - (gWidth / 2.0f);
				selectionStart.y = mousePosition.y - (gHeight / 2.0f);
				selectionTimeStart = mClock->GetElapsedTime().AsSeconds();
				isSelecting = true;
			}
		}

		// add waypoints to selected models
		if( Mouse::IsButtonPressed( sf::Mouse::Right ) ) //{{{
		{
			if(( mousePosition.x > 0 && mousePosition.x < gWidth ) &&
				( mousePosition.y > 0 && mousePosition.y < gHeight ))
			{
				for( auto i = models.begin(); i != models.end(); ++i )
				{
					if( (*i) == NULL )
						continue;

					if( (*i)->isSelected() )
						(*i)->addDestination(
							mousePosition.x - (gWidth / 2.0f),
							mousePosition.y - (gHeight / 2.0f) );
				}
			}
		} //}}}

		for( auto i = models.begin(); i != models.end(); ++i )
		{
			if( (*i) == NULL )
				continue;
			(*i)->update();
			(*i)->render();
		}

		mGame->Display();
	}

} //}}}

Model* createModel( string fileName )
{ //{{{
	Model* rModel = new Model();
	if( fileExists( fileName + ".model" ) )
	{
		if( ! rModel->loadModel( fileName ) )
		{
			cerr << "createModel: " << fileName <<
				".model is corrupt, recompiling" << endl;
			Model::compileModel( fileName );
			if( ! rModel->loadModel( fileName ) )
			{
				cerr << "createModel: model is still bad" << endl;
				return NULL;
			}
		}
	}
	else if( ! rModel->loadModel( fileName ) )
	{
		cerr << "createModel: model is bad" << endl;
		return NULL;
	}
	return rModel;
} //}}}

