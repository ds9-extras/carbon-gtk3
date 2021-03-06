#ifndef carbonstyleoptions_h
#define carbonstyleoptions_h

/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "carbonflags.h"
#include "carbongtkutils.h"
#include "carbonpalette.h"

#include <iostream>
#include <gtk/gtk.h>

namespace Carbon
{

    //! internal option flags to pass arguments around
    enum StyleOption
    {
        Blend = 1<<0,
        Sunken = 1<<1,
        Active = 1<<2,
        Flat = 1<<3,
        Focus = 1<<4,
        Hover = 1<<5,
        NoFill = 1<<6,
        Vertical = 1<<7,
        Alpha = 1<<8,
        Round = 1<<9,
        Contrast = 1<<10,
        Selected = 1<<11,
        Disabled = 1<<12,
        Menu = 1<<13
    };

    class StyleOptions: public Flags<StyleOption>
    {

        public:

        //! constructor
        StyleOptions( void )
        {}

        //! constructor
        StyleOptions(StyleOption f):
            Flags<StyleOption>( f )
        {}

        //! constructor
        StyleOptions( Flags<StyleOption> f):
            Flags<StyleOption>( f )
        {}

        //! constructor from state flags
        StyleOptions( GtkStateFlags flags )
        {

            if( flags & GTK_STATE_FLAG_INSENSITIVE ) (*this) |= Disabled;
            if( flags & GTK_STATE_FLAG_PRELIGHT ) (*this) |= Hover;
            if( flags & GTK_STATE_FLAG_SELECTED ) (*this) |= (Selected|Active);
            if( flags & GTK_STATE_FLAG_ACTIVE ) (*this) |= Sunken;

            #if GTK_CHECK_VERSION( 3, 13, 7 )
            if( flags & GTK_STATE_FLAG_CHECKED ) (*this) |= Sunken;
            #endif

            // TODO: check whether one should use this, or gtk_widget_has_focus
            if( flags & GTK_STATE_FLAG_FOCUSED ) (*this) |= Focus;

        }

        //! constructor from widget and state flags
        StyleOptions( GtkWidget* widget, GtkStateFlags flags )
        {

            if( flags & GTK_STATE_FLAG_INSENSITIVE ) (*this) |= Disabled;
            if( flags & GTK_STATE_FLAG_PRELIGHT ) (*this) |= Hover;
            if( flags & GTK_STATE_FLAG_SELECTED ) (*this) |= (Selected|Active);
            if( flags & GTK_STATE_FLAG_ACTIVE ) (*this) |= Sunken;

            #if GTK_CHECK_VERSION( 3, 13, 7 )
            if( flags & GTK_STATE_FLAG_CHECKED ) (*this) |= Sunken;
            #endif

            if( flags & GTK_STATE_FLAG_FOCUSED ) (*this) |= Focus;
            else if( GTK_IS_WIDGET( widget ) && gtk_widget_has_focus(widget) ) (*this)|=Focus;
        }

        //! constructor from widget with GtkStateType state
        StyleOptions( GtkWidget* widget, GtkStateType state )
        {
            if( state == GTK_STATE_INSENSITIVE ) (*this) |= Disabled;
            else if( state == GTK_STATE_PRELIGHT ) (*this) |= Hover;
            else if( state == GTK_STATE_SELECTED ) (*this) |= Selected;
            else if( state == GTK_STATE_ACTIVE ) (*this) |= Active;

            if( GTK_IS_WIDGET( widget ) && gtk_widget_has_focus(widget) ) (*this)|=Focus;
        }


        //! destructor
        virtual ~StyleOptions( void )
        {}


        //! color set
        /*! it is used to pass custom colors to painting routines */
        Palette::ColorSet _customColors;

        //! streamer
        friend std::ostream& operator << (std::ostream& out, const StyleOptions& options );

    };

}

OX_DECLARE_OPERATORS_FOR_FLAGS( Carbon::StyleOptions )

#endif
