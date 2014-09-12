package startat.de.thegame.client.communication;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class OutPacket {
	ByteArrayOutputStream byteOut = new ByteArrayOutputStream();
	DataOutputStream buf = new DataOutputStream(byteOut);

	public void send(Socket sock) {
		byte[] message = byteOut.toByteArray();
		try {
			DataOutputStream out = new DataOutputStream(sock.getOutputStream());
			out.writeInt(message.length);
			out.write(message);
			out.flush();
		} catch (IOException e) {
			System.err.println("Error sending:" + message + " through Socket:"
					+ sock);

			e.printStackTrace();
		}
	}

	public void reset(){
		byteOut.reset();
	}
	
	public void writeInt(Integer i) {
		try {
			buf.writeInt(i);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void writeFloat(Float f) {
		try {
			buf.writeFloat(f);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void writeString(String s) {
		try {
			buf.writeInt(s.length());
			buf.writeBytes(s);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void writeType(Integer type) {
		try {
			buf.writeInt(type);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
