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
#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>

class Model
{
	public:
		struct PolarPoint
		{
			long double theta;
			long double r;

			PolarPoint( long double iTheta, long double iR ) :
				theta( iTheta ),
				r( iR )
			{
			}
		};

		struct RectangularPoint
		{
			long double x;
			long double y;

			RectangularPoint( long double iX, long double iY ) :
				x( iX ),
				y( iY )
			{
			}
		};

		typedef std::vector< PolarPoint* > PointVector;

		Model();

		void render() const;
		void update();

		void setDestination( long double nX, long double nY );

		void buildModel();
		void makeHardCoded( unsigned int number = 0 );

	private:
		// points are stored in polar coordinates
		PointVector points;
		long double theta;
		long double radialVelocity;
		RectangularPoint center;
		RectangularPoint velocity;
		RectangularPoint destination;
		bool isBuilt;
		bool reflect;
};

#endif // MODEL_HPP
