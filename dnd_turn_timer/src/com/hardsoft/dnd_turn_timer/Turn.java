package com.hardsoft.dnd_turn_timer;

import android.content.ContentValues;
import android.database.Cursor;

class Turn
{
    public static final String TABLE = "turns";

    public static final String COLUMN_NAME_ID               = "_id";
    public static final String COLUMN_NAME_NAME             = "name";
    public static final String COLUMN_NAME_TYPE             = "type";
    public static final String COLUMN_NAME_INITIATIVE_ROLL  = "initiative_roll";
    public static final String COLUMN_NAME_INITIATIVE_BONUS = "initiative_bonus";
    public static final String COLUMN_NAME_TOTAL_TIME       = "total_time";
    public static final String COLUMN_NAME_TOTAL_TURNS      = "total_turns";
    public static final String COLUMN_NAME_ENCOUNTER_TIME   = "encounter_time";
    public static final String COLUMN_NAME_ENCOUNTER_TURNS  = "encounter_turns";
    public static final String COLUMN_NAME_ROUND_TIME       = "round_time";
    public static final String COLUMN_NAME_POSITION         = "position";

    public static final String COLUMN_TYPE_ID               = "integer primary key autoincrement";
    public static final String COLUMN_TYPE_NAME             = "text not null";
    public static final String COLUMN_TYPE_TYPE             = "integer not null";
    public static final String COLUMN_TYPE_INITIATIVE_ROLL  = "integer not null";
    public static final String COLUMN_TYPE_INITIATIVE_BONUS = "integer not null";
    public static final String COLUMN_TYPE_TOTAL_TIME       = "integer not null";
    public static final String COLUMN_TYPE_TOTAL_TURNS      = "integer not null";
    public static final String COLUMN_TYPE_ENCOUNTER_TIME   = "integer not null";
    public static final String COLUMN_TYPE_ENCOUNTER_TURNS  = "integer not null";
    public static final String COLUMN_TYPE_ROUND_TIME       = "integer not null";
    public static final String COLUMN_TYPE_POSITION         = "integer not null";

    public static final String [] COLUMN_NAMES =
    {
        COLUMN_NAME_ID,
        COLUMN_NAME_NAME,
        COLUMN_NAME_TYPE,
        COLUMN_NAME_INITIATIVE_ROLL,
        COLUMN_NAME_INITIATIVE_BONUS,
        COLUMN_NAME_TOTAL_TIME,
        COLUMN_NAME_TOTAL_TURNS,
        COLUMN_NAME_ENCOUNTER_TIME,
        COLUMN_NAME_ENCOUNTER_TURNS,
        COLUMN_NAME_ROUND_TIME,
        COLUMN_NAME_POSITION
    };

    public static final String [] COLUMN_TYPES =
    {
        COLUMN_TYPE_ID,
        COLUMN_TYPE_NAME,
        COLUMN_TYPE_TYPE,
        COLUMN_TYPE_INITIATIVE_ROLL,
        COLUMN_TYPE_INITIATIVE_BONUS,
        COLUMN_TYPE_TOTAL_TIME,
        COLUMN_TYPE_TOTAL_TURNS,
        COLUMN_TYPE_ENCOUNTER_TIME,
        COLUMN_TYPE_ENCOUNTER_TURNS,
        COLUMN_TYPE_ROUND_TIME,
        COLUMN_TYPE_POSITION
    };

    public enum TurnType
    {
        PLAYER         ( "Player" ),
        DUNGEON_MASTER ( "Dungeon Master" );

        private String mSelectionText;

        private TurnType( String selectionText )
        {
            mSelectionText = selectionText;
        }

        public String getSelectionText()
        {
            return mSelectionText;
        }

        public static TurnType findBySelectionText( String selectionText )
        {
            for ( TurnType t : values() )
            {
                if ( t.getSelectionText().equals( selectionText ) )
                {
                    return t;
                }
            }

            return null;
        }
    }

    public int mId;
    public String mName;
    public TurnType mType;
    public int mInitiativeRoll;
    public int mInitiativeBonus;
    public long mTotalTime;
    public int mTotalTurns;
    public long mEncounterTime;
    public int mEncounterTurns;
    public long mRoundTime;
    public int mPosition;

    public static ContentValues makeContentValues( Turn turn )
    {
        ContentValues values = new ContentValues();
        values.put( COLUMN_NAME_NAME, turn.mName );
        values.put( COLUMN_NAME_TYPE, turn.mType.toString() );
        values.put( COLUMN_NAME_INITIATIVE_ROLL, turn.mInitiativeRoll );
        values.put( COLUMN_NAME_INITIATIVE_BONUS, turn.mInitiativeBonus );
        values.put( COLUMN_NAME_TOTAL_TIME, turn.mTotalTime );
        values.put( COLUMN_NAME_TOTAL_TURNS, turn.mTotalTurns );
        values.put( COLUMN_NAME_ENCOUNTER_TIME, turn.mEncounterTime );
        values.put( COLUMN_NAME_ENCOUNTER_TURNS, turn.mEncounterTurns );
        values.put( COLUMN_NAME_ROUND_TIME, turn.mRoundTime );
        values.put( COLUMN_NAME_POSITION, turn.mPosition );
        return values;
    }

    public static Turn makeTurn( Cursor cursor )
    {
        Turn turn = new Turn(
            cursor.getString( cursor.getColumnIndex( COLUMN_NAME_NAME ) ),
            TurnType.valueOf( cursor.getString( cursor.getColumnIndex( COLUMN_NAME_TYPE ) ) ),
            cursor.getInt( cursor.getColumnIndex( COLUMN_NAME_INITIATIVE_ROLL ) ),
            cursor.getInt( cursor.getColumnIndex( COLUMN_NAME_INITIATIVE_BONUS ) )
        );

        turn.mId = cursor.getInt( cursor.getColumnIndex( COLUMN_NAME_ID ) );
        turn.mTotalTime = cursor.getLong( cursor.getColumnIndex( COLUMN_NAME_TOTAL_TIME ) );
        turn.mTotalTurns = cursor.getInt( cursor.getColumnIndex( COLUMN_NAME_TOTAL_TURNS ) );
        turn.mEncounterTime = cursor.getLong( cursor.getColumnIndex( COLUMN_NAME_ENCOUNTER_TIME ) );
        turn.mEncounterTurns = cursor.getInt( cursor.getColumnIndex( COLUMN_NAME_ENCOUNTER_TURNS ) );
        turn.mRoundTime = cursor.getLong( cursor.getColumnIndex( COLUMN_NAME_ROUND_TIME ) );
        turn.mPosition = cursor.getInt( cursor.getColumnIndex( COLUMN_NAME_POSITION ) );

        return turn;
    }

    Turn( String name, TurnType type, int initiativeRoll, int initiativeBonus )
    {
        mId = -1;
        mName = name;
        mType = type;
        mInitiativeRoll = initiativeRoll;
        mInitiativeBonus = initiativeBonus;
        mTotalTime = 0;
        mTotalTurns = 0;
        mEncounterTime = 0;
        mEncounterTurns = 0;
        mRoundTime = 0;
        mPosition = 0;
    }

    public int totalInitiative()
    {
        return mInitiativeRoll + mInitiativeBonus;
    }

    public long totalTimeAverage()
    {
        return ( mTotalTurns == 0 ? 0 : mTotalTime / mTotalTurns );
    }

    public long encounterTimeAverage()
    {
        return ( mEncounterTurns == 0 ? 0 : mEncounterTime / mEncounterTurns );
    }

    public void addTurnTime( long turnTime )
    {
        mTotalTime += turnTime;
        mEncounterTime += turnTime;
        mRoundTime += turnTime;
    }

    public void turnFinished()
    {
        mTotalTurns += 1;
        mEncounterTurns += 1;
        mRoundTime = 0;
    }

    public void resetEncounterTime()
    {
        mEncounterTime = 0;
        mRoundTime = 0;
    }
}
