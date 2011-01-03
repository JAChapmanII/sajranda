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

#include <SFML/Graphics.hpp>
using sf::RenderWindow;
using sf::VideoMode;
using sf::Event;
using sf::Color;
using sf::Input;

#include "model.hpp"

#include "util.hpp"
using util::fileExists;

int main( int argc, char** argv )
{ //{{{
	static const float gWidth = 960;
	static const float gHeight = 540;
	RenderWindow* mGame =
		new RenderWindow(
			VideoMode( gWidth, gHeight ), "sajranda", sf::Style::Close );
	mGame->SetFramerateLimit( 60 );
	const Input* mInput = &(mGame->GetInput());

	Event* mEvent = new Event;
	Model mModel;
	string fileName = "dat/001";
	if( fileExists( fileName + ".model" ) )
	{
		if( ! mModel.loadModel( fileName ) )
		{
			cerr << "It appears " << fileName << ".model is corrupt, recompiling" << endl;
			Model::compileModel( fileName );
			if( ! mModel.loadModel( fileName ) )
			{
				cerr << "Model is still bad" << endl;
				return 1;
			}
		}
	}
	else if( ! mModel.loadModel( fileName ) )
	{
		cerr << "Model is bad" << endl;
		return -1;
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(
		-(gWidth / 2.0f), (gWidth / 2.0f),
		(gHeight / 2.0f), -(gHeight / 2.0f),
		-1, 1 );
	glMatrixMode( GL_MODELVIEW );

	while( mGame->IsOpened() )
	{
		while( mGame->GetEvent( *mEvent ) )
		{
			if( mEvent->Type == Event::Closed )
				mGame->Close();

			/*
			if( mEvent->Type == Event::MouseButtonPressed )
			{
				if(( mEvent->MouseButton.X < 0 || mEvent->MouseButton.X > gWidth ) ||
					( mEvent->MouseButton.Y < 0 || mEvent->MouseButton.Y > gHeight ))
					continue;
				if( mEvent->MouseButton.Button == sf::Mouse::Right )
				{
					sf::Vector2f sCoords = mGame->ConvertCoords(
							mEvent->MouseButton.X, mEvent->MouseButton.Y );
					mModel.addDestination(
						sCoords.x - (gWidth / 2.0f), sCoords.y - (gHeight / 2.0f) );
				}
			}
			*/

			if( mEvent->Type == Event::KeyPressed )
			{
				if( mEvent->Key.Code == sf::Key::Escape )
				{
					mGame->Close();
				}
			}
		}
		if( !mGame->IsOpened() )
			break;

		if( mInput->IsMouseButtonDown( sf::Mouse::Right ) )
		{
			if(( mInput->GetMouseX() > 0 && mInput->GetMouseX() < gWidth ) &&
				( mInput->GetMouseY() > 0 && mInput->GetMouseY() < gHeight ))
			{
				mModel.addDestination(
					mInput->GetMouseX() - (gWidth / 2.0f),
					mInput->GetMouseY() - (gHeight / 2.0f) );
			}
		}


		mGame->Clear( Color::White );

		mModel.update();
		mModel.render();

		mGame->Display();
	}

} //}}}

