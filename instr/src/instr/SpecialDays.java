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
		
		public boolean isLeftNeighbourOf(SpecialDaysRange r, Calendar calendar) {
			
			calendar.setTime(r.begin_date);
			calendar.add(Calendar.DAY_OF_MONTH, -1);
			Date decr_date = calendar.getTime();
			
			return end_date.equals(decr_date);
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
		@Override
		public final boolean isLeftNeighbourOf(SpecialDaysRange r, Calendar calendar) {
			
			assert(r instanceof Vacation);
			
			Vacation R = (Vacation) r;
			return id_worker == R.id_worker && super.isLeftNeighbourOf(r, calendar);
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
		
		rs = stmt.executeQuery("SELECT * FROM vacations ORDER BY id_worker, begin_date");
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
	
	private int findHoliday(Date date, SpecialDaysRange item_cached) {
		
		item_cached.begin_date = date;
		item_cached.end_date = date;
		
		return Collections.binarySearch(holidays_cache, item_cached);
	}
	
	private boolean isWeekend(Date date, Calendar calendar) {
		
		calendar.setTime(date);
		int day = calendar.get(Calendar.DAY_OF_WEEK);
		return (day == Calendar.SATURDAY || day == Calendar.SUNDAY);
	}
	
	public boolean isHoliday(Date date, Calendar calendar) {
		
		calendar.setTime(date);
		int day = calendar.get(Calendar.DAY_OF_WEEK);
		if(day == Calendar.SATURDAY || day == Calendar.SUNDAY)
			return true;
		
		SpecialDaysRange item = new SpecialDaysRange();
		
		int index = findHoliday(date, item);
		return index >= 0;
	}

	private int findVacationDay(int id_worker, Date date, Vacation item_cached) {
		
		item_cached.id_worker = id_worker;
		item_cached.begin_date = date;
		item_cached.end_date = date;
		
		return Collections.binarySearch(vacations_cache, item_cached);
	}
	
	public boolean isVacationDay(int id_worker, Date date) {

		Vacation item = new Vacation();
		int index = findVacationDay(id_worker, date, item);
		return index >= 0;
	}

	private Date shiftDateIfAtWeekend(Date date, Calendar calendar) {
		
		calendar.setTime(date);
		int day = calendar.get(Calendar.DAY_OF_WEEK);
		int shift = 0;
		
		if (day == Calendar.SATURDAY) shift = 1;
		else if (day == Calendar.SUNDAY) shift = 2;
		
		if(shift > 0) {
			calendar.add(Calendar.DAY_OF_MONTH, -shift);
			return calendar.getTime();
		}
		return date;
	}
	
	private Date decDate(Date date, Calendar calendar) {
		
		calendar.setTime(date);
		calendar.add(Calendar.DAY_OF_MONTH, -1);
		return calendar.getTime();
	}
	
	public Date findNearestWorkingDay(int id_worker, Date date, 
									int direction, Calendar calendar) {
		
		Vacation item = new Vacation();
		date = shiftDateIfAtWeekend(date, calendar);
		
		int index_holiday = findHoliday(date, item);
		int index_vacation = 0;
		
		while(index_holiday >= 0 || index_vacation >= 0) {
						
			if(index_holiday >= 0) {
				index_holiday = getFirstDayOf(holidays_cache, index_holiday, calendar);
				if(index_holiday >= 0) {
					date = holidays_cache.get(index_holiday).begin_date;
					if(isWeekend(date, calendar)) 
						date = shiftDateIfAtWeekend(date, calendar);
					else date = decDate(date, calendar);
				}
			}
						
			index_vacation = findVacationDay(id_worker, date, item);
			if(index_vacation >= 0) {
				index_vacation = getFirstDayOf(vacations_cache, index_vacation, calendar);
					
				if(index_vacation >= 0) {
					date = vacations_cache.get(index_vacation).begin_date;
					if(isWeekend(date, calendar)) 
						date = shiftDateIfAtWeekend(date, calendar);
					else date = decDate(date, calendar);
				}
			}
			
			index_holiday = findHoliday(date, item);
		}
		return date;
	}
	
	private int getFirstDayOf(ArrayList<? extends SpecialDaysRange> cache, 
													int index, Calendar calendar) {
		
		assert(index >= 0 && index < cache.size());

		while(index > 0 && 
				cache.get(index - 1).isLeftNeighbourOf(cache.get(index), calendar)) {

			--index;
		} 

		return index;
	}
	
}