#pragma once

struct CPaymentsConstraints {
	bool wrong_zone;
	bool stage_finalized;
	bool old_stage_locked;
	bool old_order_locked;
};