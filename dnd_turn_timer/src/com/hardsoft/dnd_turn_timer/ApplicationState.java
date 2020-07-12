package com.hardsoft.dnd_turn_timer;

import java.util.Date;

import android.content.ContentValues;
import android.database.Cursor;

class ApplicationState
{
    public static final String TABLE = "application_state";

    public static final String COLUMN_NAME_NAME   = "name";
    public static final String COLUMN_NAME_VALUE = "value";

    public static final String COLUMN_TYPE_NAME  = "text not null primary key";
    public static final String COLUMN_TYPE_VALUE = "text not null";

    public static final String [] COLUMN_NAMES  = { COLUMN_NAME_NAME, COLUMN_NAME_VALUE };
    public static final String [] COLUMN_TYPES = { COLUMN_TYPE_NAME, COLUMN_TYPE_VALUE };

    private static final String NAME_ROUND_NUMBER            = "round_number";
    private static final String NAME_PLAYER_TURN_TIME        = "player_turn_time";
    private static final String NAME_DM_TURN_TIME            = "dm_turn_time";
    private static final String NAME_CURRENT_TURN_ID         = "current_turn_id";
    private static final String NAME_CURRENT_TURN_START_TIME = "current_turn_start_time";

    private static final String DEFAULT_ROUND_NUMBER     = "1";
    private static final String DEFAULT_PLAYER_TURN_TIME = "120";
    private static final String DEFAULT_DM_TURN_TIME     = "120";

    private DatabaseAdapter mDatabaseAdapter;

    ApplicationState( DatabaseAdapter databaseAdapter )
    {
        mDatabaseAdapter = databaseAdapter;
    }

    public int getRoundNumber()
    {
        return Integer.parseInt( mDatabaseAdapter.selectApplicationState( NAME_ROUND_NUMBER, DEFAULT_ROUND_NUMBER ) );
    }

    public int getPlayerTurnTime()
    {
        return Integer.parseInt( mDatabaseAdapter.selectApplicationState( NAME_PLAYER_TURN_TIME, DEFAULT_PLAYER_TURN_TIME ) );
    }

    public int getDmTurnTime()
    {
        return Integer.parseInt( mDatabaseAdapter.selectApplicationState( NAME_DM_TURN_TIME, DEFAULT_DM_TURN_TIME ) );
    }

    public int getCurrentTurnId()
    {
        return Integer.parseInt( mDatabaseAdapter.selectApplicationState( NAME_CURRENT_TURN_ID, "-1" ) );
    }

    public Date getCurrentTurnStartTime()
    {
        String currentTurnStartTimeString = mDatabaseAdapter.selectApplicationState( NAME_CURRENT_TURN_START_TIME, "" );

        if ( currentTurnStartTimeString.length() > 0 )
        {
            return new Date( Long.parseLong( currentTurnStartTimeString ) );
        }
        else return new Date();
    }

    public void setRoundNumber( int roundNumber )
    {
        mDatabaseAdapter.updateApplicationState( NAME_ROUND_NUMBER, Integer.toString( roundNumber ) );
    }

    public void setPlayerTurnTime( int playerTurnTime )
    {
        mDatabaseAdapter.updateApplicationState( NAME_PLAYER_TURN_TIME, Integer.toString( playerTurnTime ) );
    }

    public void setDmTurnTime( int dmTurnTime )
    {
        mDatabaseAdapter.updateApplicationState( NAME_DM_TURN_TIME, Integer.toString( dmTurnTime ) );
    }

    public void setCurrentTurnId( int currentTurnId )
    {
        mDatabaseAdapter.updateApplicationState( NAME_CURRENT_TURN_ID, Integer.toString( currentTurnId ) );
    }

    public void setCurrentTurnStartTime( Date currentTurnStartTime )
    {
        mDatabaseAdapter.updateApplicationState( NAME_CURRENT_TURN_START_TIME, Long.toString( currentTurnStartTime.getTime() ) );
    }
}
