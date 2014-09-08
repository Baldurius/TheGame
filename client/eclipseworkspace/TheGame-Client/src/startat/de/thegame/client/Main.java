package startat.de.thegame.client;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;

public class Main {
	public static void main(String[] args) {
		String host = args[0];
		Integer port = Integer.valueOf(args[1]);
		
		System.out.println("attempt send to " + host + ":" + port);
		
		InetAddress addr;
		
		try {
			System.out.println("Resolving Address");
			addr = InetAddress.getByName(host);
			
			System.out.println("Connecting Port");
			Socket sock = new Socket(addr, port);
			
			System.out.println("Creating Reader and receiving");
			
			
			BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			String text = in.readLine();
			System.out.println("Received: " + text);
			
			System.out.println("Creating Writer and sending");

			Thread.sleep(1000);
			
			DataOutputStream dataout = new DataOutputStream(sock.getOutputStream());
			
			dataout.writeInt(9);
			dataout.writeInt(5);
			dataout.writeChars("Hallo");
			
			dataout.flush();
			
//			BufferedWriter out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()));
//			out.write("Hallo");
//			// zeilenumbruch senden
//			out.newLine();
//			out.flush();
//			
			
			Thread.sleep(1000);
			
			in.close();
			dataout.close();
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
