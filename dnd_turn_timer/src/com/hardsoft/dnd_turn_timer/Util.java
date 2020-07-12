package com.hardsoft.dnd_turn_timer;

import java.text.NumberFormat;

public class Util
{
    public static String formatTimePeriod( long seconds )
    {
        String formatted = "";
        NumberFormat formatter = NumberFormat.getInstance();
        formatter.setMinimumIntegerDigits( 2 );

        if ( seconds / 3600 > 0 )
        {
            formatted += Long.toString( seconds / 3600 ) + ":";
            formatted += formatter.format( seconds / 60 );
        }
        else formatted += Long.toString( seconds / 60 );

        formatted += ":" + formatter.format( seconds % 60 );
        return formatted;
    }
}
