CREATE DEFINER=`root`@`%` TRIGGER set_is_paid BEFORE UPDATE ON payments
FOR EACH ROW
BEGIN
	IF IsNull(NEW.payment_date) THEN
		SET NEW.is_paid = NULL;
	ELSEIF NEW.id_center = 1 OR NEW.id_center = 9 OR NEW.order_date < '2017-01-01' THEN
		SET NEW.is_paid = 'РЦ';
	ELSE
		SET NEW.is_paid = 'МЦ';
END IF;
END