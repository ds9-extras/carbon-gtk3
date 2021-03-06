#ifndef carbonpaneddata_h
#define carbonpaneddata_h
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

#include <gtk/gtk.h>

namespace Carbon
{
    class PanedData
    {

        public:

        //! constructor
        PanedData( void ):
            _cursorLoaded( false ),
            _cursor( 0L )
        {}

        //! destructor
        virtual ~PanedData( void )
        {
            disconnect( 0L );
            if( _cursor ) g_object_unref( _cursor );
        }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        //! update cursor
        virtual void updateCursor( GtkWidget* );

        //! realization hook
        static void realizeEvent( GtkWidget*, gpointer );

        private:

        //! realization signal
        Signal _realizeId;

        //! cursor
        bool _cursorLoaded;
        GdkCursor* _cursor;

    };

}

#endif
