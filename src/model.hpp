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
#include <string>

class Model
{
	public:
		struct PolarPoint
		{ //{{{
			long double theta;
			long double r;

			PolarPoint( long double iTheta, long double iR ) :
				theta( iTheta ),
				r( iR )
			{
			}
		}; //}}}

		struct RectangularPoint
		{ //{{{
			long double x;
			long double y;

			RectangularPoint( long double iX, long double iY ) :
				x( iX ),
				y( iY )
			{
			}
		}; //}}}

		struct ColorPoint
		{ //{{{
			long double r;
			long double g;
			long double b;

			ColorPoint( long double iR, long double iG, long double iB ) :
				r( iR ),
				g( iG ),
				b( iB )
			{
			}
		}; //}}}

		typedef std::vector< PolarPoint* > PointVector;
		typedef std::vector< RectangularPoint* > PlaceVector;

		// create an empty model
		Model();

		// renders this model
		void render() const;

		// updates this point's position and theta
		void update();

		// sets the destination of this model to a new point
		void setDestination( long double nX, long double nY );

		// adds a destination to this model's queue
		void addDestination( long double nX, long double nY );

		// clears all of this model's destinations
		void clearDestination();

		// returns this model's center x
		long double centerX() const;

		// returns this model's center y
		long double centerY() const;

		// sets this model's center
		void setCenter( RectangularPoint nCenter );

		// selects this model
		void select();

		// unselects this model
		void unselect();

		// returns the value of this->isSelected
		bool isSelected() const;

		// returns the minum radius value of any of this's points
		long double getMinimumRadius() const;

		// builds the display list for this model
		void buildModel();

		// compile a .in file into a .model file
		static bool compileModel( std::string fileName );

		// load a .model file into this, compiling if necessary
		bool loadModel( std::string fileName );

	private:
		// points are stored in polar coordinates
		PointVector points;
		long double theta;
		long double radialVelocity;
		PlaceVector place;
		RectangularPoint velocity;
		ColorPoint color;
		bool selected;
		bool built;
		bool reflect;
		long double minimumRadius;
};

#endif // MODEL_HPP
