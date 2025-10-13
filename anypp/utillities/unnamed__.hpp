#pragma once

#define ANY_MERGE_(a, b) a##b
#define ANY_LABEL_(a) ANY_MERGE_(unique_name_, a)
#define ANY_UNIQUE_NAME_ ANY_LABEL_(__COUNTER__)
#define ANY_UNIQUE_NAME ANY_UNIQUE_NAME_
#define __ ANY_UNIQUE_NAME_
