public class Main {
    public static void main(String[] args) {
	// TODO Auto-generated method stub
	String table_name = "Integer_Numbers";
	String[] families = {"Values"};
	String[] families_result = {"Occurrences"};
	
	try {
	    HBaseManager.creatTable(table_name, families);
	    HBaseManager.creatTable("MR_Results", families_result);
	    
	} catch (Exception e) {
	    // TODO Auto-generated catch block
	    e.printStackTrace();
	}
	
	try {
	    HBaseManager.addRecord(table_name, "1", families[0], "list", "1 1 1 1 2 2 3 4");
	    HBaseManager.addRecord(table_name, "2", families[0], "list", "2 2 2 2 2 2 3 2");
	    HBaseManager.addRecord(table_name, "3", families[0], "list", "2 3 1 1 2 2 3 3");
	    HBaseManager.addRecord(table_name, "4", families[0], "list", "1 4 1 1 2 2 3 1");
	    
	} catch (Exception e) {
	    // TODO Auto-generated catch block
	    e.printStackTrace();
	}	
    }
}
