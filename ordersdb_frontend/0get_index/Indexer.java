import java.util.Arrays;

public class Indexer {

	public static void main(String[] args) {

		System.out.println("Common:");
		process("a.zone, c.center_short_name, b.adv_name_short, a.id, a.order_date," +
				"t.type_name, a.client_name, a.bdate, sta.stage_name, aa.cycle," +
				"aa.fee, aa.fee_parus, aa.outg_post, aa.outg_daynight, aa.act_no, aa.id_act,"+
				"aa.act_reg_date, aa.act_date, aa.bank_reg_date, aa.payment_date,"+
				"aa.article, uu.user_full_name, a.reason, a.cancel_order, a.cancel_date, inf.informer_name,"+
				"aa.age, aa.inv, aa.lang, aa.ill, aa.zek, aa.vpr, aa.reduce, aa.change_,"+
				"aa.close, aa.zv, aa.min, aa.nm_suv, aa.zv_kr, aa.No_Ch_Ist, aa.Koef,"+
				"aa.id_stage,a.id_center_legalaid,a.id_adv,a.id_order_type,aa.id_informer,aa.id_checker");

		System.out.println("Leven:");
		process("a.zone, c.center_short_name, b.adv_name_short, a.id, a.order_date," +
				" t.type_name, a.client_name, a.bdate, sta.stage_name, aa.cycle," +
				" aa.fee, aa.fee_parus, aa.outg_post, aa.outg_daynight, aa.act_no, aa.id_act," +
				" aa.act_reg_date, aa.act_date, aa.bank_reg_date, aa.payment_date," +
				" aa.age, aa.inv, aa.lang, aa.ill, aa.zek, aa.vpr, aa.reduce, aa.change_," +
				" aa.close, aa.zv, aa.min, aa.nm_suv, aa.zv_kr, aa.No_Ch_Ist, aa.Koef," +
				" a.reason, a.cancel_order, a.cancel_date, inf.informer_name, aa.article, uu.user_full_name,"+
				"aa.id_stage,a.id_center_legalaid,a.id_adv,a.id_order_type,aa.id_informer,aa.id_checker");

		System.out.println("Duty:");
		process("a.zone, c.center_short_name, b.adv_name_short, a.id, a.order_date," +
				" t.type_name, a.client_name, a.bdate, sta.stage_name, aa.cycle," +
				" a.reason, a.cancel_order, a.cancel_date, inf.informer_name, aa.article," +
				" aa.fee, aa.fee_parus, aa.outg_post, aa.outg_daynight, aa.act_no, aa.id_act," +
				" aa.act_reg_date, aa.act_date, aa.bank_reg_date, aa.payment_date, uu.user_full_name," +
				" aa.age, aa.inv, aa.lang, aa.ill, aa.zek, aa.vpr, aa.reduce, aa.change_," +
				" aa.close, aa.zv, aa.min, aa.nm_suv, aa.zv_kr, aa.No_Ch_Ist, aa.Koef,"+
				"aa.id_stage,a.id_center_legalaid,a.id_adv,a.id_order_type,aa.id_informer,aa.id_checker");
	}

	private static void process(String unordered) {

		String[] a = unordered.split(",");
		if (a.length != fields.length) {
			System.err.println("the arrays are not match");
			return;
		}

		int[] indexes = new int[fields.length];
		for (int j = 0; j < fields.length; j++) {
			String field = a[j].trim();

			int i = 0;
			for (; i < fields.length; i++)
				if (fields[i].equals(field)) break;

			indexes[j] = i < fields.length ? i : -1;
		}

		System.out.println(Arrays.toString(indexes));
	}

	static String[] fields = {"a.zone", "c.center_short_name", "b.adv_name_short", "a.id", "a.order_date",
			"t.type_name", "a.client_name", "a.bdate", "sta.stage_name", "aa.cycle",
			"aa.fee", "aa.fee_parus", "aa.outg_post", "aa.outg_daynight", "aa.act_no", "aa.id_act",
			"aa.act_reg_date", "aa.act_date", "aa.bank_reg_date", "aa.payment_date",
			"aa.article", "uu.user_full_name", "a.reason", "a.cancel_order", "a.cancel_date", "inf.informer_name",
			"aa.age", "aa.inv", "aa.lang", "aa.ill", "aa.zek", "aa.vpr", "aa.reduce", "aa.change_",
			"aa.close", "aa.zv", "aa.min", "aa.nm_suv", "aa.zv_kr", "aa.No_Ch_Ist", "aa.Koef",
			"aa.id_stage", "a.id_center_legalaid", "a.id_adv", "a.id_order_type", "aa.id_informer", "aa.id_checker"};
}