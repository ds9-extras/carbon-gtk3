#ifndef carbonshadowhelper_h
#define carbonshadowhelper_h
/*
* this file is part of the carbon gtk engine
* Copyright (c) 2011 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "carbonapplicationname.h"
#include "carboncairosurface.h"
#include "carbonhook.h"
#include "carbonsignal.h"
#include "carbontileset.h"
#include "carbonwindowshadow.h"

#include <vector>
#include <map>

#ifdef GDK_WINDOWING_X11
#include <X11/Xdefs.h>
#include <X11/Xlib.h>
#endif

namespace Carbon
{

    //! handles X11 property passed to menu windows for window manager to draw proper shadows
    class ShadowHelper
    {

        public:

        //!@name property names
        //@{
        static const char* const netWMShadowAtomName;
        //@}

        //! constructor
        ShadowHelper( void );

        //! destructor
        virtual ~ShadowHelper( void );

        //! true is supported
        void setSupported( bool value )
        { _supported = value; }

        //! true if supported
        bool isSupported( void ) const
        { return _supported; }

        //! reset
        void reset( void );

        //! initialize hooks
        void initializeHooks( void );

        //! application name
        void setApplicationName( const ApplicationName& applicationName )
        { _applicationName = applicationName; }

        //! initialize
        void initialize( const ColorUtils::Rgba&, const WindowShadow& );

        //! register widget
        bool registerWidget( GtkWidget* );

        //! unregister widget
        void unregisterWidget( GtkWidget* );

        //! true if widget is menu
        bool isMenu( GtkWidget* ) const;

        //! true if widget is menu
        bool isToolTip( GtkWidget* ) const;

        protected:

        //! true if shadow should be installed on widget
        bool acceptWidget( GtkWidget* ) const;

        //! create pixmaps
        void createPixmapHandles( void );

        #ifdef GDK_WINDOWING_X11
        //! create Pixmap for given surface
        Pixmap createPixmap( const Cairo::Surface&, int opacity = 255 ) const;
        #endif

        //! install shadow X11 property on given widget
        /*!
        shadow atom and property specification available at
        http://community.kde.org/KWin/Shadow
        */
        void installX11Shadows( GtkWidget* );

        //! uninstall shadow X11 property on given widget
        void uninstallX11Shadows( GtkWidget* ) const;

        //! map event hook
        static gboolean realizeHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        //! destruction callback
        static gboolean destroyNotifyEvent( GtkWidget*, gpointer );

        private:

        //! true if shadows are supported
        bool _supported;

        //! shadow size
        int _size;

        //! shadow tileset
        TileSet _roundTiles;

        //! shadow tileset
        TileSet _squareTiles;

        //! application name
        ApplicationName _applicationName;

        #ifdef GDK_WINDOWING_X11
        //! shadow atom
        Atom _atom;
        #endif

        //! number of pixmaps
        enum { numPixmaps = 8 };

        //! round shadows pixmap handles
        typedef std::vector<unsigned long> PixmapList;
        PixmapList _roundPixmaps;

        //! square shadows pixmap handles
        PixmapList _squarePixmaps;

        //! widget data
        class WidgetData
        {

            public:

            //! constructor
            WidgetData( void )
            {}

            //! destroy signal
            Signal _destroyId;

        };

        //! map widgets and window id
        typedef std::map<GtkWidget*, WidgetData> WidgetMap;
        WidgetMap _widgets;

        //! true if hooks are initialized
        bool _hooksInitialized;

        //! map-event hook
        Hook _realizeHook;

    };

}

#endif
