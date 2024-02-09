package com.ipvs.test;

public class StringBufferTest {
	public  static void  process (String s) {
        for (int i=0; i < s.length(); i++) {
            if (s.charAt(i) != 'A') {
                System.out.println ("Character :" + (int)s.charAt(i));
            }
        }
    }
	

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int ctr=0;
        if (args.length == 0) {
            System.out.println (" Test to reproduce String Buffer corruption ");
            System.out.print ("Usage : java -server -Xms32m -Xmx512m -cp ");
            System.out.print ("mediaroom.jar com.ipvs.test.StringBufferTest ");
            System.out.println ("<buffersize in bytes>");
            System.exit(1);
        }
		int maxBytes= Integer.parseInt(args[0]);
	    int sbchksum;

	    StringBuffer sb=null;
        String val = new String("A");

		sb = new StringBuffer();
        while (sb.length() < maxBytes) {
			sb.append(val);
        }
        System.out.println ("String Buffer Length ->" + sb.length() +
                    " maxBytes :" + maxBytes);
		sbchksum = sb.toString().hashCode();
		while(true) {
			ctr++;
			if (ctr % 50 == 0) {
				System.out.println(" Iteration:" + ctr);
			}
		    String msg = sb.toString();
            StringBuffer msg2=new StringBuffer();
            //char data[] = msg.toCharArray();
            //msg2 = new String(msg);
            msg2.append(msg);
            String msg1 = null;
            msg1 = msg2.toString();
            //StringBufferTest.process(msg1);
		    int msgchksum = msg1.hashCode();
		    if (msgchksum != sbchksum) {
			    System.out.println (" Iteration :" + ctr +" Checksum Original:" + sbchksum + " new (msg1) :" + msgchksum + " msg :" + msg.hashCode());
		    }
			try {
				Thread.sleep(1);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}
		

	}


		



}
