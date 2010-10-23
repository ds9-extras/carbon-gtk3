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

#include "oxygengtkcellinfo.h"

#include <iostream>
#include <cassert>

namespace Gtk
{

    //____________________________________________________________________________
    bool CellInfo::hasParent( GtkTreeView* treeView ) const
    {
        // check treeview and path
        if( !( treeView && _path ) ) return false;

        // get model
        GtkTreeModel* model( gtk_tree_view_get_model( treeView ) );
        if( !model ) return false;

        // get iterator
        GtkTreeIter iter;
        if( !gtk_tree_model_get_iter( model, &iter, _path ) ) return false;

        GtkTreeIter parent;
        return gtk_tree_model_iter_parent( model, &parent, &iter );

    }

    //____________________________________________________________________________
    CellInfo CellInfo::parent( void ) const
    {
        CellInfo out;
        out._column = _column;

        // check path
        if( !_path ) return out;

        GtkTreePath* parent( gtk_tree_path_copy( _path ) );
        if( gtk_tree_path_up( parent ) ) out._path = parent;
        else gtk_tree_path_free( parent );

        return out;

    }

    //____________________________________________________________________________
    bool CellInfo::hasChildren( GtkTreeView* treeView ) const
    {
        // check treeview and path
        if( !( treeView && _path ) ) return false;

        // get model
        GtkTreeModel* model( gtk_tree_view_get_model( treeView ) );
        if( !model ) return false;

        // get iterator
        GtkTreeIter iter;
        if( !gtk_tree_model_get_iter( model, &iter, _path ) ) return false;
        return gtk_tree_model_iter_has_child( model, &iter );

    }

    //____________________________________________________________________________
    bool CellInfo::isLast( GtkTreeView* treeView ) const
    {
       // check treeview and path
        if( !( treeView && _path ) ) return false;

        // get model
        GtkTreeModel* model( gtk_tree_view_get_model( treeView ) );
        if( !model ) return false;

        // get iterator
        GtkTreeIter iter;
        if( !gtk_tree_model_get_iter( model, &iter, _path ) ) return false;
        return !gtk_tree_model_iter_next( model, &iter );
    }

    //____________________________________________________________________________
    GdkRectangle CellInfo::backgroundRect( GtkTreeView* treeView ) const
    {
        GdkRectangle out = {0, 0, -1, -1 };
        if( treeView && isValid() )
        { gtk_tree_view_get_background_area( treeView, _path, _column, &out ); }

        return out;

    }

    //____________________________________________________________________________
    CellInfoFlags::CellInfoFlags( GtkTreeView* treeView, const CellInfo& cellInfo ):
        _depth( cellInfo.depth() ),
        _expanderSize(0),
        _levelIndent(gtk_tree_view_get_level_indentation(treeView))
    {
        if( cellInfo.hasParent( treeView ) ) _flags |= HasParent;
        if( cellInfo.hasChildren( treeView ) ) _flags |= HasChildren;
        if( cellInfo.isLast( treeView ) ) _flags |= IsLast;

        gtk_widget_style_get( GTK_WIDGET( treeView ), "expander-size", &_expanderSize, NULL );

        _isLast = std::vector<bool>(_depth, false);

        unsigned int index( _depth-1 );
        for( CellInfo parent = cellInfo; parent.isValid() && parent.depth() > 0; parent = parent.parent() )
        {
            assert( index >= 0 );
            _isLast[index] = parent.isLast( treeView );
            --index;
        }

    }

}