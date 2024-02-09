package com.ipvs.test;

public class TestDBCache {
    
}
//public class TestDBCache {
//    public static void main(String[] args) {
//        try {
//            new TestChacheThread(250, 464, 3, 500).start();
//            Thread.sleep(780);
//            new TestChacheThread(1150, 323, 3, 600).start();
//            Thread.sleep(780);
//            new TestChacheThread(150, 435, 1, 400).start();
//            
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//        System.out.println("Done");
//        //System.exit(0);
//    }
//}
//
//class TestChacheThread extends Thread {
//    int sleep1 = 100;
//    int sleep2 = 200;
//    int iCounter = 0;
//    int iCounterMax = 0;
//    //int enableLogLevel = MRLog.ERROR; // MRLog.ERROR;
//    //int enableLogLevel2 = MRLog.DEBUG;// DEBUG; // MRLog.ERROR;
//    MRLog log = MRLog.getInstance(MRLog.CONSOLE);
//    int count = 500;
//    Element elGroups = null;
//    int iCallSeq = 0;
//    
//    public TestChacheThread(int sleep1, int sleep2, int iCallSeq, int iCounterMax) {
//        this.sleep1 = sleep1;
//        this.sleep2 = sleep2;
//        this.iCallSeq = iCallSeq;
//        this.iCounterMax = iCounterMax;
//    }
//    
//    public void run() {
//        try {
//            //			System.out.println("*****************Total Memory:" + (Runtime.getRuntime().totalMemory() / 1000) + " k");
//            //			System.out.println("*****************Free Memory:" + (Runtime.getRuntime().freeMemory() / 1000) + " k");
//            //			//log.addSystemOut(enableLogLevel2, MRLog.OK, " DBCache ", "2");
//            //			//log.addSystemOut(enableLogLevel2, MRLog.OK, "*****************Total Memory:" + (Runtime.getRuntime().totalMemory() / 1000) + " k", "2");
//            //			//log.addSystemOut(enableLogLevel2, MRLog.OK, "*****************Free Memory:" + (Runtime.getRuntime().freeMemory() / 1000) + " k", "2");
//            //			MRConfig mrConfig = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, "User", log);
//            //			Document retDoc = null;
//            //			String xpath = "//Groups";
//            //			retDoc = mrConfig.getXmlRepresentation(log).getElementFromXpath(true, -1, retDoc, xpath, null, null, log, null, null, null);
//            //			elGroups = retDoc.getRootElement();
//            //			//log.addSystemOut(enableLogLevel2, MRLog.OK, "*****************Free Memory:" + (Runtime.getRuntime().freeMemory() / 1000) + " k", "2");
//            //			
//            //			while (iCounter < iCounterMax) {
//            //				System.out.println("DBCache size " + DBCache.getInstance().size() +" iCounter " + iCounter);
//            //				if(iCallSeq==1){
//            //					remove();
//            //					get();
//            //					add();
//            //				} else if(iCallSeq==2){
//            //					get();
//            //					add();
//            //					remove();
//            //				} else if(iCallSeq==3){
//            //					add();
//            //					get();
//            //					remove();
//            //				}
//            //				iCounter++;
//            //			}
//            //			
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//        System.out.println("Thread Done");
//    }
//    
//    void remove() {
//        try {
//            boolean add = false;
//            for (int i = 0; i < count; i++) {
//                Element temp = DBCache.getInstance().removeDBCache(i + "");
//                if (!add) {
//                    add = true;
//                    Thread.sleep(sleep1);
//                } else {
//                    add = false;
//                    Thread.sleep(sleep2);
//                }
//                if (temp != null) {
//                    //	log.addSystemOut(enableLogLevel2, MRLog.OK, i + "******** "+DBCache.getInstance().size()+" *********removeDBCache "+MRXMLUtils.elementToString(temp)+"  Free Memory:" + (Runtime.getRuntime().freeMemory() / 1000) + " k", "2");
//                }
//            }
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//    }
//    
//    void get() {
//        try {
//            boolean add = false;
//            for (int i = 0; i < count; i++) {
//                Element temp = DBCache.getInstance().getDBCache(i + "");
//                if (!add) {
//                    add = true;
//                    Thread.sleep(sleep1);
//                } else {
//                    add = false;
//                    Thread.sleep(sleep2);
//                }
//                if (temp != null) {
//                    //log.addSystemOut(enableLogLevel2, MRLog.OK, i + "###############  "+DBCache.getInstance().size()+"  ############getDBCache  "+MRXMLUtils.elementToString(temp)+" Free Memory:" + (Runtime.getRuntime().freeMemory() / 1000) + " k", "2");
//                }
//            }
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//    }
//    
//    void add() {
//        try {
//            boolean add = false;
//            for (int i = 0; i < count; i++) {
//                DBCache.getInstance().addDBCache(i + "", (Element) elGroups.clone());
//                if (!add) {
//                    add = true;
//                    Thread.sleep(sleep1);
//                } else {
//                    add = false;
//                    Thread.sleep(sleep2);
//                }
//                
//                //log.addSystemOut(enableLogLevel2, MRLog.OK, i + "!!!!!!!!!!!!!!"+DBCache.getInstance().size()+"!!!!!!!!!!!!!!addDBCache "+MRXMLUtils.elementToString(elGroups)+" Free Memory:" + (Runtime.getRuntime().freeMemory() / 1000) + " k", "2");
//            }
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//    }
//}
