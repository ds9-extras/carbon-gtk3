#ifndef carbonscrollbardata_h
#define carbonscrollbardata_h
/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "carbonsignal.h"
#include "carbontimer.h"

#include <gtk/gtk.h>

namespace Carbon
{
    // track scrollbars
    class ScrollBarData
    {

        public:

        //! constructor
        ScrollBarData( void );

        //! destructor
        virtual ~ScrollBarData( void );

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        static void valueChanged( GtkRange*, gpointer );

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! pointer to associated widget
        GtkWidget* _target;

        //! true if updates are delayed
        bool _updatesDelayed;

        //! update delay
        int _delay;

        //! timer
        Timer _timer;

        //! true if next update must be delayed
        bool _locked;

        //! signal
        Signal _valueChangedId;

    };

}

#endif
