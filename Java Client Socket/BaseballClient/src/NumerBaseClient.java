import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Scanner;
public class NumerBaseClient {
	public Socket socket = null;
	static Scanner sc = new Scanner(System.in);
	public static ByteBuffer sendByteBuffer = null;
	public static int point = 0;
	public static NumerBaseClient client = null;
	public static OutputStream os = null;
	static BufferedInputStream bis = null;
	public static int [] arr = new int[3];
	public static int count = 0;
	
	
	public static void main(String[] args) throws Exception{
		// TODO Auto-generated method stub
		try {
			InetSocketAddress isa = new InetSocketAddress("192.168.183.246", 9161);
			Socket socket = new Socket();
			socket.connect(isa);
			client =  new NumerBaseClient();
			bis = new BufferedInputStream(socket.getInputStream());
			
			os = socket.getOutputStream();
			while(true) {
	            System.out.print("�� �����Է��� �ּ���(ex: 1 2 3) -->");
	            //ByteBuffer
	            for(int i =0; i<arr.length; i++) {
	               arr[i] = sc.nextInt();
	            }
	            client.resetByteBuffer();
	 
	            for(int i = 0; i<arr.length; i++) { // sendByteBuffer�� �Է��� ������ ��� for��
	               sendByteBuffer.putInt(arr[i]);
	            }
	 
	 
	            os.write(sendByteBuffer.array()); // �Էµ� bytebuffer�� array()�� �����Ѵ�.
	            os.flush();
				
	            byte[] buff = new byte[8]; // �������� strike�� ball int�� ũ�Ⱑ 2�� �迭�� �������ֱ� ������ 8byte �迭�� �Ҵ�.
	            int read = 0;
	            read = bis.read(buff, 0, 8); // 0���� 8 ���� ����
	            if(read<0) break;
	            int strike = 0;
	            strike = client.byteToint(buff);
	            
	            if(strike == 3 && count < 10) {
	            	buff = new byte[4];
	            	read = bis.read(buff,0,4);
	            	point = (buff[0] & 0xff)<<24 | (buff[1] & 0xff)<<16 | (buff[2] & 0xff)<<8 | (buff[3] & 0xff); // ������ int������ ��ȯ���ִ� �Լ�
	            	System.out.println("�÷��̾� �¸�");
	            	System.out.printf("100���� ȹ���ϼ̽��ϴ�! �÷��̾��� ������ %d���Դϴ�.",point);
	            	System.out.println("");
	            	
	            	if(client.YN()) {
	            		client.restart(); // ����� ���� �Լ�
	            	}else break;
	            	continue;
	            }
	            
	            if(count>=10) {
	            	System.out.println("�÷��̾� �й�");
	            	if(client.YN()) {
	            		client.restart();  // ����� ���� �Լ�
	            	}else break;
	            	continue;
	            	
	            }
	            count++;
			}
			bis.close();
			os.close();
			socket.close();
			
		}catch(Exception e) {
			
		}
	}
	
	public NumerBaseClient() {
		System.out.println("�߱� ������ �����մϴ�.");
		System.out.println("���ڴ� 0���� 9���� �Դϴ�.");
	}
	
	public int byteToint(byte[] arr){ // Byte -> Int������ ��ȯ���ִ� �޼���
	       int num1 = (arr[0] & 0xff)<<24 | (arr[1] & 0xff)<<16 | (arr[2] & 0xff)<<8 | (arr[3] & 0xff);
	       int num2 = (arr[4] & 0xff)<<24 | (arr[5] & 0xff)<<16 | (arr[6] & 0xff)<<8 | (arr[7] & 0xff);
	       System.out.println(String.format("%d ��Ʈ����ũ %d��", num1, num2));
	       return num1;
	    }
	
	public boolean YN() {
		String check;
		while(true) {
			System.out.print("��� �Ͻðڽ��ϱ�?(y/n)?");
			check = sc.next();
			if(check.equals("y") || check.equals("Y")) {
				return true;
			}else if(check.equals("n") || check.equals("N")) {
				System.out.println("������ ����˴ϴ�. �̿����ּż� �����մϴ�.");
				return false;
			}else {
				System.out.println("�ٽ� �Է��ϼ���");
			}
		}
		
	}
	public void resetByteBuffer() { // ���� �� ���� ���� �Է¹ޱ� ���� �ʱ�ȭ ���ִ� �޼���
		sendByteBuffer = null;
        sendByteBuffer = ByteBuffer.allocate(12); // C���� int�� �迭 ũ�� 3�ΰ����� recv�ϱ� ������ 4byte * 3�� 12byte�� �Ҵ��ؾ���
        sendByteBuffer.order(ByteOrder.BIG_ENDIAN);
	}
	public void restart() {
		count = 0;
		client.resetByteBuffer();
        for(int i = 0; i<arr.length; i++) { // sendByteBuffer�� �Է��� ������ ��� for��
               sendByteBuffer.putInt(0);
            }
        try {
			os.write(sendByteBuffer.array()); // �Էµ� bytebuffer�� array()�� �����Ѵ�.
			os.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
