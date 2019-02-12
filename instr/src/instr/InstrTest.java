package instr;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class InstrTest {

	public static void main(String args[]) {
		
		SimpleDateFormat format_german = new SimpleDateFormat("dd.MM.yyyy");
		Calendar calendar = Calendar.getInstance();
		
		Connection conn = null;
		try {
			conn = initConnection();
			
			SpecialDays d = new SpecialDays(conn);
						
			System.out.println(d.isHoliday(format_german.parse("09.05.2018"), calendar));
			System.out.println(d.isHoliday(format_german.parse("12.05.2018"), calendar));
			System.out.println(d.isVacationDay(5, format_german.parse("16.07.2015")));
			/*Instr instr = new Instr();
			
			instr.evaluateInstr();
			instr.print();*/
		}
		catch(Exception e) {
			
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
