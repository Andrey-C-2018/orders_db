package gov.legalaid.zt;

import java.io.FileInputStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Properties;
import java.sql.*;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
public class Main {
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

					DateFormat dt_format = new SimpleDateFormat("y-M-d");
					try (ResultSet rs = stmt.executeQuery()) {

						int i = 0;
						while (rs.next()) {
							int id_center = rs.getInt(1);
							int id_order = rs.getInt(2);
							Date order_date = rs.getDate(3);
							int id_stage = rs.getInt(4);
							String act_no = rs.getString(5);
							int cycle = rs.getInt(6);
							i++;

							stmt_update.setInt(1, i);
							stmt_update.setInt(2, id_center);
							stmt_update.setInt(3, id_order);
							stmt_update.setDate(4, order_date);
							stmt_update.setInt(5, id_stage);
							stmt_update.setString(6, act_no);
							stmt_update.setInt(7, cycle);
							int affected_recs_count = stmt_update.executeUpdate();
							if (affected_recs_count != 1)
								logger.warn(String.format(
										"команда UPDATE повернула %d " +
											"для %d %d %s %d %s %d",
										affected_recs_count,
										id_center, id_order,
										dt_format.format(order_date),
										id_stage, act_no, cycle));

							if (i % 100 == 0)
								logger.info("опрацьовано " + i + " рядків");
						}
						logger.info("опрацьовано " + i + " рядків");
					}
				}
			}
		}
		catch (Exception e) {

			logger.error(e.getMessage());
		}

	}

	private static final Logger logger = LogManager.getRootLogger();
}