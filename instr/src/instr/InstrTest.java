package instr;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class InstrTest {

	public static void main(String args[]) {
		
		Connection conn = null;
		try {
			conn = initConnection();
			
			Instr instr = new Instr(conn);
			
			instr.evaluateInstr("instr_labor");
			//instr.print("instr_fire");
			System.out.println("Done");
		}
		catch(Exception e) {
			
			System.err.println(e.getMessage());
			e.printStackTrace();
		}

		try {
			if (conn != null) conn.close();
		} catch (SQLException e) {
			System.err.println(e.getCause());
		}
	}
	
	private static final Connection initConnection() throws SQLException {
		
		Connection conn = DriverManager.getConnection(
							    	"jdbc:mysql://127.0.0.1:3310/staff",
							    	"staff_user", "123");

	    if (conn == null) {
	     	System.out.println("Can't connect to the db");
	    	System.exit(0);
	    }
	    
	    return conn;
	}
}
