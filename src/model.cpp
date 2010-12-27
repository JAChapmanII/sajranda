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

#include <GL/gl.h>
#include <cmath>

static const long double PI = 3.1415926535898;

Model::Model() :
	points(),
	theta( 0.0f ),
	isBuilt( false )
{
}

void Model::render() const
{
	glLineWidth( 2.0f );
	glBegin( GL_LINE_STRIP );
	glColor3f( 0.0f, 0.0f, 0.0f );
	for( Model::PointVector::const_iterator point = this->points.begin();
		point != this->points.end(); ++point )
	{
		glVertex3f(
				10.0f * (*point)->r * cos( (*point)->theta + this->theta ),
				10.0f * (*point)->r * sin( (*point)->theta + this->theta ), 0.0f );
	}
	glEnd();
}

void Model::buildModel()
{
}

void Model::makeOne()
{
	this->points.clear();
	this->points.push_back( new Model::PolarPoint( 0, 2.0 ) );
	this->points.push_back( new Model::PolarPoint( PI / 2, 1.0 ) );
	this->points.push_back( new Model::PolarPoint( 7 * PI / 8, 1.5 ) );
	this->points.push_back( new Model::PolarPoint( 9 * PI / 8, 1.5 ) );
	this->points.push_back( new Model::PolarPoint( 3 * PI / 2, 1.0 ) );
	this->points.push_back( new Model::PolarPoint( 0, 2.0 ) );
}

