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
	            System.out.print("세 수를입력해 주세요(ex: 1 2 3) -->");
	            //ByteBuffer
	            for(int i =0; i<arr.length; i++) {
	               arr[i] = sc.nextInt();
	            }
	            client.resetByteBuffer();
	 
	            for(int i = 0; i<arr.length; i++) { // sendByteBuffer에 입력한 정수를 담는 for문
	               sendByteBuffer.putInt(arr[i]);
	            }
	 
	 
	            os.write(sendByteBuffer.array()); // 입력된 bytebuffer를 array()로 전달한다.
	            os.flush();
				
	            byte[] buff = new byte[8]; // 서버에서 strike와 ball int형 크기가 2인 배열을 전달해주기 때문에 8byte 배열을 할당.
	            int read = 0;
	            read = bis.read(buff, 0, 8); // 0부터 8 까지 전달
	            if(read<0) break;
	            int strike = 0;
	            strike = client.byteToint(buff);
	            
	            if(strike == 3 && count < 10) {
	            	buff = new byte[4];
	            	read = bis.read(buff,0,4);
	            	point = (buff[0] & 0xff)<<24 | (buff[1] & 0xff)<<16 | (buff[2] & 0xff)<<8 | (buff[3] & 0xff); // 점수를 int형으로 전환해주는 함수
	            	System.out.println("플레이어 승리");
	            	System.out.printf("100점을 획득하셨습니다! 플레이어의 점수는 %d점입니다.",point);
	            	System.out.println("");
	            	
	            	if(client.YN()) {
	            		client.restart(); // 재시작 위한 함수
	            	}else break;
	            	continue;
	            }
	            
	            if(count>=10) {
	            	System.out.println("플레이어 패배");
	            	if(client.YN()) {
	            		client.restart();  // 재시작 위한 함수
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
		System.out.println("야구 게임을 시작합니다.");
		System.out.println("숫자는 0부터 9까지 입니다.");
	}
	
	public int byteToint(byte[] arr){ // Byte -> Int형으로 변환해주는 메서드
	       int num1 = (arr[0] & 0xff)<<24 | (arr[1] & 0xff)<<16 | (arr[2] & 0xff)<<8 | (arr[3] & 0xff);
	       int num2 = (arr[4] & 0xff)<<24 | (arr[5] & 0xff)<<16 | (arr[6] & 0xff)<<8 | (arr[7] & 0xff);
	       System.out.println(String.format("%d 스트라이크 %d볼", num1, num2));
	       return num1;
	    }
	
	public boolean YN() {
		String check;
		while(true) {
			System.out.print("계속 하시겠습니까?(y/n)?");
			check = sc.next();
			if(check.equals("y") || check.equals("Y")) {
				return true;
			}else if(check.equals("n") || check.equals("N")) {
				System.out.println("게임이 종료됩니다. 이용해주셔서 감사합니다.");
				return false;
			}else {
				System.out.println("다시 입력하세요");
			}
		}
		
	}
	public void resetByteBuffer() { // 숫자 세 개에 대해 입력받기 위해 초기화 해주는 메서드
		sendByteBuffer = null;
        sendByteBuffer = ByteBuffer.allocate(12); // C에서 int형 배열 크기 3인것으로 recv하기 때문에 4byte * 3인 12byte를 할당해야함
        sendByteBuffer.order(ByteOrder.BIG_ENDIAN);
	}
	public void restart() {
		count = 0;
		client.resetByteBuffer();
        for(int i = 0; i<arr.length; i++) { // sendByteBuffer에 입력한 정수를 담는 for문
               sendByteBuffer.putInt(0);
            }
        try {
			os.write(sendByteBuffer.array()); // 입력된 bytebuffer를 array()로 전달한다.
			os.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
