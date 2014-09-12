package startat.de.thegame.client;

import java.io.*;
import java.net.InetAddress;
import java.net.Socket;

import startat.de.thegame.client.communication.ClientMessage;
import startat.de.thegame.client.communication.InPacket;
import startat.de.thegame.client.communication.OutPacket;
import startat.de.thegame.client.communication.ServerMessage;

public class Main {
    public static void main(String[] args) {	
    	String host = args.length > 0 ? String.valueOf( args[ 0 ] ) : "127.0.0.1";
        Integer port = args.length > 1 ? Integer.valueOf( args[ 1 ] ) : 7777;
        
        try {
            System.out.println("Resolving Address");
            InetAddress addr = InetAddress.getByName(host);
            
            System.out.println("Connecting Port");
            Socket sock = new Socket(addr, port);
          
            InPacket inPacket = new InPacket();
            inPacket.receive(sock);
            if(inPacket.readType() == ClientMessage.HELLO){
            	String s = inPacket.readString();
            	System.out.println(s);
            }
            
            OutPacket outPacket = new OutPacket();
            outPacket.writeType(ServerMessage.LOGIN);
            outPacket.writeString("StenTheJavaGuy");
            outPacket.send(sock);
            
            sock.close();
            
        } catch (NumberFormatException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
