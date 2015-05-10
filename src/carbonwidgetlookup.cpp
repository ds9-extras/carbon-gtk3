/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* Hook-setup code provided by Ruslan
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

#include "carbonwidgetlookup.h"
#include "config.h"

#include <iostream>
#include <cairo/cairo-gobject.h>

namespace Carbon
{

    //__________________________________________________________________
    WidgetLookup::WidgetLookup( void ):
        _hooksInitialized( false ),
        _context( 0L ),
        _widget( 0L )
    {}


    //_____________________________________________________
    WidgetLookup::~WidgetLookup( void )
    {

        // disconnect hooks
        _drawHook.disconnect();

    }

    //_____________________________________________________
    void WidgetLookup::initializeHooks( void )
    {

        #if CARBON_DEBUG
        std::cerr << "Carbon::WidgetLookup::initializeHooks" << std::endl;
        #endif

        if( _hooksInitialized ) return;

        // install hook and test
        if( !_drawHook.connect( "draw", (GSignalEmissionHook)drawHook, this ) ) return;

        // set initialization flag
        _hooksInitialized = true;

        return;

    }

    //_____________________________________________________
    GtkWidget* WidgetLookup::find( cairo_t* context, const GtkWidgetPath* path ) const
    {

        // check path
        if( !path ) return 0L;

        // get length and check
        const gint length( gtk_widget_path_length( path ) );
        if( length < 1 ) return 0L;

        // lookup last type
        return find( context, gtk_widget_path_iter_get_object_type( path, length-1 ) );

    }

    //_____________________________________________________
    GtkWidget* WidgetLookup::find( cairo_t* context, GType type ) const
    {
        // check context
        if( context != _context )
        {

            if( GTK_IS_WIDGET( _widget ) && G_OBJECT_TYPE( _widget ) == type && GTK_IS_SCROLLED_WINDOW( gtk_widget_get_parent( _widget ) ) )
            {

                // FIXME: this is very fragile
                // in latest gtk3 versions, scrolled window children do not get the right context passed when drawn, with respect to what is
                // sent by the "draw" signal. Hence the context does not match.
                // Assuming this is the case here, we return the latest widget nonetheless
                return _widget;

            } else {

                #if CARBON_DEBUG
                std::cerr
                    << "Carbon::WidgetLookup::find -"
                    << " context: " << context
                    << " type: " << g_type_name( type )
                    << " invalid context."
                    << std::endl;
                #endif
                return 0L;

            }

        }

        #if CARBON_DEBUG
        std::cerr
            << "Carbon::WidgetLookup::find -"
            << " context: " << context
            << " type: " << g_type_name( type )
            << std::endl;
        #endif

        // look for type in stored widgets
        /* we loop backward, since last added widgets are more likely to be looked up */
        for( WidgetList::const_reverse_iterator iter = _widgets.rbegin(); iter != _widgets.rend(); ++iter )
        {
            // compare types and return if matched
            if( G_OBJECT_TYPE( *iter ) == type )
            {
                #if CARBON_DEBUG
                std::cerr
                    << "Carbon::WidgetLookup::find -"
                    << " context: " << context
                    << " type: " << g_type_name( type )
                    << " found: " << *iter
                    << std::endl;
                #endif
                return *iter;
            }
        }

        #if CARBON_DEBUG
        std::cerr
            << "Carbon::WidgetLookup::find -"
            << " context: " << context
            << " type: " << g_type_name( type )
            << " - no match found"
            << std::endl;
        #endif

        return 0L;

    }

    //_____________________________________________________
    void WidgetLookup::bind( GtkWidget* widget, cairo_t* context )
    {
        // check if context has changed and clear widgets if yes
        if( context != _context )
        {

            #if CARBON_DEBUG
            std::cerr
                << "Carbon::WidgetLookup::bind -"
                << " context: " << _context
                << " widgets: " << _widgets.size()
                << std::endl;
            #endif

            _context = context;
            _widgets.clear();
        }

        _widgets.push_back( widget );
        _widget = widget;

        // add to all widgets map
        if( _allWidgets.find( widget ) == _allWidgets.end() )
        {
            Signal destroyId;
            destroyId.connect( G_OBJECT( widget ), "destroy", G_CALLBACK( destroyNotifyEvent ), this );
            _allWidgets.insert( std::make_pair( widget, destroyId ) );
        }

    }

    //____________________________________________________________________________________________
    void WidgetLookup::unregisterWidget( GtkWidget* widget )
    {

        #if CARBON_DEBUG
        std::cerr << "Carbon::WidgetLookup::unregisterWidget - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        // find in map
        WidgetMap::iterator iter( _allWidgets.find( widget ) );
        assert( iter != _allWidgets.end() );

        // disconnect signal
        iter->second.disconnect();

        // erase from lists
        _allWidgets.erase( widget );
        _widgets.remove( widget );

        // also clean direct widget
        if( _widget == widget ) { _widget = 0L; }

    }

    //_____________________________________________________
    gboolean WidgetLookup::drawHook( GSignalInvocationHint*, guint numParams, const GValue* params, gpointer data )
    {

        // check number of parameters
        if( numParams < 2 ) return FALSE;

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // check second parameter type.
        if( !G_VALUE_HOLDS( params+1, CAIRO_GOBJECT_TYPE_CONTEXT ) )
        { return FALSE; }

        // retrieve context and cast
        cairo_t* context( static_cast<cairo_t*>( g_value_get_boxed(params+1) ) );

        #if CARBON_DEBUG
        std::cerr
            << "Carbon::WidgetLookup::drawHook -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " context: " << context
            << std::endl;
        #endif

        // bind widget and context
        static_cast<WidgetLookup*>( data )->bind( widget, context );

        return TRUE;

    }


    //____________________________________________________________________________________________
    gboolean WidgetLookup::destroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<WidgetLookup*>(data)->unregisterWidget( widget );
        return FALSE;
    }

}
