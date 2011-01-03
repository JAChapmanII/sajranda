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
#include "model.hpp"
using std::string;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <SFML/Window.hpp>

#include <cmath>

#include "util.hpp"
using util::fileExists;
using util::ldtob;

#include <iostream>
using std::cerr;
using std::endl;

static const long double T_PI = 6.2831853071796;
static const long double PI = 3.1415926535898;
static const long double PI_2 = 1.5707963267949;

Model::Model() :
	points(), //{{{
	theta( PI / 4 ),
	radialVelocity( 0.0f ),
	center( 0.0, 0.0 ),
	velocity( 0.0, 0.0 ),
	destination( 0.0, 0.0 ),
	color( 0.0, 0.0, 0.0 ),
	isBuilt( false ),
	reflect( true )
{
} //}}}

void Model::render() const
{ //{{{
	glLineWidth( 1.5f );
	glBegin( GL_LINE_STRIP );
	glColor3f( this->color.r, this->color.g, this->color.b );
	for( PointVector::const_iterator point = this->points.begin();
		point != this->points.end(); ++point )
	{
		glVertex2f(
			(*point)->r * cos( (*point)->theta + this->theta ) + this->center.x,
			(*point)->r * sin( (*point)->theta + this->theta ) + this->center.y );
	}
	if( reflect )
	{
		for( PointVector::const_reverse_iterator point = this->points.rbegin();
			point != this->points.rend(); ++point )
		{
			glVertex2f(
				(*point)->r * cos( -(*point)->theta + this->theta ) + this->center.x,
				(*point)->r * sin( -(*point)->theta + this->theta ) + this->center.y );
		}
	}
	glEnd();
} //}}}

void Model::update()
{ //{{{
	RectangularPoint locDest(
			this->destination.x - this->center.x,
			this->destination.y - this->center.y );
	long double delta = sqrt( locDest.x * locDest.x + locDest.y * locDest.y );
	if( delta < 0.00001 && delta > -0.00001 )
		return;
	long double destinationTheta = acos( locDest.x / delta );
	if( locDest.y < 0 )
		destinationTheta = 2*PI - destinationTheta;
	while( destinationTheta > 2*PI )
		destinationTheta -= 2*PI;
	while( destinationTheta < 0 )
		destinationTheta += 2*PI;


	this->radialVelocity = destinationTheta - this->theta;
	if( this->theta + PI < destinationTheta )
		this->radialVelocity -= T_PI;
	else if( this->theta - PI > destinationTheta )
		this->radialVelocity += T_PI;


	if( this->radialVelocity > 0.02 || this->radialVelocity < -0.02 )
		this->radialVelocity /= 5.0f;

	this->velocity.x = locDest.x / 25.0f;
	this->velocity.y = locDest.y / 25.0f;

	this->theta += this->radialVelocity;
	while( this->theta > 2*PI )
		this->theta -= 2*PI;
	while( this->theta < 0 )
		this->theta += 2*PI;

	this->center.x += this->velocity.x;
	this->center.y += this->velocity.y;
} //}}}

void Model::setDestination( long double iX, long double iY )
{ //{{{
	this->destination.x = iX;
	this->destination.y = iY;
} //}}}

void Model::buildModel()
{ //{{{
} //}}}

bool Model::compileModel( string fileName )
{ //{{{
	if( !fileExists( fileName + ".in" ) )
	{
		cerr << "In file " << fileName << " does not exist" << endl;
		return false;
	}

	ifstream inFile( (fileName + ".in").c_str() );
	ofstream outFile( (fileName + ".model").c_str(), std::ios::binary );

	char header = 0x00;
	string firstWord;
	inFile >> firstWord;
	if( firstWord[ 0 ] == 'y' || firstWord[ 0 ] == 'Y' )
	{
		header |= 0x80;
	}
	else if( firstWord[ 0 ] != 'n' && firstWord[ 0 ] != 'N' )
	{
		cerr << "The first character of " << fileName << ".in is invalid" << endl;
		inFile.close();
		outFile.close();
		return false;
	}

	int totalPoints = 0;
	inFile >> totalPoints;
	if( !inFile.good() )
	{
		cerr << "Could not read number of points from " << fileName << ".in" << endl;
		inFile.close();
		outFile.close();
		return false;
	}
	totalPoints %= 128;

	header |= totalPoints;
	outFile.write( &header, 1 );

	long double r = 0, g = 0, b = 0;
	inFile >> r;
	inFile >> g;
	inFile >> b;
	if( !inFile.good() )
	{
		cerr << "Could not read color from " << fileName << ".in" << endl;
		inFile.close();
		outFile.close();
		return false;
	}

	ldtob converter;

	converter.in = r;
	outFile.write( converter.out, sizeof( long double ) );
	converter.in = g;
	outFile.write( converter.out, sizeof( long double ) );
	converter.in = b;
	outFile.write( converter.out, sizeof( long double ) );

	long double theta = 0;
	for( int i = 0; i < totalPoints; ++i )
	{
		inFile >> r; inFile >> theta;
		if( !inFile.good() )
		{
			cerr << "Error reading " << fileName << ".in on point " << i << endl;
			inFile.close();
			outFile.close();
			return false;
		}

		converter.in = r;
		outFile.write( converter.out, sizeof( long double ) );

		converter.in = theta;
		outFile.write( converter.out, sizeof( long double ) );
	}

	inFile.close();
	outFile.close();
	return true;
} //}}}

bool Model::loadModel( string fileName )
{ //{{{
	if( !fileExists( fileName + ".model" ) )
	{
		cerr << "Model " << fileName << " does not exist, attempting compiling" << endl;
		if( ! Model::compileModel( fileName ) )
			return false;
	}


	ifstream inFile( (fileName + ".model").c_str(), std::ios::binary );
	char header = 0;
	inFile.read( &header, 1 );

	if( header & 0x80 )
		this->reflect = true;
	else
		this->reflect = false;

	int totalPoints = header & 0x7f;
	this->points.clear();
	this->points.reserve( totalPoints );

	ldtob converter;
	inFile.read( converter.out, sizeof( long double ) );
	this->color.r = converter.in;
	inFile.read( converter.out, sizeof( long double ) );
	this->color.g = converter.in;
	inFile.read( converter.out, sizeof( long double ) );
	this->color.b = converter.in;

	if( !inFile.good() )
	{
		cerr << "Colud not load color from " << fileName << ".model" << endl;
		inFile.close();
		return false;
	}

	long double r = 0, ttheta = 0;
	static const long double radiusM = 15.0f;
	for( int i = 0; i < totalPoints; ++i )
	{
		inFile.read( converter.out, sizeof( long double ) );
		ttheta = converter.in;
		inFile.read( converter.out, sizeof( long double ) );
		r = converter.in * radiusM;

		if( !inFile.good() )
		{
			cerr << "Error reading " << fileName << ".model on point " << i << endl;
			inFile.close();
			return false;
		}

		this->points.push_back( new PolarPoint( ttheta * PI, r ) );
	}

	return true;
} //}}}

