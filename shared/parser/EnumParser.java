import java.util.Scanner;
import java.io.File;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.io.BufferedWriter;
import java.io.FileWriter;

public class EnumParser
{
    public static void main( String[] args )
    {
        String language = args[ 0 ];
        String inFile = args[ 1 ];
        String outFile = args[ 2 ];

        Map< String, Map< String, String > > enumerations =
            new HashMap< String, Map< String, String > >();
       
        // read available enum values
        try
        { 
            String currentEnum = "";

            Scanner in = new Scanner( new File( inFile ) );

            while( in.hasNext() )
            {
                String word = in.next();
                if( word.equals( "enum" ) )
                    currentEnum = in.next();
                else
                {
                    Map< String, String > enumeration;
                    enumeration = enumerations.get( currentEnum );
                    if( enumeration == null )
                    {
                        enumeration = new HashMap< String, String >();
                        enumerations.put( currentEnum, enumeration );
                    }

                    enumeration.put( word, in.next() );
                }
            }

            in.close();
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }

        switch( language.toLowerCase() )
        {
            case "java":
            {
                for( Entry< String, Map< String, String > > entry : enumerations.entrySet() )
                {
                    try
                    {
                        BufferedWriter out = new BufferedWriter( new FileWriter(
                            outFile.replace( "%s", entry.getKey() ) ) );

                        out.append( "package startat.de.thegame.client.communication;\n\n")
						   .append( "public class " )
                           .append( entry.getKey() )
                           .append( "\n{\n" );
                        for( Entry< String, String > en : entry.getValue().entrySet() )
                        {
                            out.append( "    public static final int " )
                               .append( en.getKey() )
                               .append( " = " )
                               .append( en.getValue() )
                               .append( ";\n" );
                        }
                        out.append( "}" );

                        out.close();
                    }
                    catch( Exception e )
                    {
                        e.printStackTrace();
                    }
                }
            } break;

            case "c++":
            {
                try
                {
                    BufferedWriter out = new BufferedWriter( new FileWriter( outFile ) );

                    out.append( "#pragma once\n" );

                    for( Entry< String, Map< String, String > > entry : enumerations.entrySet() )
                    {
                        out.append( "\nenum class " )
                           .append( entry.getKey() )
                           .append( " : uint32_t\n{\n" );
                        for( Entry< String, String > en : entry.getValue().entrySet() )
                        {
                            out.append( "    " )
                               .append( en.getKey() )
                               .append( " = " )
                               .append( en.getValue() )
                               .append( ",\n" );
                        }
                        out.append( "};\n" );
                    }

                    out.close();
                }
                catch( Exception e )
                {
                    e.printStackTrace();
                }
            } break;

            default:
                break;
        }
    }
}
