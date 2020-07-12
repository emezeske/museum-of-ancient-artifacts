package com.hardsoft.dnd_turn_timer;

import java.util.ArrayList;

import android.app.ListActivity;
import android.content.Intent;
import android.content.Context;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.LayoutInflater;
import android.widget.TextView;
import android.widget.BaseAdapter;
import android.widget.AdapterView;
import android.graphics.Color;

public class TurnList extends ListActivity
{
    private static final String TAG = "TurnList";

    public static final String KEY_TURN_ID = "turn_id";

    private static final int MENU_ADD_TURN    = Menu.FIRST + 1;
    private static final int MENU_SETTINGS    = Menu.FIRST + 2;
    private static final int MENU_SHOW_TIMER  = Menu.FIRST + 3;

    private static final int REQUEST_CODE_CREATE_TURN   = 0;
    private static final int REQUEST_CODE_EDIT_TURN     = 1;
    private static final int REQUEST_CODE_EDIT_SETTINGS = 2;
    private static final int REQUEST_CODE_SHOW_TIMER    = 3;

    private DatabaseAdapter mDatabaseAdapter;
    private ApplicationState mApplicationState;
    private ArrayList<Turn> mTurns;
    private TurnListAdapter mListAdapter;

    class TurnListAdapter extends BaseAdapter
    {
        class ViewHolder
        {
            TextView mName;
            TextView mTotalTime;
            TextView mTotalTimeAverage;
            TextView mEncounterTime;
            TextView mEncounterTimeAverage;
            TextView mInitiativeRoll;
            TextView mInitiativeBonus;
        }

        private LayoutInflater mInflater;
        private Context mContext;
        
        TurnListAdapter( Context context )
        {
            mContext = context;
            mInflater = LayoutInflater.from( mContext );
        }

        public int getCount()
        {
            return mTurns.size();
        }

        public Object getItem( int position )
        {
            return mTurns.get( position );
        }

        public long getItemId( int position )
        {
            return position;
        }

        public View getView( int position, View convertView, ViewGroup parent )
        {
            ViewHolder holder;

            if ( convertView == null )
            {
                convertView = mInflater.inflate( R.layout.turn, null );

                holder = new ViewHolder();
                holder.mName = ( TextView ) convertView.findViewById( R.id.name );
                holder.mTotalTime = ( TextView ) convertView.findViewById( R.id.total_time );
                holder.mTotalTimeAverage = ( TextView ) convertView.findViewById( R.id.total_time_average );
                holder.mEncounterTime = ( TextView ) convertView.findViewById( R.id.encounter_time );
                holder.mEncounterTimeAverage = ( TextView ) convertView.findViewById( R.id.encounter_time_average );
                holder.mInitiativeRoll = ( TextView ) convertView.findViewById( R.id.initiative_roll );
                holder.mInitiativeBonus = ( TextView ) convertView.findViewById( R.id.initiative_bonus );

                convertView.setTag( holder );
            }
            else
            {
                holder = ( ViewHolder ) convertView.getTag();
            }

            Turn turn = mTurns.get( position );

            holder.mName.setText( turn.mName );

            int turnTime;

            if ( turn.mType == Turn.TurnType.PLAYER )
            {
                holder.mName.setTextColor( Color.WHITE );
                turnTime = mApplicationState.getPlayerTurnTime();
            }
            else
            {
                holder.mName.setTextColor( Color.RED );
                turnTime = mApplicationState.getDmTurnTime();
            }

            if ( turn.totalTimeAverage() <= turnTime )
            {
                holder.mTotalTime.setTextColor( Color.GREEN );
                holder.mTotalTimeAverage.setTextColor( Color.GREEN );
            }
            else
            {
                holder.mTotalTime.setTextColor( Color.RED );
                holder.mTotalTimeAverage.setTextColor( Color.RED );
            }

            holder.mTotalTime.setText( Util.formatTimePeriod( turn.mTotalTime ) );
            holder.mTotalTimeAverage.setText( Util.formatTimePeriod( turn.totalTimeAverage() ) );

            if ( turn.encounterTimeAverage() <= turnTime )
            {
                holder.mEncounterTime.setTextColor( Color.GREEN );
                holder.mEncounterTimeAverage.setTextColor( Color.GREEN );
            }
            else
            {
                holder.mEncounterTime.setTextColor( Color.RED );
                holder.mEncounterTimeAverage.setTextColor( Color.RED );
            }

            holder.mEncounterTime.setText( Util.formatTimePeriod( turn.mEncounterTime ) );
            holder.mEncounterTimeAverage.setText( Util.formatTimePeriod( turn.encounterTimeAverage() ) );

            holder.mInitiativeRoll.setText( Integer.toString( turn.mInitiativeRoll ) );
            holder.mInitiativeBonus.setText( Integer.toString( turn.mInitiativeBonus ) );

            return convertView;
        }
    }

    public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );

        mDatabaseAdapter = new DatabaseAdapter( this );
        mApplicationState = new ApplicationState( mDatabaseAdapter );

        mTurns = mDatabaseAdapter.selectAllTurns();
        mListAdapter = new TurnListAdapter( this );
        setListAdapter( mListAdapter );

        getListView().setOnItemLongClickListener( new AdapterView.OnItemLongClickListener()
        {
            public boolean onItemLongClick( AdapterView av, View view, int position, long id )
            {
                return onListItemLongClick( view, position, id );
            }
        } );
    }

    public boolean onListItemLongClick( View view, int position, long id ) 
    {
        Turn turn = mTurns.get( position );
        Intent intent = new Intent( this, TurnEditor.class );
        intent.putExtra( KEY_TURN_ID, turn.mId );
        startActivityForResult( intent, REQUEST_CODE_EDIT_TURN );
        return true;
    }

    public boolean onCreateOptionsMenu( Menu menu )
    {
        super.onCreateOptionsMenu( menu );

        menu.add( 0, MENU_ADD_TURN, 0, R.string.menu_add_turn )
            .setIcon( android.R.drawable.ic_menu_add );
        menu.add( 0, MENU_SETTINGS, 0, R.string.menu_settings )
            .setIcon( android.R.drawable.ic_menu_preferences );
        menu.add( 0, MENU_SHOW_TIMER, 0, R.string.menu_show_timer )
            .setIcon( android.R.drawable.ic_menu_rotate ); // TODO Make an icon

        return true;
    }

    public boolean onOptionsItemSelected( MenuItem item )
    {
        super.onOptionsItemSelected( item );

        switch ( item.getItemId() )
        {
            case MENU_ADD_TURN:
                startActivityForResult( new Intent( this, TurnEditor.class ), REQUEST_CODE_CREATE_TURN );
                return true;

            case MENU_SETTINGS:
                startActivityForResult( new Intent( this, Settings.class ), REQUEST_CODE_EDIT_SETTINGS );
                return true;

            case MENU_SHOW_TIMER:
            {
                startActivityForResult( new Intent( this, TurnTimer.class ), REQUEST_CODE_SHOW_TIMER );
                return true;
            }
        }

        return false;
    }

    protected void onActivityResult( int requestCode, int resultCode, Intent data )
    {
        super.onActivityResult( requestCode, resultCode, data );

        switch ( requestCode )
        {
            case REQUEST_CODE_EDIT_TURN:
            case REQUEST_CODE_CREATE_TURN:
            case REQUEST_CODE_EDIT_SETTINGS:
            case REQUEST_CODE_SHOW_TIMER:
                updateTurns();
                break;
        }
    }

    private void updateTurns()
    {
        mTurns = mDatabaseAdapter.selectAllTurns();
        mListAdapter.notifyDataSetChanged();
    }
}
