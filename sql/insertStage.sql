USE orders;
DROP procedure IF EXISTS `insertStage`;

DELIMITER $$
CREATE PROCEDURE `insertStage`(id_center INT, id INT, order_date DATE, fee DECIMAL(10,2), 
                              id_stage INT, article VARCHAR(255), id_informer INT, id_act CHAR(40), 
                              act_no CHAR(11), act_date DATE, cycle INT, id_user INT, act_reg_date DATE, 
                              payment_date DATE, bank_reg_date DATE, id_checker INT, 
                              outg_post DECIMAL(8, 2), outg_daynight DECIMAL(8,2),
                              rejection_date DATE, rejection_reason VARCHAR(120))
BEGIN
   
    DECLARE EXIT HANDLER FOR 1062
    BEGIN
        ROLLBACK;
        SIGNAL SQLSTATE '23000' SET MYSQL_ERRNO = 1062, MESSAGE_TEXT = "payments";
    END;

    DECLARE EXIT HANDLER FOR 1452
    BEGIN
       ROLLBACK;
       SIGNAL SQLSTATE '23000' SET MYSQL_ERRNO = 1452, MESSAGE_TEXT = "payments";
    END;
 	
    INSERT INTO payments(id_center,id_order,order_date,fee,id_stage,article,id_informer, id_act, 
                        act_no, act_date, cycle, id_user, last_changed, id_act_parus,
                        is_paid, act_reg_date, payment_date, fee_parus, age, inv, lang, ill, zek,
                        vpr, reduce, change_, close, zv, min, nm_suv, zv_kr, No_Ch_Ist, Koef,
                        bank_reg_date, id_checker, outg_post, outg_daynight, appeal_softer,
                        detect_softer, reject_appeal, change_kval_kr, reduce_ep, change_med, cancel_med,
                        rejection_date, rejection_reason) 
    VALUES(id_center, id, order_date, fee, id_stage, article, id_informer, id_act, 
           act_no, act_date, cycle, id_user, NOW(), NULL, 
           IF(payment_date IS NOT NULL,IF(id_center<>1 AND order_date>="2017-01-01",'МЦ','РЦ'),NULL), 
           act_reg_date, payment_date, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 
           bank_reg_date, id_checker, outg_post, outg_daynight, 0, 0, 0, 0, 0, 0, 0,
           rejection_date, rejection_reason);
END$$

DELIMITER ;

GRANT EXECUTE ON PROCEDURE `orders`.`insertStage` TO 'orders_user_new'@'%';