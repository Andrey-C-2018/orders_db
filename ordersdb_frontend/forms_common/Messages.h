#pragma once

constexpr wchar_t E_WRONG_ZONE[] = L"Неможливо змінити цю стадію, оскільки вона належить до іншого центру";
constexpr wchar_t E_OLD_ORDER[] = L"Неможливо змінити це доручення, оскільки воно було додане більше року тому";
constexpr wchar_t E_OLD_STAGE[] = L"Неможливо змінити цю стадію, оскільки вона належить до минулого періоду";

const wchar_t *E_ENTRIES[] = { E_OLD_ORDER, E_OLD_STAGE };
enum E_Indexes {
	E_OLD_ORDER_INDEX = 0, \
	E_OLD_STAGE_INDEX = 1
};