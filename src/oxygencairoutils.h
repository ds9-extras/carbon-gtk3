#ifndef oxygencairoutils_h
#define oxygencairoutils_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
*
* This library is distributed  in the hope that it will be useful,
* but  WITHOUT ANY WARRANTY; without even  the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License  along  with  this library;  if not,  write to  the Free
* Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

#include "oxygencairopattern.h"
#include "oxygenflags.h"
#include "oxygengeometry.h"

#include <cairo.h>
#include <gdk/gdk.h>

namespace Oxygen
{

    namespace ColorUtils
    {
        class Rgba;
    }

    //! draw arc with the parameters similar to those of QPainter::drawArc() (but using diameter instead of width&height). Also, angles are specified in degrees, not in 16ths of degrees
    void cairo_arc_qt( cairo_t*, double, double, double, double, double );

    //!@name color handling
    //@{
    //! add color to pattern
    void cairo_pattern_add_color_stop( cairo_pattern_t*, double x, const ColorUtils::Rgba& );

    //! set source from pattern
    inline void cairo_set_source( cairo_t* context, const Cairo::Pattern& pattern )
    { ::cairo_set_source( context, (cairo_pattern_t*)( pattern ) ); }

    //! set source from color
    void cairo_set_source( cairo_t*, const ColorUtils::Rgba& );

    //@}

    //!@name patterns
    //@{

    inline cairo_pattern_t* cairo_pattern_create_radial( double x, double y, double r )
    { return ::cairo_pattern_create_radial( x, y, 0, x, y, r ); }

    //@}

    //!@name path
    //@{

    enum Corner
    {
        CornersNone = 0,
        CornersTopLeft = 1<<0,
        CornersTopRight = 1<<1,
        CornersBottomLeft = 1<<2,
        CornersBottomRight = 1<<3,
        CornersTop = CornersTopLeft|CornersTopRight,
        CornersBottom = CornersBottomLeft|CornersBottomRight,
        CornersLeft = CornersTopLeft|CornersBottomLeft,
        CornersRight = CornersTopRight|CornersBottomRight,
        CornersAll = CornersTop|CornersBottom
    };

    typedef Flags<Corner> Corners;

    //! rounded rectangle
    void cairo_rounded_rectangle( cairo_t*, double x, double y, double width, double height, double radius, Corners corners = CornersAll );

    //! rounded rectangle
    void cairo_rounded_rectangle_negative( cairo_t*, double x, double y, double width, double height, double radius, Corners corners = CornersAll );

    //! ellipse
    void cairo_ellipse( cairo_t*, double x, double y, double width, double height );

    //! ellipse
    void cairo_ellipse_negative( cairo_t*, double x, double y, double width, double height );

    //! polygon
    void cairo_polygon( cairo_t*, const Polygon& );

    //@}

    //!@name gdk path
    //@{
    //! rounded rectangle
    inline void gdk_cairo_rounded_rectangle( cairo_t* context, GdkRectangle* rect, double radius, Corners corners = CornersAll )
    { cairo_rounded_rectangle( context, rect->x, rect->y, rect->width, rect->height, radius, corners ); }

    //! rounded rectangle
    inline void gdk_cairo_rounded_rectangle_negative( cairo_t* context, GdkRectangle* rect, double radius, Corners corners = CornersAll )
    { cairo_rounded_rectangle_negative( context, rect->x, rect->y, rect->width, rect->height, radius, corners ); }

    //! ellipse
    inline void gdk_cairo_ellipse( cairo_t* context, GdkRectangle* rect )
    { cairo_ellipse( context, rect->x, rect->y, rect->width, rect->height ); }

    //! ellipse
    inline void gdk_cairo_ellipse_negative( cairo_t* context, GdkRectangle* rect )
    { cairo_ellipse_negative( context, rect->x, rect->y, rect->width, rect->height ); }

    //@}

}

#endif
