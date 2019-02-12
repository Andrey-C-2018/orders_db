package instr;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class SpecialDays {

	private class SpecialDaysRange implements Comparable<SpecialDaysRange> {

		public Date begin_date = null;
		public Date end_date = null;
		public String descr = null;

		@Override
		public int compareTo(SpecialDaysRange r) {
			
			return end_date.before(r.begin_date) ? -1 : 
					(begin_date.after(r.end_date) ? 1 : 0);
		}
	}

	private class Vacation extends SpecialDaysRange {
		public static final int VACATION_REGULAR = 0;
		public static final int VACATION_UNPAID = 1;
		
		public int id_worker = -1;
		public int type = -1;
		
		@Override
		public int compareTo(SpecialDaysRange r) {
			
			assert(r instanceof Vacation);
			
			Vacation R = (Vacation) r;
			return id_worker != R.id_worker ? id_worker - R.id_worker : super.compareTo(r);
		}
	}

	private ArrayList<SpecialDaysRange> holidays_cache;
	private ArrayList<Vacation> vacations_cache;

	//----------------------------------------------------------
	
	public static int DIRECTION_LEFT = 0;
	public static int DIRECTION_RIGHT = 1;
	
	public SpecialDays(Connection conn) throws SQLException {
		
		assert(conn != null);
		holidays_cache = new ArrayList<SpecialDaysRange>();
		
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
		
		vacations_cache = new ArrayList<Vacation>();
		
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
	
	public boolean isHoliday(Date date, Calendar calendar) {
		
		calendar.setTime(date);
		int day = calendar.get(Calendar.DAY_OF_WEEK);
		if(day == Calendar.SATURDAY || day == Calendar.SUNDAY)
			return true;
		
		SpecialDaysRange item = new SpecialDaysRange();
		item.begin_date = date;
		item.end_date = date;
		
		int index = Collections.binarySearch(holidays_cache, item);
		return index >= 0;
	}

	public boolean isVacationDay(int id_worker, Date date) {

		Vacation item = new Vacation();
		item.id_worker = id_worker;
		item.begin_date = date;
		item.end_date = date;
		
		int index = Collections.binarySearch(vacations_cache, item);
		return index >= 0;
	}

	public Date findNearestWorkingDay(int id_worker, Date date, 
									int direction, Calendar calendar) {
		
		boolean is_holiday = isHoliday(date, calendar);
		boolean is_vacation_day = isVacationDay(id_worker, date);
		
		if(!is_holiday && !is_vacation_day)
			return date;
		
		return date;
	}
}