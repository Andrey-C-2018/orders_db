package instr;

import java.util.Vector;
import java.util.Date;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class SpecialDays {

	private interface IDateComparable {
		boolean equals(Date date);
		boolean lessThan(Date date);
		boolean biggerThan(Date date);
	}
	
	private class SpecialDaysRange implements IDateComparable{

		public Date begin_date = null;
		public Date end_date = null;
		public String descr = null;
		@Override
		public boolean equals(Date date) {
			
			return date.equals(begin_date) || date.equals(end_date) ||
					(date.after(begin_date) && date.before(end_date));
		}
		@Override
		public boolean lessThan(Date date) {
			
			return date.before(begin_date);
		}
		@Override
		public boolean biggerThan(Date date) {
			
			return date.after(end_date);
		}
	}

	private class Vacation extends SpecialDaysRange {
		public static final int VACATION_REGULAR = 0;
		public static final int VACATION_UNPAID = 1;
		
		public int id_worker = -1;
		public int type = -1;
	}

	private Vector<SpecialDaysRange> holidays_cache;
	private Vector<Vacation> vacations_cache;
	
	private abstract class SpecialDaysBasicIterator {
		private int curr_index = 0;
		
		protected SpecialDaysBasicIterator() { }
		protected SpecialDaysBasicIterator(int index) { curr_index = index; }
		
		public abstract SpecialDaysIterator clone();
		
		public final int getIndex() { return curr_index; }
		
		public final void setIndex(SpecialDaysBasicIterator obj) { curr_index = obj.curr_index; }
		
		public final void setIndexToTheMiddleOfRange(SpecialDaysBasicIterator l, 
													SpecialDaysBasicIterator r) {
			
			curr_index = (l.curr_index + r.curr_index) / 2;
		}
		
		public final boolean lessThan(SpecialDaysBasicIterator obj) { 
			
			return curr_index < obj.curr_index; 
		}
		
		public final SpecialDaysBasicIterator inc() { ++curr_index; return this; }
		
		public abstract IDateComparable get();
	}
	
	private class SpecialDaysIterator extends SpecialDaysBasicIterator {
		private Vector<SpecialDaysRange> v;
		
		public SpecialDaysIterator(Vector<SpecialDaysRange> v) { this.v = v; }
		public SpecialDaysIterator(Vector<SpecialDaysRange> v, int index) { 
		
			super(index);
			this.v = v;
		}
		@Override
		public SpecialDaysIterator clone() { 
			
			return new SpecialDaysIterator(v, getIndex()); 
		}
		@Override
		public IDateComparable get() {
			
			int curr_index = getIndex();
			assert(curr_index < v.size());
			return v.elementAt(curr_index);
		}
	}
	
	private class VacationIterator extends SpecialDaysBasicIterator {
		private Vector<Vacation> v;
		
		public VacationIterator(Vector<Vacation> v) { this.v = v; }
		public VacationIterator(Vector<Vacation> v, int index) { 
		
			super(index);
			this.v = v;
		}
		
		@Override
		public VacationIterator clone() { 
			
			return new VacationIterator(v, getIndex()); 
		}
		@Override
		public IDateComparable get() {
			
			int curr_index = getIndex();
			assert(curr_index < v.size());
			return v.elementAt(curr_index);
		}
	}
	
	private static final int NOT_FOUND = -1;
	private static int binary_search(SpecialDaysIterator l, SpecialDaysIterator r, 
									Date date_to_search) {
		
		SpecialDaysIterator m = l.clone();
		while(l.lessThan(r)) {
			m.setIndexToTheMiddleOfRange(l, r);
			if(m.get().lessThan(date_to_search)) {
				l.setIndex(m);
				l.inc();
			}
			else if(m.get().biggerThan(date_to_search))
				r.setIndex(m);
			else
				return m.getIndex();
		}
		
		return NOT_FOUND;
	}

	//----------------------------------------------------------
	
	public static int DIRECTION_LEFT = 0;
	public static int DIRECTION_RIGHT = 1;
	
	public SpecialDays(Connection conn) throws SQLException {
		
		assert(conn != null);
		holidays_cache = new Vector<SpecialDaysRange>();
		
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
		
		vacations_cache = new Vector<Vacation>();
		
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
		
		int end = holidays_cache.size();
		return binary_search(new SpecialDaysIterator(holidays_cache), 
							new SpecialDaysIterator(holidays_cache, end), 
								date) != NOT_FOUND;
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
					(date.after(begin_date) || date.equals(begin_date)) &&
					(end_date.after(date) || end_date.equals(date));
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
					(date.after(begin_date) || date.equals(begin_date)) &&
					(end_date.after(date) || end_date.equals(date));
		}
	
		return found;
	}

	public Date findNearestWorkingDay(Date date, int direction) {
		
		return date;
	}
}