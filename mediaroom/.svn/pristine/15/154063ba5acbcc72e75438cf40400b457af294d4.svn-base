import java.net.URLDecoder;

import com.ipvs.common.service.MRXMLUtils;


public class UnicodeTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		System.out.println("Memory Size="+Runtime.getRuntime().totalMemory());
		String xmlString = "";

		String xmlString1 = ""; 

		//System.out.println("Memory Size="+Runtime.getRuntime().freeMemory());
		int i=0;
		int max =1;
		try {
			while(i <max) {
			/*System.out.println("Run ="+ (++i));
			String encodeString = URLEncoder.encode(xmlString, "UTF-8");
			System.out.println("String Length ="+encodeString.getBytes().length);
			
			System.out.println("Memory Size="+Runtime.getRuntime().freeMemory());
	*/		String decodeString  = URLDecoder.decode(xmlString, "UTF-8");
			System.out.println("xml ----->"+decodeString);
			System.out.println("xml ----->"+MRXMLUtils.stringToDocument(decodeString));
			
			String decodeString1  = URLDecoder.decode(xmlString1, "UTF-8");
			System.out.println("xml ----->"+decodeString1);
			System.out.println("xml ----->"+MRXMLUtils.stringToDocument(decodeString1));
		
		
			i++;
			/*System.out.println("String Length ="+decodeString.getBytes().length);
			System.out.println("Memory Size="+Runtime.getRuntime().freeMemory());*/
			}
			
		} catch (Exception e) {			
			e.printStackTrace();
		}
	}
}
		
