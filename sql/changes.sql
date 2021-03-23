ALTER TABLE `orders`.`payments` 
ADD COLUMN `act_no` ENUM('Винагорода', 'Витрати') NULL DEFAULT 'Винагорода' AFTER `id_act`;

ALTER TABLE `orders`.`payments` 
CHANGE COLUMN `act_no` `act_no` ENUM('Винагорода', 'Витрати') CHARACTER SET 'utf8' NOT NULL ,
DROP PRIMARY KEY,
ADD PRIMARY KEY (`id_center`, `id_order`, `order_date`, `id_stage`, `cycle`, `act_no`);

INSERT INTO payments SELECT id_center,id_order,order_date,outgoings,id_stage,article,id_informer,id_act,"Витрати",act_date,cycle,id_user,last_changed,id_act_parus,is_paid,act_reg_date,payment_date,outgoings,outgoings,age,inv,lang,ill,zek,vpr,reduce,change_,close,zv,min,nm_suv,zv_kr,No_ch_Ist,Koef,bank_reg_date,id_checker,outg_post,outg_daynight FROM payments WHERE outgoings <> 0;

UPDATE payments SET fee = fee - outgoings WHERE act_no = "Винагорода"
UPDATE payments SET outgoings = 0, outg_post = 0, outg_daynight = 0 WHERE act_no = "Винагорода"
