package com.hardsoft.dnd_turn_timer;

import java.lang.Math;
import java.lang.Runnable;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.Button;
import android.widget.TextView;
import android.graphics.Color;

public class TurnTimer extends Activity
{
    private static final String TAG = "TurnTimer";

    private static final int MENU_RESET_ENCOUNTER = Menu.FIRST + 1;

    DatabaseAdapter mDatabaseAdapter;
    ApplicationState mApplicationState;

    ArrayList<Turn> mTurns;
    Turn mCurrentTurn;
    Date mCurrentTurnStartTime;
    int mRoundNumber;

    TextView mRoundField;
    TextView mNameField;
    TextView mTimeLeftField;
    Button mPauseResumeButton;
    boolean mPaused; // TODO: This needs to be persistent.

    Handler mUiEventHandler;
    Runnable mUpdateTimeLeftField;
    Timer mCountdownTimer;

    public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.turntimer );

        mDatabaseAdapter = new DatabaseAdapter( this );
        mApplicationState = new ApplicationState( mDatabaseAdapter );

        mTurns = mDatabaseAdapter.selectAllTurns();

        mNameField = ( TextView ) findViewById( R.id.name );
        mRoundField = ( TextView ) findViewById( R.id.round );

        int currentTurnId = mApplicationState.getCurrentTurnId();

        if ( currentTurnId != -1 )
        {
            setCurrentTurn( mDatabaseAdapter.selectTurn( currentTurnId ) );
            mCurrentTurnStartTime = mApplicationState.getCurrentTurnStartTime();
        }
        else resetCurrentTurn(); // TODO: resetEncounter() ?

        updateRound( mApplicationState.getRoundNumber() );
        setupFinishTurnButton();
        setupPauseResumeButton();
        setupCountdownTimer();
    }

    public void onDestroy()
    {
        super.onDestroy();
        mCountdownTimer.cancel();
    }

    public boolean onCreateOptionsMenu( Menu menu )
    {
        super.onCreateOptionsMenu( menu );

        menu.add( 0, MENU_RESET_ENCOUNTER, 0, R.string.menu_reset_encounter )
            .setIcon( android.R.drawable.ic_menu_close_clear_cancel ); // TODO: Make an icon.

        return true;
    }

    public boolean onOptionsItemSelected( MenuItem item )
    {
        super.onOptionsItemSelected( item );

        switch ( item.getItemId() )
        {
            case MENU_RESET_ENCOUNTER:
                resetEncounter();
                return true;
        }

        return false;
    }

    private void setupFinishTurnButton()
    {
        final Button finishTurnButton = ( Button ) findViewById( R.id.finish_turn );
        finishTurnButton.setOnClickListener( new View.OnClickListener()
        {
            public void onClick( View view )
            {
                Turn nextTurn = null;

                for ( int i = 0; i + 1 < mTurns.size(); ++i )
                {
                    Turn turn = mTurns.get( i );

                    if ( turn.mId == mCurrentTurn.mId )
                    {
                        nextTurn = mTurns.get( i + 1 );
                        break;
                    }
                }

                if ( nextTurn == null )
                {
                    nextTurn = mTurns.get( 0 );
                    updateRound( ++mRoundNumber );
                }
                
                mCurrentTurn.addTurnTime( getElapsedTime() );
                mCurrentTurn.turnFinished();
                mDatabaseAdapter.updateTurn( mCurrentTurn );

                setCurrentTurn( nextTurn );
                resetCurrentTurnStartTime();
                updateTimeLeftField();
            }
        } );
    }

    private void setupPauseResumeButton()
    {
        mPaused = false;
        mPauseResumeButton = ( Button ) findViewById( R.id.pause );
        mPauseResumeButton.setOnClickListener( new View.OnClickListener()
        {
            public void onClick( View view )
            {
                if ( mPaused )
                {
                    resume();
                }
                else pause();
            }
        } );
    }

    private void setupCountdownTimer()
    {
        mTimeLeftField = ( TextView ) findViewById( R.id.time_left );

        mUiEventHandler = new Handler();
        mUpdateTimeLeftField = new Runnable()
        {
            public void run() { updateTimeLeftField(); }
        };

        scheduleCountdownTimer();
    }

    private void scheduleCountdownTimer()
    {
        mCountdownTimer = new Timer();
        mCountdownTimer.scheduleAtFixedRate( new TimerTask()
        {
            public void run() { mUiEventHandler.post( mUpdateTimeLeftField ); }
        }, 0, 1000 );
    }

    private void updateTimeLeftField()
    {
        long turnTime;

        if ( mCurrentTurn.mType == Turn.TurnType.PLAYER )
        {
            turnTime = mApplicationState.getPlayerTurnTime();
        }
        else turnTime = mApplicationState.getDmTurnTime();

        long timeLeft = turnTime - mCurrentTurn.mRoundTime - getElapsedTime();
        mTimeLeftField.setText( Util.formatTimePeriod( Math.abs( timeLeft ) ) );

        if ( timeLeft > 0 )
        {
            mTimeLeftField.setTextColor( Color.GREEN );
        }
        else mTimeLeftField.setTextColor( Color.RED );
    }

    private void updateRound( int roundNumber )
    {
        mRoundNumber = roundNumber;
        mApplicationState.setRoundNumber( mRoundNumber );
        mRoundField.setText( Integer.toString( mRoundNumber ) );
    }

    private long getElapsedTime()
    {
        return ( ( new Date() ).getTime() - mCurrentTurnStartTime.getTime() ) / 1000;
    }

    private void resetEncounter()
    {
        Iterator<Turn> it = mTurns.iterator();

        while ( it.hasNext() )
        {
            Turn turn = it.next();
            turn.resetEncounterTime();
            mDatabaseAdapter.updateTurn( turn );
        }

        resetCurrentTurn();
        updateRound( 1 );
    }

    private void resetCurrentTurn()
    {
        Turn first_turn = mTurns.get( 0 );
        mApplicationState.setCurrentTurnId( first_turn.mId );
        resetCurrentTurnStartTime();
        setCurrentTurn( first_turn );
    }

    private void setCurrentTurn( Turn turn )
    {
        mCurrentTurn = turn;
        mNameField.setText( mCurrentTurn.mName );
        mApplicationState.setCurrentTurnId( mCurrentTurn.mId );
    }

    private void resetCurrentTurnStartTime()
    {
        mCurrentTurnStartTime = new Date();
        mApplicationState.setCurrentTurnStartTime( mCurrentTurnStartTime );
    }

    private void pause()
    {
        // TODO: Cause the time-left field to flash.
        mPaused = true;
        mCurrentTurn.addTurnTime( getElapsedTime() );
        mCountdownTimer.cancel();
        mPauseResumeButton.setText( "Resume" ); // TODO resourcify
    }

    private void resume()
    {
        mPaused = false;
        resetCurrentTurnStartTime();
        scheduleCountdownTimer();
        mPauseResumeButton.setText( "Pause" ); // TODO resourcify
    }
}
