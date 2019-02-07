package instr;

import java.util.Vector;
import java.util.Date;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class SpecialDays {

	private class SpecialDaysRange {

		public Date begin_date = null;
		public Date end_date = null;
		public String descr = null;
	}

	private class Vacation extends SpecialDaysRange {
		public static final int VACATION_REGULAR = 0;
		public static final int VACATION_UNPAID = 1;
		
		public int id_worker = -1;
		public int type = -1;
	}

	private Vector<SpecialDaysRange> holidays_cache;
	private Vector<Vacation> vacations_cache;

	//----------------------------------------------------------
	
	public static int DIRECTION_LEFT = 0;
	public static int DIRECTION_RIGHT = 1;
	
	SpecialDays(Connection conn) throws SQLException {
		
		assert(conn != null);
		
		Statement stmt = conn.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT * FROM holidays ORDER BY begin_date");
			
		while(rs.next()) {
			SpecialDaysRange item = new SpecialDaysRange();
				
			item.begin_date = rs.getDate("begin_date");
			item.end_date = rs.getDate("end_date");
			item.descr = rs.getString("descr");
			holidays_cache.add(item);
		}
		rs.close();
		
		rs = stmt.executeQuery("SELECT * FROM vacations ORDER BY begin_date");
		while(rs.next()) {
			Vacation item = new Vacation();
				
			item.id_worker = rs.getInt("id_worker");
			item.type = rs.getString("vacation_type") == "Оплачувана" ? 
							Vacation.VACATION_REGULAR : Vacation.VACATION_UNPAID;
			item.begin_date = rs.getDate("begin_date");
			item.end_date = rs.getDate("end_date");
			item.descr = rs.getString("descr");
			vacations_cache.add(item);
		}
		
	    rs.close();
	    stmt.close();
	}
	
	public boolean isHoliday(Date date) {
		
		int i = 0;
		boolean found = false;
		while(!found && i < holidays_cache.size()) {
			
			Date begin_date = holidays_cache.elementAt(i).begin_date;
			Date end_date = holidays_cache.elementAt(i).end_date;
			
			found = (date.after(begin_date) || date == begin_date) &&
					(end_date.after(date) || end_date == date);
		}
	
		return found;
	}

	public boolean isDayOff(int id_worker, Date date) {

		int i = 0;
		boolean found = false;
		while(!found && i < vacations_cache.size()) {
			
			int type = vacations_cache.elementAt(i).type;
			int id = vacations_cache.elementAt(i).id_worker;
			Date begin_date = vacations_cache.elementAt(i).begin_date;
			Date end_date = vacations_cache.elementAt(i).end_date;
			
			found = type == Vacation.VACATION_UNPAID &&
					id == id_worker &&
					(date.after(begin_date) || date == begin_date) &&
					(end_date.after(date) || end_date == date);
		}
	
		return found;
	}

	public boolean isVacationDay(int id_worker, Date date) {

		int i = 0;
		boolean found = false;
		while(!found && i < vacations_cache.size()) {
			
			int type = vacations_cache.elementAt(i).type;
			int id = vacations_cache.elementAt(i).id_worker;
			Date begin_date = vacations_cache.elementAt(i).begin_date;
			Date end_date = vacations_cache.elementAt(i).end_date;
			
			found = type == Vacation.VACATION_REGULAR &&
					id == id_worker &&
					(date.after(begin_date) || date == begin_date) &&
					(end_date.after(date) || end_date == date);
		}
	
		return found;
	}

	public Date findNearestWorkingDay(Date date, int direction) {
		
		return date;
	}
}