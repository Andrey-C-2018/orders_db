#pragma once

struct DBaseHeader { 
	uint8_t version;
	uint8_t last_update[3];
	uint32_t recs_count;
	uint16_t length;
	uint16_t rec_len;
	uint8_t  reserved[20];
};

struct DBaseField {
	char name[11];
	uint8_t type;
	uint8_t gap[4];
	uint8_t length;
	uint8_t dec_point;
	uint8_t fill[14];
};

enum DBaseFieldTypes {
	DBF_FIELD_CHAR = 'C', \
	DBF_FIELD_NUM = 'N', \
	DBF_FIELD_LOGIC = 'L', \
	DBF_FIELD_MEMO = 'M', \
	DBF_FIELD_DATE = 'D'
};
