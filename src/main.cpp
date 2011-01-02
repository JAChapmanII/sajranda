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

#include <SFML/Graphics.hpp>
using sf::RenderWindow;
using sf::VideoMode;
using sf::Event;
using sf::Color;

#include "model.hpp"

union ldtob
{ //{{{
	long double in;
	char out[ sizeof( long double ) ];
}; //}}}

void makeModel();

int main( int argc, char** argv )
{ //{{{
	static const float gWidth = 960;
	static const float gHeight = 540;
	RenderWindow* mGame =
		new RenderWindow(
			VideoMode( gWidth, gHeight ), "sajranda", sf::Style::Close );
	mGame->SetFramerateLimit( 60 );

	bool done = false;
	Event* mEvent = new Event;
	Model mModel;
	mModel.makeHardCoded( 0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(
		-(gWidth / 2.0f), (gWidth / 2.0f),
		(gHeight / 2.0f), -(gHeight / 2.0f),
		-1, 1 );
	glMatrixMode( GL_MODELVIEW );

	while( mGame->IsOpened() && !done )
	{
		while( mGame->GetEvent( *mEvent ) )
		{
			if( mEvent->Type == Event::Closed )
				mGame->Close();
			if( mEvent->Type == Event::MouseMoved )
			{
				if(( mEvent->MouseMove.X < 0 || mEvent->MouseMove.X > gWidth ) ||
					( mEvent->MouseMove.Y < 0 || mEvent->MouseMove.Y > gHeight ))
					continue;
				sf::Vector2f sCoords =
					mGame->ConvertCoords( mEvent->MouseMove.X, mEvent->MouseMove.Y );
				mModel.setDestination(
					sCoords.x - (gWidth / 2.0f), sCoords.y - (gHeight / 2.0f) );
			}
			if( mEvent->Type == Event::KeyPressed )
			{
				if( mEvent->Key.Code == sf::Key::Escape )
				{
					mGame->Close();
				}
			}
		}

		mGame->Clear( Color::White );

		mModel.update();
		mModel.render();

		mGame->Display();
	}

} //}}}

void makeModel()
{ //{{{
	cout << "Input long double: ";
	long double ld;
	cin >> ld;
	cout << "= " << ld << endl;
	ldtob converter;
	converter.in = ld;

	cout << "Outputting data to \"ld.out\"\n"
		<< "size is " << sizeof( long double ) << " bytes\n";
	ofstream fout( "ld.out", std::ios::binary | std::ios::out );
	fout.write( converter.out, sizeof( long double ) );
	fout.close();
	cout << "Done\n";
} //}}}

