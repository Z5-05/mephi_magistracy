Win32 need to be run in Windows, so other methods runner in file ```runner.sh``` which will go to container with linux and get file with results back to Windows.

```
docker build -t iverendeev/c_homework:v1 .
docker run --name c_homework -p 8000:8000 iverendeev/c_homework:v1
```

docker cp iverendeev/c_homework:v1:/dir/runner.sh runner.sh