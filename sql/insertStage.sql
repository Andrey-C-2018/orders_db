CREATE DEFINER=`root`@`localhost` PROCEDURE `insertStage`(id_center INT, id INT, order_date DATE, fee DECIMAL(10,2), id_stage INT, article VARCHAR(255), id_informer INT, id_act CHAR(40), act_no INT, act_date DATE, cycle INT, id_user INT, act_reg_date DATE, payment_date DATE, outgoings DECIMAL(8,2), bank_reg_date DATE, id_checker INT, outg_post DECIMAL(8, 2), outg_daynight DECIMAL(8,2))
BEGIN
   
	DECLARE EXIT HANDLER FOR 1062
		BEGIN
			ROLLBACK;
				SIGNAL SQLSTATE '23000' SET 
					MYSQL_ERRNO = 1062,
					MESSAGE_TEXT = "payments";
        END;
	DECLARE EXIT HANDLER FOR 1452
		BEGIN
			ROLLBACK;
				SIGNAL SQLSTATE '23000' SET 
					MYSQL_ERRNO = 1452,
					MESSAGE_TEXT = "payments";
        END;
 	
    INSERT INTO payments VALUES(id_center, id, order_date, fee, id_stage, article, id_informer, id_act, act_no, act_date, cycle, id_user, NOW(), NULL, IF(payment_date IS NOT NULL,IF(id_center<>1 AND order_date>="2017-01-01",'Ьж','аж'),NULL), act_reg_date, payment_date, outgoings, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, bank_reg_date, id_checker, outg_post, outg_daynight);
    
END