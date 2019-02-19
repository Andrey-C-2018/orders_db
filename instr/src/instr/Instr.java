package instr;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.text.SimpleDateFormat;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.SQLException;
import java.sql.ResultSet;

public class Instr {
	
	private class StaffListItem {
		public int id = -1;
		public String name;
		public Date startwork_date;
		public Date dismiss_date;
		public int id_employer = -1;
	}
	
	private final static int INSTR_OFFSET_IN_MONTHES = 6;
	private final static int MAX_INSTRS_COUNT = 4;
		
	private Connection conn = null;
	private SimpleDateFormat format_german = null;
	private SimpleDateFormat format_sql = null;
	private SpecialDays special_days = null;
	private GregorianCalendar calendar = null;
	
	private StaffListItem[] getWorkersWhoNeedInstr() throws SQLException {
		StaffListItem[] staff_list = null;
		
		assert(conn != null);

		Statement stmt = conn.createStatement();
		
		StringBuilder query = new StringBuilder();
		query.append("SELECT COUNT(*) FROM (");
		int main_query_begin = query.length();

		query.append("SELECT s.id, s.name, sa.startwork_date, sa.dismiss_date, sa.id_employer ");
		query.append("FROM staff s INNER JOIN staff_assignments sa ON s.id = sa.id_worker"); 
		query.append(" INNER JOIN (SELECT id_worker, MAX(instr_date) as max_instr_date FROM instr GROUP BY id_worker"); 
		query.append(" UNION SELECT id, '1901-01-01' FROM staff ss LEFT OUTER JOIN instr ii ON ss.id = ii.id_worker WHERE ii.id_worker IS NULL) i");
		query.append(" ON s.id = i.id_worker "); 
		query.append("WHERE sa.dismiss_date > DATE_ADD(i.max_instr_date, INTERVAL ");
		query.append(INSTR_OFFSET_IN_MONTHES); 
		query.append(" MONTH)"); 
		query.append(" OR (sa.dismiss_date IS NULL AND DATE_ADD(i.max_instr_date, INTERVAL ");
		query.append(INSTR_OFFSET_IN_MONTHES); 
		query.append(" MONTH) <= NOW())"); 
		query.append(" OR (sa.startwork_date > i.max_instr_date AND sa.startwork_date <= NOW())");

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
	    	staff_list[i].id_employer = rs.getInt("id_employer");
			++i;
		}
	    			
	    rs.close();
	    stmt.close();
				
		return staff_list;
	}
	
	private StaffListItem[] mergeAssigmentRangesIfNecessary(StaffListItem workers[], 
															GregorianCalendar calendar) {
		class Gap {
			int start;
			int end;
			Gap(int start, int end) { this.start = start; this.end = end; }
		}
		ArrayList<Gap> gaps = new ArrayList<Gap>();
			
		for(int i = 1; i < workers.length; ++i) {
			if(workers[i - 1].dismiss_date == null) continue;
			
			calendar.setTime(workers[i - 1].dismiss_date);
			calendar.add(Calendar.DAY_OF_MONTH, 1);
			Date date_next_to_dismiss = calendar.getTime();
			
			if(workers[i - 1].id_employer == workers[i].id_employer &&
					workers[i - 1].id == workers[i].id &&
					workers[i].startwork_date.equals(date_next_to_dismiss)) {
				
				workers[i].startwork_date = workers[i - 1].startwork_date;
				workers[i - 1] = null;
				
				int gaps_size = gaps.size();
				if(gaps_size > 0 && gaps.get(gaps_size - 1).end == i - 1)
					++gaps.get(gaps_size - 1).end;
				else
					gaps.add(new Gap(i - 1, i));
			}
		}
		
		if(gaps.size() != 0) {
			int start = gaps.get(0).start;
		
			gaps.add(new Gap(workers.length, workers.length));
			for (int j = 0; j < gaps.size() - 1; ++j) {

				int end = gaps.get(j).end;
				int next = gaps.get(j + 1).start;

				for(int i = end, k = start; i < next; ++i, ++k) {
					assert(workers[k] == null);
					workers[k] = workers[i];
					workers[i] = null;
				}
				start += next - end;
			}
			
			workers = Arrays.copyOfRange(workers, 0, start);
		}
		
		return workers;
	}
	
	private void performInstr(int id_worker, Date instr_date, 
								String instr_type) throws SQLException {

		StringBuilder query = new StringBuilder();

		Statement stmt = conn.createStatement();

		query.append("INSERT INTO instr VALUES(");
		query.append(id_worker);
		query.append(", '");
		query.append(format_sql.format(instr_date));
		query.append("', '");
		query.append(instr_type);
		query.append("')");

		stmt.executeUpdate(query.toString());
		stmt.close();
	}

	private void performInitialInstr(int id_worker, Date instr_date) throws SQLException {

		assert(instr_date != null);
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
		
		return date != null ? format_german.format(date) : "NULL";
	}
	
	private int getInstrsCount(Date date) throws SQLException {
		
		assert(conn != null);

		Statement stmt = conn.createStatement();
		
		StringBuilder query = new StringBuilder();
		query.append("SELECT COUNT(id_worker) as count_instr FROM instr ");
		query.append("WHERE instr_date = '");
		query.append(format_sql.format(date));
		query.append("'");

	    ResultSet rs = stmt.executeQuery(query.toString());
	    int count_instr = 0;
	    if(rs.next())
	    	count_instr = rs.getInt("count_instr");
	    		
	    rs.close();
	    stmt.close();
	    
	    return count_instr;
	}
	
	private Date calcNextInstrDate(int id_worker, 
									Date prev_instr_date, 
									GregorianCalendar calendar) throws SQLException, InstrException {
		
		calendar.setTime(prev_instr_date);
		calendar.add(Calendar.MONTH, INSTR_OFFSET_IN_MONTHES);
	
		Date next_instr_date = calendar.getTime();
		next_instr_date = special_days.findNearestWorkingDay(id_worker, next_instr_date, 
													SpecialDays.DIRECTION_LEFT, 
													calendar);
		while(getInstrsCount(next_instr_date) >= Instr.MAX_INSTRS_COUNT) {
			calendar.setTime(next_instr_date);
			calendar.add(Calendar.DAY_OF_MONTH, -1);
			
			next_instr_date = calendar.getTime();
			next_instr_date = special_days.findNearestWorkingDay(id_worker, next_instr_date, 
														SpecialDays.DIRECTION_LEFT, 
														calendar);
		}
		
		if(prev_instr_date.equals(next_instr_date) || 
				prev_instr_date.after(next_instr_date))
			throw new InstrException(InstrException.E_NEXT_INSTR, 
									"Can't evaluate next instr date! id worker = "
										+ id_worker + ", previous instr date = "
										+ format_german.format(prev_instr_date));
				
		return next_instr_date;
	}
	
	//----------------------------------------------------------
	
	public Instr(Connection conn) throws SQLException {
	
		this.conn = conn;
		
		format_german = new SimpleDateFormat("dd.MM.yyyy");
		format_sql = new SimpleDateFormat("yyyy-MM-dd");
		special_days = new SpecialDays(conn);
		
		calendar = new GregorianCalendar();
	}

	public void evaluateInstr() throws SQLException, InstrException {
		
		Date now = new Date();
		
		StaffListItem workers[] = getWorkersWhoNeedInstr();
		if(workers == null) return;
		workers = mergeAssigmentRangesIfNecessary(workers, calendar);
				
		for(int i = 0; i < workers.length; ++i){
			
			int id = workers[i].id;
			Date prev_instr_date = getPrevInstrDate(id);
			Date startwork_date = workers[i].startwork_date;
			Date dismiss_date = workers[i].dismiss_date;
			
			if(prev_instr_date == null || 
					(prev_instr_date != null && startwork_date.after(prev_instr_date))) {
				
				performInitialInstr(id, startwork_date);
				prev_instr_date = startwork_date;
			}
				
			if(dismiss_date == null) dismiss_date = now;
			
			Date next_instr_date = calcNextInstrDate(id, prev_instr_date, calendar);
			while(dismiss_date.after(next_instr_date)) {
					
				performInstr(id, next_instr_date);
				next_instr_date = calcNextInstrDate(id, next_instr_date, calendar);
			}
		}
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
			System.out.print(rs.getInt("id"));
			System.out.print(' ');
			System.out.print(rs.getString("name") + ' ');
			System.out.print(rs.getString("position") + ' ');
			System.out.print(DateToString(rs.getDate("startwork_date")));
			System.out.print(' ');
			System.out.print(DateToString(rs.getDate("dismiss_date")));
			System.out.print(' ');
			System.out.print(DateToString(rs.getDate("instr_date")));
			System.out.println();
		}
	    			
	    stmt.close();
	}
}
