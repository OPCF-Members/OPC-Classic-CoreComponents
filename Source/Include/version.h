
#define xstr(s) str(s)
#define str(s) #s

#define BUILD_VERSION 9999
#define BUILD_NUMBER  9998

#define COPYRIGHT_DATE "9998998-2020"
#define FILE_VERSION_TEXT xstr(BUILD_VERSION) "." xstr(BUILD_NUMBER)
#define PRODUCT_VERSION_TEXT xstr(BUILD_VERSION) ".0"
