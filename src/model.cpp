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
	place(),
	velocity( 0.0, 0.0 ),
	color( 0.0, 0.0, 0.0 ),
	selected( false ),
	built( false ),
	reflect( true )
{
	this->place.push_back( new RectangularPoint( 0.0f, 0.0f ) );
} //}}}

void Model::render() const
{ //{{{
	if( this->selected && this->place.size() > 1 )
	{ //{{{
		unsigned int start = 0;
		long double delta = 0;

		while( delta < 10 && start < this->place.size() )
		{
			++start;
			RectangularPoint locDest(
				this->place[ start ]->x - this->place[ 0 ]->x,
				this->place[ start ]->y - this->place[ 0 ]->y );
			delta = sqrt( locDest.x * locDest.x + locDest.y * locDest.y );
		}
		if( start != place.size() )
		{
			glColor3f( 0.0, 0.0, 0.0 );
			glLineWidth( 0.5f );
			glBegin( GL_LINE_STRIP );
			if( start == 1 )
				glVertex2f( this->place[ 0 ]->x, this->place[ 0 ]->y );
			for( unsigned int i = start; i < this->place.size(); ++i )
				glVertex2f( this->place[ i ]->x, this->place[ i ]->y );
			glEnd();
		}
	} //}}}

	// draw a selection line
	if( this->selected ) //{{{
	{
		glLineWidth( 3.0f );
		glBegin( GL_LINE_STRIP );
		glColor3f( this->color.r * 0.6, this->color.g * 0.6, this->color.b * 0.6 );
		for( PointVector::const_iterator point = this->points.begin();
			point != this->points.end(); ++point )
		{
			glVertex2f(
				0.9f * (*point)->r * cos( (*point)->theta + this->theta ) + this->place[ 0 ]->x,
				0.9f * (*point)->r * sin( (*point)->theta + this->theta ) + this->place[ 0 ]->y );
		}
		if( reflect )
		{
			for( PointVector::const_reverse_iterator point = this->points.rbegin();
				point != this->points.rend(); ++point )
			{
				glVertex2f(
					0.9f * (*point)->r * cos( this->theta - (*point)->theta ) + this->place[ 0 ]->x,
					0.9f * (*point)->r * sin( this->theta - (*point)->theta ) + this->place[ 0 ]->y );
			}
		}
		glEnd();

		glLineWidth( 3.0f );
		glBegin( GL_LINE_STRIP );
		glColor3f( this->color.r * 0.6, this->color.g * 0.6, this->color.b * 0.6 );
		for( PointVector::const_iterator point = this->points.begin();
			point != this->points.end(); ++point )
		{
			glVertex2f(
				1.1f * (*point)->r * cos( (*point)->theta + this->theta ) + this->place[ 0 ]->x,
				1.1f * (*point)->r * sin( (*point)->theta + this->theta ) + this->place[ 0 ]->y );
		}
		if( reflect )
		{
			for( PointVector::const_reverse_iterator point = this->points.rbegin();
				point != this->points.rend(); ++point )
			{
				glVertex2f(
					1.1f * (*point)->r * cos( this->theta - (*point)->theta ) + this->place[ 0 ]->x,
					1.1f * (*point)->r * sin( this->theta - (*point)->theta ) + this->place[ 0 ]->y );
			}
		}
		glEnd();
	} //}}}

	glLineWidth( 1.5f );
	glBegin( GL_LINE_STRIP );
	glColor3f( this->color.r, this->color.g, this->color.b );
	for( PointVector::const_iterator point = this->points.begin();
		point != this->points.end(); ++point )
	{
		glVertex2f(
			(*point)->r * cos( (*point)->theta + this->theta ) + this->place[ 0 ]->x,
			(*point)->r * sin( (*point)->theta + this->theta ) + this->place[ 0 ]->y );
	}
	if( reflect )
	{
		for( PointVector::const_reverse_iterator point = this->points.rbegin();
			point != this->points.rend(); ++point )
		{
			glVertex2f(
				(*point)->r * cos( this->theta - (*point)->theta ) + this->place[ 0 ]->x,
				(*point)->r * sin( this->theta - (*point)->theta ) + this->place[ 0 ]->y );
		}
	}
	glEnd();
} //}}}

void Model::update()
{ //{{{
	if( this->place.size() < 2 )
		return;

	this->place[ 0 ]->x += this->velocity.x;
	this->place[ 0 ]->y += this->velocity.y;

	RectangularPoint locDest(
			this->place[ 1 ]->x - this->place[ 0 ]->x,
			this->place[ 1 ]->y - this->place[ 0 ]->y );
	long double delta = sqrt( locDest.x * locDest.x + locDest.y * locDest.y );
	// if we've gotten to the waypoint, delete it
	if( delta < 10 && delta > -10 )
	{
		delete this->place[ 1 ];
		this->place.erase( ++this->place.begin() );
		this->velocity.x = 0; this->velocity.y = 0;
		// update again incase there is another one
		this->update();
	}

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
	// double the speed when within 25 of destination, and has a destination
	// afterwards. This allows for smoothing animation when there are multiple
	// way-points that this model must visit
	if( this->place.size() > 2 && delta < 25 )
	{
		this->velocity.x *= 2.0f;
		this->velocity.y *= 2.0f;
	}

	this->theta += this->radialVelocity;
	while( this->theta > 2*PI )
		this->theta -= 2*PI;
	while( this->theta < 0 )
		this->theta += 2*PI;
} //}}}

void Model::setDestination( long double nX, long double nY )
{ //{{{
	this->clearDestination();
	this->place.push_back( new RectangularPoint( nX, nY ) );
} //}}}

void Model::addDestination( long double nX, long double nY )
{ //{{{
	this->place.push_back( new RectangularPoint( nX, nY ) );
} //}}}

void Model::clearDestination()
{ //{{{
	// don't delete the current place
	while( this->place.size() > 1 )
	{
		delete this->place[ this->place.size() - 1 ];
		this->place.pop_back();
	}
} //}}}

long double Model::centerX() const
{ //{{{
	return this->place[ 0 ]->x;
} //}}}

long double Model::centerY() const
{ //{{{
	return this->place[ 0 ]->y;
} //}}}

void Model::setCenter( RectangularPoint nCenter )
{ //{{{
	this->place[ 0 ]->x = nCenter.x;
	this->place[ 0 ]->y = nCenter.y;
} //}}}

void Model::select()
{ //{{{
	this->selected = true;
} //}}}

void Model::unselect()
{ //{{{
	this->selected = false;
} //}}}

bool Model::isSelected() const
{ //{{{
	return this->selected;
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

