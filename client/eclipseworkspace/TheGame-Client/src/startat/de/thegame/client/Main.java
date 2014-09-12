package startat.de.thegame.client;

import java.io.*;
import java.net.InetAddress;
import java.net.Socket;

import startat.de.thegame.client.communication.CLIENT_MESSAGE;
import startat.de.thegame.client.communication.InPacket;
import startat.de.thegame.client.communication.OutPacket;
import startat.de.thegame.client.communication.SERVER_MESSAGE;

public class Main {
    public static void main(String[] args) {	
    	String host = "88.73.206.221";
        Integer port = 7777;
    
        try {
            System.out.println("Resolving Address");
            InetAddress addr = InetAddress.getByName(host);
            
            System.out.println("Connecting Port");
            Socket sock = new Socket(addr, port);
          
            InPacket inPacket = new InPacket();
            inPacket.receive(sock);
            if(inPacket.readType() == CLIENT_MESSAGE.HELLO){
            	String s = inPacket.readString();
            	System.out.println(s);
            }
            
            
            OutPacket outPacket = new OutPacket();
            outPacket.writeType(SERVER_MESSAGE.LOGIN);
            outPacket.writeString("StenTheJavaGuy");
            outPacket.send(sock);
            
            /*
            { System.out.println("Receiving:");
          
            }

            Thread.sleep(1000);
            
            { System.out.println("Loggin in:");
                DataOutputStream out = new DataOutputStream(sock.getOutputStream());
                out.writeInt(16);           // packet size
                out.writeInt(0x00000001);   // packet type (LOGIN)
                out.writeInt(8);            // name length
                out.writeBytes("Testuser"); // name
                out.flush();
            }

            while(true)
            { }
            */
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
