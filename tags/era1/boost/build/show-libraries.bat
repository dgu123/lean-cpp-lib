@CALL boost-dir.bat
@pushd
@cd "%LE_BJAMDIR%"
bjam --show-libraries
REM to be used in --with-##### or --without-#####
@popd
@PAUSE