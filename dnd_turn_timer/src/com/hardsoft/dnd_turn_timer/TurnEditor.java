package com.hardsoft.dnd_turn_timer;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.os.Bundle;
import android.view.View;
import android.view.KeyEvent;
import android.view.View.OnKeyListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Button;
import android.widget.Spinner;
import android.text.method.DigitsKeyListener;

public class TurnEditor extends Activity
{
    private static final String TAG = "TurnEditor";

    private DatabaseAdapter mDatabaseAdapter;

    private Turn mTurn;

    private EditText mNameField;
    private Spinner mTypeField;
    private EditText mInitiativeRollField;
    private EditText mInitiativeBonusField;

    public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.turneditor );

        mDatabaseAdapter = new DatabaseAdapter( this );
        mTurn = null;

        mNameField = ( EditText ) findViewById( R.id.turn_name );
        mTypeField = ( Spinner ) findViewById( R.id.turn_type );
        mInitiativeRollField = ( EditText ) findViewById( R.id.initiative_roll );
        mInitiativeBonusField = ( EditText ) findViewById( R.id.initiative_bonus );

        ArrayAdapter turnTypeAdapter = ArrayAdapter.createFromResource( this, R.array.turn_types, android.R.layout.simple_spinner_item );
        turnTypeAdapter.setDropDownViewResource( android.R.layout.simple_spinner_dropdown_item );
        mTypeField.setAdapter( turnTypeAdapter );

        mInitiativeRollField.setKeyListener( new DigitsKeyListener() );
        mInitiativeBonusField.setKeyListener( new DigitsKeyListener() );

        if ( getIntent().getExtras() != null )
        {
            int turnId = getIntent().getExtras().getInt( TurnList.KEY_TURN_ID );
            mTurn = mDatabaseAdapter.selectTurn( turnId );

            mNameField.setText( mTurn.mName );
            mTypeField.setSelection( turnTypeAdapter.getPosition( mTurn.mType.getSelectionText() ) );
            mInitiativeRollField.setText( Integer.toString( mTurn.mInitiativeRoll ) );
            mInitiativeBonusField.setText( Integer.toString( mTurn.mInitiativeBonus ) );

            setupDeleteButton();
        }

        setupSaveButton();
    }

    private void setupSaveButton()
    {
        final Button saveButton = ( Button ) findViewById( R.id.save );
        saveButton.setOnClickListener( new View.OnClickListener()
        {
            public void onClick( View view )
            {
                String name = mNameField.getText().toString();
                if ( name.length() == 0 ) name = "Anonymous";

                Turn.TurnType type = Turn.TurnType.findBySelectionText( ( String ) mTypeField.getSelectedItem() );
                if ( type == null ) type = Turn.TurnType.PLAYER;

                int initiativeRoll = parseInteger( mInitiativeRollField );
                int initiativeBonus = parseInteger( mInitiativeBonusField );

                if ( mTurn == null )
                {
                    mTurn = new Turn( name, type, initiativeRoll, initiativeBonus ); 
                    mDatabaseAdapter.insertTurn( mTurn );
                }
                else
                {
                    mTurn.mName = name;
                    mTurn.mType = type;
                    mTurn.mInitiativeRoll = initiativeRoll;
                    mTurn.mInitiativeBonus = initiativeBonus;
                    mDatabaseAdapter.updateTurn( mTurn );
                }

                mDatabaseAdapter.resetTurnPositions();
                setResult( RESULT_OK );
                finish();
            }
        } );
    }

    private void setupDeleteButton()
    {
        final Button deleteButton = ( Button ) findViewById( R.id.delete );
        deleteButton.setVisibility( View.VISIBLE );
        deleteButton.setOnClickListener( new View.OnClickListener()
        {
            public void onClick( View view )
            {
                new AlertDialog.Builder( TurnEditor.this )
                    .setMessage( "Delete this turn?" ) // TODO: resourcify
                    .setPositiveButton( "Yep", new DialogInterface.OnClickListener() // TODO: resourcify
                    {
                        public void onClick( DialogInterface dialog, int which )
                        {
                            mDatabaseAdapter.deleteTurn( mTurn.mId );
                            setResult( RESULT_OK );
                            finish();
                        }
                    } )
                    .setNegativeButton( "Nope", null ) // TODO: resourcify
                    .show();
            }
        } );
    }

    private int parseInteger( EditText field )
    {
        String integerText = field.getText().toString();
        return integerText.length() > 0 ? Integer.parseInt( integerText ) : 0;
    }
}
