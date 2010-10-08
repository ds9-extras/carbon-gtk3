#ifndef oxygenmainwindowdata_h
#define oxygenmainwindowdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* MainWindow prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc.
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
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

#include <gtk/gtkwidget.h>

namespace Oxygen
{
    // track main window resize events
    class MainWindowData
    {

        public:

        //! constructor
        MainWindowData( void ):
            _target(0L),
            _timeOutCount(0),
            _configureId(-1),
            _width(-1),
            _height(-1)
        {}

        //! destructor
        virtual ~MainWindowData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        //! update size
        void updateSize( GtkWidget*, int width, int height );

        //!name static callbacks
        static gboolean configureNotifyEvent( GtkWidget*, GdkEventConfigure*, gpointer);

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! pointer to associated widget
        GtkWidget* _target;

        //! keep track of how many timeouts are running
        int _timeOutCount;

        //! configure signal id
        int _configureId;

        //! old width
        int _width;

        //! old height
        int _height;


    };

}

#endif