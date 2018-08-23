#pragma once

constexpr char getQuoteChar(const char) { return '"'; }
constexpr wchar_t getQuoteChar(const wchar_t) { return L'"'; }

constexpr char getMinusChar(const char) { return '-'; }
constexpr wchar_t getMinusChar(const wchar_t) { return L'-'; }

constexpr char getZeroChar(const char) { return '0'; }
constexpr wchar_t getZeroChar(const wchar_t) { return L'0'; }

constexpr char getNineChar(const char) { return '9'; }
constexpr wchar_t getNineChar(const wchar_t) { return L'9'; }

constexpr const char *getEmptyStr(const char) { return ""; }
constexpr const wchar_t *getEmptyStr(const wchar_t) { return L""; }