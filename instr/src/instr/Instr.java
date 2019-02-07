package instr;

import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.text.SimpleDateFormat;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.ResultSet;

public class Instr {
	
	private class StaffListItem {
		public int id = -1;
		public String name;
		public Date startwork_date;
		public Date dismiss_date;
	}
	
	private final static int INSTR_OFFSET_IN_MONTHES = 6;
	private final static int MIN_INSTR_OFFSET_IN_MONTHES = 3;
	
	private Connection conn = null;
	private SimpleDateFormat format_german = null;
	private SpecialDays special_days = null;
	
	private final void initConnection() throws SQLException {
		
		conn = DriverManager.getConnection(
		    	"jdbc:mysql://127.0.0.1:3310/staff",
		    	"staff_user", "123");

	    if (conn == null) {
	     	System.out.println("Can't connect to the db");
	    	System.exit(0);
	    }
	}
	
	private void closeConnection() throws SQLException {
		
		if (conn != null) conn.close();
	}
	
	private void performInstr(int id_worker, Date instr_date, 
								String instr_type) throws SQLException {

		StringBuilder query = new StringBuilder();

		Statement stmt = conn.createStatement();

		query.append("INSERT INTO instr VALUES(");
		query.append(id_worker);
		query.append(", '");
		query.append(instr_date);
		query.append("', '");
		query.append(instr_type);
		query.append("')");

		stmt.executeUpdate(query.toString());
		stmt.close();
	}

	private void performInitialInstr(int id_worker, Date instr_date) throws SQLException {

		assert(instr_date != null);
		assert(!special_days.isHoliday(instr_date) && 
				!special_days.isDayOff(id_worker, instr_date) && 
				!special_days.isVacationDay(id_worker, instr_date));
		
		performInstr(id_worker, instr_date, "вступний");
	}

	private void performInstr(int id_worker, Date instr_date) throws SQLException {

		assert(instr_date != null);
		performInstr(id_worker, instr_date, "первинний");
	}

	private Date getPrevInstrDate(int id_worker) throws SQLException {

		assert(id_worker != -1);
		assert(conn != null);

		Date prev_instr_date = null;
		Statement stmt = conn.createStatement();
		
		StringBuilder query = new StringBuilder();
		query.append("SELECT MAX(instr_date) as latest_instr_date FROM instr ");
		query.append("WHERE id_worker = ");
		query.append(id_worker);

	    ResultSet rs = stmt.executeQuery(query.toString());
	    if(rs.next())
	    	prev_instr_date = rs.getDate("latest_instr_date");
	    		
	    rs.close();
	    stmt.close();
		
		return prev_instr_date;
	}
	
	private String DateToString(Date date) {
		
		return format_german.format(date);
	}
	
	//----------------------------------------------------------
	
	public Instr() throws SQLException {
	
		initConnection();
		
		format_german = new SimpleDateFormat("dd.MM.yyyy");
		special_days = new SpecialDays(conn);
	}

	public void evaluateInstr() throws SQLException {
		
		GregorianCalendar calendar = new GregorianCalendar();
		
		StaffListItem workers[] = getWorkersWhoNeedInstr();
		
		for(int i = 0; i < workers.length; ++i){
			
			int id = workers[i].id;
			Date prev_instr_date = getPrevInstrDate(id);
			Date startwork_date = workers[i].startwork_date;
			
			if(prev_instr_date == null)
				performInitialInstr(id, startwork_date);
			else {
				
				if(startwork_date.after(prev_instr_date)) {
					performInitialInstr(id, startwork_date);
					continue;
				}
				
				Date dismiss_date = workers[i].dismiss_date;
				if(dismiss_date.after(prev_instr_date)) {
									
					calendar.setTime(prev_instr_date);
					calendar.add(Calendar.MONTH, INSTR_OFFSET_IN_MONTHES);
				
					Date new_instr_date = calendar.getTime();
				
					Date corr_new_instr_date = special_days.findNearestWorkingDay(new_instr_date, 
															SpecialDays.DIRECTION_LEFT);
					calendar.setTime(prev_instr_date);
					calendar.add(Calendar.MONTH, MIN_INSTR_OFFSET_IN_MONTHES);
					if(corr_new_instr_date.before(calendar.getTime()))
						corr_new_instr_date = special_days.findNearestWorkingDay(new_instr_date, 
															SpecialDays.DIRECTION_RIGHT);
					performInstr(id, corr_new_instr_date);
				}
			}
		}
	}
	
	private StaffListItem[] getWorkersWhoNeedInstr() throws SQLException {
		StaffListItem[] staff_list = null;
		
		assert(conn != null);

		Statement stmt = conn.createStatement();
		
		StringBuilder query = new StringBuilder();
		query.append("SELECT COUNT(*) FROM (");
		int main_query_begin = query.length();
		query.append("SELECT s.id, s.name, sa.startwork_date, sa.dismiss_date "); 
		query.append("FROM staff s INNER JOIN staff_assignments sa ON s.id = sa.id_worker "); 
		query.append("WHERE s.id IN (SELECT id_worker FROM instr GROUP BY id_worker");
		query.append(" HAVING sa.dismiss_date > DATE_ADD(MAX(instr_date), INTERVAL ");
		query.append(INSTR_OFFSET_IN_MONTHES);
		query.append(" MONTH) OR sa.startwork_date > MAX(instr_date)");
		query.append(" OR DATE_ADD(MAX(instr_date), INTERVAL 6 MONTH) <= NOW()) ");
		query.append("OR id NOT IN (SELECT DISTINCT id FROM instr) "); 
		query.append("ORDER BY startwork_date, dismiss_date");
	    int main_query_end = query.length();
	    query.append(") aaa");
	    
	    ResultSet rs = stmt.executeQuery(query.toString());
	    int records_count = 0;
	    if(rs.next()) records_count = rs.getInt(1);
	    	
	    if(records_count == 0) return null;
	    	
		staff_list = new StaffListItem[records_count];
			
		rs = stmt.executeQuery(query.substring(main_query_begin, main_query_end));
	
		int i = 0;
	    while (rs.next()) {
	    	staff_list[i] = new StaffListItem();
	    	
			staff_list[i].id = rs.getInt("id");
	    	staff_list[i].name = rs.getString("name");
			staff_list[i].startwork_date = rs.getDate("startwork_date");
	    	staff_list[i].dismiss_date = rs.getDate("dismiss_date");
			++i;
		}
	    			
	    rs.close();
	    stmt.close();
				
		return staff_list;
	}

	public void print() throws SQLException {
		
		assert(conn != null);
		
		Statement stmt = conn.createStatement();
		
		StringBuilder query = new StringBuilder();

		query.append("SELECT s.id, s.name, sa.position, sa.startwork_date, sa.dismiss_date,");
		query.append(" i.instr_date FROM staff s INNER JOIN staff_assignments sa");
		query.append(" ON s.id = sa.id_worker INNER JOIN instr i ON s.id = i.id_worker "); 
	    query.append("ORDER BY instr_date");
	    		    		    	
	    ResultSet rs = stmt.executeQuery(query.toString());
	    				
	    while (rs.next()) {
			System.out.print(rs.getInt("id") + ' ');
			System.out.print(rs.getString("name") + ' ');
			System.out.print(rs.getString("position") + ' ');
			System.out.print(DateToString(rs.getDate("startwork_date")));
			System.out.print(' ');
			System.out.print(DateToString(rs.getDate("dismiss_date")));
			System.out.print(' ');
			System.out.print(DateToString(rs.getDate("instr_date")));
		}
	    			
	    stmt.close();
	}
}
