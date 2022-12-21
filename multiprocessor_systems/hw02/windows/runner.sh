g++ -O0 win32.cpp -o win32

> logger_win32.txt

for var in {100000..120000}
do
	./win32 $var >> logger_win32.txt
done

rm win32.exe