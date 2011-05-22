@boost-dir.bat
@set LE_CALLDIR=%CD%
@cd "%LE_BJAMDIR%"
bjam stage ^
 link=shared ^
 --toolset=msvc-10.0 ^
 --build-dir="%LE_CALLDIR%/build" ^
 --stagedir="%LE_CALLDIR%/dll" ^
 --with-date_time ^
 --with-filesystem ^
 --with-regex ^
 --wigh-signals ^
 --with-system ^
 --with-test ^
 --with-thread ^
 define=_CRT_NONSTDC_NO_DEPRECATE ^
 define=_CRT_SECURE_NO_DEPRECATE ^
 define=_SCL_SECURE_NO_DEPRECATE ^
 define=_SECURE_SCL=0 ^
 define=_HAS_ITERATOR_DEBUGGING=0 ^
 runtime-link=shared,static ^
 threading=multi ^
 >"%LE_CALLDIR%/build-dll.log" 2>&1
@cd "%LE_CALLDIR%"
@PAUSE