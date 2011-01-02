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
//using Model::PolarPoint;
//using Model::RectangularPoint;

// gives access to opengl commands
#include <SFML/Window.hpp>

#include <cmath>

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
	isBuilt( false ),
	reflect( true )
{
} //}}}

void Model::render() const
{ //{{{
	glLineWidth( 1.5f );
	glBegin( GL_LINE_STRIP );
	glColor3f( 1.000f, 0.086f, 0.086f );
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
{
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
}

void Model::setDestination( long double iX, long double iY )
{ //{{{
	this->destination.x = iX;
	this->destination.y = iY;
} //}}}

void Model::buildModel()
{
}

void Model::makeHardCoded( unsigned int number )
{ //{{{
	this->points.clear();
	static const long double radiusM = 30.0;

	switch( number )
	{
		case 0:
		{
			this->points.push_back( new PolarPoint( 0, 2.0 * radiusM ) );
			this->points.push_back( new PolarPoint( PI / 2, 1.0 * radiusM ) );
			this->points.push_back( new PolarPoint( 7 * PI / 8, 1.5 * radiusM ) );
			break;
		}
		case 1:
		{
			this->points.push_back( new PolarPoint(  0, 2.0 * radiusM ) );
			this->points.push_back( new PolarPoint(  5 * PI / 4, 1.0 * radiusM ) );
			this->points.push_back( new PolarPoint( -1 * PI / 3, 1.0 * radiusM ) );
			this->points.push_back( new PolarPoint( -3 * PI / 5, 1.5 * radiusM ) );
			this->points.push_back( new PolarPoint( -7 * PI / 8, 1.5 * radiusM ) );
			break;
		}
		case 2:
		{
			this->points.push_back( new PolarPoint( 0.0, 2.0 * radiusM ) );
			this->points.push_back( new PolarPoint( PI / 12, 13 * radiusM / 8 ) );
			this->points.push_back( new PolarPoint( PI_2, 2 * radiusM / 8 ) );
			this->points.push_back( new PolarPoint( 5 * PI / 24, 14 * radiusM / 8 ) );
			this->points.push_back( new PolarPoint( 9 * PI / 24, 14 * radiusM / 8 ) );
			this->points.push_back( new PolarPoint( 15 * PI / 24, 14 * radiusM / 8 ) );
			this->points.push_back( new PolarPoint( 19 * PI / 24, 14 * radiusM / 8 ) );
			this->points.push_back( new PolarPoint( 22 * PI / 24, 13 * radiusM / 8 ) );
			break;
		}
		default:
			break;
	}
} //}}}

