package Instr;

import Vector;

public class SpecialDays {

	private class SpecialDayItem {

		public Date begin_date;
		public Date end_date;
		public String descr;
	}

	private class DayOffItem : extends SpecialDayItem {
		
		public int id_worker;
	}

	private Vector<SpecialDayItem> holidays_cache;
	private Vector<DayOffItem> days_off_cache;
	private Vector<DayOffItem> vacations_cache;

	public boolean isHoliday() {

		return false;
	}

	public boolean isDayOff() {

		return false;
	}

	public boolean isVacationDay() {

		return false;
	}

}