package startat.de.thegame.client.communication;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;

public class InPacket {

	DataInputStream data;

	public void receive(Socket sock) {

		byte[] byteArray;

		try {
			DataInputStream in = new DataInputStream(sock.getInputStream());

			Integer packetSize = in.readInt();
			byteArray = new byte[packetSize];

			int readbytes = 0;
			do {
				readbytes += in.read(byteArray);
			} while (readbytes != packetSize);

			data = new DataInputStream(new ByteArrayInputStream(byteArray));

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public Integer readInt() {
		Integer i = null;

		try {
			i = data.readInt();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return i;
	}

	public Float readFloat() {
		Float f = null;

		try {
			f = data.readFloat();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return f;
	}

	public String readString() {
		String s = null;
		try {
			byte[] bytes = new byte[data.readInt()];

			data.readFully(bytes);

			s = new String(bytes);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return s;
	}

	public Integer readType() {
		Integer i = null;
		
		try {
			i = data.readInt();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return i;
	}

}
