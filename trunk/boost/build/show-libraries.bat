@set /p LE_BJAMDIR=Enter Boost/BJam directory: 
@pushd
@cd "%LE_BJAMDIR%"
bjam --show-libraries
REM to be used in --with-##### or --without-#####
@popd
@PAUSE