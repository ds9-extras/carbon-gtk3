/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* GdkPixbuf modification code from Walmis
* <http://gnome-look.org/content/show.php?content=77783&forumpage=3>
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

#include "carbongtkutils.h"
#include "carbongtktypenames.h"
#include "config.h"

#include <cmath>
#include <cstring>
#include <gtk/gtk.h>
#include <iostream>
#include <set>

namespace Carbon
{

    //_____________________________________________________________________________
    std::ostream& operator << ( std::ostream& out, const GtkWidgetPath* path )
    {
        if( !path )
        {

            out << " (null)";

        } else {

            for( gint pos=0; pos<gtk_widget_path_length( path ); ++pos )
            {
                const char* name( g_type_name( gtk_widget_path_iter_get_object_type( path, pos ) ) );
                if(!name) break;
                out << "/" << name;
            }

        }

        return out;

    }



    //____________________________________________________________
    void Gtk::gtk_container_adjust_buttons_state( GtkContainer* container, gpointer data )
    {
        if( GTK_IS_BUTTON( container ) )
        {

            GtkWidget* button( GTK_WIDGET(container) );
            GdkWindow* window( gtk_widget_get_window( button ) );
            if( !window ) return;

            // get widget allocation
            GtkAllocation allocation( gtk_widget_get_allocation( button ) );

            // get device position
            int x(0),y(0);
            GdkDeviceManager* manager( gdk_display_get_device_manager( gtk_widget_get_display( button ) ) );
            GdkDevice* pointer( gdk_device_manager_get_client_pointer( manager ) );
            gdk_window_get_device_position( window, pointer, &x, &y, 0L);

            if( !(x>0 && y>0 &&
                x < allocation.width &&
                y < allocation.height) &&
                (gtk_widget_get_state_flags( button )&GTK_STATE_FLAG_ACTIVE) )
            { gtk_widget_set_state_flags( button, GTK_STATE_FLAG_NORMAL, true ); }

            gtk_button_set_relief(GTK_BUTTON(button),GTK_RELIEF_NORMAL);
            gtk_widget_set_size_request(button,16,16);

            return;

        } else if( GTK_IS_CONTAINER( container ) ) {

            gtk_container_foreach(container,(GtkCallback)gtk_container_adjust_buttons_state,0L);

        }

    }

    //____________________________________________________________
    bool Gtk::gtk_widget_path_has_type( const GtkWidgetPath* path, GType type )
    {

        if( !path ) return false;
        for( gint pos=0; pos<gtk_widget_path_length( path ); ++pos )
        {
            const GType local( gtk_widget_path_iter_get_object_type( path, pos ) );
            if( local == type || g_type_is_a( local, type ) )
            { return true; }
        }

        return false;

    }

    //____________________________________________________________
    bool Gtk::gtk_widget_is_applet( GtkWidget* widget )
    {
        if( !GTK_IS_WIDGET( widget ) ) return false;

        #if CARBON_DEBUG
        std::cerr << "Gtk::gtk_widget_is_applet(): " << Gtk::gtk_widget_path(widget) << std::endl;
        #endif

        static const char* names[] =
        {
            "Panel",
            "PanelWidget",
            "PanelApplet",
            "XfcePanelWindow",
            0
        };

        // check widget name
        std::string name( G_OBJECT_TYPE_NAME( widget ) );
        for( unsigned int i = 0; names[i]; ++i )
        { if( g_object_is_a( G_OBJECT( widget ), names[i] ) || name.find( names[i] ) == 0  ) return true; }

        // also check parent
        if( GtkWidget* parent = gtk_widget_get_parent( widget ) )
        {
            name = G_OBJECT_TYPE_NAME( parent );
            for( unsigned int i = 0; names[i]; ++i )
            { if( g_object_is_a( G_OBJECT( parent ), names[i] ) || name.find( names[i] ) == 0 ) return true; }

        }

        // also check first widget path element (needed for xfce panel)
        std::string widgetPath=Gtk::gtk_widget_path(widget);
        {
            for( unsigned int i = 0; names[i]; ++i )
            {
                if( widgetPath.find(names[i]) != std::string::npos )
                    return true;
            }
        }

        return false;

    }

    //____________________________________________________________
    void Gtk::gtk_widget_print_tree( GtkWidget* widget )
    {

        if( !GTK_IS_WIDGET( widget ) ) return;

        std::cerr << "Carbon::Gtk::gtk_widget_print_tree - widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;

        bool first( true );
        while( ( widget = gtk_widget_get_parent( widget ) ) )
        {
            first = false;
            std::cerr << "    parent: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        }

        if( !first ) std::cerr << std::endl;

    }

    //________________________________________________________
    bool Gtk::gdk_default_screen_is_composited( void )
    {
        GdkScreen* screen( gdk_screen_get_default() );
        return (screen && gdk_screen_is_composited( screen ) );
    }

    //________________________________________________________
    bool Gtk::gtk_widget_has_rgba( GtkWidget* widget )
    {

        if( !GTK_IS_WIDGET( widget ) ) return false;
        if( !gdk_default_screen_is_composited() ) return false;
        return gdk_visual_has_rgba( gtk_widget_get_visual( widget ) );

    }

    //________________________________________________________
    bool Gtk::gdk_window_is_base( GdkWindow* window )
    {

        if( !GDK_IS_WINDOW( window ) ) return false;

        GdkWindowTypeHint hint = gdk_window_get_type_hint( window );

        #if CARBON_DEBUG
        std::cerr << "Gtk::gdk_window_is_base - " << TypeNames::windowTypeHint( hint ) << std::endl;
        #endif

        return(
            hint == GDK_WINDOW_TYPE_HINT_NORMAL ||
            hint == GDK_WINDOW_TYPE_HINT_DIALOG ||
            hint == GDK_WINDOW_TYPE_HINT_UTILITY );
    }

    //________________________________________________________
    bool Gtk::gdk_window_nobackground( GdkWindow* window )
    {
        if( !GDK_IS_WINDOW( window ) ) return false;

        GdkWindowTypeHint hint = gdk_window_get_type_hint( window );
        return( hint == GDK_WINDOW_TYPE_HINT_COMBO || hint == GDK_WINDOW_TYPE_HINT_TOOLTIP );

    }

    //________________________________________________________
    bool Gtk::gdk_window_has_rgba( GdkWindow* window )
    {

        if( !GDK_IS_WINDOW( window ) ) return false;

        if( !gdk_default_screen_is_composited() ) return false;
        return gdk_visual_has_rgba( gdk_window_get_visual( window ) );

    }

    //________________________________________________________
    bool Gtk::gdk_visual_has_rgba( GdkVisual* visual )
    {

        // check visual
        if( !GDK_IS_VISUAL( visual ) ) return false;

        // check depth
        if( gdk_visual_get_depth( visual ) != 32 ) return false;

        // check red pixel
        guint32 redMask;
        gdk_visual_get_red_pixel_details( visual, &redMask, 0L, 0L );
        if( redMask != 0xff0000 ) return false;

        // check green pixel
        guint32 greenMask;
        gdk_visual_get_green_pixel_details( visual, &greenMask, 0L, 0L );
        if( greenMask != 0x00ff00 ) return false;

        // check blue pixel
        guint32 blueMask;
        gdk_visual_get_blue_pixel_details( visual, &blueMask, 0L, 0L );
        if( blueMask != 0x0000ff ) return false;

        return true;

    }

    //________________________________________________________
    bool Gtk::g_object_is_a( const GObject* object, const std::string& typeName )
    {

        if( object )
        {
            const GType tmp( g_type_from_name( typeName.c_str() ) );
            if( tmp )
            { return g_type_check_instance_is_a( (GTypeInstance*) object, tmp ); }
        }

        return false;
    }

    //________________________________________________________
    std::string Gtk::gtk_widget_path( GtkWidget* widget )
    {

        if(GTK_IS_WIDGET(widget))
        {
            GtkWidgetPath* path( gtk_widget_get_path( widget ) );
            char* widgetPath( gtk_widget_path_to_string( path ) );
            const std::string  out( widgetPath );
            g_free( widgetPath );
            return out;
        }
        return std::string("not a widget");

    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_widget_find_parent( GtkWidget* widget, GType type )
    {

        if( !GTK_IS_WIDGET( widget ) ) return 0L;
        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        { if( G_TYPE_CHECK_INSTANCE_TYPE( parent, type ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_groupbox( GtkWidget* widget )
    {

        if( !GTK_IS_WIDGET( widget ) ) return 0L;
        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        { if( gtk_widget_is_groupbox( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    bool Gtk::gtk_widget_is_parent( GtkWidget* widget, GtkWidget* potentialParent )
    {

        if( !GTK_IS_WIDGET( widget ) ) return false;
        for( GtkWidget* parent = gtk_widget_get_parent( widget ); parent; parent = gtk_widget_get_parent( parent ) )
        { if( potentialParent==parent ) return true; }

        return false;
    }

    //________________________________________________________
    bool Gtk::gtk_parent_is_shadow_in( GtkWidget* widget )
    {
        if( !GTK_IS_WIDGET( widget ) ) return false;
        for( GtkWidget* parent = gtk_widget_get_parent( widget ); parent; parent = gtk_widget_get_parent( parent ) )
        {
            if( GTK_IS_FRAME( parent ) && gtk_frame_get_shadow_type( GTK_FRAME( parent ) ) == GTK_SHADOW_IN ) return true;
            if( GTK_IS_SCROLLED_WINDOW( parent ) && gtk_scrolled_window_get_shadow_type( GTK_SCROLLED_WINDOW( parent ) ) == GTK_SHADOW_IN ) return true;
        }

        return false;
    }

    //________________________________________________________
    bool Gtk::gtk_button_is_flat( GtkWidget* widget )
    {
        if( !GTK_IS_BUTTON( widget ) ) return false;
        return ( gtk_button_get_relief( GTK_BUTTON( widget ) ) == GTK_RELIEF_NONE );
    }

    //________________________________________________________
    bool Gtk::gtk_button_is_header( GtkWidget* widget )
    { return GTK_IS_BUTTON( widget ) && gtk_parent_tree_view( widget ); }

    //________________________________________________________
    bool Gtk::gtk_button_is_in_path_bar( GtkWidget* widget )
    {
        if( !( GTK_IS_BUTTON( widget ) && gtk_widget_get_parent( widget ) ) ) return false;

        std::string name(G_OBJECT_TYPE_NAME( gtk_widget_get_parent( widget ) ) );
        return name == "GtkPathBar" || name == "NautilusPathBar";
    }

    //________________________________________________________
    bool Gtk::gtk_path_bar_button_is_last( GtkWidget* widget )
    {

        if( !GTK_IS_BUTTON( widget ) ) return false;
        GtkWidget* parent( gtk_widget_get_parent( widget ) );

        // get parent and check type
        if( !( parent && GTK_IS_CONTAINER( parent ) ) ) return false;

        // get children
        GList* children( gtk_container_get_children( GTK_CONTAINER( parent ) ) );

        /*
        for some reason, pathbar buttons are ordered in the container in reverse order.
        meaning that the last button (in the pathbar) is stored first in the list.
        */
        bool result = (widget == g_list_first( children )->data );
        if( children ) g_list_free( children );
        return result;

    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_button_find_image(GtkWidget* button)
    {

        // check widget type
        if(!GTK_IS_CONTAINER(button)) return 0L;

        GtkWidget* result( 0L );
        GList* children( gtk_container_get_children( GTK_CONTAINER( button ) ) );
        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {
            if( GTK_IS_IMAGE( child->data ) )
            {
                result = GTK_WIDGET( child->data );
                break;

            } else if( GTK_IS_CONTAINER( child->data ) ) {

                result = gtk_button_find_image( GTK_WIDGET(child->data ) );
                break;

            }

        }

        if( children ) g_list_free( children );
        return result;

    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_button_find_label(GtkWidget* button)
    {

        // check widget type
        if( !GTK_IS_CONTAINER(button) ) return 0L;

        GtkWidget* result( 0L );
        GList* children( gtk_container_get_children( GTK_CONTAINER( button ) ) );
        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {

            if( GTK_IS_LABEL( child->data) )
            {
                result = GTK_WIDGET( child->data );
                break;

            } else if( GTK_IS_CONTAINER( child->data ) ) {

                result = gtk_button_find_image(GTK_WIDGET(child->data));
                break;

            }

        }

        if( children ) g_list_free( children );
        return result;

    }

    //________________________________________________________
    bool Gtk::gtk_combobox_has_frame( GtkWidget* widget )
    {

        if( !GTK_IS_WIDGET( widget ) ) return false;

        // get has-frame value
        GValue val = { 0, };
        g_value_init(&val, G_TYPE_BOOLEAN);
        g_object_get_property( G_OBJECT( widget ), "has-frame", &val );
        return (bool) g_value_get_boolean( &val );

    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_tree_view( GtkWidget* widget )
    { return GTK_IS_TREE_VIEW( widget ) && gtk_combobox_is_scrolled_window( gtk_widget_get_parent( widget ) ); }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_scrolled_window( GtkWidget* widget )
    {
        // check type
        if( !GTK_IS_SCROLLED_WINDOW( widget ) ) return false;

        // retrieve parent and type
        GtkWidget* parent( gtk_widget_get_parent( widget ) );
        if( !parent ) return false;

        // retrieve parent name
        const gchar* name( gtk_widget_get_name( parent ) );
        return ( name && std::string( name ) == "gtk-combobox-popup-window" );
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_viewport( GtkWidget* widget )
    {
        if( !GTK_IS_VIEWPORT(widget) ) return false;
        static const std::string match( "gtk-combo-popup-window" );
        return Gtk::gtk_widget_path( widget ).substr( 0, match.size() ) == match;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_frame( GtkWidget* widget )
    {
        if( !GTK_IS_FRAME(widget) ) return false;
        static const std::string match( "gtk-combo-popup-window" );
        return Gtk::gtk_widget_path( widget ).substr( 0, match.size() ) == match;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_appears_as_list( GtkWidget* widget )
    {
        if( !GTK_IS_WIDGET( widget ) ) return false;
        gboolean appearsAsList;
        gtk_widget_style_get( widget, "appears-as-list", &appearsAsList, NULL );
        return (bool) appearsAsList;
    }

    //________________________________________________________
    bool Gtk::gtk_notebook_tab_contains( GtkWidget* widget, int tab, int x, int y )
    {

        if( !( tab >= 0 && GTK_IS_NOTEBOOK( widget ) ) ) return false;

        // cast to notebook and check against number of pages
        GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
        if( tab >= gtk_notebook_get_n_pages( notebook ) ) return false;

        // retrieve page and tab label
        GtkWidget* page( gtk_notebook_get_nth_page( notebook, tab ) );
        GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );

        // get allocted size and compare to position
        const GtkAllocation allocation( gtk_widget_get_allocation( tabLabel ) );
        return Gtk::gdk_rectangle_contains( &allocation, x, y );

    }

     //________________________________________________________
    int Gtk::gtk_notebook_find_tab( GtkWidget* widget, int x, int y )
    {

        if( !GTK_IS_NOTEBOOK( widget ) ) return -1;

        // cast to notebook and check against number of pages
        GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
        int tab(-1);
        int minDistance( -1 );
        for( int i = gtk_notebook_find_first_tab( widget ); i <  gtk_notebook_get_n_pages( notebook ); ++i )
        {

            // retrieve page and tab label
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            if( !page ) continue;

            // get label
            GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );
            if(!tabLabel) continue;

            // get allocted size and compare to position
            const GtkAllocation allocation( gtk_widget_get_allocation( tabLabel ) );

            // get manhattan length
            const int distance = int(
                std::abs( double( allocation.x + allocation.width/2 - x ) ) +
                std::abs( double( allocation.y + allocation.height/2 - y ) ) );
            if( minDistance < 0 || distance < minDistance )
            {
                tab = i;
                minDistance = distance;
            }
        }

        return tab;

    }

    //________________________________________________________
    int Gtk::gtk_notebook_find_first_tab( GtkWidget* widget )
    {

        // TODO: reimplement with gtk+3.0
        return 0;

//         if( !GTK_IS_NOTEBOOK( widget ) ) return 0;
//
//         // cast to notebook
//         GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
//         return g_list_position( notebook->children, notebook->first_tab );

    }

    //____________________________________________________________
    bool Gtk::gtk_notebook_is_tab_label(GtkNotebook* notebook, GtkWidget* widget )
    {

        if( !GTK_IS_NOTEBOOK( notebook ) ) return false;
        for( int i = 0; i <  gtk_notebook_get_n_pages( notebook ); ++i )
        {
            // retrieve page and tab label
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            if( !page ) continue;

            GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );
            if( widget == tabLabel ) return true;
        }

        return false;

    }


    //____________________________________________________________
    void Gtk::gtk_notebook_get_tabbar_rect( GtkNotebook* notebook, GdkRectangle* rect )
    {
        // check notebook and rect
        if( !( GTK_IS_NOTEBOOK( notebook ) && rect ) ) return;

        // check tab visibility
        GList* children( gtk_container_get_children( GTK_CONTAINER( notebook ) ) );
        if( !( gtk_notebook_get_show_tabs( notebook ) && children ) )
        {
            if( children ) g_list_free( children );
            *rect = gdk_rectangle();
            return;
        }

        // free children
        if( children ) g_list_free( children );

        // get full rect
        gtk_widget_get_allocation( GTK_WIDGET( notebook ), rect );

        // adjust to account for borderwidth
        guint borderWidth( gtk_container_get_border_width( GTK_CONTAINER( notebook ) ) );
        rect->x += borderWidth;
        rect->y += borderWidth;
        rect->height -= 2*borderWidth;
        rect->width -= 2*borderWidth;

        // get current page
        int pageIndex( gtk_notebook_get_current_page( notebook ) );
        if( !( pageIndex >= 0 && pageIndex < gtk_notebook_get_n_pages( notebook ) ) )
        {
            *rect = gdk_rectangle();
            return;
        }

        GtkWidget* page( gtk_notebook_get_nth_page( notebook, pageIndex ) );
        if( !page )
        {
            *rect = gdk_rectangle();
            return;
        }

        // removes page allocated size from rect, based on tabwidget orientation
        const GtkAllocation pageAllocation( gtk_widget_get_allocation( page ) );
        switch( gtk_notebook_get_tab_pos( notebook ) )
        {
            case GTK_POS_BOTTOM:
            rect->height += rect->y - (pageAllocation.y + pageAllocation.height);
            rect->y = pageAllocation.y + pageAllocation.height;
            break;

            case GTK_POS_TOP:
            rect->height = pageAllocation.y - rect->y;
            break;

            case GTK_POS_RIGHT:
            rect->width += rect->x - (pageAllocation.x + pageAllocation.width);
            rect->x = pageAllocation.x + pageAllocation.width;
            break;

            case GTK_POS_LEFT:
            rect->width = pageAllocation.x - rect->x;
            break;
        }

        return;

    }

    //____________________________________________________________
    bool Gtk::gtk_notebook_has_visible_arrows( GtkNotebook* notebook )
    {

        if( !GTK_IS_NOTEBOOK( notebook ) ) return false;
        if( !gtk_notebook_get_show_tabs( notebook ) ) return false;

        // loop over pages
        for( int i = 0; i <  gtk_notebook_get_n_pages( notebook ); ++i )
        {

            // retrieve page and tab label
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            if( !page ) continue;

            GtkWidget* label( gtk_notebook_get_tab_label( notebook, page ) );
            if( label && !gtk_widget_get_mapped( label ) ) return true;
        }

        return false;

    }

    //____________________________________________________________
    bool Gtk::gtk_notebook_update_close_buttons(GtkNotebook* notebook)
    {
        if( !GTK_IS_NOTEBOOK( notebook ) ) return false;

        int numPages=gtk_notebook_get_n_pages( notebook );
        for( int i = 0; i < numPages; ++i )
        {

            // retrieve page
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            if( !page ) continue;

            // retrieve tab label
            GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );
            if( tabLabel && GTK_IS_CONTAINER( tabLabel ) )
            { gtk_container_adjust_buttons_state( GTK_CONTAINER( tabLabel ) ); }

        }
        return FALSE;
    }

    //________________________________________________________
    bool Gtk::gtk_notebook_is_close_button(GtkWidget* widget)
    {

        if( GtkNotebook* nb=GTK_NOTEBOOK(gtk_parent_notebook(widget) ) )
        {
            // check if the button resides on tab label, not anywhere on the tab
            bool tabLabelIsParent=false;
            for( int i=0; i < gtk_notebook_get_n_pages(nb); ++i )
            {
                GtkWidget* tabLabel( gtk_notebook_get_tab_label(nb,gtk_notebook_get_nth_page( nb, i ) ) );
                if( gtk_widget_is_parent( widget, GTK_WIDGET(tabLabel) ) )
                { tabLabelIsParent=true; }
            }

            if( !tabLabelIsParent ) return false;

            // make sure button has no text and some image (for now, just hope it's a close icon)
            if( gtk_button_find_image(widget) && !gtk_button_get_label( GTK_BUTTON(widget) ) )
            { return true; }

            // check for pidgin 'x' close button
            if( GtkWidget* label = gtk_button_find_label(widget) )
            {

                const gchar* labelText=gtk_label_get_text( GTK_LABEL(label) );
                if(!strcmp(labelText,"×")) // It's not letter 'x' - it's a special symbol
                {
                    gtk_widget_hide( label );
                    return true;
                }

            }

        }

        return false;

    }

    //________________________________________________________
    bool Gtk::gtk_scrolled_window_force_sunken( GtkWidget* widget)
    {

        // FMIconView (from nautilus) always get sunken
        if( g_object_is_a( G_OBJECT( widget ), "FMIconView" ) ) return true;

        // other checks require widget to be of type GtkBin
        if( !GTK_IS_BIN( widget ) ) return false;

        // retrieve child
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
        if( GTK_IS_TREE_VIEW( child ) || GTK_IS_ICON_VIEW( child ) || GTK_IS_TEXT_VIEW( child ) ) return true;
        else return false;

    }

    //________________________________________________________
    bool Gtk::gdk_window_map_to_toplevel( GdkWindow* window, gint* x, gint* y, gint* w, gint* h, bool frame )
    {

        // always initialize arguments (to invalid values)
        if( x ) *x=0;
        if( y ) *y=0;
        if( w ) *w = -1;
        if( h ) *h = -1;

        if( !( window && GDK_IS_WINDOW( window ) ) ) return false;
        if( gdk_window_get_window_type( window ) == GDK_WINDOW_OFFSCREEN ) return false;

        // get window size and height
        if( frame ) gdk_toplevel_get_frame_size( window, w, h );
        else gdk_toplevel_get_size( window, w, h );
        Gtk::gdk_window_get_toplevel_origin( window, x, y );
        return ((!w) || *w > 0) && ((!h) || *h>0);

    }

    //________________________________________________________
    bool Gtk::gtk_widget_map_to_toplevel( GtkWidget* widget, gint* x, gint* y, gint* w, gint* h, bool frame )
    {

        // always initialize arguments (to invalid values)
        if( x ) *x=0;
        if( y ) *y=0;
        if( w ) *w = -1;
        if( h ) *h = -1;

        if( !GTK_IS_WIDGET( widget ) ) return false;

        // get window
        GdkWindow* window( gtk_widget_get_parent_window( widget ) );
        if( !( window && GDK_IS_WINDOW( window ) ) ) return false;
        if( gdk_window_get_window_type( window ) == GDK_WINDOW_OFFSCREEN ) return false;

        if( frame ) gdk_toplevel_get_frame_size( window, w, h );
        else gdk_toplevel_get_size( window, w, h );
        int xlocal, ylocal;
        const bool success( gtk_widget_translate_coordinates( widget, gtk_widget_get_toplevel( widget ), 0, 0, &xlocal, &ylocal ) );
        if( success )
        {

            if( x ) *x=xlocal;
            if( y ) *y=ylocal;

        }

        return success && ((!w) || *w > 0) && ((!h) || *h>0);

    }

    //________________________________________________________
    bool Gtk::gtk_widget_map_to_parent( GtkWidget* widget, GtkWidget* parent, gint* x, gint* y, gint* w, gint* h )
    {

        // always initialize arguments (to invalid values)
        if( x ) *x=0;
        if( y ) *y=0;
        if( w ) *w = -1;
        if( h ) *h = -1;

        if( !( GTK_IS_WIDGET( widget ) && GTK_IS_WIDGET( parent ) ) ) return false;

        const GtkAllocation allocation( gtk_widget_get_allocation(  parent ) );
        if( w ) *w = allocation.width;
        if( h ) *h = allocation.height;

        int xlocal, ylocal;
        const bool success( gtk_widget_translate_coordinates( widget, parent, 0, 0, &xlocal, &ylocal ) );
        if( success )
        {

            if( x ) *x=xlocal;
            if( y ) *y=ylocal;

        }

        return success && ((!w) || *w > 0) && ((!h) || *h>0);

    }

    //________________________________________________________
    bool Gtk::gdk_window_translate_origin( GdkWindow* parent, GdkWindow* child, gint* x, gint* y )
    {
        if( x ) *x = 0;
        if( y ) *y = 0;
        if( !( GTK_IS_WIDGET( parent ) && GTK_IS_WIDGET( child ) ) ) return false;
        while( child && GDK_IS_WINDOW( child ) &&
            child != parent &&
            gdk_window_get_window_type( child ) == GDK_WINDOW_CHILD )
        {
            gint xloc;
            gint yloc;
            gdk_window_get_position( child, &xloc, &yloc );
            if( x ) *x += xloc;
            if( y ) *y += yloc;
            child = gdk_window_get_parent( child );
        }

        return( child == parent );

    }

    //________________________________________________________
    void Gtk::gdk_toplevel_get_size( GdkWindow* window, gint* w, gint* h )
    {

        if( !GDK_IS_WINDOW( window ) )
        {
            if( w ) *w = -1;
            if( h ) *h = -1;
            return;
        }

        if( GdkWindow* topLevel = gdk_window_get_toplevel( window ) )
        {

            if( w ) *w = gdk_window_get_width( topLevel );
            if( h ) *h = gdk_window_get_height( topLevel );

        } else {

            if( w ) *w = gdk_window_get_width( window );
            if( h ) *h = gdk_window_get_height( window );

        }

        return;

    }

    //________________________________________________________
    void Gtk::gdk_toplevel_get_frame_size( GdkWindow* window, gint* w, gint* h )
    {

        if( !GDK_IS_WINDOW( window ) )
        {
            if( w ) *w = -1;
            if( h ) *h = -1;
            return;
        }

        GdkWindow* topLevel = gdk_window_get_toplevel( window );
        if( topLevel && GDK_IS_WINDOW( topLevel ) )
        {
            if( gdk_window_get_window_type( topLevel ) == GDK_WINDOW_OFFSCREEN )
            {

                if( w ) *w = gdk_window_get_width(topLevel);
                if( h ) *h = gdk_window_get_height(topLevel);

            } else {

                GdkRectangle rect = {0, 0, -1, -1};
                gdk_window_get_frame_extents( topLevel, &rect );

                if( w ) *w = rect.width;
                if( h ) *h = rect.height;

            }
        }

        return;

    }

    //________________________________________________________
    void Gtk::gdk_window_get_toplevel_origin( GdkWindow* window, gint* x, gint* y )
    {
        if( x ) *x = 0;
        if( y ) *y = 0;
        while( GDK_IS_WINDOW( window ) && gdk_window_get_window_type( window ) == GDK_WINDOW_CHILD )
        {
            gint xloc;
            gint yloc;
            gdk_window_get_position( window, &xloc, &yloc );
            if( x ) *x += xloc;
            if( y ) *y += yloc;
            window = gdk_window_get_parent( window );
        }

        return;
    }

    //___________________________________________________________
    GdkPixbuf* Gtk::gdk_pixbuf_set_alpha( const GdkPixbuf *pixbuf, double alpha )
    {

        if( !GDK_IS_PIXBUF( pixbuf ) ) return 0L;

        /* Returns a copy of pixbuf with it's non-completely-transparent pixels to
        have an alpha level "alpha" of their original value. */
        GdkPixbuf* target( gdk_pixbuf_add_alpha( pixbuf, false, 0, 0, 0 ) );
        if( alpha >= 1.0 ) return target;
        if( alpha < 0 ) alpha = 0;

        const int width( gdk_pixbuf_get_width( target ) );
        const int height( gdk_pixbuf_get_height( target ) );
        const int rowstride( gdk_pixbuf_get_rowstride( target ) );
        unsigned char* data = gdk_pixbuf_get_pixels( target );

        for( int y = 0; y < height; ++y )
        {

            for( int x = 0; x < width; ++x )
            {
                /* The "4" is the number of chars per pixel, in this case, RGBA,
                the 3 means "skip to the alpha" */
                unsigned char* current = data + ( y*rowstride ) + ( x*4 ) + 3;
                *(current) = (unsigned char) ( *( current )*alpha );
            }
        }

        return target;
    }

    //_________________________________________________________
    bool Gtk::gdk_pixbuf_to_gamma(GdkPixbuf* pixbuf, double value)
    {

        if( !GDK_IS_PIXBUF( pixbuf ) ) return false;
        if(gdk_pixbuf_get_colorspace(pixbuf)==GDK_COLORSPACE_RGB &&
            gdk_pixbuf_get_bits_per_sample(pixbuf)==8 &&
            gdk_pixbuf_get_has_alpha(pixbuf) &&
            gdk_pixbuf_get_n_channels(pixbuf)==4)
        {
            double gamma=1./(2.*value+0.5);
            unsigned char* data=gdk_pixbuf_get_pixels(pixbuf);
            const int height=gdk_pixbuf_get_height(pixbuf);
            const int width=gdk_pixbuf_get_width(pixbuf);
            const int rowstride=gdk_pixbuf_get_rowstride(pixbuf);
            for(int x=0;x<width;++x)
            {
                for(int y=0; y<height; y++)
                {
                    unsigned char* p=data + y*rowstride + x*4;
                    *p=(char)(pow((*p/255.),gamma)*255); ++p;
                    *p=(char)(pow((*p/255.),gamma)*255); ++p;
                    *p=(char)(pow((*p/255.),gamma)*255);
                }

            }

            return true;

        } else return false;

    }

    //___________________________________________________________
    GdkPixbuf* Gtk::gdk_pixbuf_resize( GdkPixbuf* src, int width, int height )
    {

        if( !GDK_IS_PIXBUF( src ) ) return 0L;
        if( width == gdk_pixbuf_get_width( src ) &&  height == gdk_pixbuf_get_height( src ) )
        {

            return static_cast<GdkPixbuf*>(g_object_ref (src));

        } else {

            return gdk_pixbuf_scale_simple( src, width, height, GDK_INTERP_BILINEAR );

        }

    }

    //___________________________________________________________
    void Gtk::gtk_viewport_get_position( GtkViewport* viewport, gint* x, gint* y )
    {

        if( !GTK_IS_VIEWPORT( viewport ) ) return;

        // initialize
        if( x ) *x = 0;
        if( y ) *y = 0;

        // get windows and derive offsets
        gint xBin(0), yBin(0);
        gdk_window_get_geometry( gtk_viewport_get_bin_window( viewport ), &xBin, &yBin, 0, 0 );

        gint xView(0), yView(0);
        gdk_window_get_geometry( gtk_viewport_get_view_window( viewport ), &xView, &yView, 0, 0 );

        // calculate offsets
        if( x ) *x = xView - xBin;
        if( y ) *y = yView - yBin;

        return;

    }

    //___________________________________________________________
    GtkWidget* Gtk::gtk_dialog_find_button(GtkDialog* dialog,gint response_id)
    {

        if( !GTK_IS_DIALOG( dialog ) ) return 0L;

        // get children of dialog's action area
        GList* children( gtk_container_get_children( GTK_CONTAINER( gtk_dialog_get_action_area( dialog ) ) ) );

        #if CARBON_DEBUG
        std::cerr << "Carbon::Gtk::gtk_dialog_find_button - buttons: ";
        #endif

        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {

            // check data
            if( !GTK_IS_WIDGET( child->data ) ) continue;
            GtkWidget* childWidget( GTK_WIDGET( child->data ) );

            const gint id( gtk_dialog_get_response_for_widget(dialog, childWidget ) );

            #if CARBON_DEBUG
            std::cerr << Gtk::TypeNames::response( (GtkResponseType) id ) << ", ";
            #endif
            if( id == response_id ) return childWidget;

        }

        #if CARBON_DEBUG
        std::cerr << std::endl;
        #endif

        if( children ) g_list_free( children );
        return 0L;

    }


}
