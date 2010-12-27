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

#include <fstream>
using std::ofstream;

#include <SFML/Graphics.hpp>
using sf::RenderWindow;
using sf::VideoMode;
using sf::Event;
using sf::Color;

union ldtob
{ //{{{
	long double in;
	char out[ sizeof( long double ) ];
}; //}}}

void makeModel();

int main( int argc, char** argv )
{ //{{{
	RenderWindow* mGame =
		new RenderWindow( VideoMode( 800, 600 ), "sajranda", sf::Style::Close );

	bool done = false;
	Event* mEvent = new Event;
	while( mGame->IsOpened() && !done )
	{
		while( mGame->GetEvent( *mEvent ) )
		{
			if( mEvent->Type == Event::Closed )
				mGame->Close();
		}

		mGame->Clear( Color::White );
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

