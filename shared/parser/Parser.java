import java.util.Scanner;
import java.io.File;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.io.BufferedWriter;
import java.io.FileWriter;

public class Parser
{
    public static void main( String[] args )
    {
        try
        {
            Map< String, Integer > serverMessages = new HashMap< String, Integer >();
            Map< String, Integer > clientMessages = new HashMap< String, Integer >();

            Scanner in = new Scanner( new File( "../lists/messages.txt" ) );
            while( in.hasNext() )
            {
                String word = in.next();
                System.out.println( word );
                if( word.equals( "Server" ) )
                {
                    while( true )
                    {
                        word = in.next();
                        if( word.equals( "end" ) )
                            break;

                        Integer value = Integer.valueOf( in.next() );
                        serverMessages.put( word, value );
                    }
                }

                if( word.equals( "Client" ) )
                {
                    while( true )
                    {
                        word = in.next();
                        if( word.equals( "end" ) )
                            break;

                        Integer value = Integer.valueOf( in.next() );
                        clientMessages.put( word, value );
                    }
                }
            }
            in.close();

            { // write c++ contants.hpp
                BufferedWriter out = new BufferedWriter( new FileWriter( new File( "../constants.hpp" ) ) );
                out.append( "#pragma once\n" )
                   .append( "\n" )
                   .append( "enum class SMsg : uint32_t\n" )
                   .append( "{\n" );
                for( Entry< String, Integer > entry : serverMessages.entrySet() )
                {
                    out.append( "    " )
                       .append( entry.getKey() )
                       .append( " = " )
                       .append( entry.getValue().toString() )
                       .append( ",\n" );
                }
                out.append( "};\n" )
                   .append( "\n" )
                   .append( "enum class CMsg : uint32_t\n" )
                   .append( "{\n" );
                for( Entry< String, Integer > entry : clientMessages.entrySet() )
                {
                    out.append( "    " )
                       .append( entry.getKey() )
                       .append( " = " )
                       .append( entry.getValue().toString() )
                       .append( ",\n" );
                }
                out.append( "};\n" );
                out.close();
            }

            { // write Java constants
                { // server
                    BufferedWriter out = new BufferedWriter( new FileWriter( new File( "../ServerMessage" ) ) );
                    out.append( "public class ServerMessage\n" )
                       .append( "{\n" );
                    for( Entry< String, Integer > entry : serverMessages.entrySet() )
                    {
                        out.append( "    public static final int " )
                           .append( entry.getKey() )
                           .append( " = " )
                           .append( entry.getValue().toString() )
                           .append( ";\n" );
                    }
                    out.append( "}" );
                    out.close();
                }

                { // client
                    BufferedWriter out = new BufferedWriter( new FileWriter( new File( "../ClientMessage" ) ) );
                    out.append( "public class ClientMessage\n" )
                       .append( "{\n" );
                    for( Entry< String, Integer > entry : clientMessages.entrySet() )
                    {
                        out.append( "    public static final int " )
                           .append( entry.getKey() )
                           .append( " = " )
                           .append( entry.getValue().toString() )
                           .append( ";\n" );
                    }
                    out.append( "}" );
                    out.close();
                }
            }
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }
}
