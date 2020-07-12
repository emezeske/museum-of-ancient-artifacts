package com.hardsoft.dnd_turn_timer;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.io.FileNotFoundException;

import android.content.Context;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;

public class DatabaseAdapter
{
    private static final int DATABASE_VERSION = 1;
    private static final String DATABASE_NAME = "turn_timer";

    private SQLiteDatabase mDatabase;

    public DatabaseAdapter( Context context )
    {
        mDatabase = context.openOrCreateDatabase( DATABASE_NAME, Context.MODE_PRIVATE, null );

        createTable( Turn.TABLE, Turn.COLUMN_NAMES, Turn.COLUMN_TYPES );
        createTable( ApplicationState.TABLE, ApplicationState.COLUMN_NAMES, ApplicationState.COLUMN_TYPES );
    }

    private void createTable( String table, String [] columnKeys, String [] columnTypes ) throws SQLException
    {
        if ( columnKeys.length != columnTypes.length )
        {
            throw new SQLException( "Number of column keys and types differ." );
        }

        String create_statement = "create table if not exists " + table + " ( ";

        for ( int i = 0; i < columnKeys.length; ++i )
        {
            create_statement += columnKeys[i] + " " + columnTypes[i];

            if ( i + 1 < columnKeys.length )
            {
                create_statement += ", ";
            }
        }

        create_statement += " );";
        mDatabase.execSQL( create_statement );
    }

    public void close()
    {
        mDatabase.close();
    }

    public long insertTurn( Turn turn )
    {
        return mDatabase.insert( Turn.TABLE, null, Turn.makeContentValues( turn ) );
    }

    public void deleteTurn( long id )
    {
        mDatabase.delete(
            Turn.TABLE,
            Turn.COLUMN_NAME_ID + "=?",
            new String [] { Long.toString( id ) }
        );
    }

    public ArrayList<Turn> selectAllTurns()
    {
        Cursor result = mDatabase.query(
            Turn.TABLE,
            Turn.COLUMN_NAMES,
            null, null, null, null,
            Turn.COLUMN_NAME_POSITION
        );

        try
        {
            ArrayList<Turn> turns = new ArrayList<Turn>();

            while ( result.moveToNext() )
            {
                turns.add( Turn.makeTurn( result ) );
            }

            return turns;
        }

        finally { result.close(); }
    }

    public void resetTurnPositions()
    {
        ArrayList<Turn> turns = selectAllTurns();

        Collections.sort( turns, new Comparator<Turn>()
        {
            public int compare( Turn a, Turn b )
            {
                int result = new Integer( a.totalInitiative() ).compareTo( b.totalInitiative() );

                if ( result == 0 )
                {
                    result = new Integer( a.mInitiativeBonus ).compareTo( b.mInitiativeBonus );

                    if ( result == 0 )
                    {
                        result = a.mName.compareTo( b.mName );
                    }
                }

                return result;
            }
        } );

        for ( int i = 0; i < turns.size(); ++i )
        {
            Turn turn = turns.get( i );
            turn.mPosition = i;
            updateTurn( turn );
        }
    }

    public Turn selectTurn( long id ) throws SQLException
    {
        Cursor result = mDatabase.query(
            true,
            Turn.TABLE,
            Turn.COLUMN_NAMES,
            Turn.COLUMN_NAME_ID + "=?",
            new String [] { Long.toString( id ) },
            null, null, null, null
        );

        try
        {
            if ( result.getCount() == 0 || !result.moveToFirst() )
            {
                throw new SQLException( "No turn matching ID: " + id );
            }

            return Turn.makeTurn( result );
        }

        finally { result.close(); }
    }

    public void updateTurn( Turn turn )
    {
        int rowsUpdated = mDatabase.update(
            Turn.TABLE,
            Turn.makeContentValues( turn ),
            Turn.COLUMN_NAME_ID + "=?",
            new String [] { Long.toString( turn.mId ) }
        );

        if ( rowsUpdated == 0 )
        {
            throw new SQLException( "Couldn't update row in " + Turn.TABLE );
        }
    }

    public String selectApplicationState( String name, String default_value )
    {
        Cursor result = mDatabase.query(
            true,
            ApplicationState.TABLE,
            ApplicationState.COLUMN_NAMES,
            ApplicationState.COLUMN_NAME_NAME + "=?",
            new String [] { name },
            null, null, null, null
        );

        try
        {
            if ( result.getCount() == 0 || !result.moveToFirst() )
            {
                return default_value;
            }
            else return result.getString( result.getColumnIndex( ApplicationState.COLUMN_NAME_VALUE ) );
        }

        finally { result.close(); }
    }

    public void updateApplicationState( String name, String value )
    {
        ContentValues values = new ContentValues();
        values.put( ApplicationState.COLUMN_NAME_NAME, name );
        values.put( ApplicationState.COLUMN_NAME_VALUE, value );

        int rowsUpdated = mDatabase.update(
            ApplicationState.TABLE,
            values,
            ApplicationState.COLUMN_NAME_NAME + "=?",
            new String [] { name }
        );

        if ( rowsUpdated == 0 )
        {
            if ( mDatabase.insert( ApplicationState.TABLE, null, values ) == -1 )
            {
                throw new SQLException( "Couldn't insert row in " + ApplicationState.TABLE );
            }
        }
    }
}
