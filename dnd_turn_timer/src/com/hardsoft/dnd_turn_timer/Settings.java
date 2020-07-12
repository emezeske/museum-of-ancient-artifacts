package com.hardsoft.dnd_turn_timer;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.net.Uri;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.View;
import android.view.KeyEvent;
import android.view.View.OnKeyListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Button;
import android.text.method.DigitsKeyListener;

// TODO: Consider extending PreferenceActivity.

public class Settings extends Activity
{
    private static final String TAG = "Settings";

    private ApplicationState mApplicationState;

    private EditText mPlayerTurnTimeMinutesField;
    private EditText mPlayerTurnTimeSecondsField;
    private EditText mDmTurnTimeMinutesField;
    private EditText mDmTurnTimeSecondsField;

    public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.settings );

        mApplicationState = new ApplicationState( new DatabaseAdapter( this ) );

        mPlayerTurnTimeMinutesField = ( EditText ) findViewById( R.id.player_turn_time_minutes );
        mPlayerTurnTimeSecondsField = ( EditText ) findViewById( R.id.player_turn_time_seconds );
        mDmTurnTimeMinutesField = ( EditText ) findViewById( R.id.dm_turn_time_minutes );
        mDmTurnTimeSecondsField = ( EditText ) findViewById( R.id.dm_turn_time_seconds );

        mPlayerTurnTimeMinutesField.setKeyListener( new DigitsKeyListener() );
        mPlayerTurnTimeSecondsField.setKeyListener( new DigitsKeyListener() );
        mDmTurnTimeMinutesField.setKeyListener( new DigitsKeyListener() );
        mDmTurnTimeSecondsField.setKeyListener( new DigitsKeyListener() );

        setTurnTimeText( mPlayerTurnTimeMinutesField, mPlayerTurnTimeSecondsField, mApplicationState.getPlayerTurnTime() );
        setTurnTimeText( mDmTurnTimeMinutesField, mDmTurnTimeSecondsField, mApplicationState.getDmTurnTime() );

        final Button saveButton = ( Button ) findViewById( R.id.save );
        saveButton.setOnClickListener( new View.OnClickListener()
        {
            public void onClick( View view )
            {
                mApplicationState.setPlayerTurnTime( getTurnTime( mPlayerTurnTimeMinutesField, mPlayerTurnTimeSecondsField ) );
                mApplicationState.setDmTurnTime( getTurnTime( mDmTurnTimeMinutesField, mDmTurnTimeSecondsField ) );

                setResult( RESULT_OK );
                finish();
            }
        } );
    }

    private void setTurnTimeText( EditText minutesField, EditText secondsField, int turnTime )
    {
        minutesField.setText( Integer.toString( turnTime / 60 ) ); 
        secondsField.setText( Integer.toString( turnTime % 60 ) ); 
    }

    private int getTurnTime( EditText minutesField, EditText secondsField )
    {
        String minutesText = minutesField.getText().toString();
        String secondsText = secondsField.getText().toString();

        int minutes = minutesText.length() > 0 ? Integer.parseInt( minutesText ) : 0;
        int seconds = secondsText.length() > 0 ? Integer.parseInt( secondsText ) : 0;

        return minutes * 60 + seconds;
    }
}
