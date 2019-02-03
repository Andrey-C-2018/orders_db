package instr;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.text.SimpleDateFormat;
import java.sql.*;

public class Instr {
	
	private class StaffListItem {
		public int id = -1;
		public String name;
		public Date startwork_date;
		public Date dismiss_date;
	}
	
	private final static int MAX_INSTR_DURATION_IN_DAYS = 10;
	private final static int INSTR_OFFSET_IN_MONTHES = 6;
	
	private Connection conn = null;
	private SimpleDateFormat format_sql = null;
	
	private final void initConnection() {
		
		try {
	    	conn = DriverManager.getConnection(
		    	"jdbc:mysql://127.0.0.1:3306/staff",
		    	"test_user", "123");

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
		
		format_sql = new SimpleDateFormat("yyyy-MM-dd");
	}

	private void performInstr(int id_worker, Date instr_date, String instr_type) {

		StringBuilder query = new StringBuilder();

		try{
			Statement stmt = conn.createStatement();

			query.append("INSERT INTO instr VALUES(");
			query.append(id_worker);
			query.append(", '");
			query.append(instr_date);
			query.append("', '");
			query.append(instr_type);
			query.append("')");

			stmt.executeQuery(query);
		}
		catch(SQLException e) {
			
			e.printStackTrace();
		}
	}

	private void performInitialInstr(int id_worker, Date instr_date) {

		performInstr(id_worker, instr_date, "initial");
	}

	private void performInstr(int id_worker, Date instr_date) {

		performInstr(id_worker, instr_date, "regular");
	}

	public void evaluateInstr() {
		
		GregorianCalendar calendar = new GregorianCalendar();
		
		StaffListItem workers[] = getWorkersWhoNeedInstr();
		
		for(int i = 0; i < workers.length; ++i){
			Date prev_instr_date = getPrevInstrDate(workers[i].id);
			
			if(prev_instr_date == null){
				assert(!isHoliday(workers[i].startwork_date) && 
					!isDayOff(workers[i].startwork_date) && 
					!isVacationDay(workers[i].startwork_date));

				performInitialInstr(workers[i].id, workers[i].startwork_date);
			}
			else {
				calendar.setTime(prev_instr_date);
				calendar.add(Calendar.MONTH, INSTR_OFFSET_IN_MONTHES);
				Date new_instr_date = calendar.getTime();

				assert(new_instr_date >= workers[i].dismiss_date &&
					workers[i].dismiss_date >= prev_instr_date);

				new_instr_date = checkDateAndCorrectIfNecessary(new_instr_date);
				performInstr(workers[i].id, new_instr_date);
			}
		}
	}
	
	private StaffListItem[] getWorkersWhoNeedInstr() {
		StaffListItem[] staff_list = null;
		
		assert(conn != null);
		try {
			Statement stmt = conn.createStatement();
		
		    	StringBuilder query = new StringBuilder();
		    	query.append("SELECT id, name, position, startwork_date, dismiss_date FROM staff ");
		    	query.append("WHERE id IN (SELECT id FROM instr");
			query.append(" WHERE staff.dismiss_date BETWEEN instr_date AND instr_date + INSTR_OFFSET_IN_MONTHES ";
			query.append(" OR staff.startwork_date BETWEEN instr_date AND instr_date + INSTR_OFFSET_IN_MONTHES) ");
			query.append("OR id NOT IN (SELECT DISTINCT id FROM instr) ");
	    		query.append("ORDER BY startwork_date, dismiss_date");
	    	
	    		ResultSet rs = stmt.executeQuery(query);
			staff_list = new StaffListItem[rs.getRowsCount()];
	
			int i = 0;
	    		while (rs.next()) {
				staff_list[i].id = rs.getInt("id");
	    			staff_list[i].name = rs.getString("name");
				staff_list[i].startwork_date = rs.getDate("startwork_date");
	    			staff_list[i].dismiss_date = rs.getString("dismiss_date");
				++i;
			}
	    			
	    		stmt.close();
		}
		catch(SQLException e) {
			
			e.printStackTrace();
		}
		
		return staff_list;
	}

	private Date getPrevInstrDate(int id_worker) {

		assert(id_worker != -1);
		assert(conn != null);

		Date prev_instr_date = null;
		try {
			Statement stmt = conn.createStatement();
		
		    	StringBuilder query = new StringBuilder();
		    	query.append("SELECT MAX(instr_date) as latest_instr_date FROM instr ");
			query.append("WHERE id = ");
			query.append(id_worker);

	    		ResultSet rs = stmt.executeQuery(query);
			prev_instr_date = rs.getDate(latest_instr_date);
		}
		catch(SQLException e) {
			
			e.printStackTrace();
		}

		return prev_instr_date;
	}
	
}
