package instr;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.text.SimpleDateFormat;
import java.sql.*;

public class Instr {
	
	private class StaffListItem {
		public int id = -1;
		public String name;
		public Date start_date;
		public Date end_date;
	}
	
	private final static int MAX_INSTR_DURATION_IN_DAYS = 10;
	
	private Connection conn = null;
	private GregorianCalendar calendar = null;
	private SimpleDateFormat format_sql = null;
	
	private final void initConnection() {
		
		try {
	    	conn = DriverManager.getConnection(
		    	"jdbc:mysql://10.37.10.252:3310/orders",
		    	"orders_stat_user", "123");

	    	if (conn == null) {
	    		System.out.println("Can't connect to the db");
	    		System.exit(0);
	    	}
		}
		catch (SQLException e) {
			
			e.printStackTrace();
		} 
		
		closeConnection();
	}
	
	private void closeConnection() {
		
		try {
			if (conn != null) conn.close();
		}
		catch(SQLException e) {
			
			System.err.println("Can't close the connection");
		}
	}
	
	//----------------------------------------------------------
	
	public Instr() {
	
		initConnection();
		
		calendar = new GregorianCalendar(2016, Calendar.OCTOBER, 12);
		format_sql = new SimpleDateFormat("yyyy-MM-dd");
	}
	
	public static void main(String args[]) { }
	
	public void evaluateInstr() {
		
		assert(conn != null);
		assert(calendar != null);
		
	}
	
	private StaffListItem[] getWorkers() {
		StaffListItem[] staff_list = null;
		int curr_size = 0;
		
		try {
			Statement stmt = conn.createStatement();
	    	
	    	StringBuilder query = new StringBuilder();
	    	query.append("SELECT name, position FROM staff ");
	    	query.append("WHERE start_date <= '");
	    	query.append(format_sql.format(calendar.getTime()));
	    	query.append("' AND end_date >= '");
	    	calendar.add(Calendar.DAY_OF_YEAR, MAX_INSTR_DURATION_IN_DAYS);
	    	query.append(format_sql.format(calendar.getTime()));
	    	query.append("'");
	    	
	    	ResultSet rs = stmt.executeQuery("SELECT type_name FROM order_types");
	
	    	while (rs.next()) {
	    		System.out.println(rs.getRow() + ". " + rs.getString("type_name"));
	    	}
	
	    	stmt.close();
		}
		catch(SQLException e) {
			
			e.printStackTrace();
		}
		
		return staff_list;
	}
	
}
