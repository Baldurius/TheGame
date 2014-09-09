package startat.de.thegame.client;

import java.io.*;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.*;

public class Main {
	public static void main(String[] args) {
        String host = "127.0.0.1";
        Integer port = 7777;

        if(args.length > 0)
            host = args[0];

        if(args.length > 1)
            port = Integer.valueOf(args[1]);
		
		try {
			System.out.println("Resolving Address");
			InetAddress addr = InetAddress.getByName(host);
			
			System.out.println("Connecting Port");
			Socket sock = new Socket(addr, port);
			
			{ System.out.println("Receiving:");
                DataInputStream in = new DataInputStream(sock.getInputStream());
                int packetSize = in.readInt();
                System.out.println("    Size of packet: " + packetSize);
                int packetType = in.readInt();
                System.out.println("    Type of packet: " + packetType);
            }

			Thread.sleep(1000);
			
            { System.out.println("Sending:");
                DataOutputStream out = new DataOutputStream(sock.getOutputStream());
                out.writeInt(4);
                out.writeInt(0x00000001);
                out.flush();
                sock.getOutputStream().flush();
            }

			Thread.sleep(5000);
			
			sock.close();
			
		} catch (NumberFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
