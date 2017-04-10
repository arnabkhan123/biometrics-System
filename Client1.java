import java.io.*;
import java.net.*;
import java.lang.*;
public class Client1
{
     public static void main(String argv[]) {
      BufferedOutputStream oos = null;
      BufferedInputStream ois = null;
      
      java.util.Date date = null;
      try {
       
        Socket socket = new Socket("192.168.80.133", 8899);
      
	
       oos = new BufferedOutputStream(socket.getOutputStream());
        ois = new BufferedInputStream(socket.getInputStream());
        
        oos.flush();
      //  oos.write(new String("Desert.pgm"));
        
        oos.flush();
       // oos.reset();
        int sz=(Integer )ois.read();
        System.out.println ("Receving "+(sz/1024)+" Bytes From Sever");
         
        byte b[]=new byte [sz];
        int bytesRead = ois.read(b, 0, b.length);
          for (int i = 0; i<sz; i++)
            {
                System.out.print(b[i]);
            }
            FileOutputStream fos=new FileOutputStream(new File("img.pgm"));
            fos.write(b,0,b.length);
        System.out.println ("From Server : "+ois.read());
        oos.close();
        ois.close();
      } catch(Exception e) {
System.out.println("hello!!!!!!!!!!\n");
        System.out.println(e.getMessage());
        e.printStackTrace();
      }
}
}