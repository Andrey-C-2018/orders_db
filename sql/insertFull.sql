USE `orders`;
DROP procedure IF EXISTS `insertFull`;

DELIMITER $$
USE `orders`$$
CREATE DEFINER=`root`@`%` PROCEDURE `insertFull`(ignore_existing_order INT(1), id_center INT, id INT, order_date DATE, id_order_type INT, id_adv INT, client_name VARCHAR(255), bdate DATE, id_user INT, zone CHAR(2), fee DECIMAL(10,2), id_stage INT, article VARCHAR(255), id_informer INT, id_act CHAR(40), act_no CHAR(11), act_date DATE, cycle INT, act_reg_date DATE, payment_date DATE, outgoings DECIMAL(8,2), bank_reg_date DATE, id_checker INT, outg_post DECIMAL(8, 2), outg_daynight DECIMAL(8,2))
BEGIN
	DECLARE table_id CHAR(15);
	DECLARE order_exists INT;
    DECLARE _id INT DEFAULT id;
    DECLARE _order_date DATE DEFAULT order_date;
    
	DECLARE EXIT HANDLER FOR 1062
		BEGIN
			ROLLBACK;
				SIGNAL SQLSTATE '23000' SET 
					MYSQL_ERRNO = 1062,
					MESSAGE_TEXT = table_id;
        END;
	DECLARE EXIT HANDLER FOR 1452
		BEGIN
			ROLLBACK;
				SIGNAL SQLSTATE '23000' SET 
					MYSQL_ERRNO = 1452,
					MESSAGE_TEXT = table_id;
        END;
 	
    START TRANSACTION;
    
	IF ignore_existing_order = 1 THEN
		SET order_exists = (SELECT a.id_center_legalaid FROM orders a WHERE a.id_center_legalaid = id_center AND a.id = _id AND a.order_date = _order_date);
	END IF;
    
    SET table_id = "orders";
    IF order_exists IS NULL THEN
    	INSERT INTO orders VALUES(id_center, id, order_date, id_order_type, id_adv, client_name, bdate, 0, 0, id_user, NOW(), NULL, NULL, NULL, zone);
	END IF;
    
    SET table_id = "payments";
    INSERT INTO payments VALUES(id_center, id, order_date, fee, id_stage, article, id_informer, id_act, act_no, act_date, cycle, id_user, NOW(), NULL, IF(payment_date IS NOT NULL,zone,NULL), act_reg_date, payment_date, outgoings, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, bank_reg_date, id_checker, outg_post, outg_daynight);
    
    COMMIT;
END$$

DELIMITER ;
