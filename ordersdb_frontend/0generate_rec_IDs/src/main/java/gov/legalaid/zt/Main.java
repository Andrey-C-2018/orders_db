package gov.legalaid.zt;

import java.io.FileInputStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.sql.*;
import java.util.stream.IntStream;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class Main {

	public static final int BATCH_SIZE = 50;

	public static void main(String[] args) {

		Properties props = new Properties();
		try(FileInputStream in = new FileInputStream("config.properties")) {

			props.load(in);
		}
		catch(Exception e) {

			logger.error(e.getMessage());
			return;
		}

		String server = props.getProperty("server");
		logger.info("server = " + server);
		String port = props.getProperty("port");
		logger.info("port = " + port);
		String database = props.getProperty("database");
		logger.info("database = " + database);
		String center = props.getProperty("center");
		logger.info("center = " + center);
		String user = props.getProperty("user");
		logger.info("user = " + user);
		String password = props.getProperty("password");

		Cryptor cryptor = new Cryptor();
		password = cryptor.decrypt(password);
		try(Connection conn = DriverManager.getConnection(
				"jdbc:mariadb://" + server + ':' + port + '/' +	database,
				user, password)) {

			logger.info("З'єднання з БД встановлено");
			String query = "SELECT id_center, id_order, order_date," +
						" id_stage, act_no, cycle, rec_id " +
						"FROM payments " +
						"WHERE order_date >= '2022-01-01' " +
						"ORDER BY 1, 3, 2, 4, 5, 6";
			try (PreparedStatement stmt = conn.prepareStatement(query)) {

				query = "UPDATE payments SET rec_id = ? " +
						"WHERE id_center = ? AND id_order = ? AND" +
						" order_date = ? AND id_stage = ? AND" +
						" act_no = ? AND cycle = ?";
				try (PreparedStatement stmt_update = conn.prepareStatement(query)) {

					try (ResultSet rs = stmt.executeQuery()) {

						int i = 1;
						ArrayList<PaymentsPrimKey> batch_list = new ArrayList<>();
						while (rs.next()) {
							PaymentsPrimKey prim_key = new PaymentsPrimKey(rs);
							batch_list.add(prim_key);

							stmt_update.setInt(1, i);
							stmt_update.setInt(2, prim_key.id_center);
							stmt_update.setInt(3, prim_key.id_order);
							stmt_update.setDate(4, prim_key.order_date);
							stmt_update.setInt(5, prim_key.id_stage);
							stmt_update.setString(6, prim_key.act_no);
							stmt_update.setInt(7, prim_key.cycle);
							stmt_update.addBatch();

							if (batch_list.size() == BATCH_SIZE) {
								insertBatch(stmt_update, batch_list);
								batch_list.clear();
							}

							if (i % 100 == 0)
								logger.info("опрацьовано " + i + " рядків");
							i++;
						}
						if (!batch_list.isEmpty())
							insertBatch(stmt_update, batch_list);

						logger.info("опрацьовано " + i + " рядків");
					}
				}
			}
		}
		catch (Exception e) {

			logger.error(e.getMessage());
		}

	}

	private static void insertBatch(PreparedStatement stmt,
									List<PaymentsPrimKey> batch_list) throws SQLException {

		assert !batch_list.isEmpty();

		int[] affected = stmt.executeBatch();
		assert affected.length == batch_list.size();
		int[] incorrect_inserts = IntStream.range(0, affected.length)
												.filter(i -> affected[i] == 0 ||
														affected[i] != Statement.SUCCESS_NO_INFO)
												.toArray();

		if (incorrect_inserts != null)
			for (int i : incorrect_inserts) {

				PaymentsPrimKey key = batch_list.get(i);
				logger.warn(String.format("команда UPDATE повернула %d для [%d %d %s %d %s %d]",
						affected[i],
						key.id_center, key.id_order,
						dt_format.format(key.order_date),
						key.id_stage, key.act_no, key.cycle));
			}

	}

	private static final class PaymentsPrimKey {

		int id_center;
		int id_order;
		Date order_date;
		int id_stage;
		String act_no;
		int cycle;

		PaymentsPrimKey(ResultSet rs) throws SQLException {

			id_center = rs.getInt(1);
			id_order = rs.getInt(2);
			order_date = rs.getDate(3);
			id_stage = rs.getInt(4);
			act_no = rs.getString(5);
			cycle = rs.getInt(6);
		}
	}

	private static final Logger logger = LogManager.getRootLogger();
	private static final DateFormat dt_format = new SimpleDateFormat("y-M-d");
}