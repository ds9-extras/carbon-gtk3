#ifndef carbonapplicationname_h
#define carbonapplicationname_h
/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* inspired notably from kdelibs/kdeui/color/kcolorutils.h
* Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
* Copyright (C) 2007 Thomas Zander <zander@kde.org>
* Copyright (C) 2007 Zack Rusin <zack@kde.org>
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

#include <gtk/gtk.h>
#include <iostream>
#include <string>

namespace Carbon
{

    //! application name enumeration
    enum AppName
    {
        Unknown,
        XUL
    };

    //! stores application name and provides some utility functions
    class ApplicationName
    {

        public:

        //! constructor
        ApplicationName( AppName name = Unknown ):
            _name( name )
        {}

        //! assignment
        ApplicationName& operator = ( const AppName& name )
        {
            _name = name;
            return *this;
        }

        //! cast
        operator const AppName& ( void ) const
        { return _name; }

        //! initialize
        /*! try derive AppName from gtk program name */
        void initialize( void );

        //!@name utilities
        //@{

        bool isXul( void ) const { return _name == XUL; }
        bool isUnknown( void ) const { return _name == Unknown; }

        //! special case for mozilla and acrobat that also check the type of the top level widget
        /*! this allows to prevent false positive for open and print dialogs */
        bool isXul( GtkWidget* widget ) const { return isXul() && !isGtkDialogWidget( widget ); }

        // true for all applications that requires a flat background to be used
        bool useFlatBackground( GtkWidget* ) const;

        //@}

        protected:

        //! determine if widget is on a GtkDialog
        bool isGtkDialogWidget( GtkWidget* ) const;

        //! get application name from Gtk
        std::string fromGtk( void ) const;

        //! get application name from pid
        std::string fromPid( int ) const;

        private:

        //! name
        AppName _name;

        //! streamer (for debugging)
        friend std::ostream& operator << ( std::ostream&, const ApplicationName& );


    };


}

#endif
