/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include <gtk/gtk.h>
#include "carboninnershadowdata.h"
#include "../carbongtkutils.h"
#include "../config.h"
#include "../carboncairocontext.h"
#include "../carboncairoutils.h"
#include "carbonanimations.h"
#include "../carbonstyle.h"
#include "../carbonmetrics.h"
#include <stdlib.h>

#include <cassert>
#include <iostream>

namespace Carbon
{

    //_____________________________________________
    void InnerShadowData::connect( GtkWidget* widget )
    {

        assert( GTK_IS_SCROLLED_WINDOW( widget ) );
        assert( !_target );

        // store target
        _target = widget;

        if( gdk_display_supports_composite( gtk_widget_get_display( widget ) ) )
        { _exposeId.connect( G_OBJECT(_target), "draw", G_CALLBACK( targetExposeEvent ), this, true ); }

        // check child
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
        if( !child ) return;

        #if CARBON_DEBUG
        std::cerr
            << "Carbon::InnerShadowData::connect -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " child: " << child << " (" << G_OBJECT_TYPE_NAME( child ) << ")"
            << std::endl;
        #endif

        registerChild( child );

    }

    //_____________________________________________
    void InnerShadowData::disconnect( GtkWidget* )
    {
        _target = 0;
        for( ChildDataMap::reverse_iterator iter = _childrenData.rbegin(); iter != _childrenData.rend(); ++iter )
        { iter->second.disconnect( iter->first ); }

        // disconnect signals
        _exposeId.disconnect();

        // clear child data
        _childrenData.clear();
    }

    //_____________________________________________
    void InnerShadowData::registerChild( GtkWidget* widget )
    {

        #if ENABLE_INNER_SHADOWS_HACK

        // check widget
        if( !GTK_IS_WIDGET( widget ) ) return;

        // make sure widget is not already in map
        if( _childrenData.find( widget ) != _childrenData.end() ) return;

        if( gtk_scrolled_window_get_shadow_type( GTK_SCROLLED_WINDOW( _target ) ) != GTK_SHADOW_IN )
        { return; }

        #if CARBON_DEBUG
        std::cerr
            << "Carbon::InnerShadowData::registerChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        GdkWindow* window(gtk_widget_get_window(widget));
        if(
            window && gdk_window_get_window_type( window ) == GDK_WINDOW_CHILD &&
            gdk_display_supports_composite( gtk_widget_get_display( widget ) ) )
        {
            ChildData data;
            data._unrealizeId.connect( G_OBJECT(widget), "unrealize", G_CALLBACK( childUnrealizeNotifyEvent ), this );
            data._initiallyComposited = gdk_window_get_composited(window);
            gdk_window_set_composited(window,TRUE);
            _childrenData.insert( std::make_pair( widget, data ) );
        }

        #endif

    }

    //________________________________________________________________________________
    void InnerShadowData::unregisterChild( GtkWidget* widget )
    {
        #if ENABLE_INNER_SHADOWS_HACK

        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter == _childrenData.end() ) return;

        #if CARBON_DEBUG
        std::cerr
            << "Carbon::InnerShadowData::unregisterChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        iter->second.disconnect( widget );
        _childrenData.erase( iter );

        #endif
    }

    //________________________________________________________________________________
    void InnerShadowData::ChildData::disconnect( GtkWidget* widget )
    {
        #if ENABLE_INNER_SHADOWS_HACK

        // disconnect signals
        _unrealizeId.disconnect();

        // remove compositing flag
        GdkWindow* window( gtk_widget_get_window( widget ) );

        #if CARBON_DEBUG
        std::cerr
            << "Carbon::InnerShadowData::ChildData::disconnect -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " window: " << window
            << std::endl;
        #endif

        // restore compositing if different from initial state
        if( GDK_IS_WINDOW( window ) && !gdk_window_is_destroyed(window) && gdk_window_get_composited( window ) != _initiallyComposited )
        { gdk_window_set_composited( window, _initiallyComposited ); }

        #endif
    }

    //____________________________________________________________________________________________
    gboolean InnerShadowData::childUnrealizeNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if CARBON_DEBUG
        std::cerr
            << "Carbon::InnerShadowData::childUnrealizeNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif
        static_cast<InnerShadowData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //_________________________________________________________________________________________
    gboolean InnerShadowData::targetExposeEvent( GtkWidget* widget, cairo_t *context, gpointer )
    {

        #if ENABLE_INNER_SHADOWS_HACK
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
        GdkWindow* childWindow( gtk_widget_get_window( child ) );

        #if CARBON_DEBUG
        std::cerr << "Carbon::InnerShadowData::targetExposeEvent -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME(widget) << ")"
            << " child: " << child << " (" << G_OBJECT_TYPE_NAME(child) << ")"
            << " path: " << Gtk::gtk_widget_path( child )
            << std::endl;
        #endif

        if( !gdk_window_get_composited( childWindow ) )
        {
            #if CARBON_DEBUG
            std::cerr << "Carbon::InnerShadowData::targetExposeEvent - Window isn't composite. Doing nohing\n";
            #endif
            return FALSE;
        }

        // make sure the child window doesn't contain garbage
        gdk_window_process_updates( childWindow, TRUE );

        GtkAllocation allocation;
        gtk_widget_translate_coordinates (child, widget, 0, 0, &allocation.x, &allocation.y);
        allocation.width = gdk_window_get_width (childWindow);
        allocation.height = gdk_window_get_height (childWindow);

        // draw child
        gdk_cairo_rectangle( context, &allocation );
        cairo_clip(context);
        gdk_cairo_set_source_window( context, childWindow, allocation.x, allocation.y );
        cairo_paint(context);

        #if CARBON_DEBUG_INNERSHADOWS
        // Show updated parts in random color
        cairo_rectangle( context, allocation.x, allocation.y, allocation.width, allocation.height );
        double red= ((double)rand())/RAND_MAX;
        double green=((double)rand())/RAND_MAX;
        double blue=((double)rand())/RAND_MAX;
        cairo_set_source_rgba(context,red,green,blue,0.5);
        cairo_fill(context);
        #endif

        // Render rounded combobox list child
        if(Gtk::gtk_combobox_is_tree_view( child ))
        {
            StyleOptions options( widget, gtk_widget_get_state_flags( widget ) );
            Corners corners(CornersAll);

            // check scrollbar visibility to decide corners
            GtkScrolledWindow* scrolledWindow( GTK_SCROLLED_WINDOW( widget ) );
            if( gtk_widget_get_mapped( gtk_scrolled_window_get_vscrollbar( scrolledWindow ) ) )
            {
                if(Gtk::gtk_widget_layout_is_reversed( widget )) corners &= ~CornersLeft;
                else corners &= ~CornersRight;
            }

            if( gtk_widget_get_mapped( gtk_scrolled_window_get_hscrollbar( scrolledWindow ) ) )
            { corners &= ~CornersBottom; }

            int x(allocation.x),y(allocation.y),w(allocation.width),h(allocation.height);
            cairo_rectangle(context,x,y,w,h);
            if(!Gtk::gdk_default_screen_is_composited())
            {
                // Take ugly shadow into account
                x+=1;
                y+=1;
                w-=2;
                h-=2;
            }
            cairo_rounded_rectangle_negative(context,x,y,w,h,2,corners);
            cairo_clip(context);

            Style::instance().renderMenuBackground( context, allocation.x,allocation.y,allocation.width,allocation.height, options );

            // Event handling finished, now let the event propagate
            return FALSE;
        }

        // we only draw SHADOW_IN here
        if( gtk_scrolled_window_get_shadow_type( GTK_SCROLLED_WINDOW( widget ) ) != GTK_SHADOW_IN )
        {
            #if CARBON_DEBUG
            std::cerr << "Carbon::InnerShadowData::targetExposeEvent - Shadow type isn't GTK_SHADOW_IN, so not drawing the shadow in expose-event handler\n";
            #endif
            return FALSE;

        }

        // draw the shadow
        int basicOffset=2;

        StyleOptions options( widget, gtk_widget_get_state_flags( widget ) );
        options|=NoFill;
        options &= ~(Hover|Focus);
        if( Style::instance().animations().scrolledWindowEngine().contains( widget ) )
        {
            if( Style::instance().animations().scrolledWindowEngine().focused( widget ) ) options |= Focus;
            if( Style::instance().animations().scrolledWindowEngine().hovered( widget ) ) options |= Hover;
        }

        const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover|AnimationFocus, AnimationFocus ) );

        int offsetX = basicOffset+Entry_SideMargin;
        int offsetY = basicOffset;

        // also need to correct from widget's margins
        #if GTK_CHECK_VERSION( 3, 11, 0 )
        const int marginX = gtk_widget_get_margin_start( child );
        const int marginW = marginX + gtk_widget_get_margin_end( child );
        #else
        const int marginX = gtk_widget_get_margin_left( child );
        const int marginW = marginX + gtk_widget_get_margin_right( child );
        #endif

        const int marginY = gtk_widget_get_margin_top( child );
        const int marginH = marginY + gtk_widget_get_margin_bottom( child );

        // hole background
        Style::instance().renderHoleBackground( context,
            gtk_widget_get_window(widget), widget,
            allocation.x - offsetX - marginX,
            allocation.y - offsetY - marginY,
            allocation.width + offsetX*2 + marginW,
            allocation.height + offsetY*2 + marginH );

        // adjust offset and render hole
        offsetX -= Entry_SideMargin;
        Style::instance().renderHole( context,
            allocation.x - offsetX - marginX,
            allocation.y - offsetY - marginY,
            allocation.width + offsetX*2 + marginW,
            allocation.height + offsetY*2 + marginH,
            options, data );

        #endif // enable inner shadows hack

        // let the event propagate
        return FALSE;
    }

}
